#pragma once

#define FDD_JOB_READ		0x1
#define FDD_JOB_WRITE		0x2

void FddInitializeDriver();
void FddpReadWriteSector(unsigned char jobType, unsigned short linearSector, unsigned char numbersOfSectors, unsigned char* data, unsigned int size);