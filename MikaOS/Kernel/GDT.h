#pragma once

#define NUM_GDT_ENTRY		7
#define NUM_GDT_ENTRY_TSS	1
#define NUM_GDT_ENTRIES		8*NUM_GDT_ENTRY + 16*NUM_GDT_ENTRY_TSS

#pragma pack(push, 1)
class CGlobalDescriptorEntry_TSS
{
public:
	unsigned short LimitLow;
	unsigned short BaseLow;
	unsigned char BaseMidLow;
	unsigned char Access;
	unsigned char LimitHigh : 4;
	unsigned char Flag : 4;
	unsigned char BaseMidHigh;
	unsigned int BaseHigh;
	unsigned int Reserved;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CTaskStateSegment
{
	unsigned int Reserved1;
	unsigned long long rsp0;
	unsigned long long rsp1;
	unsigned long long rsp2;
	unsigned long long Reserved2;
	unsigned long long ist0;
	unsigned long long ist1;
	unsigned long long ist2;
	unsigned long long ist3;
	unsigned long long ist4;
	unsigned long long ist5;
	unsigned long long ist6;
	unsigned long long Reserved3;
	unsigned short Reserved4;
	unsigned short IOMapBase;
};
#pragma pack(pop)

#pragma pack(push, 1)
class CGlobalDescriptorEntry
{
public:
	unsigned short LimitLow;
	unsigned short BaseLow;
	unsigned char BaseMid;
	unsigned char Access;
	unsigned char FlagLimit;
	unsigned char BaseHigh;

	static unsigned char Entries[NUM_GDT_ENTRIES];
	static CTaskStateSegment TaskStateSegments[NUM_GDT_ENTRY_TSS];
};
#pragma pack(pop)

#pragma pack(push, 1)
class CGlobalDescriptorTable
{
public:
	unsigned short Size;
	unsigned char* Address;
};
#pragma pack(pop)