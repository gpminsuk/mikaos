#include "Global.h"
#include "CDisplayDriver.h"
#include "vbe.h"
#include "liballoc_1_1.h"
#include "fat16.h"
#include "fdddrv.h"
#include "string.h"
#include "Loader.h"

#include "ISR.h"
#include "GDT.h"

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
	AsmOut(PIC_MASTER_DATA_PORT, ~0x40);

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

extern void (*start_ctors)();
extern void (*end_ctors)();
extern void (*start_dtors)();
extern void (*end_dtors)();

extern "C" void Construct()
{
	void(**iterator)() = reinterpret_cast<void(**)()>(&start_ctors);
	while (iterator < reinterpret_cast<void(**)()>(&end_ctors))
	{
		void(*fp)() = reinterpret_cast<void(*)()>(*iterator);
		fp();
		iterator++;
	}
}

extern "C" void Destruct()
{
	void(**iterator)() = reinterpret_cast<void(**)()>(&start_dtors);
	while (iterator < reinterpret_cast<void(**)()>(&end_dtors))
	{
		void(*fp)() = reinterpret_cast<void(*)()>(*iterator);
		fp();
		iterator++;
	}
}

bool Fsp_of_callback2(FILE_INFO* fileInfo, void* context)
{
	//if (!strcmp(fileInfo->filename, "BOOTLDR2BIN")) {
	CDisplayDriver::PrintString("%s", fileInfo->filename);
	return 1;
}

extern "C" void start()
{
	Construct();
	InitializeGDT();
	InitializeISR();
	InitializeVbe();
	FddInitializeDriver();
	FsInitializeModule();
	CDisplayDriver::PrintString("START");
	FsGetFileList(Fsp_of_callback2, 0);
	CDisplayDriver::PrintString("END");
	char* kernelDLL = (char*)0x20000;
	SPEFile kernel;
	LoadPE("Kernel.dll", kernelDLL, &kernel);
	CDisplayDriver::PrintString("%X", kernel.peoHeader->AddressOfEntryPoint);
	register char* EntryAddress __asm("%rax");
	EntryAddress = (char*)0x20000 + kernel.peoHeader->AddressOfEntryPoint;
	__asm(
		"pushq	$0x10;"
		"pushq	$0x50000;"
		"pushfq;"
		"pushq	$0x08;"
		"pushq	%rax;"
		"iretq;"
	);
	Destruct();
	AsmHalt();
}

extern "C" void __gxx_personality_seh0()
{

}

extern "C" void _Unwind_Resume()
{

}
	
extern "C" void atexit()
{
}

extern "C" void __cxa_pure_virtual()
{
	while (1);
}

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
