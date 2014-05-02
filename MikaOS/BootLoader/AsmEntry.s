bits 64

extern start

LongMode:
	call	start

global AsmHalt
AsmHalt:
	cli
	hlt
	
global AsmIn
AsmIn:
	push	rdx

	mov		rdx, rcx
	in		al, dx
	
	pop		rdx
	ret
	
global AsmOut
AsmOut:
	push	rdx
	push	rax

    mov		al, dl
	mov		rdx, rcx
	out		dx, al
	
	pop		rax
	pop		rdx
	ret

global AsmLIDT
AsmLIDT:
	lidt	[rcx]
	ret

global AsmLGDT
AsmLGDT:
	lgdt	[rcx]
	ret	
	
global AsmLTR
AsmLTR:
	ltr		cx
	ret

%macro ReadTaskFrame 0
	pop		rax
	mov		cr3, rax

	pop		rsi
	pop		rdi
	popfq
	pop		rdx
	pop		rcx	
	pop		rbx
	pop		rax
	
	pop		rbp
%endmacro

global AsmFirstSoftwareContextSwitch
AsmFirstSoftwareContextSwitch:
	mov		rsp, [rcx]

	ReadTaskFrame
	iretq

global AsmSoftwareContextSwitchByInterrupt
AsmSoftwareContextSwitchByInterrupt:
	cli
	hlt

global AsmSoftwareContextSwitchBySystemCall
AsmSoftwareContextSwitchBySystemCall:
	cli
	hlt