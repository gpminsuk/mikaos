#pragma once

#define IDTC_TASKGATE			0x5
#define IDTC_INTGATE			0xE
#define IDTC_TRAPGATE			0x7

#define IDTC_DPL0				0x00
#define IDTC_DPL1				0x20
#define IDTC_DPL2				0x40
#define IDTC_DPL3				0x60

#define IDTC_PRESENT			0x80
#define IDTC_STORAGE			0x10

#define MAX_IDT					0x40

#define ASM_IRETQ()				__asm__("leave; iretq");
#define EOI_MASTER()			AsmOut(PIC_MASTER_COMMAND_PORT, PIC_EOI_VALUE);
#define EOI_SLAVE()				AsmOut(PIC_MASTER_COMMAND_PORT, PIC_EOI_VALUE); AsmOut(PIC_SLAVE_COMMAND_PORT, PIC_EOI_VALUE);

#define PIC_INIT_START			0x11
#define PIC_MASTER_VECTOR		0x20
#define PIC_SLAVE_VECTOR		0x28
#define PIC_MASTER_EDGE			0x04
#define PIC_SLAVE_EDGE			0x02
#define PIC_SELECT_8086			0xA1

#define PIC_MASTER_COMMAND_PORT	0x20
#define PIC_MASTER_DATA_PORT	0x21
#define PIC_SLAVE_COMMAND_PORT	0xA0
#define PIC_SLAVE_DATA_PORT		0xA1

#define PIC_EOI_VALUE			0x20

extern "C" void AsmSoftwareContextSwitchByInterrupt();
extern "C" void AsmSoftwareContextSwitchBySystemCall();

#pragma pack(push, 1)
class CInterruptHandler_IDT
{
public:
	unsigned short OffsetLow;
	unsigned short Selector;
	unsigned char Zero1;
	unsigned char Type;
	unsigned short OffsetMid;
	unsigned int OffsetHigh;
	unsigned int Zero2;

	static CInterruptHandler_IDT Handlers[MAX_IDT];
};
#pragma pack(pop)

#pragma pack(push, 1)
class CInterruptHandler
{
public:
	unsigned char Number;
	void(*Func)();
	unsigned short Type;

	static CInterruptHandler Handlers[];
	static void DivideByZero();
	static void DebugException();
	static void NMI();
	static void Breakpoint();
	static void INTO();
	static void BOUNDS();
	static void InvaildOpcode();
	static void DeviceNotAvailable();
	static void DoubleFault();
	static void CoprocessorOverrun();
	static void InvalidTSS();
	static void SegmentNotPresent();
	static void StackException();
	static void GeneralProtection();
	static void PageFault();
	static void FloatingPointError();
	static void AlignmentCheck();
	static void MachineCheck();

	static void TMR_TSS_SEG();

	static void Keyboard();
	static void ReqFromSlave8259();
	static void COM2();
	static void COM1();
	static void LPT2();
	static void FloppyDisk();
	static void LPT1();
	static void CMOSClock();
	static void VGA();
	static void Mouse();
	static void MathCoprocessor();
	static void HardDrive();
};
#pragma pack(pop)

#pragma pack(push, 1)
class CInterruptHandlerDescriptor {
public:
	unsigned short Size;
	CInterruptHandler_IDT* Address;
};
#pragma pack(pop)
