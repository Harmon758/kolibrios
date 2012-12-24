use32
	org 0x0
	db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
	dd 0x1
	dd start
	dd i_end ;ࠧ��� �ਫ������
	dd mem,stacktop
	dd 0,sys_path

include '../../../../../macros.inc'
include '../../../../../proc32.inc'
include '../../../../../develop/libraries/box_lib/load_lib.mac'
include '../../../../../dll.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc, dll.Load

struct FileInfoBlock
	Function dd ?
	Position dd ?
	Flags	 dd ?
	Count	 dd ?
	Buffer	 dd ?
		db ?
	FileName dd ?
ends

IMAGE_FILE1_SIZE equ 200*186*3 ;ࠧ��� 䠩�� � ����ࠦ����� 200 x 100

BUF_STRUCT_SIZE equ 21
buf2d_data equ dword[edi] ;����� ���� ����ࠦ����
buf2d_w equ dword[edi+8] ;�ਭ� ����
buf2d_h equ dword[edi+12] ;���� ����
buf2d_l equ word[edi+4]
buf2d_t equ word[edi+6] ;����� ᢥ���
buf2d_size_lt equ dword[edi+4] ;����� ᫥�� � �ࠢ� ��� ����
buf2d_color equ dword[edi+16] ;梥� 䮭� ����
buf2d_bits equ byte[edi+20] ;������⢮ ��� � 1-� �窥 ����ࠦ����
vox_offs_tree_table equ 4

macro load_image_file path,buf,size { ;����� ��� ����㧪� ����ࠦ����
	;path - ����� ���� ��६����� ��� ��ப��� ��ࠬ��஬
	if path eqtype '' ;�஢��塞 ����� �� ��ப�� ��ࠬ��� path
		jmp @f
			local .path_str
			.path_str db path ;�ନ�㥬 �������� ��६�����
			db 0
		@@:
		;32 - �⠭����� ���� �� ���஬� ������ ���� ���� � ��⥬�� ��⥬
		copy_path .path_str,[32],file_name,0x0
	else
		copy_path path,[32],file_name,0x0 ;�ନ�㥬 ����� ���� � 䠩�� ����ࠦ����, ���ࠧ㬥���� �� �� � ����� ����� � �ணࠬ���
	end if

	stdcall mem.Alloc, dword size ;�뤥�塞 ������ ��� ����ࠦ����
	mov [buf],eax

	mov eax,70 ;70-� �㭪�� ࠡ�� � 䠩����
	mov [run_file_70.Function], 0
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov [run_file_70.Count], dword size
	m2m [run_file_70.Buffer], [buf]
	mov byte[run_file_70+20], 0
	mov [run_file_70.FileName], file_name
	mov ebx,run_file_70
	int 0x40 ;����㦠�� 䠩� ����ࠦ����
	cmp ebx,0xffffffff
	je @f
		;��।��塞 ��� ����ࠦ���� � ��ॢ���� ��� �� �६���� ���� image_data
		stdcall dword[img_decode], dword[buf],ebx,0
		mov dword[image_data],eax
		;�८�ࠧ㥬 ����ࠦ���� � �ଠ�� rgb
		stdcall dword[img_to_rgb2], dword[image_data],dword[buf]
		;㤠�塞 �६���� ���� image_data
		stdcall dword[img_destroy], dword[image_data]
	@@:
}

align 4
start:
	load_libraries l_libs_start,load_lib_end

	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ��� ����
	mov	ebp,lib0
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:
	mov	ebp,lib1
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:

	mcall 40,0x27
	mcall 48,3,sc,sizeof.system_colors ;����砥� ��⥬�� 梥�

	load_image_file 'img1.png',image_data_rgb, IMAGE_FILE1_SIZE
	stdcall [buf2d_create_f_img], buf_0,[image_data_rgb] ;ᮧ���� ����
	stdcall [buf2d_create_f_img], buf_1,[image_data_rgb] ;ᮧ���� ����
	stdcall [buf2d_create_f_img], buf_2,[image_data_rgb] ;ᮧ���� ����
	stdcall mem.Free,[image_data_rgb] ;�᢮������� ������

	stdcall [buf2d_filter_dither], buf_1,2
	stdcall [buf2d_filter_dither], buf_2,3

align 4
red_win:
	call draw_window

align 4
still: ;������ 横�
	mcall 10

	cmp al,0x1 ;���������� ��������� ����
	jz red_win
	cmp al,0x2
	jz key
	cmp al,0x3
	jz button

	jmp still

align 4
key:
	push eax ebx
	mcall 2
;...
	pop ebx eax
	jmp still


align 4
draw_window:
	pushad
	mcall 12,1

	mov edx,[sc.work]
	or  edx,0x33000000
	mcall 0,(20 shl 16)+670,(20 shl 16)+520,,,caption ;ᮧ����� ����

	stdcall [buf2d_draw], buf_0
	stdcall [buf2d_draw], buf_1
	stdcall [buf2d_draw], buf_2
	
	mcall 12,2
	popad
	ret

head_f_i:
head_f_l  db '���⥬��� �訡��',0

system_dir0 db '/sys/lib/'
name_buf2d db 'buf2d.obj',0
err_message_found_lib0 db '�� 㤠���� ���� ������⥪� buf2d.obj',0
err_message_import0 db '�訡�� �� ������ ������⥪� buf2d.obj',0

system_dir1 db '/sys/lib/'
name_libimg db 'libimg.obj',0
err_message_found_lib1 db '�� 㤠���� ���� ������⥪� libimg.obj',0
err_message_import1 db '�訡�� �� ������ ������⥪� libimg.obj',0

;library structures
l_libs_start:
	lib0 l_libs name_buf2d,  sys_path, file_name, system_dir0, err_message_found_lib0, head_f_l, import_buf2d_lib, err_message_import0, head_f_i
	lib1 l_libs name_libimg, sys_path, file_name, system_dir1, err_message_found_lib1, head_f_l, import_libimg, err_message_import1, head_f_i
load_lib_end:

align 4
button:
	mcall 17 ;������� ��� ����⮩ ������
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_0 ;㤠�塞 ����
	stdcall [buf2d_delete],buf_1 ;㤠�塞 ����
	stdcall [buf2d_delete],buf_2 ;㤠�塞 ����
	mcall -1 ;��室 �� �ணࠬ��

image_data dd 0 ;������ ��� �८�ࠧ������ ���⨭�� �㭪�ﬨ libimg
image_data_gray dd 0 ;������ � �८�ࠧ������ ����ࠦ����� � �ଠ� 8-bit
image_data_rgb dd 0 ;������ � �८�ࠧ������ ����ࠦ����� � �ଠ� rgb
image_data_foto dd 0

run_file_70 FileInfoBlock
caption db 'Draw images 13.09.11',0 ;������� ����
sc system_colors  ;��⥬�� 梥�

align 4
buf_0:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 5 ;+4 left
	dw 5 ;+6 top
	dd 200 ;+8 w
	dd 186 ;+12 h
	dd 0xffffff ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_1:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 5 ;+4 left
	dw 200 ;+6 top
	dd 200 ;+8 w
	dd 186 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_2:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 210 ;+4 left
	dw 200 ;+6 top
	dd 200 ;+8 w
	dd 186 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

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
import_buf2d_lib:
	dd sz_lib_init
	buf2d_create dd sz_buf2d_create
	buf2d_create_f_img dd sz_buf2d_create_f_img
	buf2d_clear dd sz_buf2d_clear
	buf2d_draw dd sz_buf2d_draw
	buf2d_delete dd sz_buf2d_delete
	buf2d_filter_dither dd sz_buf2d_filter_dither
	dd 0,0
	sz_lib_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_filter_dither db 'buf2d_filter_dither',0

i_end:
	rb 1024
stacktop:
	sys_path rb 4096
	file_name:
		rb 4096
	plugin_path:
		rb 4096
	openfile_path:
		rb 4096
	filename_area:
		rb 256
mem:
