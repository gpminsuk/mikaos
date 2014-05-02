bits 64

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

extern BiosInterrupt_CompatiabilityMode

global BiosInterrupt:function
BiosInterrupt:
	mov		[0x7BF8], rsp
	mov		rax, 0x7BF8
	mov		rsp, rax

	push	qword 0x30
	push	qword 0x7BF8
	pushfq
	push	qword 0x28
	push	qword BiosInterrupt_CompatiabilityMode
	iretq
	
global AsmIntSoftwareContextSwitch
AsmIntSoftwareContextSwitch:
	int		0x30
	ret

%macro WriteTaskFrame 1
	push	qword %1

	push	rax
	push	rbx
	push	rcx
	push	rdx
	pushfq
	push	rdi
	push	rsi

	mov		rax, cr3	; paging register
	push	rax		
%endmacro

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

global AsmInitializeProcess
AsmInitializeProcess:
	push	rbp

	mov		rbp, rsp
	mov		rsp, [rdx]
	
	; iretq
	push	qword [rdx + 16]	; stack segment
	push	qword [rdx]		; stack pointer
	pushfq				; EFLAGS
	push	qword [rdx + 8]	; code segment
	push	qword rcx			; instruction pointer
	
	WriteTaskFrame [rdx]
	
	mov		[rdx], rsp
	mov		rsp, rbp

	pop		rbp
	ret
	
extern GCurrentTask
extern NextTaskRing
%macro SoftwareContextSwitch 0
	WriteTaskFrame rbp
	
	mov		rax, GCurrentTask
	mov		rax, [rax]
	mov		[rax], rsp
	call	NextTaskRing
	mov		rax, GCurrentTask
	mov		rax, [rax]
	mov		rsp, [rax]

	ReadTaskFrame
%endmacro

global AsmSoftwareContextSwitchByInterrupt
AsmSoftwareContextSwitchByInterrupt:
	mov		rax, GCurrentTask
	cmp		qword [rax], 0
	je		Skip_AsmSoftwareContextSwitchByInterrupt

	SoftwareContextSwitch
	
	Skip_AsmSoftwareContextSwitchByInterrupt:

	push	rax
	push	rdx
	mov		al, 0x20
	mov		dx, 0x20
	out		dx, al		; EOI_MASTER()
	pop		rdx
	pop		rax

	iretq
	
global AsmSoftwareContextSwitchBySystemCall
AsmSoftwareContextSwitchBySystemCall:
	mov		rax, GCurrentTask
	cmp		qword [rax], 0
	je		Skip_AsmSoftwareContextSwitchBySystemCall

	SoftwareContextSwitch

	Skip_AsmSoftwareContextSwitchBySystemCall:
	iretq