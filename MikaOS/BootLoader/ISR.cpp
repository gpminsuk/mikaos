#include "ISR.h"
#include "CDisplayDriver.h"

static unsigned char m_AsciiCode[5][128] = {
	{ /* ascii table */
		0x00, 0x1b, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, 0x09,
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0x0d, 0x00, 'a', 's',
		'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 0x27, '`', 0x00, 0x5c, 'z', 'x', 'c', 'v',
		'b', 'n', 'm', ',', '.', '/', 0x00, '*', 0x00, ' ', 0x00, 0x03, 0x03, 0x03, 0x03, 0x08,
		0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, '-', 0x00, 0x00, 0x00, '+', 0x00,
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x5c, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x0d, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	{ /* ascii table with capslock */
		0x00, 0x1b, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08, 0x09,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', 0x0d, 0x00, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', 0x27, '`', 0x00, 0x5c, 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', ',', '.', '/', 0x00, '*', 0x00, ' ', 0x00, 0x03, 0x03, 0x03, 0x03, 0x08,
		0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, '-', 0x00, 0x00, 0x00, '+', 0x00,
		0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x5c, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x0d, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	{ /* ascii with shift */
		0x00, 0x1b, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x7e, 0x7e,
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0x7e, 0x00, 'A', 'S',
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 0x27, '~', 0x00, '|', 'Z', 'X', 'C', 'V',
		'B', 'N', 'M', '<', '>', '?', 0x00, '*', 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, '-', 0x00, 0x00, 0x00, '+', 0x00,
		0x00, 0x00, 0x01, 0x7f, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x0d, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	{ /* ascii with capslock & shift */
		0x00, 0x1b, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x7e, 0x7e,
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', 0x7e, 0x00, 'a', 's',
		'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', 0x27, '~', 0x00, '|', 'z', 'x', 'c', 'v',
		'b', 'n', 'm', '<', '>', '?', 0x00, '*', 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, '-', 0x00, 0x00, 0x00, '+', 0x00,
		0x00, 0x00, 0x01, 0x7f, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x0d, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, '/', 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	{ /* ascii with control */
		0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x7f, 0x7f,
		0x11, 0x17, 0x05, 0x12, 0x14, 0x19, 0x15, 0x09, 0x0f, 0x10, 0x02, 0x02, 0x0a, 0x00, 0x01, 0x13,
		0x04, 0x06, 0x07, 0x08, 0x0a, 0x0b, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x18, 0x03, 0x16,
		0x02, 0x0e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	}
};

CInterruptHandler_IDT CInterruptHandler_IDT::Handlers[MAX_IDT];
CInterruptHandler CInterruptHandler::Handlers[] = {
	{ 0x00, CInterruptHandler::DivideByZero, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x01, CInterruptHandler::DebugException, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x02, CInterruptHandler::NMI, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x03, CInterruptHandler::Breakpoint, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x04, CInterruptHandler::INTO, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x05, CInterruptHandler::BOUNDS, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x06, CInterruptHandler::InvaildOpcode, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x07, CInterruptHandler::DeviceNotAvailable, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x08, CInterruptHandler::DoubleFault, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x09, CInterruptHandler::CoprocessorOverrun, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x0a, CInterruptHandler::InvalidTSS, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x0b, CInterruptHandler::SegmentNotPresent, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x0c, CInterruptHandler::StackException, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x0d, CInterruptHandler::GeneralProtection, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x0e, CInterruptHandler::PageFault, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x10, CInterruptHandler::FloatingPointError, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x11, CInterruptHandler::AlignmentCheck, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x12, CInterruptHandler::MachineCheck, IDTC_INTGATE | IDTC_PRESENT },

	{ 0x20, AsmSoftwareContextSwitchByInterrupt, IDTC_INTGATE | IDTC_PRESENT },

	{ 0x21, CInterruptHandler::Keyboard, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x22, CInterruptHandler::ReqFromSlave8259, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x23, CInterruptHandler::COM2, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x24, CInterruptHandler::COM1, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x25, CInterruptHandler::LPT2, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x26, CInterruptHandler::FloppyDisk, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x27, CInterruptHandler::LPT1, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x28, CInterruptHandler::CMOSClock, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x29, CInterruptHandler::VGA, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x2c, CInterruptHandler::Mouse, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x2d, CInterruptHandler::MathCoprocessor, IDTC_INTGATE | IDTC_PRESENT },
	{ 0x2e, CInterruptHandler::HardDrive, IDTC_INTGATE | IDTC_PRESENT },

	{ 0x30, AsmSoftwareContextSwitchBySystemCall, IDTC_INTGATE | IDTC_PRESENT | IDTC_DPL1 },
	{ 0, 0, 0 }
};

void CInterruptHandler::DivideByZero()
{
	//GShell->PrintString("DivideByZero");
	ASM_IRETQ();
}
void CInterruptHandler::DebugException()
{
	//GShell->PrintString("DebugException");
	ASM_IRETQ();
}
void CInterruptHandler::NMI()
{
	//GShell->PrintString("NMI");
	ASM_IRETQ();
}
void CInterruptHandler::Breakpoint()
{
	//GShell->PrintString("Breakpoint");
	ASM_IRETQ();
}
void CInterruptHandler::INTO()
{
	//GShell->PrintString("INTO");
	ASM_IRETQ();
}
void CInterruptHandler::BOUNDS()
{
	//GShell->PrintString("BOUNDS");
	ASM_IRETQ();
}
void CInterruptHandler::InvaildOpcode()
{
	//GShell->PrintString("InvaildOpcode");
	ASM_IRETQ();
}
void CInterruptHandler::DeviceNotAvailable()
{
	//GShell->PrintString("DeviceNotAvailable");
	ASM_IRETQ();
}
void CInterruptHandler::DoubleFault()
{
	//GShell->PrintString("DoubleFault");
	ASM_IRETQ();
}
void CInterruptHandler::CoprocessorOverrun()
{
	//GShell->PrintString("CoprocessorOverrun");
	ASM_IRETQ();
}
void CInterruptHandler::InvalidTSS()
{
	//GShell->PrintString("InvalidTSS");
	ASM_IRETQ();
}
void CInterruptHandler::SegmentNotPresent()
{
	//GShell->PrintString("SegmentNotPresent");
	ASM_IRETQ();
}
void CInterruptHandler::StackException()
{
	//GShell->PrintString("StackException");
	ASM_IRETQ();
}
void CInterruptHandler::GeneralProtection()
{
	//AsmHalt();
	ASM_IRETQ();
}
void CInterruptHandler::PageFault()
{
	AsmHalt();
	//GShell->PrintString("PageFault");
	ASM_IRETQ();
}
void CInterruptHandler::FloatingPointError()
{
	//GShell->PrintString("FloatingPointError");
	ASM_IRETQ();
}
void CInterruptHandler::AlignmentCheck()
{
	//GShell->PrintString("AlignmentCheck");
	ASM_IRETQ();
}
void CInterruptHandler::MachineCheck()
{
	//GShell->PrintString("MachineCheck");
	ASM_IRETQ();
}

void CInterruptHandler::TMR_TSS_SEG()
{
	/*
	register char* Stack __asm("%rsp");
	__asm("push		%rbp");

	__asm("push		%rax");
	__asm("push		%rbx");
	__asm("push		%rcx");
	__asm("push		%rdx");
	__asm("pushfq");

	__asm("push		%rdi");
	__asm("push		%rsi");

	__asm("movq		%cr3, %rax");
	__asm("push		%rax");

	GProcessScheduler.Current->Process->rsp = Stack;
	Stack = GProcessScheduler.Next->Process->rsp;

	__asm("pop		%rax");
	__asm("movq		%rax, %cr3");

	__asm("pop		%rsi");
	__asm("pop		%rdi");
	__asm("popfq");
	__asm("pop		%rdx");
	__asm("pop		%rcx");
	__asm("pop		%rbx");
	__asm("pop		%rax");

	__asm("pop		%rbp");

	GProcessScheduler.ScheduleNext();*/

	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::Keyboard()
{
	static bool shift_key_pressed;
	unsigned char keyCode = AsmIn(0x60);

	if (keyCode & 0x80) {
		if (keyCode == 0xaa || keyCode == 0xb6)
			shift_key_pressed = 0;
		/*else if (keyCode == 0xb8)
			alt_key_pressed = 0;
		else if (keyCode == 0x9d)
			ctrl_key_pressed = 0;*/
	}

	if (keyCode == 0x2a || keyCode == 0x36)
	{
		shift_key_pressed = 1;
	}
	else if (keyCode == 0x1d)
	{
		//ctrl_key_pressed = 1;
	}
	else if (keyCode == 0x38)
	{
		//alt_key_pressed = 1;
	}
	unsigned char key_family = 0;
	key_family |= (shift_key_pressed ? 0x02 : 0);
	//GShell->PrintString("%c ", m_AsciiCode[key_family][keyCode]);
	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::ReqFromSlave8259()
{
	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::COM2()
{
	//GShell->PrintString("COM2");
	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::COM1()
{
	//GShell->PrintString("COM1");
	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::LPT2()
{
	//GShell->PrintString("LPT2");
	EOI_MASTER();
	ASM_IRETQ();
}
extern void Fdd_IRQ_Handler();
void CInterruptHandler::FloppyDisk()
{
	//CDisplayDriver::PrintString("FloppyDisk");
	Fdd_IRQ_Handler();
	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::LPT1()
{
	//GShell->PrintString("LPT1");
	EOI_MASTER();
	ASM_IRETQ();
}
void CInterruptHandler::CMOSClock()
{
	//GShell->PrintString("CMOSClock");
	EOI_SLAVE();
	ASM_IRETQ();
}
void CInterruptHandler::VGA()
{
	//GShell->PrintString("VGA");
	EOI_SLAVE();
	ASM_IRETQ();
}
void CInterruptHandler::Mouse()
{
	//GShell->PrintString("Mouse");
	EOI_SLAVE();
	ASM_IRETQ();
}
void CInterruptHandler::MathCoprocessor()
{
	//GShell->PrintString("MathCoprocessor");
	EOI_SLAVE();
	ASM_IRETQ();
}
void CInterruptHandler::HardDrive()
{
	//GShell->PrintString("HardDrive");
	EOI_SLAVE();
	ASM_IRETQ();
}