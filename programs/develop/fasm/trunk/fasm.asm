;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                 ;;
;;  flat assembler source                          ;;
;;  Copyright (c) 1999-2006, Tomasz Grysztar       ;;
;;  All rights reserved.                           ;;
;;                                                 ;;
;;  Menuet port by VT                              ;;
;;                                                 ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

NORMAL_MODE    = 8
CONSOLE_MODE   = 32

MAGIC1	       = 6*(text.line_size-1)+14
MAX_PATH       = 100

APP_MEMORY     = 0x00800000

;; Menuet header

appname equ "flat assembler "

use32

  org 0x0
  db 'MENUET01'  ; 8 byte id
  dd 0x01	 ; header version
  dd START	 ; program start
  dd program_end ; program image size
  dd stacktop	 ; required amount of memory
  dd stacktop	 ; stack
  dd params,cur_dir_path  ; parameters,icon

include 'lang.inc'
include '../../../macros.inc'
purge add,sub	 ; macros.inc does incorrect substitution
include 'fasm.inc'

include '../../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../../develop/libraries/box_lib/load_lib.mac'
  @use_library



center fix true

START:	    ; Start of execution
	mov	edi, fileinfos
	mov	ecx, (fileinfos_end-fileinfos)/4
	or	eax, -1
	rep	stosd
	push	68
	pop	eax
	push	11
	pop	ebx
	mcall

   cmp	  [params],0
   jz	    start_1

   mov	  ecx,10
   mov	  eax,'    '
   mov	  edi,infile
   push   ecx
   cld
   rep	  stosd
   mov	  ecx,[esp]
   mov	  edi,outfile
   rep	  stosd
   pop	  ecx
   mov	  edi,path
   rep	  stosd

   mov	   esi,params
;  DEBUGF  "params: %s\n",esi
   mov	   edi,infile
   call    mov_param_str
;  mov     edi,infile
;  DEBUGF  " input: %s\n",edi
   mov	   edi,outfile
   call    mov_param_str
;  mov     edi,outfile
;  DEBUGF  "output: %s\n",edi
   mov	   edi,path
   call    mov_param_str
;  mov     edi,path
;  DEBUGF  "  path: %s\n",edi
   dec     esi
   cmp	   [esi], dword ',run'
   jne	   @f
   mov	   [_run_outfile],1
  @@:

   mov	   [_mode],CONSOLE_MODE
   jmp	   start

start_1:
sys_load_library  library_name, cur_dir_path, library_path, system_path, \
  err_message_found_lib, head_f_l, myimport, err_message_import, head_f_i

  cmp eax,-1
  jne @f
    mcall -1 ;exit if not open box_lib.obj
  @@:
  mcall 40,0x27 ;��᪠ ��⥬��� ᮡ�⨩

  get_sys_colors 1,0
  edit_boxes_set_sys_color edit1,editboxes_end,sc
  check_boxes_set_sys_color ch1_dbg,ch1_dbg+ch_struc_size,sc

red:	; Redraw
    call draw_window

still:	
    push 10	     ; Wait here for event
    pop eax 
    mcall
    cmp al,6
    je  call_mouse
    dec eax 
    je	red	     ; Redraw request
    dec eax 
    jne button	     ; Button in buffer

key:		     ; Key
    mov  al,2	     ; Read it and ignore
    mcall

    push dword edit1
    call [edit_box_key]
    push dword edit2
    call [edit_box_key]
    push dword edit3
    call [edit_box_key]

    jmp  still

call_mouse:
    call mouse
    jmp  still

button:    ; Button in Window

    mov  al,17
    mcall

    cmp     ah,1
    jne     noclose
    or	    eax,-1
    mcall

noclose:    
    cmp  ah,2	      ; Start compiling
    je	 start
    cmp  ah,3	      ; Start compiled file
    jnz  norunout

    mov  edx,outfile
    call make_fullpaths
    mcall  70,file_info_start
