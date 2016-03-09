use32
	org 0x0
	db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
	dd 1, start, i_end, mem, stacktop, 0, sys_path

include '../../../../macros.inc'
include '../../../../proc32.inc'
include '../../../../KOSfuncs.inc'
include '../../../../load_img.inc'
include '../../../../develop/libraries/box_lib/trunk/box_lib.mac'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load
caption db '��ᬮ�� ���ଠ樨 Exif 10.03.16',0 ;������� ����

run_file_70 FileInfoBlock

fn_toolbar db 'toolbar.png',0
IMAGE_TOOLBAR_ICON_SIZE equ 16*16*3
image_data_toolbar dd 0

memory_file_size dd 512*1024 ;ࠧ��� ����� ��� ������ 䠩��� (� ��砫� 512 Kb, �� ����� 㢥��稢���� �� ����室�����)

align 4
start:
	load_libraries l_libs_start,l_libs_end
	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ������⥪�
	mov	ebp,lib_0
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall SF_TERMINATE_PROCESS
	@@:
	mcall SF_STYLE_SETTINGS,SSF_GET_COLORS,sc,sizeof.system_colors
	mcall SF_SET_EVENTS_MASK,0xC0000027
	stdcall [OpenDialog_Init],OpenDialog_data ;�����⮢�� �������

	stdcall [buf2d_create], buf_0 ;ᮧ����� ����
	stdcall [buf2d_create], buf_thumb

	load_image_file 'font8x9.bmp', image_data_toolbar
	stdcall [buf2d_create_f_img], buf_1,[image_data_toolbar] ;ᮧ���� ����
	stdcall mem.Free,[image_data_toolbar] ;�᢮������� ������
	stdcall [buf2d_conv_24_to_8], buf_1,1 ;������ ���� �஧�筮�� 8 ���
	stdcall [buf2d_convert_text_matrix], buf_1

	load_image_file fn_toolbar, image_data_toolbar

	stdcall mem.Alloc,[memory_file_size]
	mov dword[open_file],eax
	
	call but_new_file

align 4
red_win:
	call draw_window

align 4
still:
	mcall SF_WAIT_EVENT

	cmp al,1
	jz red_win
	cmp al,2
	jz key
	cmp al,3
	jz button
	cmp al,6 ;����
	jne @f
		jmp mouse
	@@:
	jmp still

align 4
draw_window:
pushad
	mcall SF_REDRAW,SSF_BEGIN_DRAW

	; *** �ᮢ���� �������� ���� (�믮������ 1 ࠧ �� ����᪥) ***
	mov edx,[sc.work]
	or  edx,(3 shl 24)+0x30000000
	mov edi,caption
	mcall SF_CREATE_WINDOW, (20 shl 16)+590, (20 shl 16)+540

	; *** ᮧ����� ������ �� ������ ***
	mov esi,[sc.work_button]
	mcall SF_DEFINE_BUTTON, (5 shl 16)+20, (5 shl 16)+20, 3

	mov ebx,(30 shl 16)+20
	mov edx,4
	int 0x40

	mov ebx,(55 shl 16)+20
	mov edx,5
	int 0x40

	mov ebx,(85 shl 16)+20
	mov edx,6
	int 0x40

	mov ebx,(110 shl 16)+20
	mov edx,7
	int 0x40

	mov ebx,(135 shl 16)+20
	mov edx,8
	int 0x40

	mov ebx,(160 shl 16)+20
	mov edx,9
	int 0x40

	; *** �ᮢ���� ������ �� ������� ***
	mcall SF_PUT_IMAGE, [image_data_toolbar], (16 shl 16)+16, (7 shl 16)+7 ;icon new

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon open
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon save
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(30 shl 16) ;app1 text
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;app1 gps
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;app2 text
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;app2 thumbnail
	int 0x40

	; *** �ᮢ���� ���� ***
	stdcall [buf2d_draw], buf_0

	mcall SF_REDRAW,SSF_END_DRAW
