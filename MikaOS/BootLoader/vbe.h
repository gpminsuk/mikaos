#pragma once

#include "Global.h"

struct VbeInfoBlock
{
	char VbeSignature[4];
	unsigned short VbeVersion;
	unsigned short OemStringPtr[2];
	unsigned char Capabilities[4];
	unsigned short VideoModePtr[2];
	unsigned short TotalMemory;
};

struct ModeInfoBlock
{
	unsigned short	ModeAttributes;
	unsigned char	WinAAttributes;
	unsigned char	WinBAttributes;
	unsigned short	WinGranularity;
	unsigned short	WinSize;
	unsigned short	WinASegment;
	unsigned short	WinBSegment;
	unsigned short	WinFuncPtr[2];
	unsigned short	BytesPerScanLine;
	unsigned short	XResolution;
	unsigned short	YResolution;
	unsigned char	XCharSize;
	unsigned char	YCharSize;
	unsigned char	NumberOfPlanes;
	unsigned char	BitsPerPixel;
	unsigned char	NumberOfBanks;
	unsigned char	MemoryModel;
	unsigned char	BankSize;
	unsigned char	NumberOfImagePages;
	unsigned char	Reserved;
	unsigned char	RedMaskSize;
	unsigned char	RedFieldPosition;
	unsigned char	GreenMaskSize;
	unsigned char	GreenFieldPosition;
	unsigned char	BlueMaskSize;
	unsigned char	BlueFieldPosition;
	unsigned char	RsvdMaskSize;
	unsigned char	RsvdFieldPosition;
	unsigned char	DirectColorModeInfo;
	unsigned long	PhysBasePtr;
	unsigned long	Reserved1;
	unsigned short	Reserved2;
	unsigned short	LinBytesPerScanLine;
	unsigned char	BnkNumberOfImagePages;
	unsigned char	LinNumberOfImagePages;
	unsigned char	LinRedMaskSize;
	unsigned char	LinRedFieldPosition;
	unsigned char	LinGreenMaskSize;
	unsigned char	LinGreenFieldPosition;
	unsigned char	LinBlueMaskSize;
	unsigned char	LinBlueFieldPosition;
	unsigned char	LinRsvdMaskSize;
	unsigned char	LinRsvdFieldPosition;
	unsigned long	MaxPixelClock;
	unsigned char	Reserved3[189];
};

extern ModeInfoBlock* GCurrentVbeModeInfo;
extern VbeInfoBlock* GVbeInfo;

void InitializeVbe();