;   xor   ecx,ecx
    jmp  still
   norunout:
    cmp  ah,4
    jnz  norundebug

    mov  edx,outfile
    call make_fullpaths
    mcall 70,file_info_debug
    jmp  still

   norundebug:

    jmp  still


mouse:
  push dword edit1
  call [edit_box_mouse]
  push dword edit2
  call [edit_box_mouse]
  push dword edit3
  call [edit_box_mouse]
  push dword ch1_dbg
  call [check_box_mouse]
  ret

draw_window:
    pusha

    mcall  12,1 ; Start of draw

    ;get_sys_colors 1,0

    xor  eax,eax		     
    mov  ebx,100*65536+280
    mov  ecx,90*65536+260
    mov  edx,[sc.work]
    or	 edx,0x33000000
    mov  edi,title	       ; Draw Window Label Text
    mcall

    mcall 9,PROCESSINFO,-1	    

    cmp dword[pinfo.box.width],230 ; ��������� ������ ����
    jge @f
      mov dword[pinfo.box.width],230 ; ���� ���� ����� �����, ����������� ������ ��� ��������� ������
    @@:

    mpack ecx,1,1
    mov   ebx,[pinfo.box.width]
    sub   ebx,10

    mov eax,8
    mov edx,0x4000000B
    mpack ebx,[pinfo.box.width],MAGIC1
    msub  ebx,MAGIC1+10+1,0
    mpack ecx,0, (14*3+16)/3-1
    madd  ecx,1,0
    mcall  ,,,0x00000002,[sc.work_button]
    madd  ecx, (14*3+16)/3+1,0
    mcall  ,,,0x00000003
    madd  ecx, (14*3+16)/3+1,0
    mcall ,,,4

    mpack ebx,6,0    ; Draw Window Text
    add  ebx,1+ 14/2-3
    mov  ecx,[sc.work_text]
    mov  edx,text
    mov  esi,text.line_size
    mov  eax,4
   newline:
    mcall
    add  ebx, 16 ;14
    add  edx,text.line_size
    cmp  byte[edx],'x'
    jne  newline

    mov   ebx,[pinfo.box.width]
    sub   ebx,MAGIC1+10+1-9
    shl   ebx,16
    add   ebx,1+( (14*3+16)/3-1)/2-3
    mcall  ,,[sc.work_button_text],s_compile,7
    add   ebx,(14*3+16)/3+1
    mcall ,,,s_run
    add   ebx,(14*3+16)/3+1
    mcall ,,,s_debug

    mpack ebx,MAGIC1+6,0
    add   ebx,1+ 14/2-3+ 14*0
    mov   esi,[pinfo.box.width]
    sub   esi,MAGIC1*2+5*2+6+3
    mov   eax,esi
    mov   cl,6
    div   cl
    cmp   al,MAX_PATH
    jbe   @f
    mov   al,MAX_PATH
@@: movzx esi,al

    call draw_messages

    mov eax,dword[pinfo.box.width]
    sub eax,127
    mov dword[edit1.width],eax ; ������������� ������ ��������� �����
    mov dword[edit2.width],eax
    mov dword[edit3.width],eax

    push dword edit1
    call [edit_box_draw]
    push dword edit2
    call [edit_box_draw]
    push dword edit3
    call [edit_box_draw]
    push dword ch1_dbg
    call [check_box_draw]

    mcall  12,2 ; End of Draw

    popa
    ret

bottom_right dd ?

draw_messages:
    mov    eax,13      ; clear work area
    mpack  ebx,7-2,[pinfo.box.width]
    sub    ebx,5*2+7*2-1-2*2
    mpack  ecx,0,[pinfo.box.height]
    madd   ecx, 14*3+16+1+7+1,-( 14*3+16+1+7*2+25)
    mov    word[bottom_right+2],bx
    mov    word[bottom_right],cx
    msub   [bottom_right],7,11
    add    [bottom_right],7 shl 16 + 53
    mov    edx,[sc.work]
    mcall
_cy = 0
_sy = 2
_cx = 4
_sx = 6
    push   ebx ecx
    mpack  ebx,4,5
    add    bx,[esp+_cx]
    mov    ecx,[esp+_sy-2]
    mov    cx,[esp+_sy]
    msub   ecx,1,1
    mcall  38,,,[sc.work_graph]
    mov    si,[esp+_cy]
    add    cx,si
    shl    esi,16
    add    ecx,esi
    madd   ecx,1,1
    mcall
    mpack  ebx,4,4
    mov    esi,[esp+_sy-2]
    mov    si,cx
    mov    ecx,esi
    mcall
    mov    si,[esp+_cx]
    add    bx,si
    shl    esi,16
    add    ebx,esi
    madd   ebx,1,1
    mcall
    pop    ecx ebx
    ret


; DATA

if lang eq ru
text:
  db ' �唠��:'
.line_size = $-text
  db '��唠��:'
  db '   ����:'
  db 'x'

  s_compile db '������.'
  s_run     db ' ���  '
  s_debug   db '�⫠���'
  s_dbgdescr db '��������� �⫠����� ���ଠ��',0
  s_dbgdescr_end:

  err_message_import db '�訡�� �� ������ box_lib.obj',0
  err_message_found_lib db '�訡�� �� ���᪥ box_lib.obj',0 ;��ப�, ����� �㤥� � ��ନ஢����� ����, �᫨ ������⥪� �� �㤥� �������
  head_f_i: 
  head_f_l db '���⥬��� �訡��',0 ;��������� ����, �� ������������� �訡��
  system_path db '/sys/lib/'
  library_name db 'box_lib.obj',0
else
text:
  db ' INFILE:'
.line_size = $-text
  db 'OUTFILE:'
  db '   PATH:'
  db 'x'

  s_compile db 'COMPILE'
  s_run     db '  RUN  '
  s_debug   db ' DEBUG '
  s_dbgdescr db 'Generate debug information',0
  s_dbgdescr_end:

  err_message_import db 'Error on load import library box_lib.obj',0
  err_message_found_lib db 'Sorry I cannot found library box_lib.obj',0 ;��ப�, ����� �㤥� � ��ନ஢����� ����, �᫨ ������⥪� �� �㤥� �������
  head_f_i: 
  head_f_l db 'System error',0 ;��������� ����, �� ������������� �訡��
  system_path db '/sys/lib/'
  library_name db 'box_lib.obj',0
end if

myimport:
  edit_box_draw  dd aEdit_box_draw
  edit_box_key	 dd aEdit_box_key
  edit_box_mouse dd aEdit_box_mouse
  ;version_ed     dd aVersion_ed

  check_box_draw  dd aCheck_box_draw
  check_box_mouse dd aCheck_box_mouse
  ;version_ch      dd aVersion_ch

  dd 0,0

  aEdit_box_draw  db 'edit_box',0
  aEdit_box_key   db 'edit_box_key',0
  aEdit_box_mouse db 'edit_box_mouse',0
  ;aVersion_ed     db 'version_ed',0

  aCheck_box_draw  db 'check_box_draw',0
  aCheck_box_mouse db 'check_box_mouse',0
  ;aVersion_ch      db 'version_ch',0

edit1 edit_box 153, 56, 1, 0xffffff, 0xff, 0x80ff, 0, 0x8000, (outfile-infile-1), infile, mouse_dd, 0, 11,11
edit2 edit_box 153, 56, 17, 0xffffff, 0xff, 0x80ff, 0, 0x8000,(path-outfile-1), outfile, mouse_dd, 0, 7,7
edit3 edit_box 153, 56, 33, 0xffffff, 0xff, 0x80ff, 0, 0x8000,(path_end-path-1), path, mouse_dd, 0, 6,6
editboxes_end:
ch1_dbg check_box 5, 49, 6, 12, 0xffffff, 0x80ff, 0, s_dbgdescr,(s_dbgdescr_end-s_dbgdescr)

mouse_dd dd 0 ;����� ��� Shift-� � editbox

infile	  db 'example.asm'
  times MAX_PATH-$+infile  db 0
outfile db 'example'
  times MAX_PATH-$+outfile db 0
path	db '/rd/1/'
  times MAX_PATH-$+path    db 0
path_end:
lf db 13,10,0


mov_param_str:
    cld
@@:
    lodsb
    cmp    al,','
    je     @f
    stosb
    test   al,al
    jnz    @b
@@:
    xor    al,al
    stosb
    ret



start:
    cmp    [_mode],NORMAL_MODE
    jne    @f
    call   draw_messages
    mov    [textxy],7 shl 16 + 70
@@:
    mov    esi,_logo
    call   display_string

 ;
 ;   Fasm native code
 ;

    mov    [input_file],infile
    mov    [output_file],outfile

    call   init_memory

    call   make_timestamp
    mov    [start_time],eax

    call   preprocessor
    call   parser
    call   assembler
    bt	   dword[ch1_dbg.flags],1 ;cmp [bGenerateDebugInfo], 0
    jae    @f			  ;jz @f
    call   symbol_dump
@@:
    call   formatter

    call   display_user_messages
    movzx  eax,[current_pass]
    inc    eax
    call   display_number
    mov    esi,_passes_suffix
    call   display_string
    call   make_timestamp
    sub    eax,[start_time]
    xor    edx,edx
    mov    ebx,100
    div    ebx
    or	     eax,eax
    jz	     display_bytes_count
    xor    edx,edx
    mov    ebx,10
    div    ebx
    push   edx
    call   display_number
    mov    dl,'.'
    call   display_character
    pop    eax
    call   display_number
    mov    esi,_seconds_suffix
    call   display_string
  display_bytes_count:
    mov    eax,[written_size]
    call   display_number
    mov    esi,_bytes_suffix
    call   display_string
    xor    al,al

    cmp    [_run_outfile],0
    je	   @f
    mov    edx,outfile
    call   make_fullpaths
    mov    eax,70
    mov    ebx,file_info_start
    xor    ecx,ecx
    mcall
@@:
    jmp    exit_program


include 'system.inc'
include 'version.inc'
include 'errors.inc'
include 'expressi.inc'
include 'preproce.inc'
include 'parser.inc'
include 'assemble.inc'
include 'formats.inc'
include 'x86_64.inc'
include 'tables.inc'
include 'symbdump.inc'
include 'messages.inc'

title db appname,VERSION_STRING,0

_logo db 'flat assembler  version ',VERSION_STRING,13,10,0

_passes_suffix db ' passes, ',0
_seconds_suffix db ' seconds, ',0
_bytes_suffix db ' bytes.',13,10,0

_include db 'INCLUDE',0

_counter db 4,'0000'

_mode	       dd NORMAL_MODE
_run_outfile  dd 0
;bGenerateDebugInfo db 0

sub_table:
times $41 db $00
times $1A db $20
times $25 db $00
times $10 db $20
times $30 db $00
times $10 db $50
times $04 db $00,$01
times $08 db $00

;include_debug_strings
program_end:
;  params db 0 ; 'TINYPAD.ASM,TINYPAD,/HD/1/TPAD4/',
params	rb 4096
cur_dir_path rb 4096
library_path rb 4096

align 4

include 'variable.inc'

program_base dd ?
buffer_address dd ?
memory_setting dd ?
start_time dd ?
memblock	dd	?

predefinitions rb 1000h

dbgfilename	rb	MAX_PATH+4

sc    system_colors
max_handles = 8
fileinfos rb (4+20+MAX_PATH)*max_handles
fileinfos_end:
pinfo process_information

align 1000h
rb 1000h
stacktop:
