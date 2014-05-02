#include "fdddrv.h"
#include "CDisplayDriver.h"
#include "fat16.h"

#define BYTES_PER_SECTOR	512
#define SECTORS_PER_TRACK	18

#define FDD_DOR_PORT		0x3f2
#define FDD_STATUS_PORT		0x3f4
#define FDD_CMD_PORT		0x3f4
#define FDD_DATA_PORT		0x3f5

#define FDD_FDC_ENABLE		0x1c

void FddInitializeDriver();
void FddpJobProcessThread(void* arg);

void FddpReadWriteSector(unsigned char jobType, unsigned short sector, unsigned char numbersOfSectors, unsigned char* data);

void FddpTurnOnMotor();
void FddpTurnOffMotor();
void FddpSetupDMA(unsigned char jobType);
void FddpWriteFdcData(unsigned char data);
unsigned char FddpReadFdcData();

static int m_FddInterruptOccurred;

void Fdd_IRQ_Handler(void)
{
	m_FddInterruptOccurred = 1;
}

void FddInitializeDriver()
{
	FddpTurnOffMotor();
}

void FddpTurnOnMotor()
{
	AsmOut(FDD_DOR_PORT, FDD_FDC_ENABLE); /* drive A, DMA, FDC Enable */
}

void FddpSenseInterrupt()
{
	FddpWriteFdcData(0x08);
	FddpReadFdcData();
	FddpReadFdcData();
	//CDisplayDriver::PrintString("Sens : %d %d", st0, cylinder);
}

void FddpTurnOffMotor()
{
	AsmOut(FDD_DOR_PORT, 0x00);
}

void FddpWriteFdcData(unsigned char data)
{
	unsigned char status;
	do {
		status = AsmIn(FDD_STATUS_PORT);
	} while ((status & 0xc0) != 0x80);
	AsmOut(FDD_DATA_PORT, data);
}

unsigned char FddpReadFdcData()
{
	unsigned char status;
	do {
		status = AsmIn(FDD_STATUS_PORT);
	} while (!(status & 0x80));
	return AsmIn(FDD_DATA_PORT);
}

void FddpSetupDMA(unsigned char jobType)
{
	AsmOut(0x0a, 0x06);
	AsmOut(0x0c, 0xff);

	/* setup address = 0x5000 */
	AsmOut(0x04, 0x00);
	AsmOut(0x04, 0x50);

	AsmOut(0x0c, 0xff);

	/* setup count */
	AsmOut(0x05, 0xff);
	AsmOut(0x05, 0x25);

	AsmOut(0x81, 0x00);

	/* enable dma controller */
	AsmOut(0x0a, 0x02); /* clear mask bit, channel 2 */

	if (jobType == FDD_JOB_READ)
	{
		AsmOut(0x0a, 0x06);
		AsmOut(0x0a, 0x5a);
		AsmOut(0x0a, 0x02);
	}
}

void FddpReadWriteSector(unsigned char jobType, unsigned short linearSector, unsigned char numbersOfSectors, unsigned char* data, unsigned int size)
{
	unsigned char drive, head, track, sector;

	/* ---------------------------------------- */ /* do not process write operation yet! */
	if (jobType == FDD_JOB_WRITE)
		return;
	/* ---------------------------------------- */

	/* determine the values of drive, head, track and sector */
	drive = 0; /* A */
	head = ((linearSector / SECTORS_PER_TRACK) % 2);
	track = (linearSector / (SECTORS_PER_TRACK * 2));
	sector = (linearSector % SECTORS_PER_TRACK) + 1;

	/* turn on motor - 1st interrupt */
	m_FddInterruptOccurred = 0; {
		FddpTurnOnMotor();
	} while (!m_FddInterruptOccurred);
	
	FddpSenseInterrupt();

	/* calibrate drive - 2nd interrupt */
	m_FddInterruptOccurred = 0; {
		FddpWriteFdcData(0x07); /* calibrate command */
		FddpWriteFdcData(0x00); /* drive 00 : A */
	} while (!m_FddInterruptOccurred);

	FddpSenseInterrupt();

	/* seek - 3rd interrupt */
	m_FddInterruptOccurred = 0; {
		FddpWriteFdcData(0x0f);
		FddpWriteFdcData((head << 2) + drive);
		FddpWriteFdcData(track);
	} while (!m_FddInterruptOccurred);

	FddpSenseInterrupt();

	FddpSetupDMA(jobType);

	/* r/w operation - 4th interrupt */
	m_FddInterruptOccurred = 0; {		
		FddpWriteFdcData(0xc6);
		FddpWriteFdcData((head << 2) + drive);
		FddpWriteFdcData(track);
		FddpWriteFdcData(head);
		FddpWriteFdcData(sector);
		FddpWriteFdcData(2);
		FddpWriteFdcData(sector + numbersOfSectors);
		FddpWriteFdcData(0x1b); // 3.5inch disk
		FddpWriteFdcData(0xff);
	} while (!m_FddInterruptOccurred);
	
	/* copy buffer */
	unsigned char* DMAAddr = (unsigned char*)0x5000;
	if (jobType == FDD_JOB_READ)
	{
		for (unsigned int i = 0; i < size; i++)
		{
			data[i] = DMAAddr[i];
		}
	}

	/* turn off motor */
	FddpTurnOffMotor();
}