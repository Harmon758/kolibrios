;---------------------------------------------------------------------
;
;   DOCPAK FOR KOLIBRI v1.2
;   Written in pure assembly by Ivushkin Andrey aka Willow
;
;---------------------------------------------------------------------

FILE_COUNT=0
DEF_FILE equ 'g'

macro embed_file fn
{
 forward
   local label,label2,label3
   dd label2-label
   dd label-label3
   label3:
   db fn
   label:
     file '%DOCDIR%' # fn
   label2:
   FILE_COUNT=FILE_COUNT+1
}

   use32
   org    0x0
   db     'MENUET01'              ; 8 byte id
   dd     0x01                    ; header version
   dd     start                   ; start of code
   dd     I_END                   ; size of image
   dd     I_END+0x400             ; memory for app
   dd     I_END+0x400             ; esp
   dd     my_param , 0x0          ; I_Param , I_Icon
include '../../../macros.inc'
include 'lang.inc'

start:
  cmp   [my_param],0
  je    red
  cmp   [my_param],'a'
  jb    .par_a
  cmp   [my_param],'z'
  jbe   .ok2
 .par_a: 
  mov   [my_param],DEF_FILE
 .ok2:
  movzx ecx,[my_param]
  mov   [my_param],'*'
 .open:
  sub   ecx,'a'-1
  mov   edx,embedded
  xor   esi,esi
 .list: 
  lea   edx,[edx+esi+8]
  mov   esi,[edx-8]
  add   edx,[edx-4]
  test  esi,esi
  jz    .close
  loop  .list
        push    edx
; convert number in esi to decimal representation
        mov     ecx, 10
        push    -'0'
        mov     eax, esi
@@:
        xor	edx, edx
        div     ecx
        push    edx
        test    eax, eax
        jnz     @b
        mov     edi, fsize
@@:
        pop     eax
        add     al, '0'
        stosb
        jnz     @b
  mcall 70,fileinfo
  mov   ecx,eax
  mcall 5,20
  pop   edx
  mcall 60,2
  cmp   [my_param],'*'
  jne   still
 .close: 
  mcall -1

red:
  mov   [my_param],'a'
  
  mcall 48, 3, sc, sizeof.system_colors

  mcall 12,1
  
  mov  edx,[sc.work]
  or   edx,0x34000000
  mcall 0, <220,120>, <30,FILECOUNT*16+35>, , ,title

  mov   ecx,FILECOUNT
  mov   ebx,5 shl 16+100
  mov   esi,[sc.work_button]
  mov   edi,5 shl 16+14
  mov   edx,10
  mov   eax,8
 .btnlp:
  push  ecx
  mcall ,,edi
  add   edi,16 shl 16
  inc   edx
  pop   ecx
  loop  .btnlp
  mov   ecx,FILECOUNT
  mov   edx,embedded
  xor   edi,edi
  mov   ebx,25 shl 16+8
  mov   eax,4
 .list: 
  lea   edx,[edx+edi+8]
  mov   edi,[edx-8]
  pusha
  sub   ebx,15 shl 16
  mcall ,,0xff0000,my_param,1
  inc   [my_param]
  popa
  push  ecx
  mcall ,,[sc.work_button_text],,[edx-4]
  pop   ecx
  add   edx,esi
  add   ebx,16
  loop  .list
  mcall 12,2

still:
  mcall 10
  cmp   eax,1
  je    red
  cmp   eax,2
  jne   .nokey
  mcall 2
  cmp   ah,27 ;Esc
  je    start.close
  cmp   ah,'a'
  jb    still
  cmp   ah,'a'+FILECOUNT
  jae   still
  jmp   .cxax
 .nokey:
  mcall 17
  cmp   ah,1
  je    start.close
  sub   ah,10-'a'
 .cxax:
  movzx  ecx,ah
  jmp   start.open

fileinfo:
        dd      7
        dd      0
        dd      param
        dd      0 ,0
        db      '/sys/TINYPAD',0

param  db '*'
fsize:
       times 10 db '0'
       db 0

embedded: 

; Please use only filenames w/o path!

; -- Start of embedding area ------
  embed_file 'README.TXT'        ;a
if lang eq ru
  embed_file 'GNU.TXT'           ;b
else
  embed_file 'COPYING.TXT'       ;b
end if
  embed_file 'HOT_KEYS.TXT'      ;c
  embed_file 'FASM.TXT'          ;d
  embed_file 'MTDBG.TXT'         ;e
if lang eq ru
  embed_file 'SYSFUNCR.TXT'      ;f
else
  embed_file 'SYSFUNCS.TXT'      ;f
end if
  embed_file 'STACK.TXT'         ;g
  embed_file 'KFAR_KEYS.TXT'     ;h
  embed_file 'INI.TXT'           ;i
  embed_file 'OPENDIAL.TXT'      ;j
; -- End of embedding area  -------

  dd 0
FILECOUNT = FILE_COUNT

  if ~ FILECOUNT>0
    error 'No embedded files'
  end if

my_param db 0
  rb 256
I_END:

title db 'Doc Pack',0

sc     system_colors