popad
	ret

align 4
key:
	mcall SF_GET_KEY
	jmp still

align 4
mouse:

	jmp still

align 4
button:
	mcall SF_GET_BUTTON
	cmp ah,3
	jne @f
		call but_new_file
	@@:
	cmp ah,4
	jne @f
		call but_open_file
	@@:
	cmp ah,5
	jne @f
		call but_save_file
	@@:
	cmp ah,6
	jne @f
		call but_1
	@@:
	cmp ah,7
	jne @f
		call but_2
	@@:
	cmp ah,8
	jne @f
		call but_3
	@@:
	cmp ah,9
	jne @f
		call but_4
	@@:
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_0
	stdcall [buf2d_delete],buf_1 ;㤠�塞 ����
	stdcall [buf2d_delete],buf_thumb
	stdcall mem.Free,[image_data_toolbar]
	stdcall mem.Free,[open_file]
	mcall SF_TERMINATE_PROCESS


align 4
but_new_file:
	mov dword[open_file_size],0
	call draw_file_1
	ret

align 4
open_file dd 0 ;㪠��⥫� �� ������ ��� ������ 䠩���
open_file_size dd 0 ;ࠧ��� ����⮣� 䠩�� (������ ���� �� ����� memory_file_size)

align 4
but_open_file:
	pushad
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],0
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],2
	je .end_open_file
	;��� �� 㤠筮� ����⨨ �������

	mov [run_file_70.Function], SSF_GET_INFO
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov dword[run_file_70.Count], 0
	m2m [run_file_70.Buffer], [open_file]
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mcall SF_FILE,run_file_70
	cmp eax,0
	jne .end_open_file

	mov eax,[open_file]
	mov ebx,dword[eax+32] ;dword[eax+32] - ࠧ��� ���뢠����� 䠩��
	mov dword[open_file_size],ebx ;ebx - ࠧ��� ���뢠����� 䠩��
	;memory_file_size - ࠧ��� �뤥������ ����� ��� 䠩��
	cmp dword[memory_file_size],ebx
	jge @f
		;㢥��稢��� ������ �᫨ �� 墠⨫�
		mov dword[memory_file_size],ebx
		stdcall mem.ReAlloc, dword[open_file],ebx
	@@:

	mov [run_file_70.Function], SSF_READ_FILE
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	m2m dword[run_file_70.Count], dword[open_file_size]
	m2m dword[run_file_70.Buffer],dword[open_file]
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mcall SF_FILE,run_file_70 ;����㦠�� 䠩� ����ࠦ����
	cmp ebx,0xffffffff
	je .end_open_file

	mov [open_file_size],ebx
	;add ebx,dword[open_file]
	;mov byte[ebx],0 ;�� ��砩 �᫨ ࠭�� �� ����� 䠩� ����襣� ࠧ��� ��⨬ ����� ���� � 䠩���
	stdcall [exif_get_app1], [open_file],h_app1
	mcall SF_SET_CAPTION,1,openfile_path

	call draw_file_1
	.end_open_file:
	popad
	ret

;description:
; ���ଠ�� exif �� app1
; ⥪�⮢� � �᫮�� �����
align 4
draw_file_1:
pushad
	stdcall [buf2d_clear], buf_0, [buf_0.color]
	cmp dword[open_file_size],0
	je .open_file

	mov eax,1
	mov ebx,1
	.cycle_0:
		stdcall [exif_get_app1_tag], h_app1,eax,txt_buf,80
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_buf,3,ebx,0xb0
		inc eax
		add ebx,10
		cmp byte[txt_buf],0
		jne .cycle_0

	;���뢠�� ���୨� ⥣� ��� 0x8769
	stdcall [exif_get_app1_child], h_app1,h_child,0x8769
	cmp dword[h_child],0
	je .no_found_child

	mov eax,1
	sub ebx,5
	.cycle_1:
		stdcall [exif_get_app1_tag], h_child,eax,txt_buf,80
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_buf,3,ebx,0xb00000
		inc eax
		add ebx,10
		cmp byte[txt_buf],0
		jne .cycle_1

	jmp @f
	.no_found_child:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_nochild,3,ebx,0xb000
		jmp @f
	.open_file:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_openfile,3,3,0xb000
	@@:
	stdcall [buf2d_draw], buf_0
