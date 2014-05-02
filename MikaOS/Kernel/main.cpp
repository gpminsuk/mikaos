#include "liballoc_1_1.h"
#include "task.h"
#include "CDisplayDriver.h"
#include "ISR.h"
#include "GDT.h"
#include "Loader.h"
#include "fat16.h"
#include "fdddrv.h"

extern "C" void __gxx_personality_seh0() { }
extern "C" void _Unwind_Resume() { }
extern "C" void atexit() { }

void* operator new[](size_t size)
{
	return PREFIX(malloc)(size);
}

void operator delete[](void* ptr)
{
	PREFIX(free)(ptr);
}

void* operator new(size_t size)
{
	return PREFIX(malloc)(size);
}

void operator delete(void* ptr)
{
	PREFIX(free)(ptr);
}

extern "C" void AsmFirstSoftwareContextSwitch(struct STask* rootTask);
extern "C" void AsmLIDT(class CInterruptHandlerDescriptor* Descriptor);
extern "C" void AsmLGDT(class CGlobalDescriptorTable* Descriptor);
extern "C" void AsmLTR(unsigned int index);

void InitializeISR()
{
	for (int i = 0; i < MAX_IDT; i++)
	{
		CInterruptHandler_IDT::Handlers[i].OffsetLow = 0;
		CInterruptHandler_IDT::Handlers[i].OffsetMid = 0;
		CInterruptHandler_IDT::Handlers[i].OffsetHigh = 0;
		CInterruptHandler_IDT::Handlers[i].Selector = 0;
		CInterruptHandler_IDT::Handlers[i].Type = 0;
		CInterruptHandler_IDT::Handlers[i].Zero1 = 0;
		CInterruptHandler_IDT::Handlers[i].Zero2 = 0;
	}
	for (int i = 0; i < MAX_IDT; i++)
	{
		if (CInterruptHandler::Handlers[i].Func == 0)
		{
			break;
		}

		unsigned long long Addr = (unsigned long long)(CInterruptHandler::Handlers[i].Func);
		if ((CInterruptHandler::Handlers[i].Type & IDTC_TASKGATE) == IDTC_TASKGATE)
		{
		}
		else
		{
			CInterruptHandler_IDT::Handlers[CInterruptHandler::Handlers[i].Number].OffsetLow = (unsigned short)(Addr & 0xFFFF);
			CInterruptHandler_IDT::Handlers[CInterruptHandler::Handlers[i].Number].OffsetMid = (unsigned short)((Addr >> 16) & 0xFFFF);
			CInterruptHandler_IDT::Handlers[CInterruptHandler::Handlers[i].Number].OffsetHigh = (unsigned int)((Addr >> 32) & 0xFFFFFFFF);
			CInterruptHandler_IDT::Handlers[CInterruptHandler::Handlers[i].Number].Selector = 0x0008;
			CInterruptHandler_IDT::Handlers[CInterruptHandler::Handlers[i].Number].Type = CInterruptHandler::Handlers[i].Type;
		}
	}
	CInterruptHandlerDescriptor Descriptor;
	Descriptor.Size = MAX_IDT*sizeof(CInterruptHandler_IDT);
	Descriptor.Address = CInterruptHandler_IDT::Handlers;
	AsmLIDT(&Descriptor);

	AsmOut(PIC_MASTER_COMMAND_PORT, PIC_INIT_START);
	AsmOut(PIC_MASTER_DATA_PORT, PIC_MASTER_VECTOR);
	AsmOut(PIC_MASTER_DATA_PORT, PIC_MASTER_EDGE);
	AsmOut(PIC_MASTER_DATA_PORT, PIC_SELECT_8086);
	AsmOut(PIC_MASTER_DATA_PORT, ~0x41);

	AsmOut(PIC_SLAVE_COMMAND_PORT, PIC_INIT_START);
	AsmOut(PIC_SLAVE_DATA_PORT, PIC_SLAVE_VECTOR);
	AsmOut(PIC_SLAVE_DATA_PORT, PIC_SLAVE_EDGE);
	AsmOut(PIC_SLAVE_DATA_PORT, PIC_SELECT_8086);
	AsmOut(PIC_SLAVE_DATA_PORT, ~0x01);
}

void InitializeGDT()
{
	unsigned char* Entries = CGlobalDescriptorEntry::Entries;
	for (int i = 0; i < NUM_GDT_ENTRIES; ++i)
	{
		CGlobalDescriptorEntry::Entries[i] = 0;
	}
	CGlobalDescriptorEntry& Null = (CGlobalDescriptorEntry&)*Entries;
	Null.Access = 0x0;
	Null.FlagLimit = 0x0;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry& KernelCode = (CGlobalDescriptorEntry&)*Entries;
	KernelCode.Access = 0x98;
	KernelCode.FlagLimit = 0x20;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry& KernelData = (CGlobalDescriptorEntry&)*Entries;
	KernelData.Access = 0x92;
	KernelData.FlagLimit = 0x00;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry& UserCode = (CGlobalDescriptorEntry&)*Entries;
	UserCode.Access = 0xB8;
	UserCode.FlagLimit = 0x20;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry& UserData = (CGlobalDescriptorEntry&)*Entries;
	UserData.Access = 0xB2;
	UserData.FlagLimit = 0x00;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry& RealCode = (CGlobalDescriptorEntry&)*Entries;
	RealCode.LimitLow = 0xffff;
	RealCode.Access = 0x98;
	RealCode.FlagLimit = 0x03;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry& RealData = (CGlobalDescriptorEntry&)*Entries;
	RealData.LimitLow = 0xffff;
	RealData.Access = 0x92;
	RealData.FlagLimit = 0x03;
	Entries += sizeof(CGlobalDescriptorEntry);

	CGlobalDescriptorEntry_TSS& TSS = (CGlobalDescriptorEntry_TSS&)*Entries;
	TSS.BaseHigh = ((unsigned long long)&CGlobalDescriptorEntry::TaskStateSegments[0]) >> 32;
	TSS.BaseMidHigh = (((unsigned long long)&CGlobalDescriptorEntry::TaskStateSegments[0]) >> 24) & 0xFF;
	TSS.BaseMidLow = (((unsigned long long)&CGlobalDescriptorEntry::TaskStateSegments[0]) >> 16) & 0xFF;
	TSS.BaseLow = ((unsigned long long)&CGlobalDescriptorEntry::TaskStateSegments[0]) & 0xFFFF;
	TSS.Access = 0xE9;
	TSS.Flag = 0x0;
	TSS.LimitLow = sizeof(CTaskStateSegment);

	CGlobalDescriptorEntry::TaskStateSegments[0].rsp0 = 0x10000;

	CGlobalDescriptorTable Descriptor;
	Descriptor.Size = NUM_GDT_ENTRIES;
	Descriptor.Address = CGlobalDescriptorEntry::Entries;
	AsmLGDT(&Descriptor);
	AsmLTR(0x3B);
}

void newTask(void* arg)
{
	CDisplayDriver::PrintString("newTask1");
	Execute("Kernel2.dll");
	while (1)
	{
	}
}

extern "C" void start()
{
	GDisplayDriver.ClearScreen();
	InitializeGDT();
	InitializeISR();
	FddInitializeDriver();
	FsInitializeModule();
	STask* task = CreateTask(newTask, 0);
	AsmFirstSoftwareContextSwitch(task);
	__asm("cli;hlt;");
}