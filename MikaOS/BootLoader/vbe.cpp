#include "vbe.h"

VbeInfoBlock* GVbeInfo;
ModeInfoBlock* GCurrentVbeModeInfo;

void InitializeVbe()
{
	GVbeInfo = (VbeInfoBlock*)0x8100;
	GCurrentVbeModeInfo = (ModeInfoBlock*)0x8300;
}