popad
	ret

;description:
; ���ଠ�� exif �� app1
; ����� � gps
align 4
draw_file_2:
pushad
	stdcall [buf2d_clear], buf_0, [buf_0.color]
	cmp dword[open_file_size],0
	je .open_file

	mov eax,1
	mov ebx,1
	.cycle_0:
		stdcall [exif_get_app1_tag], h_app1,eax,txt_buf,80
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_buf,3,ebx,0xb0
		inc eax
		add ebx,10
		cmp byte[txt_buf],0
		jne .cycle_0

	;���뢠�� ���୨� ⥣� ��� 0x8825
	stdcall [exif_get_app1_child], h_app1,h_child,0x8825
	cmp dword[h_child],0
	je .no_found_child

	mov eax,1
	sub ebx,5
	.cycle_1:
		stdcall [exif_get_app1_tag], h_child,eax,txt_buf,80
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_buf,3,ebx,0xb00000
		inc eax
		add ebx,10
		cmp byte[txt_buf],0
		jne .cycle_1

	jmp @f
	.no_found_child:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_nochild,3,ebx,0xb000
		jmp @f
	.open_file:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_openfile,3,3,0xb000
	@@:
	stdcall [buf2d_draw], buf_0
popad
	ret

;description:
; ���ଠ�� exif �� app2
; ⥪�⮢� � �᫮�� �����
align 4
draw_file_3:
pushad
	stdcall [buf2d_clear], buf_0, [buf_0.color]
	cmp dword[open_file_size],0
	je .open_file

	mov eax,1
	mov ebx,1
	.cycle_0:
		stdcall [exif_get_app1_tag], h_app1,eax,txt_buf,80
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_buf,3,ebx,0xb0
		inc eax
		add ebx,10
		cmp byte[txt_buf],0
		jne .cycle_0

	;��室�� app2
	stdcall [exif_get_app2], h_app1,h_child
	cmp dword[h_child],0
	je .no_found_child

	mov eax,1 ;25
	sub ebx,5
	.cycle_1:
		stdcall [exif_get_app1_tag], h_child,eax,txt_buf,80
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_buf,3,ebx,0xb00000
		inc eax
		add ebx,10
		cmp byte[txt_buf],0
		jne .cycle_1

	jmp @f
	.no_found_child:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_nochild,3,ebx,0xb000
		jmp @f
	.open_file:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_openfile,3,3,0xb000
	@@:
	stdcall [buf2d_draw], buf_0
popad
	ret

;description:
; ���ଠ�� exif �� app2
; ���⨭�� ��� ����� Nikon
align 4
draw_file_4:
pushad
	stdcall [buf2d_clear], buf_0, [buf_0.color]
	cmp dword[open_file_size],0
	je .open_file

	mov eax,1
	mov ebx,1

	;��室�� app2
	stdcall [exif_get_app2], h_app1,h_child
	cmp dword[h_child],0
	je .no_found_child
	;���뢠�� ���୨� ⥣�
	stdcall [exif_get_app1_child], h_child,h_child,0x0011
	cmp dword[h_child],0
	je .no_found_child

	stdcall [exif_get_app1_child], h_child,h_child_siz,0x0202
	cmp dword[h_child_siz],0
	je .no_found_child
	
	stdcall [exif_get_app1_child], h_child,h_child,0x0201
	cmp dword[h_child],0
	je .no_found_child

	;��।��塞 ��� ����ࠦ���� � ��ॢ���� ��� �� �६���� ���� ebx
	stdcall [img_decode],dword[h_child],dword[h_child_siz],0
	cmp dword[eax+4],1
	jl .no_found_child
	cmp dword[eax+8],1
	jl .no_found_child
	mov ebx,eax
	stdcall [buf2d_resize],buf_thumb,[eax+4],[eax+8],1
	;�८�ࠧ㥬 ����ࠦ���� � �ଠ�� rgb
	stdcall [img_to_rgb2],ebx,[buf_thumb]
	;㤠�塞 �६���� ���� ebx
	stdcall [img_destroy],ebx

	stdcall [buf2d_bit_blt], buf_0, 0,15, buf_thumb ;��㥬 ����ࠦ����
	stdcall [buf2d_draw_text], buf_0, buf_1,txt_thumb,3,3,0xb000
	jmp @f

	.no_found_child:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_nochild,3,3,0xb000
		jmp @f
	.open_file:
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_openfile,3,3,0xb000
	@@:
	stdcall [buf2d_draw], buf_0
