WriteString:
  lodsb
  or     al, al
  jz     WriteString_done
  
  mov    ah, 0xe
  mov    bx, 9
  int    0x10
  
  jmp    WriteString
WriteString_done:
  ret

ReadSector:
  xor     cx, cx

 readsect:
  push    ax
  push    cx
  push    bx
   
  ; Calculate cylinder, head and sector:
  ; Cylinder = (LBA / SectorsPerTrack) / NumHeads
  ; Sector   = (LBA mod SectorsPerTrack) + 1
  ; Head     = (LBA / SectorsPerTrack) mod NumHeads
    
  mov     bx, word [iTrackSect]
  xor     dx, dx
  div     bx
  inc     dx
  mov     cl, dl
  
  mov     bx, word [iHeadCnt]
  xor     dx, dx
  div     bx
  mov     ch, al
  xchg    dl, dh
  
  mov     ax, 0x0201
  mov     dl, byte [iBootDrive]
  pop     bx
  int     0x13
  jc      readfail
  
  pop     cx
  pop     ax
  ret

readfail:   
  pop     cx
  inc     cx
  cmp     cx, 4
  je      bootFailure

  xor     ax, ax
  int     0x13
  
  pop     ax
  jmp     readsect
