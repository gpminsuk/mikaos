bits 16

extern start

main:
	jmp		AsmStart

gdtptr:
	dw	gdt_end - gdt - 1
	dd  gdt
gdt:
	dq	0x0

	dw	0x0000
	dw	0x0000
	db	0x00
	db	0x98
	db	0x20
	db	0x00
		
	dw	0x0000
	dw	0x0000
	db	0x00
	db	0x92
	db	0x00
	db	0x00

	dw	0xffff
	dw	0x0000
	db	0x00
	db	0x98
	db	0x02
	db	0x00

	dw	0xffff
	dw	0x0000
	db	0x00
	db	0x92
	db	0x02
	db	0x00
gdt_end:
		
AsmStart:
	mov		ax, 0x0
	mov		es, ax

	mov		ax, 0x1000
	mov		ds, ax

	mov		edi, 0x1000
	
	; Clear 16KiB for Paging
    push	edi
    mov		ecx, 0x1000
    xor		eax, eax
    rep		stosd
    pop		edi
	
	; Build PML4
    lea		eax, [di + 0x1000]
    or		eax, 0x03
    mov		[es:di], eax

	; Build Page Pointer Table
    lea		eax, [di + 0x2000]
    or		eax, 0x03
    mov		[es:di + 0x1000], eax

	; Build Page Directory
    lea		eax, [di + 0x3000]
    or		eax, 0x03
    mov		[es:di + 0x2000], eax

	; Build Page Table
    push	edi
    lea		di, [di + 0x3000]
    mov		eax, 0x03
	LoopPageTable:
		mov		[es:di], eax
		add		eax, 0x1000
		add		di, 8
		cmp		eax, 0x200000
		jb		LoopPageTable 
    pop		edi
		   
    mov al, 0xFF
    out 0xA1, al
    out 0x21, al
	
	sidt	[es:0x8000]
	sgdt	[es:0x8010]

	lgdt	[gdtptr]
	
	nop

	; Enable PAE, PGE
	mov		eax, 0b10100000
    mov		cr4, eax

	; Set page table
    mov		edx, edi
    mov		cr3, edx
	
	; Enable Long-mode in MSR
	mov		ecx, 0xC0000080
	rdmsr 
	or		eax, 0x00000100
	wrmsr
	
	; Enable protected mode and paging 	
	mov		ebx, cr0
	or		ebx, 0x80000001
	mov		cr0, ebx
	
	;jmp		0x0008:LongMode
	db 0x66
	db 0xEA
	dd LongMode
	dw 0x0008

bits 64
LongMode:
	xor		ax, ax
	mov		ds, ax
	mov		es, ax
	call	start