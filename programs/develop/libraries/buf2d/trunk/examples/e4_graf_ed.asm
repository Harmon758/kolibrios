use32
	org 0x0
	db 'MENUET01' ;�������. ������������ ����� ������ 8 ����
	dd 0x1
	dd start
	dd i_end ;������ ����������
	dd mem,stacktop
	dd 0,cur_dir_path

include '../../../../../macros.inc'
include '../../../../../proc32.inc'
include '../../../../../develop/libraries/box_lib/load_lib.mac'
include 'mem.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc, 0 ;dll.Load

buf2d_l equ word[edi+4] ;������ �����
buf2d_t equ word[edi+6] ;������ ������

align 4
start:
	load_library vectors_name, cur_dir_path, library_path, system_path, \
		err_message_found_lib, head_f_l, import_buf2d_lib, err_message_import, head_f_i
	cmp eax,-1
	jz button.exit

	mcall 40,0x27
	mcall 48,3,sc,sizeof.system_colors ;�������� ��������� �����
	stdcall [buf2d_create], buf_0 ;������� �����

align 4
red_win:
	call draw_window

align 4
still:
	mcall 10
	cmp al,0x1 ;���. ��������� ����
	jz red_win
	cmp al,0x2
	jz key
	cmp al,0x3
	jz button
	cmp al,0x6
	jz mouse
	jmp still

align 4
draw_window:
	pushad
	mcall 12,1

	mov edx,[sc.work]
	or  edx,0x33000000
	mcall 0,(50 shl 16)+500,(30 shl 16)+370,,,caption

	stdcall [buf2d_draw], buf_0

	mcall 12,2
	popad
	ret

align 4
key:
	mcall 2
;       cmp ah,27 ;Esc
;       je button.exit
	jmp still

align 4
button:
	mcall 17 ;�������� ��� ������� ������
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_0 ;������� �����
	mcall -1 ;����� �� ���������

align 4
mouse:
	;������������ ���� ���������
	mcall 37,2 ;get mouse buttons
	cmp al,1
	jne @f
		mcall 37,1 ;get mouse coords
		mov ebx,eax
		shr ebx,16 ;� eax ���������� ���� �� ��� 'x'
		and eax,0xffff ;� eax ���������� ���� �� ��� 'y'
		mov edi,buf_0
		sub ax,buf2d_t ;�������� ���������� �������� �������� ������
		sub bx,buf2d_l

		;��������� ��� �������
		;stdcall [buf2d_circle], edi, ebx, eax, 20, 0xff0000 ;������ ����������
		;stdcall [buf2d_flood_fill], edi, ebx, eax, 0, 0xff,0xff0000 ;������� ������
		sub eax,5
		sub ebx,5
		stdcall [buf2d_filled_rect_by_size], edi, ebx,eax, 10,10, 0xff0000 ;������ �������������
		stdcall [buf2d_draw], edi ;��������� �����
	@@:
	jmp still

caption db 'Press left mouse button',0
sc system_colors  ;��������� �����

;--------------------------------------------------
align 4
import_buf2d_lib:
	dd sz_lib_init
	buf2d_create dd sz_buf2d_create
	buf2d_create_f_img dd sz_buf2d_create_f_img
	buf2d_clear dd sz_buf2d_clear
	buf2d_draw dd sz_buf2d_draw
	buf2d_delete dd sz_buf2d_delete
	buf2d_resize dd sz_buf2d_resize
	buf2d_line dd sz_buf2d_line
	buf2d_rect_by_size dd sz_buf2d_rect_by_size
	buf2d_filled_rect_by_size dd sz_buf2d_filled_rect_by_size
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
	buf2d_crop_color dd sz_buf2d_crop_color
	buf2d_offset_h dd sz_buf2d_offset_h
	buf2d_flood_fill dd sz_buf2d_flood_fill
	buf2d_set_pixel dd sz_buf2d_set_pixel

	dd 0,0
	sz_lib_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_resize db 'buf2d_resize',0
	sz_buf2d_line db 'buf2d_line',0
	sz_buf2d_rect_by_size db 'buf2d_rect_by_size',0
	sz_buf2d_filled_rect_by_size db 'buf2d_filled_rect_by_size',0
	sz_buf2d_circle db 'buf2d_circle',0
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
	sz_buf2d_crop_color db 'buf2d_crop_color',0
	sz_buf2d_offset_h db 'buf2d_offset_h',0
	sz_buf2d_flood_fill db 'buf2d_flood_fill',0
	sz_buf2d_set_pixel db 'buf2d_set_pixel',0

align 4
buf_0:
	dd 0 ;��������� �� ����� �����������
	dw 5 ;+4 left
	dw 5 ;+6 top
	dd 480 ;+8 w
	dd 320 ;+12 h
.color: dd 0xffffff ;+16 color
	db 24 ;+20 bit in pixel

;--------------------------------------------------
system_path db '/sys/lib/'
vectors_name db 'buf2d.obj',0
err_message_found_lib db 'Sorry I cannot load library buf2d.obj',0
head_f_i:
head_f_l db 'System error',0
err_message_import db 'Error on load import library buf2d.obj',0
;--------------------------------------------------

i_end: ;����� ����
	rb 2*4096
stacktop:
	cur_dir_path:
		rb 4096
	library_path:
		rb 4096
mem:

