bits 16
org 0x7C00

%define LOAD_SEGMENT	0x1000
%define FAT_SEGMENT		0x0ee0

main:
    jmp		start

%include "Macros.inc"
%include "Functions.inc"

start:
	cli
	xor		ax, ax
	mov		ds, ax
	mov		es, ax
	mov		ss, ax
	mov		sp, 0x7C00
	sti

	xor  ax, ax
	int  0x13
	jc   bootFailure
	
	mResetDiskSystem

	mov		si, loadmsg
	call	WriteString

	mFindFile filename, LOAD_SEGMENT
	mReadFAT FAT_SEGMENT
	mReadFile LOAD_SEGMENT, FAT_SEGMENT

    jmp     LOAD_SEGMENT:0

bootFailure:
	cli
	hlt

filename	db "BOOTLDR2BIN"
rebootmsg	db "Press any key to reboot."
diskerror	db "Disk error. "
loadmsg		db "Loading DevOS..."

root_strt	dw 0
root_scts	dw 0
file_strt	dw 0

times 510 - ($ - $$) db 0
dw 0xAA55