popad
	ret

align 4
but_save_file:
	pushad
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],1
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],2
	je .end_save_file
	;��� �� 㤠筮� ����⨨ �������

	mov [run_file_70.Function], SSF_CREATE_FILE
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0

if 1 ;�⠢�� 1 �᫨ ��࠭���� �� ����ࠦ����
	mov ebx, dword[open_file]
	mov [run_file_70.Buffer], ebx
	mov ebx,[open_file_size]
end if
if 0 ;�⠢�� 1 �᫨ ��࠭���� �᪨� ����ࠦ����
	;��室�� app2
	stdcall [exif_get_app2], h_app1,h_child
	cmp dword[h_child],0
	je .end_save_file
	;���뢠�� ���୨� ⥣�
	stdcall [exif_get_app1_child], h_child,h_child,0x0011
	cmp dword[h_child],0
	je .end_save_file
	stdcall [exif_get_app1_child], h_child,h_child,0x0201
	cmp dword[h_child],0
	je .end_save_file

	mov ebx,dword[h_child] ;dword[open_file]
	mov [run_file_70.Buffer], ebx
	mov ebx,... ; ��� ������ ���� ࠧ��� �᪨�� ����ࠦ���� ����� ����� 祬 [open_file_size]
end if
	mov dword[run_file_70.Count], ebx ;ࠧ��� 䠩��
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mcall SF_FILE,run_file_70 ;��࠭塞 䠩�
	;cmp ebx,0xffffffff
	;je .end_save_file
	; ... ᮮ�饭�� � ��㤠筮� ��࠭���� ...

	.end_save_file:
	popad
	ret

align 4
but_1:
	call draw_file_1 ;�᭮���� ���ଠ��
	ret

align 4
but_2:
	call draw_file_2 ;���ଠ�� �� GPS
	ret

align 4
but_3:
	call draw_file_3 ;���ଠ�� app2
	ret

align 4
but_4:
	call draw_file_4 ;�� �����-� ���ଠ�� ...
	ret

;����� ��� ������� ������ 䠩���
align 4
OpenDialog_data:
.type			dd 0 ;0 - ������, 1 - ��࠭���, 2 - ����� ��४���
.procinfo		dd procinfo	;+4
.com_area_name		dd communication_area_name	;+8
.com_area		dd 0	;+12
.opendir_path		dd plugin_path	;+16
.dir_default_path	dd default_dir ;+20
.start_path		dd file_name ;+24 ���� � ������� ������ 䠩���
.draw_window		dd draw_window	;+28
.status 		dd 0	;+32
.openfile_path		dd openfile_path	;+36 ���� � ���뢠����� 䠩��
.filename_area		dd filename_area	;+40
.filter_area		dd Filter
.x:
.x_size 		dw 420 ;+48 ; Window X size
.x_start		dw 10 ;+50 ; Window X position
.y:
.y_size 		dw 320 ;+52 ; Window y size
.y_start		dw 10 ;+54 ; Window Y position

default_dir db '/rd/1',0

