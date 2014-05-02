#include "CKernel.h"
#include "CDisplayDriver.h"
#include "fat16.h"
#include "vgadrv.h"
#include "vbe.h"
#include "CPaging.h"
#include "Loader.h"

bool Fsp_of_callback2(FILE_INFO* fileInfo, void* context)
{
	//if (!strcmp(fileInfo->filename, "BOOTLDR2BIN")) {
	CDisplayDriver::PrintString("%s", fileInfo->filename);
	return 1;
}

void putpixel_vesa_640x480(int x, int y, int color)
{
	int address = y * GCurrentVbeModeInfo->YResolution + x;
	int bank_size = GCurrentVbeModeInfo->WinGranularity * GCurrentVbeModeInfo->XResolution;
	int bank_number = address / bank_size;
	int bank_offset = address % bank_size;

	BiosInterrupt((char*)VbeSetBank, bank_number);

	unsigned char* a = (unsigned char*)0xA0000;
	a[bank_offset] = color;
}

void KernelStart(void* arg)
{
	CDisplayDriver::PrintString("START");
	FsGetFileList(Fsp_of_callback2, 0);
	CDisplayDriver::PrintString("END");
	LoadPE("wmerror.dll", (char*)0x20000);
	/*BiosInterrupt((char*)VbeGetControllerInfo);
	BiosInterrupt((char*)VbeGetModeInfo);
	for (int i = 0; i < 1024; ++i)
	{
		putpixel_vesa_640x480(100, i, 128);
	}*/
	//demo_graphics();
	while (1)
	{
	}
}