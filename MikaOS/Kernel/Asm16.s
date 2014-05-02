bits 16

global BiosInterrupt_CompatiabilityMode
BiosInterrupt_CompatiabilityMode:
	mov		ax, 0x0030
	mov		es, ax
	mov		ds, ax

	sidt	[0x8020]
	sgdt	[0x8030]

	mov		eax, cr0
    and		eax, 0x7FFFFFFE
    mov		cr0, eax
	
	mov		eax, BiosInterrupt_RealMode
	
	push	0x1000
	push	ax
	retf

BiosInterrupt_RealMode:
	xor		ax, ax
	mov		ss, ax
	mov		ds, ax

	lidt	[0x8000]
	lgdt	[0x8010]
	
	call	cx
	
	mov		eax, cr0
	or		eax, 0x80000001
	mov		cr0, eax

	lidt	[0x8020]
	lgdt	[0x8030]

	db 0x66
	db 0xEA
	dd ReturnToLongMode
	dw 0x0008
	
global VbeGetModeInfo
VbeGetModeInfo:
	mov		ax, 0x0000
	mov		es, ax
	mov		ax, 0x8300
	mov		di, ax
	mov		ax, 0x4F01
	mov		cx, 0x0118
	int		0x10
	ret
	
global VbeSetBank
VbeSetBank:
	mov		ax, 0x4F05
	mov		bx, 0x0
	int		0x10
	ret

global VbeGetControllerInfo
VbeGetControllerInfo:
	mov		ax, 0x0000
	mov		es, ax
	mov		ax, 0x8100
	mov		di, ax
	mov		ax, 0x4F00
	int		0x10
	mov		ax, 0x4F02
	mov		bx, 0x0118
	int		0x10
	ret

bits 64
ReturnToLongMode:
	mov		rax, 0x10
	mov		ss, rax
	
	pop		rsp
	ret