communication_area_name:
	db 'FFFFFFFF_open_dialog',0
open_dialog_name:
	db 'opendial',0
communication_area_default_path:
	db '/rd/1/File managers/',0

Filter:
dd Filter.end - Filter ;.1
.1:
db 'JPG',0
db 'JPEG',0
.end:
db 0



head_f_i:
head_f_l db '���⥬��� �訡��',0

system_dir_0 db '/sys/lib/'
lib_name_0 db 'proc_lib.obj',0
err_message_found_lib_0 db '�� ������� ������⥪� ',39,'proc_lib.obj',39,0
err_message_import_0 db '�訡�� �� ������ ������⥪� ',39,'proc_lib.obj',39,0

system_dir_1 db '/sys/lib/'
lib_name_1 db 'libimg.obj',0
err_message_found_lib_1 db '�� ������� ������⥪� ',39,'libimg.obj',39,0
err_message_import_1 db '�訡�� �� ������ ������⥪� ',39,'libimg.obj',39,0

system_dir_2 db '/sys/lib/'
lib_name_2 db 'buf2d.obj',0
err_msg_found_lib_2 db '�� ������� ������⥪� ',39,'buf2d.obj',39,0
err_msg_import_2 db '�訡�� �� ������ ������⥪� ',39,'buf2d',39,0

system_dir_3 db '/sys/lib/'
lib_name_3 db 'exif.obj',0
err_msg_found_lib_3 db '�� ������� ������⥪� ',39,'exif.obj',39,0
err_msg_import_3 db '�訡�� �� ������ ������⥪� ',39,'exif',39,0

l_libs_start:
	lib_0 l_libs lib_name_0, sys_path, file_name, system_dir_0,\
		err_message_found_lib_0, head_f_l, proclib_import,err_message_import_0, head_f_i
	lib_1 l_libs lib_name_1, sys_path, file_name, system_dir_1,\
		err_message_found_lib_1, head_f_l, import_libimg, err_message_import_1, head_f_i
	lib_2 l_libs lib_name_2, sys_path, file_name, system_dir_2,\
		err_msg_found_lib_2,head_f_l,import_buf2d,err_msg_import_2,head_f_i
	lib_3 l_libs lib_name_3, sys_path, file_name, system_dir_3,\
		err_msg_found_lib_3,head_f_l,import_exif,err_msg_import_3,head_f_i
l_libs_end:

align 4
import_libimg:
	dd alib_init1
	img_is_img  dd aimg_is_img
	img_info    dd aimg_info
	img_from_file dd aimg_from_file
	img_to_file dd aimg_to_file
	img_from_rgb dd aimg_from_rgb
	img_to_rgb  dd aimg_to_rgb
	img_to_rgb2 dd aimg_to_rgb2
	img_decode  dd aimg_decode
	img_encode  dd aimg_encode
	img_create  dd aimg_create
	img_destroy dd aimg_destroy
	img_destroy_layer dd aimg_destroy_layer
	img_count   dd aimg_count
	img_lock_bits dd aimg_lock_bits
	img_unlock_bits dd aimg_unlock_bits
	img_flip    dd aimg_flip
	img_flip_layer dd aimg_flip_layer
	img_rotate  dd aimg_rotate
	img_rotate_layer dd aimg_rotate_layer
	img_draw    dd aimg_draw

	dd 0,0
	alib_init1   db 'lib_init',0
	aimg_is_img  db 'img_is_img',0 ;��।���� �� �����, ����� �� ������⥪� ᤥ���� �� ��� ����ࠦ����
	aimg_info    db 'img_info',0
	aimg_from_file db 'img_from_file',0
	aimg_to_file db 'img_to_file',0
	aimg_from_rgb db 'img_from_rgb',0
	aimg_to_rgb  db 'img_to_rgb',0 ;�८�ࠧ������ ����ࠦ���� � ����� RGB
	aimg_to_rgb2 db 'img_to_rgb2',0
	aimg_decode  db 'img_decode',0 ;��⮬���᪨ ��।���� �ଠ� ����᪨� ������
	aimg_encode  db 'img_encode',0
	aimg_create  db 'img_create',0
	aimg_destroy db 'img_destroy',0
	aimg_destroy_layer db 'img_destroy_layer',0
	aimg_count   db 'img_count',0
	aimg_lock_bits db 'img_lock_bits',0
	aimg_unlock_bits db 'img_unlock_bits',0
	aimg_flip    db 'img_flip',0
	aimg_flip_layer db 'img_flip_layer',0
	aimg_rotate  db 'img_rotate',0
	aimg_rotate_layer db 'img_rotate_layer',0
	aimg_draw    db 'img_draw',0

align 4
proclib_import: ;���ᠭ�� �ᯮ���㥬�� �㭪権
	OpenDialog_Init dd aOpenDialog_Init
	OpenDialog_Start dd aOpenDialog_Start
dd 0,0
	aOpenDialog_Init db 'OpenDialog_init',0
	aOpenDialog_Start db 'OpenDialog_start',0

align 4
import_buf2d:
	init dd sz_init
	buf2d_create dd sz_buf2d_create
	buf2d_create_f_img dd sz_buf2d_create_f_img
	buf2d_clear dd sz_buf2d_clear
	buf2d_draw dd sz_buf2d_draw
	buf2d_delete dd sz_buf2d_delete
	buf2d_resize dd sz_buf2d_resize
	buf2d_conv_24_to_8 dd sz_buf2d_conv_24_to_8
	buf2d_bit_blt dd sz_buf2d_bit_blt
	buf2d_convert_text_matrix dd sz_buf2d_convert_text_matrix
	buf2d_draw_text dd sz_buf2d_draw_text
	dd 0,0
	sz_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_resize db 'buf2d_resize',0
	sz_buf2d_conv_24_to_8 db 'buf2d_conv_24_to_8',0
	sz_buf2d_bit_blt db 'buf2d_bit_blt',0
	sz_buf2d_convert_text_matrix db 'buf2d_convert_text_matrix',0
	sz_buf2d_draw_text db 'buf2d_draw_text',0

align 4
import_exif: ;���ᠭ�� �ᯮ���㥬�� �㭪権
	exif_get_app1 dd sz_exif_get_app1
	exif_get_app1_tag dd sz_exif_get_app1_tag
	exif_get_app1_child dd sz_exif_get_app1_child
	exif_get_app2 dd sz_exif_get_app2
dd 0,0
	sz_exif_get_app1 db 'exif_get_app1',0
	sz_exif_get_app1_tag db 'exif_get_app1_tag',0
	sz_exif_get_app1_child db 'exif_get_app1_child',0
	sz_exif_get_app2 db 'exif_get_app2',0

sc system_colors 

align 16
procinfo process_information 

align 4
buf_0: dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 5 ;+4 left
	dw 31 ;+6 top
.w: dd 570 ;+8 w
.h: dd 480 ;+12 h
.color: dd 0xffffd0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_1:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 25 ;+4 left
	dw 25 ;+6 top
	dd 128 ;+8 w
	dd 144 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_thumb:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 0 ;+4 left
	dw 0 ;+6 top
	dd 120 ;+8 w
	dd 100 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

h_app1	rb 14 ;������� ��� ��������� ������� ⥣��
h_child rb 14 ;������� ��� ��������� ���୨� ⥣��
h_child_siz rb 14

txt_openfile db '��ன� 䠩� ����ࠦ���� � �ଠ� *.jpg.',0
txt_thumb db '�� �᪨� ����ࠦ���� �� ������ app2.',0
txt_nochild  db '���ଠ�� �� �⮩ ������ �� �������.',0
txt_buf rb 80

align 16
i_end:
	rb 2048
stacktop:
	sys_path rb 1024
	file_name rb 4096 
	plugin_path rb 4096
	openfile_path rb 4096
	filename_area rb 256
mem:
