use32
	org 0
	db 'MENUET01'
	dd 1,start,i_end,mem,stacktop,0,cur_dir_path

include '../../../../../KOSfuncs.inc'
include '../../../../../macros.inc'
include '../../../../../proc32.inc'
include '../../../../../dll.inc'
include '../../../../../load_lib.mac'

@use_library mem.Alloc,mem.Free,mem.ReAlloc, 0 ;dll.Load

align 4
start:
	load_library lib0_name, library_path, system_path, import_buf2d_lib
	cmp eax,-1
	jz button.exit

	mcall SF_SET_EVENTS_MASK, 0x27
	stdcall [buf2d_create], buf_0 ;ᮧ���� ����
	stdcall [buf2d_line], buf_0, 110, 20, 125, 90, 0xffff00 ;��㥬 �����
	stdcall [buf2d_line], buf_0, 60, 120, 110, 20, 0xd000 ;��㥬 �����
	stdcall [buf2d_curve_bezier], buf_0, (10 shl 16)+20,(110 shl 16)+10,(50 shl 16)+90, dword 0xff
	stdcall [buf2d_circle], buf_0, 125, 90, 30, 0xffffff ;��㥬 ���㦭����
	stdcall [buf2d_circle], buf_0, 25, 70, 15, 0xff0000 ;��㥬 ���㦭����

	stdcall [buf2d_img_hdiv2], buf_0 ;ᦨ���� ����ࠦ���� � ���� �� ���� � 2 ࠧ�
	stdcall [buf2d_img_wdiv2], buf_0 ;ᦨ���� ����ࠦ���� � ���� �� �ਭ� � 2 ࠧ�
	stdcall [buf2d_create_f_img], buf_1,[buf_0] ;ᮧ���� �����쪨� ���� buf_1, �� �᭮�� ᦠ⮣� ����ࠦ���� �� buf_0

	;�᫨ �� �ᯮ�짮������ �㭪�� buf2d_create, ⮣�� ���� ����ࠦ����
	;��諮�� �� ����஢��� ᫥��騬� ��ப���:
	;movzx ecx,word[buf_1.size_x]
	;movzx eax,word[buf_1.size_y]
	;imul ecx,eax
	;imul ecx,3 ;ecx - �������⢮ ���� � �����쪮� ����
	;stdcall mem_copy, dword[buf_0],dword[buf_1],ecx

	stdcall [buf2d_clear], buf_0,0xffffff
	stdcall [buf2d_bit_blt], buf_0, 15,10, buf_1
	stdcall [buf2d_bit_blt], buf_0, 110,65, buf_1

	stdcall [buf2d_conv_24_to_8], buf_1
	;stdcall [buf2d_bit_blt_alpha], buf_0, 1,1, buf_1,0xff8080 ;��㥬 ������� ����ࠦ���� �� �⮣�䨨

align 4
red_win:
	call draw_window

align 4
still:
	mcall SF_WAIT_EVENT
	cmp al,1 ;���. ��������� ����
	jz red_win
	cmp al,2
	jz key
	cmp al,3
	jz button
	jmp still

align 4
draw_window:
	pushad
	mcall SF_REDRAW, SSF_BEGIN_DRAW

	;mov edx,0x32000000
	mov edx,0x33000000
	mcall SF_CREATE_WINDOW, (50 shl 16)+330,(30 shl 16)+275,,,caption

	stdcall [buf2d_draw], buf_0

	mcall SF_REDRAW, SSF_END_DRAW
	popad
	ret

align 4
key:
	mcall SF_GET_KEY

	cmp ah,27 ;Esc
	je button.exit

	jmp still

align 4
button:
	mcall SF_GET_BUTTON
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_0 ;㤠�塞 ����
	stdcall [buf2d_delete],buf_1 ;㤠�塞 ����
	mcall SF_TERMINATE_PROCESS

caption db 'Test buf2d library, [Esc] - exit',0

;--------------------------------------------------
align 4
import_buf2d_lib:
	dd sz_lib_init
	buf2d_create dd sz_buf2d_create
	buf2d_create_f_img dd sz_buf2d_create_f_img
	buf2d_clear dd sz_buf2d_clear
	buf2d_draw dd sz_buf2d_draw
	buf2d_delete dd sz_buf2d_delete
	buf2d_line dd sz_buf2d_line
	buf2d_circle dd sz_buf2d_circle
	buf2d_img_hdiv2 dd sz_buf2d_img_hdiv2
	buf2d_img_wdiv2 dd sz_buf2d_img_wdiv2
	buf2d_conv_24_to_8 dd sz_buf2d_conv_24_to_8
	buf2d_conv_24_to_32 dd sz_buf2d_conv_24_to_32
	buf2d_bit_blt dd sz_buf2d_bit_blt
	buf2d_bit_blt_transp dd sz_buf2d_bit_blt_transp
	buf2d_bit_blt_alpha dd sz_buf2d_bit_blt_alpha
	buf2d_curve_bezier dd sz_buf2d_curve_bezier
	buf2d_convert_text_matrix dd sz_buf2d_convert_text_matrix
	buf2d_draw_text dd sz_buf2d_draw_text
	dd 0,0
	sz_lib_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_line db 'buf2d_line',0
	sz_buf2d_circle db 'buf2d_circle',0 ;�ᮢ���� ���㦭���
	sz_buf2d_img_hdiv2 db 'buf2d_img_hdiv2',0
	sz_buf2d_img_wdiv2 db 'buf2d_img_wdiv2',0
	sz_buf2d_conv_24_to_8 db 'buf2d_conv_24_to_8',0
	sz_buf2d_conv_24_to_32 db 'buf2d_conv_24_to_32',0
	sz_buf2d_bit_blt db 'buf2d_bit_blt',0
	sz_buf2d_bit_blt_transp db 'buf2d_bit_blt_transp',0
	sz_buf2d_bit_blt_alpha db 'buf2d_bit_blt_alpha',0
	sz_buf2d_curve_bezier db 'buf2d_curve_bezier',0
	sz_buf2d_convert_text_matrix db 'buf2d_convert_text_matrix',0
	sz_buf2d_draw_text db 'buf2d_draw_text',0

align 4
buf_0:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 20 ;+4 left
	dw 10 ;+6 top
	dd 160 ;+8 w
	dd 128 ;+12 h
	dd 0x80 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_1:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 10 ;+4 left
	dw 10 ;+6 top
.size_x: dd 80 ;+8 w
.size_y: dd 64 ;+12 h
	dd 0x80 ;+16 color
	db 24 ;+20 bit in pixel

;--------------------------------------------------
system_path db '/sys/lib/'
lib0_name db 'buf2d.obj',0
;--------------------------------------------------

i_end: ;����� ����
	rb 1024
stacktop:
cur_dir_path rb 4096
library_path rb 4096
mem:
