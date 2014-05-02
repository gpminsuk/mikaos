bootsector:
 iOEM        db  "DevOS   "    ; OEM String
 iSectSize   dw  0x200         ; Bytes per sector
 iClustSize  db  1             ; Sectors per cluster
 iResSect    dw  1             ; #of reserved sectors
 iFatCnt     db  2             ; #of fat copies
 iRootSize   dw  224           ; size of root directory
 iTotalSect  dw  2880          ; total ; of sectors if below 32 MB
 iMedia      db  0xF0          ; Media Descriptor
 iFatSize    dw  9             ; Size of each FAT
 iTrackSect  dw  18            ; Sectors per track
 iHeadCnt    dw  2             ; number of read-write heads
 iHiddenSect dd  0             ; number of hidden sectors
 iSect32     dd  0             ; ; sectors if over 32 MB
 iBootDrive  db  0             ; holds drive that the boot sector came from
 iReserved   db  0             ; reserved, empty
 iBootSign   db  0x29          ; extended boot sector signature
 iVolID      db "seri"        ; disk serial
 acVolLabel  db "MYVOLUME   " ; just placeholder. We don't yet use volume labels.
 acFSType    db "FAT16   "    ; file system type

 %macro mResetDiskSystem 0
  mov  dl, byte [iBootDrive]
  xor  ax, ax
  int  0x13
  jc   bootFailure
%endmacro

%macro mFindFile 2
  mov     ax, %2
  mov     es, ax
  
  mov     ax, 32
  xor     dx, dx
  mul     word [iRootSize]
  div     word [iSectSize]
  mov     cx, ax
  mov     word [root_scts], cx

  xor   ax, ax
  mov   al, byte [iFatCnt]
  mov   bx, word [iFatSize]
  mul   bx
  add   ax, word [iHiddenSect]
  add   ax, word [iResSect]
  mov   word [root_strt], ax
  
  read_next_sector:
    push   cx
    push   ax
    xor    bx, bx
    call   ReadSector
  check_entry:
    mov    cx, 11
    mov    di, bx
    mov    si, %1
    repz   cmpsb
    je     found_file
    add    bx, 32
    cmp    bx, word [iSectSize]
    jne    check_entry
    
    pop    ax
    inc    ax
    pop    cx
    loopnz read_next_sector
    jmp    bootFailure
    
  found_file:
    mov    ax, [es:bx+0x1a]
    mov    word [file_strt], ax
%endmacro

%macro mReadFAT 1
    mov   ax, %1
    mov   es, ax
  
    mov   ax, word [iResSect]
    add   ax, word [iHiddenSect]
  
    mov   cx, word [iFatSize]
    xor   bx, bx
  read_next_fat_sector:
    push  cx
    push  ax
    call  ReadSector
    pop   ax
    pop   cx
    inc   ax
    add   bx, word [iSectSize]
    loopnz read_next_fat_sector
%endmacro

%macro mReadFile 2
    mov     ax, %1
    mov     es, ax
    xor     bx, bx
        
    mov     cx, word [file_strt]
    
  read_file_next_sector:
    mov     ax, cx
    add     ax, word [root_strt]
    add     ax, word [root_scts]
    sub     ax, 2
    
    push    cx
    call    ReadSector
    pop     cx
    add     bx, word [iSectSize]
    
    push    ds
    mov     dx, %2
    mov     ds, dx
    
    mov     si, cx
    mov     dx, cx
    shr     dx, 1
    add     si, dx
    
    mov     dx, [ds:si]
    test    dx, 1
    jz      read_next_file_even
    and     dx, 0x0fff
    jmp     read_next_file_cluster_done
read_next_file_even:    
    shr     dx, 4
read_next_file_cluster_done:    
    pop     ds
    mov     cx, dx
    cmp     cx, 0xff8
    jl      read_file_next_sector
%endmacro
