use32
	org 0x0
	db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
	dd 0x1
	dd start
	dd i_end ;ࠧ��� �ਫ������
	dd mem
	dd stacktop
	dd 0
	dd sys_path

ini_def_c_bkgnd equ 0
ini_def_c_border equ 0xff0000
color_s0 equ 0xff ;ᨣ��� 0
color_s1 equ 0xffffff ;ᨣ��� 1
color_s2 equ 0xff00 ;�窠 ��� ����祭��
color_s3 equ 0xff0000 ;�६����� ���祭�� ��� ��࠭����

color_border dd ini_def_c_border
opt_sign_moves dd 4
opt_fast_mode dd 0 ;������ ०��

debug equ 0

;����� ������:
el_icon_group equ 0 ;��㯯����
el_icon_points equ 1 ;�窨
el_icon_sub_points equ 2 ;�����樨
el_icon_elems equ 3 ;�����
el_icon_captions equ 4 ;������

size_el_opt equ 34 ;ࠧ��� �������� � ���ﬨ �����
el_offs_nam equ 0 ;ᬥ饭�� ��� ��砫� ����� �����
el_offs_col equ 16 ;梥� �����
el_offs_box_x equ 20 ;�ਭ� ��஡�� �����
el_offs_box_y equ 21 ;���� ��஡�� �����
el_offs_table equ 22 ;㪠��⥫� �� ⠡���� ࠡ��� �����
el_offs_legs_inp equ 26 ;㪠��⥫� �� ⠡���� ���ᠭ�� �室��� ���
el_offs_legs_out equ 30 ;ᬥ饭�� �� ���ᠭ�� ��室��� ���

sp_offs_el_angle equ 8 ;ᬥ饭�� ��� 㣫� ������ ����� � ᯨ᪥
sp_offs_el_type equ 9 ;ᬥ饭�� ��� ⨯� ����� � ᯨ᪥

points_max equ 1000
capt_offs equ 10 ;ᬥ饭�� ��� ��砫� ������ � ���� tree1

include '../../../macros.inc'
include '../../../proc32.inc'
include '../../../develop/libraries/box_lib/load_lib.mac'
include '../../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../../dll.inc'
include 'le_pole.inc'
include 'le_signal.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load
caption db '�����᪨� ������ 15.11.15',0 ;������� ����

panel_0_coord_top equ 5 ;������ ���न��� 0-�� �鸞 ������ �����㬥�⮢
panel_1_coord_top equ 35
panel_2_coord_top equ 60
panel_3_coord_top equ 85

align 4
proc move_rotate_x_n90 uses ecx edi, d_x:dword, angle:dword
	mov edi,[angle] ;㣮� ������ / 90 (�� 0-3)
	and edi,3
	shl edi,4 ;edi*=16
	add edi,mcs

	mov ecx,[d_x]
	imul ecx,dword[edi]
	add eax,ecx

	mov ecx,[d_x]
	imul ecx,dword[edi+8]
	add ebx,ecx

	ret
endp

struct FileInfoBlock
	Function dd ?
	Position dd ?
	Flags	 dd ?
	Count	 dd ?
	Buffer	 dd ?
		db ?
	FileName dd ?
ends

macro elOpt nam,col,box_x,box_y,table, tbl_i_legs, ol0, ol1, ol2
{
	@@: db nam,0
	rb @b+el_offs_col-$
	dd col
	db box_x
	db box_y
	dd table+0 ;+el_offs_table
	dd tbl_i_legs+0 ;+26 �室�� ����
;+30 ��室�� ����
	db ol0+0, ol1+0, ol2+0, 0
}

align 4
el_opt_beg:
elOpt 'or[2]', 0xff00ff,5,5,tbl_or, tbl_il_2, 2
elOpt 'or[3]', 0xff00ff,5,7,tbl_or, tbl_il_3, 3
elOpt 'or[4]', 0xff00ff,5,9,tbl_or, tbl_il_4, 4
elOpt 'and[2]',0xffff00,5,5,tbl_and.2, tbl_il_2, 2
elOpt 'and[3]',0xffff00,5,7,tbl_and.3, tbl_il_3, 3
elOpt 'and[4]',0xffff00,5,9,tbl_and.4, tbl_il_4, 4
elOpt 'and[5]',0xffff00,5,11,tbl_and.5, tbl_il_5, 5
elOpt 'not',   0xffff,	3,3,tbl_not, tbl_il_1, 1
elOpt 'xor',   0x8000ff,5,5,tbl_xor, tbl_il_2, 2
elOpt 'sm[1]', 0x8080ff,7,7,tbl_sm,  tbl_il_3, 1,4
elOpt 'ms[2]', 0x8080ff,7,14,tbl_ms_2,  tbl_il_m2,6
elOpt 'ms[2]e',0x8080ff,7,17,tbl_ms_2_e,tbl_il_m2e,8
;elOpt 'cd[8]', 0x8000, 7,17,tbl_cd_8,tbl_il_8, 6,2,2 ;����� �� 8 �室��
.end:
elOpt '???', 0x808080,3,3,tbl_and.3, tbl_il_1, 1 ;�� ��������� �����

;⠡���� �� ����� �������� �ࠢ��� ࠡ��� ����⮢
align 4
tbl_or db 0,1,1,1, 1,1,1,1 ;or2, or3
	db 1,1,1,1,1,1,1,1 ;or4
tbl_and:
.5: dq 0,0
.4: dq 0
.3: dd 0
.2: db 0,0,0,1
tbl_xor db 0,1,1,0
tbl_sm db 0,2,2,1, 2,1,1,3
tbl_not db 1,0 ;not

;���⨯����� ��� 2-� ࠧ�來��� ᨣ����
align 4
tbl_ms_2_e: ;� �室�� 'e'
dq 0,0,0,0,0,0,0,0
tbl_ms_2:
db 0,1,0,1,0,1,0,1
db 0,1,0,1,0,1,0,1
db 0,0,1,1,0,0,1,1
db 0,0,1,1,0,0,1,1
db 0,0,0,0,1,1,1,1
db 0,0,0,0,1,1,1,1
dq 0
db 1,1,1,1,1,1,1,1
;tbl_cd_8 db ;256=2^8

;⠡���� ��� �室��� ���
tbl_il_1 db 1,0   ;����� �� 1 ����
tbl_il_2 db 1,2,0 ;����� �� 2 ����
tbl_il_3 db 1,2,2,0
tbl_il_4 db 1,2,2,2,0
tbl_il_5 db 1,2,2,2,2,0
tbl_il_m2  db 1,2,2,2,3,2,0
tbl_il_m2e db 1,2,2,2,3,2,3,0
;tbl_il_8 db 1,2,2,2,2,2,2,2,0

time dd 0
tim_ch db 0
pen_mode dd 0 ;०�� �ᮢ���� �஢���
pen_coord_x dd 0 ;���न��� x ��砫쭮� �窨 �ᮢ����
pen_coord_y dd 0

txt_set_0 db '0',0
txt_set_1 db '1',0
;txt_mov_l db 27,0 ;<-
;txt_mov_r db 26,0 ;->

txt_size db 'size',0
txt_elements db 'elements',0
txt_points db 'points',0
txt_sub_points db 'subpoints',0
txt_captions db 'captions',0

;����� ��ᨭ�ᮢ � ᨭ�ᮢ, �ᯮ��㥬�� ��� �����⮢ ᨣ����� � ����⮢
align 4
mcs dd 1, 0, 0, 1,\
	   0, 1,-1, 0,\
	  -1, 0, 0,-1,\
	   0,-1, 1, 0

run_file_70 FileInfoBlock
image_data dd 0 ;㪠��⥫� �� �६����� ������. ��� �㦥� �८�ࠧ������ ����ࠦ����

IMAGE_TOOLBAR_ICON_SIZE equ 16*16*3
IMAGE_TOOLBAR_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*25
image_data_toolbar dd 0

TREE_ICON_SYS16_BMP_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*11+54 ;ࠧ��� bmp 䠩�� � ��⥬�묨 ��������
icon_tl_sys dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ��⥬��� ������
TOOLBAR_ICON_BMP_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*5+54 ;ࠧ��� bmp 䠩�� � �������� ��ꥪ⮢
icon_toolbar dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ������ ��ꥪ⮢

IMAGE_FONT_SIZE equ 128*144*3
image_data_gray dd 0 ;������ � �६���묨 ��묨 ����ࠦ���ﬨ � �ଠ� 24-bit, �� ������ ���� ᮧ�������� ������

cursors_count equ 4
IMAGE_CURSORS_SIZE equ 4096*cursors_count ;ࠧ��� ���⨭�� � ����ࠬ�

macro load_image_file path,buf,size { ;����� ��� ����㧪� ����ࠦ����
	;path - ����� ���� ��६����� ��� ��ப��� ��ࠬ��஬
	if path eqtype '' ;�஢��塞 ����� �� ��ப�� ��ࠬ��� path
		jmp @f
			local .path_str
			.path_str db path ;�ନ�㥬 �������� ��६�����
			db 0
		@@:
		;32 - �⠭����� ���� �� ���஬� ������ ���� ���� � ��⥬�� ��⥬
		copy_path .path_str,[32],file_name,0
	else
		copy_path path,[32],file_name,0x0 ;�ନ�㥬 ����� ���� � 䠩�� ����ࠦ����, ���ࠧ㬥���� �� �� � ����� ����� � �ணࠬ���
	end if

	stdcall mem.Alloc, dword size ;�뤥�塞 ������ ��� ����ࠦ����
	mov [buf],eax

	mov [run_file_70.Function], 0
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov [run_file_70.Count], dword size
	m2m [run_file_70.Buffer], eax
	mov byte[run_file_70+20], 0
	mov [run_file_70.FileName], file_name
	mcall 70,run_file_70 ;����㦠�� 䠩� ����ࠦ����
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

ini_name db 'log_el.ini',0 ;��� 䠩��
ini_sec_color db 'Colors',0
key_color_bkgnd db 'background',0
key_color_border db 'border',0
key_color_s0 db 's0',0
key_color_s1 db 's1',0
key_color_s2 db 's2',0
key_color_captions db 'captions',0
ini_sec_options db 'Options',0
key_opt_sign_moves db 'signal_moves',0
key_opt_fast_mode db 'fast_mode',0

align 4
start:
	load_libraries l_libs_start,l_libs_end
	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ������⥪�
	mov	ebp,lib_7
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:
	mcall 48,3,sc,sizeof.system_colors
	mcall 40,0x27

	;*** ���뢠��� ����஥� �� *.ini 䠩��
	copy_path ini_name,sys_path,file_name,0x0

	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_bkgnd,ini_def_c_bkgnd
	mov dword[buf_0.color],eax
	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_border,ini_def_c_border
	mov dword[color_border],eax
	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_s0,color_s0
	mov dword[shem_colors],eax
	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_s1,color_s1
	mov dword[shem_colors+4],eax
	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_s2,color_s2
	mov dword[shem_colors+8],eax
	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_captions,[color_captions]
	mov dword[color_captions],eax

	mov ebx,el_opt_beg+el_offs_nam
	mov ecx,(el_opt_beg.end-el_opt_beg)/size_el_opt ;�������⢮ ⨯�� ����⮢
	cld
	@@:
		push ecx
		stdcall dword[ini_get_color],file_name,ini_sec_color,ebx,[ebx+el_offs_col-el_offs_nam]
		pop ecx
		mov dword[ebx+el_offs_col-el_offs_nam],eax
		add ebx,size_el_opt
		loop @b

	stdcall dword[ini_get_int],file_name,ini_sec_options,key_opt_sign_moves,[opt_sign_moves]
	mov dword[opt_sign_moves],eax
	stdcall dword[ini_get_int],file_name,ini_sec_options,key_opt_fast_mode,[opt_fast_mode]
	mov dword[opt_fast_mode],eax

	;*** �����⮢�� �������
	stdcall [OpenDialog_Init],OpenDialog_data
	stdcall [buf2d_create], buf_0 ;ᮧ����� ����
	load_image_file 'toolbar.png', image_data_toolbar,IMAGE_TOOLBAR_SIZE

	stdcall pole_init, pole
	stdcall dword[tl_data_init], tree1
	stdcall dword[tl_data_init], tree2

	;��⥬�� ������ 16*16 ��� tree_list
	load_image_file 'tl_sys_16.png', icon_tl_sys,TREE_ICON_SYS16_BMP_SIZE
	;�᫨ ����ࠦ���� �� ���뫮��, � � icon_tl_sys ����
	;�� ���樠����஢���� �����, �� �訡�� �� �㤥�, �. �. ���� �㦭��� ࠧ���
	mov eax,dword[icon_tl_sys]
	mov dword[tree1.data_img_sys],eax
	mov dword[tree2.data_img_sys],eax

	load_image_file 'objects.png', icon_toolbar,TOOLBAR_ICON_BMP_SIZE
	mov eax,dword[icon_toolbar]
	mov dword[tree1.data_img],eax
	mov dword[tree2.data_img],eax

	;*** ����㧪� ����
	load_image_file 'font6x9.bmp', image_data_gray,IMAGE_FONT_SIZE
	stdcall [buf2d_create_f_img], buf_font,[image_data_gray] ;ᮧ���� ����
	stdcall mem.Free,[image_data_gray] ;�᢮������� ������

	stdcall [buf2d_conv_24_to_8], buf_font,1 ;������ ���� �஧�筮�� 8 ���
	stdcall [buf2d_convert_text_matrix], buf_font

	;*** ����㧪� ����஢
	load_image_file 'cursors_gr.png',image_data_gray,IMAGE_CURSORS_SIZE
	stdcall [buf2d_create_f_img], buf_curs_8,[image_data_gray] ;ᮧ���� ����
	stdcall mem.Free,[image_data_gray] ;�᢮������� ������

	load_image_file 'cursors.png',image_data_gray, IMAGE_CURSORS_SIZE
	stdcall [buf2d_create_f_img], buf_curs,[image_data_gray] ;ᮧ���� ����
	stdcall mem.Free,[image_data_gray] ;�᢮������� ������

	stdcall [buf2d_conv_24_to_8], buf_curs_8,1 ;������ ���� �஧�筮�� 8���
	stdcall [buf2d_conv_24_to_32],buf_curs,buf_curs_8 ;������ ���� rgba 32���

	stdcall sign_init, 3000
	mov eax,el_opt_beg+el_offs_nam
	mov ecx,(el_opt_beg.end-el_opt_beg)/size_el_opt ;�������⢮ ⨯�� ����⮢
	cld
	@@:
		stdcall [tl_node_add], tree2,el_icon_elems shl 16,eax
		stdcall [tl_cur_next], tree2
		add eax,size_el_opt
		loop @b
	stdcall [tl_cur_beg], tree2

	;*** ��⠭���� �६��� ��� ⠩���
	mcall 26,9
	mov [last_time],eax

align 4
red_win:
	call draw_window

align 4
still:
	mcall 26,9
	mov ebx,[last_time]
	add ebx,10 ;����প�
	cmp ebx,eax
	jge @f
		mov ebx,eax
	@@:
	sub ebx,eax
	;cmp ebx,10 ;����প�
	;ja timer_funct
	;test ebx,ebx
	;jz timer_funct
	mcall 23
	cmp eax,0
	je timer_funct

	cmp al,1
	jz red_win
	cmp al,EV_KEY
	jz key
	cmp al,3
	jz button
	cmp al,EV_MOUSE
	jne @f
		call mouse
	@@:
	jmp still

align 4
timer_funct:
	pushad
	mcall 26,9
	mov [last_time],eax

	cmp byte[tim_ch],0
	je @f
		inc dword[time]
	cmp dword[opt_fast_mode],0
	jne .fast_m
		call sign_move
		stdcall [buf2d_draw], buf_0
		mov eax,[time]
		xor edx,edx
		div dword[opt_sign_moves]
		cmp edx,0 ;�஢��塞 ��⭮��� opt_sign_moves
		jnz @f
			call sign_from_elems
			call sign_from_captions
		jmp @f
	.fast_m:
		mov ecx,[opt_sign_moves]
		cld
		.cycle:
		call sign_move
		loop .cycle
		call sign_from_elems
		call sign_from_captions
		stdcall [buf2d_draw], buf_0
	@@:

	popad
	jmp still

align 4
mouse:
	stdcall [tl_mouse], tree1
	stdcall [tl_mouse], tree2

	pushad
	mcall 37,2 ;������ ������ ���
	bt eax,0 ;����� ������ �����?
	jc @f
	bt eax,1 ;�ࠢ�� ������ �����?
	jc @f
		xor eax,eax
		mov [pen_coord_x],eax
		mov [pen_coord_y],eax
		jmp .end_buf_wnd
	@@:
	mov esi,eax

	mcall 37,1 ;eax = (x shl 16) + y
	cmp ax,word[buf_0.t]
	jl .end_buf_wnd ;�� ������ � ���� ���� �� �� y

	mov ebx,eax
	shr ebx,16
	cmp bx,word[buf_0.l]
	jl .end_buf_wnd ;�� ������ � ���� ���� �� �� x

		and eax,0xffff ;��⠢�塞 ���न���� y
		sub ax,word[buf_0.t]
		sub bx,word[buf_0.l]
		;*** ������� �� zoom
		movzx ecx,byte[zoom]
		xor edx,edx
		div ecx
		xchg eax,ebx ;ebx ����� �� ecx
		xor edx,edx
		div ecx

		sub eax,[Cor_x]
		sub ebx,[Cor_y]

		;*** �஢�ઠ �� ��������� � �奬�
		bt eax,31
		jc .end_buf_wnd
		bt ebx,31
		jc .end_buf_wnd
		cmp eax,[shem_w]
		jge .end_buf_wnd
		cmp ebx,[shem_h]
		jge .end_buf_wnd

		cmp byte[pen_mode],0
		jne .end_mode_0
			bt esi,1
			jc .end_mode_0
			;०�� ����� (�롮� ����� �� ����⨨)
			stdcall element_is_click,eax,ebx
			test eax,eax
			jz .end_buf_wnd ;�� ����⨨ �� ������ �� �� ���� �� ����⮢
				push eax
					stdcall [tl_node_get_data], tree1
					mov ebx,eax
				pop eax
				cmp eax,ebx
				je .end_buf_wnd ;�᫨ 㦥 ����� �⮨� ⠬ ��� �㦭�
				
				stdcall [tl_cur_beg], tree1
				.cycle0:
				push eax
					stdcall [tl_node_get_data], tree1
					mov ebx,eax
				pop eax
				test ebx,ebx
				jz .end_buf_wnd
				cmp eax,ebx
				je @f
					stdcall [tl_cur_next], tree1
					jmp .cycle0
				@@:
				stdcall [tl_draw], tree1
			jmp .end_buf_wnd
		.end_mode_0:
		cmp byte[pen_mode],1
		jne .end_mode_1
			;०�� �ᮢ���� �஢���
			cmp dword[pen_coord_x],0
			jne @f
			cmp dword[pen_coord_y],0
			jne @f
				mov [pen_coord_x],eax
				mov [pen_coord_y],ebx
			@@:

			cmp dword[pen_coord_x],eax
			je .beg_draw
			cmp dword[pen_coord_y],ebx
			je .beg_draw

			mov ecx,eax
			sub ecx,[pen_coord_x]
			bt ecx,31
			jnc @f
				neg ecx
				inc ecx
			@@:
			mov edx,ebx
			sub edx,[pen_coord_y]
			bt edx,31
			jnc @f
				neg edx
				inc edx
			@@:
			cmp ecx,edx
			jl @f
				mov ebx,[pen_coord_y] ;�ਢ離� � ���न��� y
				jmp .beg_draw
			@@:
			mov eax,[pen_coord_x] ;�ਢ離� � ���न��� x

			.beg_draw:
			bt esi,1
			jc @f
				stdcall pole_cell_creat, pole,eax,ebx,0
				;��祣� �� �ࠫ��� redraw_pole �� ���室��, �. �. ����� ���� �� �㦭�
				stdcall pole_paint, pole
				stdcall [buf2d_draw], buf_0
				jmp .end_buf_wnd
			@@:
				stdcall pole_cell_delete, pole,eax,ebx
				call redraw_pole
				jmp .end_buf_wnd
		.end_mode_1:
		cmp byte[pen_mode],2
		jne @f
			;०�� �ᮢ���� �����樨 ��� �஢���
			bt esi,1
			jc .mode_2_del
				stdcall pole_cell_creat, pole,eax,ebx,2
				jmp .mode_2_draw
			.mode_2_del:
				;��࠭�� �����樨
				mov ecx,eax
				stdcall pole_cell_find, pole,ecx,ebx
				test eax,eax
				jz .end_buf_wnd
				stdcall pole_cell_creat, pole,ecx,ebx,0
			.mode_2_draw:
			stdcall pole_paint, pole
			stdcall [buf2d_draw], buf_0
			jmp .end_buf_wnd
		@@:
		cmp byte[pen_mode],3
		jne @f
			bt esi,1
			jc @f
			;०�� ��࠭�� �஢���
			stdcall pole_cell_delete, pole,eax,ebx
			call redraw_pole
			jmp .end_buf_wnd
		@@:
		cmp byte[pen_mode],4
		jne @f
			bt esi,1
			jc .rotate
			;०�� ᮧ����� ����⮢
			stdcall shem_element_creat, eax,ebx
			stdcall pole_paint, pole
			stdcall [buf2d_draw], buf_0
			jmp .end_buf_wnd
			.rotate:
			;������ ����⮢ �� 90 �ࠤ�ᮢ
			stdcall element_is_click,eax,ebx
			test eax,eax
			jz .end_buf_wnd
				inc byte[eax+sp_offs_el_angle]
				and byte[eax+sp_offs_el_angle],3 ;��� ����஫� �� ��९�������
				call redraw_pole
			jmp .end_buf_wnd
		@@:

	.end_buf_wnd:
if debug
stdcall but_test_pole, pole
end if
	popad
	ret

;output:
; eax - pointer to element data
align 4
proc element_is_click uses ebx ecx edx esi edi, coord_x:dword, coord_y:dword
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	@@:
		cmp esi,0
		je @f
		cmp word[esi],el_icon_elems ;����祭�� �१ esi ⨯ ������
		jne .end_add_p1
			stdcall [tl_node_poi_get_data], tree1,esi
			mov ecx,eax

			movzx edx,byte[ecx+sp_offs_el_type]
			imul edx,size_el_opt
			add edx,el_opt_beg ;��室�� ���� � ᢮��⢠�� ������� �����

			mov eax,[ecx] ;element coord x
			mov ebx,[ecx+4] ;element coord y
			movzx edi,byte[ecx+sp_offs_el_angle]
			push edi
			movzx edi,byte[edx+el_offs_box_y]
			dec edi
			push edi
			movzx edi,byte[edx+el_offs_box_x]
			dec edi
			push edi
			stdcall move_rotate_n90 ;,[edx+el_offs_box_x],[edx+el_offs_box_y],[ecx+sp_offs_el_angle]
			;Rect(eax,ebx,[ecx],[ecx+4])
			stdcall point_in_rect, [coord_x],[coord_y], eax,ebx,[ecx],[ecx+4]
			test eax,eax
			jz .end_add_p1
				mov eax,ecx
				jmp .end_f
		.end_add_p1:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp @b
	@@:
		xor eax,eax ;�᫨ �� ��諨
	.end_f:
	ret
endp

;�஢�ઠ ��������� �窨 � ��אַ㣮�쭨�
;१����:
; �᫨ �� �������� � eax=0
; �᫨ �������� � eax=1
align 4
proc point_in_rect uses ebx ecx, px:dword, py:dword, rx0:dword, ry0:dword, rx1:dword, ry1:dword
	xor eax,eax

	;�஢�ઠ �� �� x
	mov ebx,[rx0]
	mov ecx,[rx1]
	cmp ebx,ecx
	jle @f
		xchg ebx,ecx
	@@:
	cmp ebx,[px]
	jg .no_contains
	cmp ecx,[px]
	jl .no_contains

	;�஢�ઠ �� �� y
	mov ebx,[ry0]
	mov ecx,[ry1]
	cmp ebx,ecx
	jle @f
		xchg ebx,ecx
	@@:
	cmp ebx,[py]
	jg .no_contains
	cmp ecx,[py]
	jl .no_contains

	;�᫨ ������ � eax=1
		inc eax
		;stdcall draw_scaled_rect, [rx0],[ry0],[rx1],[ry1], 0xffffff
		;stdcall [buf2d_draw], buf_0
	.no_contains:
	ret
endp

align 4
draw_window:
pushad
	mcall 12,1

	; *** �ᮢ���� �������� ���� (�믮������ 1 ࠧ �� ����᪥) ***
	xor eax,eax
	mov ebx,(20 shl 16)+580
	mov ecx,(20 shl 16)+415
	mov edx,[sc.work]
	or  edx,(3 shl 24)+0x10000000+0x20000000
	mov edi,caption
	int 0x40

	; *** ᮧ����� ������ �� ������ ***
	mov eax,8
	mov ebx,(5 shl 16)+20
	mov ecx,(panel_0_coord_top shl 16)+20
	mov edx,3
	mov esi,[sc.work_button]
	int 0x40

	add ebx,25 shl 16
	mov edx,4
	int 0x40

	add ebx,25 shl 16
	mov edx,5
	int 0x40

	add ebx,30 shl 16
	mov edx,6 ;��� | ��⠭����
	int 0x40

	add ebx,25 shl 16
	mov edx,7
	int 0x40

	add ebx,25 shl 16
	mov edx,8
	int 0x40

	add ebx,25 shl 16
	mov edx,9
	int 0x40

	add ebx,25 shl 16
	mov edx,10
	int 0x40

	add ebx,25 shl 16
	mov edx,11
	int 0x40

	add ebx,25 shl 16
	mov edx,12
	int 0x40

	add ebx,30 shl 16
	mov edx,13 ;業�஢�� �奬�
	int 0x40

	add ebx,25 shl 16
	mov edx,14
	int 0x40

	add ebx,25 shl 16
	mov edx,15
	int 0x40

	add ebx,25 shl 16
	mov edx,16 ;ᤢ�� �奬� �����
	int 0x40

	add ebx,25 shl 16
	mov edx,17 ;ᤢ�� �奬� ����
	int 0x40

	; *** �ᮢ���� ������ �� ������� ***
	mov eax,7
	mov ebx,[image_data_toolbar]
	mov ecx,(16 shl 16)+16
	mov edx,(7 shl 16)+panel_0_coord_top+2 ;icon new
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon open
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon save
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(30 shl 16) ;+
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;-
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(30 shl 16) ;center
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;m
	int 0x40

	; *** �ᮢ���� ���� ***
	stdcall [buf2d_draw], buf_0
	mov dword[wScr1.all_redraw],1
	stdcall [tl_draw], tree1
	mov dword[wScr2.all_redraw],1
	stdcall [tl_draw], tree2
	stdcall pole_draw_pok, pole

	; *** ᮧ����� ������ ��⠭���� ᨣ����� set_0 � set_1 ***
	mov esi,[sc.work_button]
	mcall 8, (5 shl 16)+20, (panel_1_coord_top shl 16)+20, 20

	add ebx,25 shl 16
	mov edx,21
	int 0x40

	add ebx,30 shl 16
	mov edx,22
	int 0x40

	add ebx,25 shl 16
	mov edx,23
	int 0x40

	add ebx,25 shl 16
	mov edx,24
	int 0x40

	add ebx,25 shl 16
	mov edx,25
	int 0x40

	mov ecx,[sc.work_text]
	or  ecx,0x80000000 ;or (1 shl 30)
	;mov edi,[sc.work]
	mcall 4, (12 shl 16)+panel_1_coord_top+6,, txt_set_0

	add ebx,25 shl 16
	mov edx,txt_set_1
	int 0x40

	; *** �ᮢ���� ������ �� ������� ***
	mov ebx,[image_data_toolbar]
	add ebx,IMAGE_TOOLBAR_ICON_SIZE*15
	mcall 7,, (16 shl 16)+16, (62 shl 16)+panel_1_coord_top+2

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16)
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16)
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16)
	int 0x40

	; *** ᮧ����� ������ �ᮢ���� �஢��� ***
	mov esi,[sc.work_button]
	mcall 8, (5 shl 16)+20, (panel_2_coord_top shl 16)+20, 30

	add ebx,30 shl 16
	mov edx,31
	int 0x40

	add ebx,25 shl 16
	mov edx,32
	int 0x40

	add ebx,25 shl 16
	mov edx,33
	int 0x40

	add ebx,25 shl 16
	mov edx,34
	int 0x40

	add ebx,25 shl 16
	mov edx,35
	int 0x40

	; *** �ᮢ���� ������ �� ������� ***
	mov eax,7
	mov ebx,[image_data_toolbar]
	mov ecx,(16 shl 16)+16
	mov edx,(7 shl 16)+panel_2_coord_top+2 ;������ ��५�

	add ebx,IMAGE_TOOLBAR_ICON_SIZE*19
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(30 shl 16) ;icon - �ᮢ���� �஢���
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon - �ᮢ���� ����祭�� �஢����
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon - �ᮢ���� �����᪨� ����⮢
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon - �ᮢ���� �����ᥩ
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon - ����窠
	int 0x40

	mcall 12,2
popad
	ret

align 4
key:
	mcall 2
	stdcall [tl_key], dword tree1
	stdcall [tl_key], dword tree2
	jmp still


align 4
button:
	mcall 17
	cmp ah,3
	jne @f
		call but_new_file
		jmp still
	@@:
	cmp ah,4
	jne @f
		call but_open_file
		jmp still
	@@:
	cmp ah,5
	jne @f
		call but_save_file
		jmp still
	@@:
	cmp ah,6
	jne @f
		call but_run_stop
		jmp still
	@@:
	cmp ah,7
	jne @f
		call but_zoom_p
		jmp still
	@@:
	cmp ah,8
	jne @f
		call but_zoom_m
		jmp still
	@@:
	cmp ah,9
	jne @f
		call but_pole_left
		jmp still
	@@:
	cmp ah,10
	jne @f
		call but_pole_right
		jmp still
	@@:
	cmp ah,11
	jne @f
		call but_pole_up
		jmp still
	@@:
	cmp ah,12
	jne @f
		call but_pole_dn
		jmp still
	@@:
	cmp ah,13
	jne @f
		call but_center
		jmp still
	@@:

	;��।������� ��� ��ꥪ⮢ �奬�
	cmp byte[tim_ch],0
	jne .no_move
	cmp ah,14
	jne @f
		stdcall pole_move_all, pole, -1, 0
		call redraw_pole
	@@:
	cmp ah,15
	jne @f
		stdcall pole_move_all, pole, 1, 0
		call redraw_pole
	@@:
	cmp ah,16 ;ᤢ�� �奬� �����
	jne @f
		stdcall pole_move_all, pole, 0, -1
		call redraw_pole
	@@:
	cmp ah,17 ;ᤢ�� �奬� ����
	jne @f
		stdcall pole_move_all, pole, 0, 1
		call redraw_pole
	@@:
	.no_move:

	cmp ah,20
	jne @f
		call but_set_0
	@@:
	cmp ah,21
	jne @f
		call but_set_1
	@@:
	cmp ah,22
	jne @f
		call but_mov_l
	@@:
	cmp ah,23
	jne @f
		call but_mov_r
	@@:
	cmp ah,24
	jne @f
		call but_mov_u
	@@:
	cmp ah,25
	jne @f
		call but_mov_d
	@@:
	cmp ah,30
	jne @f
		call but_set_none
	@@:
	cmp ah,31
	jne @f
		stdcall set_pen_mode,1,0,((9 shl 8)+9) shl 16 ;��⠭���� ०��� �ᮢ���� �஢���
	@@:
	cmp ah,32
	jne @f
		stdcall set_pen_mode,2,1,((9 shl 8)+9) shl 16
	@@:
	cmp ah,33
	jne @f
		stdcall set_pen_mode,4,3,((9 shl 8)+9) shl 16 ;��⠭���� ०��� ᮧ����� ����⮢
	@@:
	cmp ah,35
	jne @f
		stdcall set_pen_mode,3,2,((15 shl 8)+9) shl 16 ;��⠭���� ०��� ��࠭�� �஢���
	@@:
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_0
	stdcall [buf2d_delete],buf_font
	stdcall [buf2d_delete],buf_curs
	stdcall mem.Free,[image_data_toolbar]
	stdcall pole_delete, pole
	call sign_delete
	stdcall [tl_data_clear], tree1
	;��⨬ 㪠��⥫� �� ����ࠦ����, ����� �뫨 ��騬� ��� ���⮢ � 㤠���� ���⮬ tree1
	mov dword[tree2.data_img_sys],0
	mov dword[tree2.data_img],0
	stdcall [tl_data_clear], tree2
	cmp [cursor_pointer],0
	je @f
		mcall 37,6,[cursor_pointer]
	@@:
	mcall -1

;ᮧ����� ����� �奬�
align 4
proc but_new_file uses eax
	call but_set_none
	stdcall [tl_info_clear],tree1
	stdcall [tl_draw],tree1
	xor eax,eax
	mov [shem_elems],eax
	mov [shem_captions],eax
	stdcall pole_clear, pole
	call redraw_pole
	ret
endp

align 4
f_size dd 0 ;ࠧ��� ���뢠����� 䠩��
shem_w dd 192 ;�ਭ� �奬�
shem_h dd 128 ;���� �奬�
shem_points dd 0 ;�������⢮ 㧫�� �� �奬�
shem_sub_points dd 0 ;�������⢮ �����஢����� 㧫�� �� �奬�
shem_elems dd 0 ;�������⢮ ����⮢ �� �奬�
shem_captions dd 0
shem_colors:
	dd color_s0, color_s1, color_s2, color_s3
color_captions dd 0x808080

align 4
open_file_lif:
	rb 2*4096 ;������� ��� ������ 䠩���
.end:

align 4
but_open_file:
	pushad
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],0
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],1 ;0 - Cancel 1 - Open 2 - Error open dialog
	jne .end_open_file
	;��� �� 㤠筮� ����⨨ �������

	mov [run_file_70.Function], 0
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov dword[run_file_70.Count], open_file_lif.end-open_file_lif
	m2m [run_file_70.Buffer], open_file_lif
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mcall 70,run_file_70 ;����㦠�� 䠩�
	cmp ebx,0xffffffff
	je .end_open_file

		mov dword[f_size],ebx
		add ebx,open_file_lif
		mov byte[ebx],0 ;�� ��砩 �᫨ ࠭�� �� ����� 䠩� ����襣� ࠧ��� ��⨬ ����� ���� � 䠩���
		mcall 71,1,openfile_path
		;---

		;������ ��������� ���祭��, �� ��砩 �᫨ � 䠩�� ���� ����४�� ࠧ����
		mov dword[shem_w],5
		mov dword[shem_h],5

		mov esi,txt_size
		call str_analiz_r
		cmp edi,0
		je @f
			stdcall str_len,esi
			add edi,eax
			stdcall conv_str_to_int,edi
			cmp eax,5
			jl @f ;�訡�� � 䠩�� (�� .end_open_file �� ���室��, ��⠥��� ������ ����� ���ଠ��)
			mov dword[shem_w],eax

			call str_next_val
			stdcall conv_str_to_int,edi
			cmp eax,5
			jl @f ;�訡�� � 䠩��
			mov dword[shem_h],eax
		@@:

		stdcall [tl_info_clear],tree1

		;*** ���������� � ᯨ᮪ �����᪨� ����⮢ ***
		stdcall [tl_node_add], tree1,(el_icon_group shl 16),txt_elements-capt_offs
		stdcall [tl_cur_next], tree1

		mov esi,txt_elements
		call str_analiz_r
		cmp edi,0
		je .end_elems
			stdcall str_len,esi
			add edi,eax
			stdcall conv_str_to_int,edi
			cmp eax,0
			jle .end_elems ;�᫨ �᫮ ����⮢ = 0
			mov [shem_elems],eax
			mov ecx,eax
			.cycle_elem:
				call str_next_val
				stdcall conv_str_to_int,edi
				mov dword[txt_buf],eax ;���न��� x
				call str_next_val
				stdcall conv_str_to_int,edi
				mov dword[txt_buf+4],eax ;���न��� y
				call str_next_val
				stdcall conv_str_to_int,edi
				mov byte[txt_buf+sp_offs_el_angle],al ;���ࠢ�����
				call str_next_val
				;�� ����� edi �������� �����
				stdcall el_get_name, edi,13 ;13 - ascii code
				mov byte[txt_buf+sp_offs_el_type],al ;⨯ �����

				stdcall make_list_capts,txt_buf,edi

				stdcall [tl_node_add], tree1,(el_icon_elems shl 16)+1,txt_buf
				stdcall [tl_cur_next], tree1
				dec ecx
				jnz .cycle_elem
		.end_elems:

		;*** ���������� � ᯨ᮪ ⥪�⮢�� �����ᥩ ***
		stdcall [tl_node_add], tree1,(el_icon_group shl 16),txt_captions-capt_offs
		stdcall [tl_cur_next], tree1

		mov esi,txt_captions
		call str_analiz_r
		cmp edi,0
		je .end_captions
			stdcall str_len,esi
			add edi,eax
			stdcall conv_str_to_int,edi
			cmp eax,0
			jle .end_captions ;�᫨ �᫮ �����ᥩ = 0
			mov [shem_captions],eax
			mov ecx,eax
			.cycle_captions:
				call str_next_val
				stdcall conv_str_to_int,edi
				mov dword[txt_buf],eax ;���न��� x
				call str_next_val
				stdcall conv_str_to_int,edi
				mov dword[txt_buf+4],eax ;���न��� y
				call str_next_val
				mov al,byte[edi] 
				mov byte[txt_buf+8],al ;��� ������ ('z' - ᨣ��� � 0, 'o' - ᨣ��� � 1, 'n' - ��� ᨣ����)
				call str_next_val

				stdcall make_list_capts,txt_buf,edi

				stdcall [tl_node_add], tree1,(el_icon_captions shl 16)+1,txt_buf
				stdcall [tl_cur_next], tree1
				dec ecx
				jnz .cycle_captions
		.end_captions:

		mov byte[txt_buf+capt_offs],0 ;���㫥��� �����ᥩ

		;*** ���������� � ᯨ᮪ ���祢�� �祪 ***
		stdcall [tl_node_add], tree1,(el_icon_group shl 16),txt_points-capt_offs
		stdcall [tl_cur_next], tree1

		mov dword[shem_points],0
		mov esi,txt_points
		call str_analiz_r
		cmp edi,0
		je .end_points ;�᫨ ���ᠭ�� �祪 ��� � 䠩��
			stdcall str_len,esi
			add edi,eax
			stdcall conv_str_to_int,edi
			cmp eax,0
			jle .end_points ;�᫨ �᫮ �祪 = 0
			mov [shem_points],eax
			mov ecx,eax
			.cycle_poi:
				call str_next_val
				stdcall conv_str_to_int,edi
				mov dword[txt_buf],eax ;���न��� x
				call str_next_val
				stdcall conv_str_to_int,edi
				mov dword[txt_buf+4],eax ;���न��� y
				call str_next_val
				stdcall conv_str_to_int,edi
				mov byte[txt_buf+8],al ;���ࠢ�����

				stdcall [tl_node_add], tree1,(el_icon_points shl 16)+1,txt_buf
				stdcall [tl_cur_next], tree1
				dec ecx
				jnz .cycle_poi
		.end_points:

		stdcall [tl_cur_beg], tree1

	;*** ���������� ����� ��ꥪ⮢ ***
	stdcall pole_clear, pole
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	@@:
		cmp esi,0
		je @f
		cmp word[esi],el_icon_points ;����祭�� �१ esi ⨯ ������, � �ய�� �� ����� ��ꥪ⮢
		jne .end_add_p0
			stdcall [tl_node_poi_get_data], tree1,esi
			stdcall pole_cell_creat, pole,dword[eax],dword[eax+4],0
		.end_add_p0:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp @b
	@@:
	stdcall pole_sort, pole

	;*** ���������� ����� ��ꥪ⮢ (�� �᭮�� �����᪨� ����⮢) ***
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	@@:
		cmp esi,0
		je @f
		cmp word[esi],el_icon_elems ;����祭�� �१ esi ⨯ ������
		jne .end_add_p3
			stdcall [tl_node_poi_get_data], tree1,esi
			mov ecx,eax

			xor edx,edx ;edx - ����� �室��� ����
			.add_p1:
				stdcall el_get_leg_coords, ecx,edx
				test eax,eax
				jnz .add_beg1
				test ebx,ebx
				jnz .add_beg1
					jmp .end_add_p1 ;�᫨ ���न���� �� ��﫨�� (eax=0 && ebx=0), ��室 �� 横��
				.add_beg1:
				stdcall pole_cell_creat, pole,eax,ebx,0
				inc edx
				jmp .add_p1
			.end_add_p1:

			mov edx,(1 shl 16) ;edx - ����� ��室��� ����
			.add_p2:
				stdcall el_get_leg_coords, ecx,edx
				test eax,eax
				jnz .add_beg2
				test ebx,ebx
				jnz .add_beg2
					jmp .end_add_p2 ;�᫨ ���न���� �� ��﫨�� (eax=0 && ebx=0), ��室 �� 横��
				.add_beg2:
				stdcall pole_cell_creat, pole,eax,ebx,0
				inc edx
				jmp .add_p2
			.end_add_p2:

		.end_add_p3:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp @b
	@@:
	stdcall pole_sort, pole

	;*** ���������� ����� ��ꥪ⮢ (�� �᭮�� �����ᥩ) ***
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	@@:
		cmp esi,0
		je @f
		cmp word[esi],el_icon_captions ;����祭�� �१ esi ⨯ ������
		jne .end_add_p6
			stdcall [tl_node_poi_get_data], tree1,esi
			mov ecx,eax
			cmp byte[ecx+8],'n'
			je .end_add_p6
				stdcall pole_cell_creat, pole,[ecx],[ecx+4],0
		.end_add_p6:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp @b
	@@:
	stdcall pole_sort, pole

	;*** �ᮢ���� �஢���� (�� �᭮�� ����� ��ꥪ⮢) ***
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	xor ecx,ecx
	@@:
		cmp esi,0
		je @f
		cmp word[esi],el_icon_points ;����祭�� �१ esi ⨯ ������, � �ய�� �� ����� ��ꥪ⮢
		jne .end_add_p4
			stdcall [tl_node_poi_get_data], tree1,esi
			movzx ebx,byte[eax+8]
			stdcall shem_create_line, dword[eax],dword[eax+4],ebx
			add ecx,edx
		.end_add_p4:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		cmp ecx,250 ;ecx - �᫮ ����������� �祪
		jl @b
		xor ecx,ecx
		stdcall pole_sort, pole ;����㥬 ��� ��⨬���樨 ����த���⢨�
		jmp @b
	@@:
	stdcall pole_sort, pole

		stdcall pole_reset_cells, pole ;��⪠ �஢����

		;*** ���������� �����樮���� ��ꥪ⮢ (��אַ �� 䠩��, ����� ������ � ᯨ᮪) ***
		mov dword[shem_sub_points],0
		mov esi,txt_sub_points
		call str_analiz_r
		cmp edi,0
		je .end_sub_points ;�᫨ ���ᠭ�� �祪 ��� � 䠩��
			stdcall str_len,esi
			add edi,eax
			stdcall conv_str_to_int,edi
			cmp eax,0
			jle .end_sub_points ;�᫨ �᫮ �祪 = 0
			mov [shem_sub_points],eax
			mov ecx,eax
			.cycle_sub_poi:
				call str_next_val
				stdcall conv_str_to_int,edi
				mov ebx,eax ;���न��� x
				call str_next_val
				stdcall conv_str_to_int,edi ;���न��� y
				stdcall pole_cell_creat, pole,ebx,eax,2

				dec ecx
				jnz .cycle_sub_poi
		.end_sub_points:

		stdcall pole_sort, pole

		;*** 㤠����� ���祢�� �祪 � ᯨ᪠ ***
		mov ecx,[shem_points]
		inc ecx
		cld
		@@:
			stdcall [tl_info_undo],tree1
		loop @b

		;*** �ନ஢���� �����ᥩ � ��ࠬ���� �奬� ***
		mov dword[txt_info.size],0
		mov eax,dword[shem_w]
		mov edi,txt_info.size
		call convert_int_to_str
		stdcall str_cat, edi,txt_mull
		mov eax,dword[shem_h]
		mov edi,txt_buf
		call convert_int_to_str
		stdcall str_cat, txt_info.size,edi
		stdcall str_cat, txt_info.size,txt_space ;�������騩 �஡��

		;---
		call but_center ;業�஢�� �奬� � ��⮬ shem_w � shem_h
		call but_set_none
	.end_open_file:
	popad
	ret

align 4
proc but_save_file
locals
	napr dd ?
	s_param dd ? ;��ࠬ��� ��� ��ᨢ��� ��࠭����
endl
pushad
	;*** �맮� ����������� ���� ��� ��࠭���� 䠩��
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],1
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],1 ;if status==1 then save
	jne .end_save_file

	;*** �஢�ઠ ���� �� �஢��� �� �奬�
	mov edi,pole
	mov esi,pole_index
	cmp dword[esi],0
	je .cycle1_beg ;.end_save_file ;�᫨ ��� �祥� (�஢����) � ��室

	;*** ��⠭�������� ��⪨ �� ���祢� �窨, ����� ���� ��࠭��� � 䠩�
	mov dword[shem_points],0 ;��� ��८�।������ �祪
	mov dword[shem_sub_points],0
	mov ecx,dword[esi]
	.cycle0: ;横� �� �ᥬ �窠�
		add esi,4
		mov ebx,[esi]
		imul ebx,sizeof.Cell
		add ebx,pole_data

		cmp byte[ebx+offs_cell_liv],2
		jne @f
			inc dword[shem_sub_points]
			jmp .cycle0_next
		@@:

		mov dword[napr],0

		mov edx,[ebx+offs_cell_y]
		push edx
		mov edx,[ebx+offs_cell_x]
		inc edx
		push edx
		stdcall pole_cell_find, edi
		cmp eax,0
		je @f
			or dword[napr],1
		@@:

		mov edx,[ebx+offs_cell_y]
		inc edx
		push edx
		mov edx,[ebx+offs_cell_x]
		push edx
		stdcall pole_cell_find, edi
		cmp eax,0
		je @f
			or dword[napr],2
		@@:

		mov edx,[ebx+offs_cell_y]
		push edx
		mov edx,[ebx+offs_cell_x]
		dec edx
		push edx
		stdcall pole_cell_find, edi
		cmp eax,0
		je @f
			or dword[napr],4
		@@:

		mov edx,[ebx+offs_cell_y]
		dec edx
		push edx
		mov edx,[ebx+offs_cell_x]
		push edx
		stdcall pole_cell_find, edi
		cmp eax,0
		je @f
			or dword[napr],8
		@@:

		cmp dword[napr],5
		je @f
		cmp dword[napr],10
		je @f
		cmp dword[napr],15
		je @f
			mov eax,dword[napr]
			mov byte[ebx+offs_cell_liv],3 ;��⠭���� ��⪨
			mov byte[ebx+offs_cell_napr],al ;��⠭���� ���ࠢ�����
			inc dword[shem_points]
		@@:
		.cycle0_next:
		dec ecx
		jnz .cycle0

	;*** ��⨥ ��⪨ � �祪, ����� ��室���� �� �室��� ����� �����᪨� ����⮢
	.cycle1_beg:
	mov dword[shem_elems],0 ;��� ������।������ �᫠ ����⮢
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	.cycle1:
		cmp esi,0
		je .cycle1_end
		cmp word[esi],el_icon_elems ;����祭�� �१ esi ⨯ ������
		jne .end_add_p1
			inc dword[shem_elems]
if 0
			xor edx,edx ;edx - ����� �室��� ����
			@@:
				stdcall el_get_leg_coords, ecx,edx
				test eax,eax
				jz @f ;�᫨ ���न���� �� ��﫨�� (eax=0 && ebx=0)
				test ebx,ebx
				jz @f ;�᫨ ���न���� �� ��﫨�� (eax=0 && ebx=0)
				stdcall pole_cell_find, edi,eax,ebx
				test eax,eax
				jz .no_erase
					get_cell_offset ebx,eax
					mov byte[ebx+offs_cell_liv],0 ;��⨥ ��⪨
					dec dword[shem_points]
				.no_erase:
				inc edx
				jmp @b
			@@:
			;mov edx,(1 shl 16) ;edx - ����� ��室��� ����
end if
		.end_add_p1:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp .cycle1
	.cycle1_end:

	;*** ᮧ����� ���ଠ樨 ��� ����� � 䠩� ***
	mov edi,open_file_lif
	stdcall mem_copy,edi,txt_size,5
	stdcall str_cat,edi,txt_space
	mov eax,dword[shem_w]
	add edi,5
	stdcall convert_int_to_str
	stdcall str_cat,edi,txt_space
	stdcall str_len,edi
	add edi,eax
	mov eax,[shem_h]
	stdcall convert_int_to_str
	stdcall str_cat,edi,txt_nl

	;*** ��࠭���� �����᪨� ����⮢ ***
	stdcall str_cat,edi,txt_elements
	stdcall str_cat,edi,txt_space
	stdcall str_len,edi
	add edi,eax
	mov eax,[shem_elems]
	stdcall convert_int_to_str
	stdcall str_cat,edi,txt_nl

	cmp eax,1
	jl .cycle2_end
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	.cycle2:
		cmp esi,0
		je .cycle2_end
		cmp word[esi],el_icon_elems ;����祭�� �१ esi ⨯ ������
		jne .end_add_p2
			stdcall [tl_node_poi_get_data], tree1,esi
			mov ecx,eax

			stdcall str_len,edi
			add edi,eax
			mov eax,[ecx] ;coord x
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space
	
			stdcall str_len,edi
			add edi,eax
			mov eax,[ecx+4] ;coord y
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			stdcall str_len,edi
			add edi,eax
			movzx eax,byte[ecx+sp_offs_el_angle] ;angle
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			;��� �����
			movzx eax,byte[ecx+sp_offs_el_type]
			imul eax,size_el_opt
			add eax,el_opt_beg+el_offs_nam
			stdcall str_cat,edi,eax

			stdcall str_cat,edi,txt_nl

		.end_add_p2:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp .cycle2
	.cycle2_end:

	;*** ��࠭���� �����ᥩ ***
	stdcall str_cat,edi,txt_captions
	stdcall str_cat,edi,txt_space
	stdcall str_len,edi
	add edi,eax
	mov eax,[shem_captions]
	stdcall convert_int_to_str
	stdcall str_cat,edi,txt_nl

	cmp eax,1
	jl .cycle3_end
	stdcall dword[tl_node_poi_get_info], tree1,0
	mov esi,eax
	.cycle3:
		cmp esi,0
		je .cycle3_end
		cmp word[esi],el_icon_captions ;����祭�� �१ esi ⨯ ������
		jne .end_add_p3
			stdcall [tl_node_poi_get_data], tree1,esi
			mov ecx,eax

			stdcall str_len,edi
			add edi,eax
			mov eax,[ecx] ;coord x
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space
	
			stdcall str_len,edi
			add edi,eax
			mov eax,[ecx+4] ;coord y
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			stdcall str_len,edi
			add edi,eax
			movzx eax,byte[ecx+8] ;n,z,o
			mov ah,' ' ;�஡�� ��᫥ �㪢�, �� �� �� ��������� txt_space
			mov dword[edi],eax ;al

			;��� �����
			mov ebx,edi
			mov edi,ecx
			add edi,capt_offs
			call str_next_val
			call str_next_val
			;call str_next_val
			xchg ebx,edi
			stdcall str_cat,edi,ebx

			stdcall str_cat,edi,txt_nl

		.end_add_p3:
		stdcall dword[tl_node_poi_get_next_info], tree1,esi
		mov esi,eax ;���室�� � ᫥��饬� 㧫�
		jmp .cycle3
	.cycle3_end:

	;*** ��࠭���� ���祢�� �祪 ***
	stdcall str_cat,edi,txt_points
	stdcall str_cat,edi,txt_space
	stdcall str_len,edi
	add edi,eax
	mov eax,[shem_points]
	stdcall convert_int_to_str
	stdcall str_cat,edi,txt_nl

	mov eax,edi
	mov edi,pole
	mov esi,pole_index
	cmp dword[esi],0
	je .no_points ;�᫨ ��� �祥� (�஢����) � �ய��
	mov ebx,pole_data
	mov dword[napr],ebx
	mov edi,eax

	mov ecx,dword[esi]
	mov dword[s_param],0
	.cycle4: ;横� �� �ᥬ �窠�
		add esi,4
		mov ebx,[esi]
		imul ebx,sizeof.Cell
		add ebx,dword[napr] ;pole_data

		movzx edx,word[ebx+offs_cell_liv] ;also use offs_cell_napr
		cmp dl,3
		jne @f
			stdcall str_len,edi
			add edi,eax
			mov eax,dword[ebx+offs_cell_x]
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			stdcall str_len,edi
			add edi,eax
			mov eax,dword[ebx+offs_cell_y]
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			stdcall str_len,edi
			add edi,eax
			movzx eax,dh
			stdcall convert_int_to_str

			cmp dword[s_param],7 ;��� �ଠ� ��ப�
			je .new_line
				inc dword[s_param]
				stdcall str_cat,edi,txt_space
			jmp @f
			.new_line:
				mov dword[s_param],0
				stdcall str_cat,edi,txt_nl
		@@:
		dec ecx
		jnz .cycle4
		cmp dword[s_param],0
		je @f
			stdcall str_cat,edi,txt_nl
		@@:

	;*** ��࠭���� �����樮���� �祪 ***
	stdcall str_cat,edi,txt_sub_points
	stdcall str_cat,edi,txt_space
	stdcall str_len,edi
	add edi,eax
	mov eax,[shem_sub_points]
	stdcall convert_int_to_str
	stdcall str_cat,edi,txt_nl

	mov eax,edi
	mov edi,pole
	mov esi,pole_index
	;cmp dword[esi],0
	;je .no_points ;�᫨ ��� �祥� (�஢����) � �ய��
	;mov ebx,pole_data
	;mov dword[napr],ebx
	mov edi,eax

	mov ecx,dword[esi]
	.cycle5: ;横� �� �ᥬ �窠�
		add esi,4
		mov ebx,[esi]
		imul ebx,sizeof.Cell
		add ebx,dword[napr] ;pole_data

		movzx edx,byte[ebx+offs_cell_liv]
		cmp dl,2
		jne @f
			stdcall str_len,edi
			add edi,eax
			mov eax,dword[ebx+offs_cell_x]
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			stdcall str_len,edi
			add edi,eax
			mov eax,dword[ebx+offs_cell_y]
			stdcall convert_int_to_str
			stdcall str_cat,edi,txt_space

			;stdcall str_cat,edi,txt_nl
		@@:
		dec ecx
		jnz .cycle5
		stdcall str_cat,edi,txt_nl

	.no_points:

	;*** ��।������ ��ࠬ��஢ 䠩��
	mov edi,open_file_lif
	stdcall str_len,edi

	;*** ������ 䠩��
	mov [run_file_70.Function], 2
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov dword[run_file_70.Count], eax
	mov [run_file_70.Buffer], edi
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mcall 70,run_file_70 ;��࠭塞 䠩�

	call redraw_pole
	.end_save_file:
popad
	ret
endp

;�ନ஢���� ������ ��� ᯨ᪠
align 4
proc make_list_capts uses eax ebx ecx edi, buf:dword, txt:dword
	mov ebx,dword[buf]

	mov edi,ebx
	add edi,capt_offs
	mov dword[edi],'    ' ;�஡��� ��� ��ࠢ������� �����쪨� �ᥫ

	mov eax,dword[ebx] ;+0 - offset coord x
	cmp eax,100
	jge @f
		inc edi
	@@:
	cmp eax,10
	jge @f
		inc edi
	@@:
	call convert_int_to_str ;���न��� x (��� ������)
	stdcall str_cat,edi,txt_space
	stdcall str_len,edi
	add edi,eax
	mov eax,dword[ebx+4] ;+4 - offset coord y
	call convert_int_to_str ;���न��� y (��� ������)
	stdcall str_cat,edi,txt_space

	mov edi,dword[txt]
	cmp edi,0
	je .end_f
		stdcall str_len,edi ;eax = strlen([edi])
		mov ecx,edi
		call str_next_spaces
		sub edi,ecx ;��।��塞 ������ ������

		cmp edi,eax
		jle @f
			mov edi,eax ;�᫨ ��ப� �����稫��� �� �஡����
		@@:
		;cmp edi,1
		;jge @f
		;       mov edi,1 ;������ 1 ᨬ���
		;@@:

		add ebx,capt_offs	
		stdcall str_n_cat, ebx,ecx,edi
	.end_f:
	ret
endp

;���������� ������ ����� �ࠢ����� �� �奬�
;�� ���������� �ᯮ������� �⠭����� ����ன��
align 4
proc shem_element_creat uses eax ebx, coord_x:dword, coord_y:dword
	mov eax,dword[coord_x]
	mov dword[txt_buf],eax ;���न��� x
	mov ebx,dword[coord_y]

	stdcall element_is_click,eax,ebx ;�஢��塞 ���� �� � ������ �窥 ��㣮� ����� ᮧ����� ࠭��
	test eax,eax
	jnz .end_f ;�� ����⨨ ������ �� ���� �� ����⮢
	mov dword[txt_buf+4],ebx ;���न��� y

	;xor eax,eax
	mov byte[txt_buf+sp_offs_el_angle],al ;���ࠢ�����

	stdcall [tl_node_get_data], tree2
	mov ebx,eax
	test ebx,ebx
	jnz @f
		mov ebx,el_opt_beg+el_offs_nam ;�᫨ �� ��﫮�� ��� �����, � �� 㬮�砭�� ��६ 1-� �� ᯨ᪠
	@@:
	stdcall el_get_name, ebx,0
	mov byte[txt_buf+sp_offs_el_type],al ;⨯ �����

	stdcall make_list_capts,txt_buf,ebx
	stdcall [tl_node_add], tree1,(el_icon_elems shl 16)+1,txt_buf
	stdcall [tl_cur_next], tree1
	stdcall [tl_draw], tree1
	.end_f:
	ret
endp

;description:
; ���������� ��ப� � ������ ���. ����� � �����頥� ����� ���. �����
;input:
; str - 㪠��⥫� �� ��ப� ᮤ�ঠ��� ��� ���. �����
; asciiz - ᨬ��� ����� ������ ���� ������� �� ascii 0
;output:
; eax - ⨯ �����
align 4
proc el_get_name uses ebx ecx edi esi, str:dword, asciiz:dword
	mov edi,[str]
	cmp dword[asciiz],0
	je @f
		mov eax,dword[asciiz]
		mov ecx,el_offs_col ;ࠧ��� ⥪�� � str �� ����� el_offs_col
		cld
		repne scasb
		mov ebx,edi
		dec ebx
		mov byte[ebx],0 ;0 - ᨬ��� ���� ��ப� ��� �ࠢ��쭮� ࠡ��� str_cmp
	@@:

	mov edi,[str]
	mov esi,el_opt_beg+el_offs_nam
	xor ecx,ecx
	@@:
		stdcall str_cmp, edi,esi
		cmp eax,0
		je @f
		add esi,size_el_opt
		cmp esi,el_opt_beg.end
		jge @f
		inc ecx
		jmp @b
	@@:
	mov eax,ecx

	cmp dword[asciiz],0
	je @f
		mov ecx,dword[asciiz]
		mov byte[ebx],cl
	@@:
	ret
endp

;input:
; el_data - 㪠��⥫� �� ����� �����
; l_opt - ����� ����, ��� ���ன ������ ���न����, �室���/��室��� ����
;output:
; eax - coord x (if not found eax=0)
; ebx - coord y (if not found ebx=0)
align 4
proc el_get_leg_coords uses ecx edx edi esi, el_data:dword, l_opt:dword
	mov edi,[el_data] ;����� �����
	movzx esi,byte[edi+sp_offs_el_type] ;⨯ �����
	imul esi,size_el_opt
	add esi,el_opt_beg
	;esi+el_offs_legs_inp - 㪠��⥫� �� ⠡���� � ��ࠬ��ࠬ� �室��� ���

	mov eax,[edi+0]
	mov ebx,[edi+4]
	mov edx,[l_opt]
	movzx edi,byte[edi+sp_offs_el_angle] ;㣮� ������ / 90 (�� 0-3)

	btr edx,16 ;�室���/��室��� ����
	jc .output_leg

	;�᫨ ���� �室���
	inc edx ;������� ��� ��稭����� � ���, ��⮬� ������塞 1
	stdcall move_rotate_x_n90, -2,edi
	mov esi,[esi+el_offs_legs_inp]
	@@:
		movzx ecx,byte[esi]
		cmp ecx,0
		je .not_found ;���� ���稫��� ࠭��, 祬 ���������
		stdcall move_rotate_n90, 0,ecx,edi
		inc esi
		dec edx
		jnz @b
		jmp .end_f

	;�᫨ ���� ��室���
	.output_leg:
	inc edx ;������� ��� ��稭����� � ���, ��⮬� ������塞 1
	movzx ecx,byte[esi+el_offs_box_x]
	add ecx,2
	stdcall move_rotate_x_n90, ecx,edi
	add esi,el_offs_legs_out
	@@:
		movzx ecx,byte[esi]
		cmp ecx,0
		je .not_found ;���� ���稫��� ࠭��, 祬 ���������
		stdcall move_rotate_n90, 0,ecx,edi
		inc esi
		dec edx
		jnz @b
		jmp .end_f

	.not_found:
		xor eax,eax
		xor ebx,ebx
	.end_f:
	ret
endp

align 4
proc move_rotate_n90 uses ecx edi, d_x:dword, d_y:dword, angle:dword
	mov edi,[angle] ;㣮� ������ / 90 (�� 0-3)
	and edi,3
	shl edi,4 ;edi*=16
	add edi,mcs

	mov ecx,[d_x]
	imul ecx,dword[edi]
	add eax,ecx
	mov ecx,[d_y]
	imul ecx,dword[edi+4]
	add eax,ecx

	mov ecx,[d_x]
	imul ecx,dword[edi+8]
	add ebx,ecx
	mov ecx,[d_y]
	imul ecx,dword[edi+12]
	add ebx,ecx
	ret
endp

align 4
proc mem_copy, destination:dword, source:dword, len:dword
  push ecx esi edi
    cld
    mov esi, dword[source]
    mov edi, dword[destination]
    mov ecx, dword[len]
    rep movsb
  pop edi esi ecx
  ret
endp

;description:
; �㭪�� �ய�᪠�� ���� ᫮�� (��� �᫮) � ��⮬ ࠧ����⥫��� ᨬ�����:
; �஡���, ⠡��樨, ����� ��ப�. �㦭� ��� ��᫥����⥫쭮�� ���뢠�� �ᥫ �� ��ப�
;input:
; edi - 㪠��⥫� �� �஡�� ��� ᫮��
;output:
; edi - 㪠��⥫� �� ᫥���饥 ᫮��
align 4
str_next_val:
	call str_skip_spaces
	@@:
		cmp byte[edi],0
		je @f

		cmp byte[edi],' '
		je @f
		cmp byte[edi],9
		je @f
		cmp byte[edi],10
		je @f
		cmp byte[edi],13
		je @f

		inc edi
		jmp @b
	@@:
	call str_skip_spaces
	ret

align 4
str_skip_spaces:
	dec edi
	@@:
		inc edi
		cmp byte[edi],' '
		je @b
		cmp byte[edi],9
		je @b
		cmp byte[edi],10
		je @b
		cmp byte[edi],13
		je @b
	ret

align 4
str_next_spaces:
	dec edi
	@@:
		inc edi
		cmp byte[edi],0
		je @f
		cmp byte[edi],' '
		je @f
		cmp byte[edi],9
		je @f
		cmp byte[edi],10
		je @f
		cmp byte[edi],13
		je @f
		jmp @b
	@@:	
	ret

;input:
; esi - 㪠��⥫� �� �᪮��� ᫮��
;output:
; edi - 㪠��⥫� �� ������ � ���ன ᫮�� �������, �᫨ ᫮�� �� ������� � edi=0
;�������� ॣ�����:
; eax ecx
align 4
str_analiz_r:
	mov edi,open_file_lif
	mov ecx,dword[f_size]
	@@:
		mov al,byte[esi] ;��⠭�������� ���� ᨬ��� ��� ���᪠
		cld
		repnz scasb
		cmp ecx,0
		je @f ;�᫨ �����稫�� ���� ⥪��, � ��室 �� �㭪樨
		;���� ᨬ��� �� ����� edi-1 ������ ���� �� ��ப� esi
		dec edi
		stdcall str_instr, edi,esi ;�஢��塞 ᫮�� �� ᮢ�������
		inc edi
		cmp al,0
		jne @b ;�᫨ ᫮�� �� ᮢ����, �饬 � ��ப� ᫥���騩 ���� ᨬ��� ��� �ࠢ�����
		;� �������� �᫨ ��諨 ᫮�� esi �� ����� edi
		jmp .exit_f
	@@:
		;� �������� �᫨ �� ��諨 ᫮�� esi �� ����� edi
		xor edi,edi
	.exit_f:
	ret

;description:
; �஢���� ᮤ�ন��� �� ��ப� str1 � ��ப� str0
; �஢�ઠ �������� ⮫쪮 ��稭�� � ����� ᨬ�����, 㪠������ � str0 � str1
; �ਬ�� 1: �᫨ str0='aaabbbccc', str1='bbb' ᮢ������� �� �㤥�
; �ਬ�� 2: �᫨ str0='aaabbbccc', str1='aaa' ᮢ������� �㤥�
;output:
; al = 0 �᫨ ��ப� str1 ᮤ�ন��� � str0
; al != 0 �᫨ ��ப� str1 �� ᮤ�ন��� � str0
align 4
proc str_instr uses edi esi, str0:dword, str1:dword
	;xor eax,eax
	mov edi,[str0]
	mov esi,[str1]
	cld
	@@:
		mov al,[esi]
		cmp al,0
		je .e1
		inc esi
		scasb ;�ࠢ������ ᨬ����
	jz @b ;�᫨ ᮢ����, � ���室�� � �ࠢ����� ᫥�����
	;� �������� �᫨ ��ப� �� ᮢ����
	sub al,[edi-1]
	.e1: ;� �������� �᫨ ��ப� str1 (esi) �����稫���
	ret
endp

;description:
; �஢���� ᮢ������� ��ப str0 � str1
;output:
; eax = 0 �᫨ str0 = str1
; eax = 1 �᫨ str0 != str1
align 4
proc str_cmp uses ecx edi esi, str0:dword, str1:dword
	xor eax,eax
	mov edi,[str0]
	mov esi,[str1]
	mov ecx,0x8fffffff ;ecx - �祭� ����讥 �᫮
	cld
	repne scasb
	;dec edi
	sub edi,[str0]
	mov ecx,edi ;ecx = strlen(str0)+1

	mov edi,[str0]
	repe cmpsb
	je @f
		inc eax
	@@:
	ret
endp

align 4
proc but_run_stop
	xor byte[tim_ch],1
	cmp byte[tim_ch],0
	jne @f
		;��⠭���� �奬�
		stdcall pole_reset_cells, pole ;��⪠ �஢����
		call redraw_pole
		jmp .end_f
	@@:
		;�����⮢�� �奬� � ������
		call sign_clear
		call sign_set_captions_angles
	.end_f:
	ret
endp

align 4
proc but_set_0 uses eax
	stdcall [tl_node_get_data], tree1
	test eax,eax
	jz .end_f
;el_icon_captions
;       cmp byte[eax+8],'n'
;       je .end_f
	cmp byte[eax+8],'o' ;�६����� ���祭��, ���� ��� �஢�ન ⨯� ⥪�饩 ������
	jne .end_f
		mov byte[eax+8],'z'
	.end_f:
	ret
endp

align 4
proc but_set_1 uses eax
	stdcall [tl_node_get_data], tree1
	test eax,eax
	jz .end_f
;el_icon_captions
;       cmp byte[eax+8],'n'
;       je .end_f
	cmp byte[eax+8],'z' ;�६����� ���祭��, ���� ��� �஢�ન ⨯� ⥪�饩 ������
	jne .end_f
		mov byte[eax+8],'o'
	.end_f:
	ret
endp

;ᤢ�� ��ꥪ� �����
align 4
proc but_mov_l uses eax edi
	cmp byte[tim_ch],0
	jne .end_f
	stdcall [tl_node_get_data], tree1
	test eax,eax
	jz .end_f
		cmp dword[eax],1
		jle .end_f
		dec dword[eax]
		mov edi,eax
		stdcall mem_copy,txt_buf,eax,capt_offs
		add edi,capt_offs
		call str_next_val
		call str_next_val
		stdcall make_list_capts,txt_buf,edi
		stdcall mem_copy,eax,txt_buf,32 ;capt_offs
		call redraw_pole
		stdcall [tl_draw],tree1
	.end_f:
	ret
endp

;ᤢ�� ��ꥪ� ��ࠢ�
align 4
proc but_mov_r uses eax edi
	cmp byte[tim_ch],0
	jne .end_f
	stdcall [tl_node_get_data], tree1
	test eax,eax
	jz .end_f
		inc dword[eax]
		mov edi,eax
		stdcall mem_copy,txt_buf,eax,capt_offs
		add edi,capt_offs
		call str_next_val
		call str_next_val
		stdcall make_list_capts,txt_buf,edi
		stdcall mem_copy,eax,txt_buf,32 ;capt_offs
		call redraw_pole
		stdcall [tl_draw],tree1
	.end_f:
	ret
endp

align 4
proc but_mov_u uses eax edi
	cmp byte[tim_ch],0
	jne .end_f
	stdcall [tl_node_get_data], tree1
	test eax,eax
	jz .end_f
		cmp dword[eax+4],1
		jle .end_f
		dec dword[eax+4]
		mov edi,eax
		stdcall mem_copy,txt_buf,eax,capt_offs
		add edi,capt_offs
		call str_next_val
		call str_next_val
		stdcall make_list_capts,txt_buf,edi
		stdcall mem_copy,eax,txt_buf,32 ;capt_offs
		call redraw_pole
		stdcall [tl_draw],tree1
	.end_f:
	ret
endp

align 4
proc but_mov_d uses eax edi
	cmp byte[tim_ch],0
	jne .end_f
	stdcall [tl_node_get_data], tree1
	test eax,eax
	jz .end_f
		inc dword[eax+4]
		mov edi,eax
		stdcall mem_copy,txt_buf,eax,capt_offs
		add edi,capt_offs
		call str_next_val
		call str_next_val
		stdcall make_list_capts,txt_buf,edi
		stdcall mem_copy,eax,txt_buf,32 ;capt_offs
		call redraw_pole
		stdcall [tl_draw],tree1
	.end_f:
	ret
endp

;��⠭���� ���筮�� ०���, ��� �ᮢ����
align 4
proc but_set_none
	mov byte[pen_mode],0
	cmp [cursor_pointer],0
	je @f
		push eax ebx ecx
		mcall 37,6,[cursor_pointer]
		pop ecx ebx eax
	@@:
	ret
endp

;hot_p - ���न���� ����祩 �窨 �����, ᬥ饭�� �� ��� 16 ((cx shl 8) + cy) shl 16
align 4
proc set_pen_mode uses eax ebx ecx edx, mode:dword, icon:dword, hot_p:dword
	mov eax,[mode]
	cmp byte[pen_mode],al
	je @f
		mov byte[pen_mode],al
		mov edx,[hot_p]
		mov dx,2 ;LOAD_INDIRECT
		mov ecx,[icon]
		shl ecx,12 ;㬭����� �� 4 ��
		add ecx,[buf_curs.data]
		mcall 37,4

		cmp eax,0
		je @f
			mov [cursor_pointer],eax
			mcall 37,5,[cursor_pointer]
	@@:
	ret
endp

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
db 'TXT',0
db 'ASM',0
.end:
db 0

include 'le_libs.inc'

mouse_dd dd 0x0
sc system_colors
last_time dd 0



align 16
procinfo process_information

align 4
buf_0: dd 0 ;
.l: dw 170 ;+4 left
.t: dw panel_1_coord_top ;+6 top
.w: dd 395 ;+8 w
.h: dd 340 ;+12 h
.color: dd ini_def_c_bkgnd ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_font: ;���� � ���⮬
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 25 ;+4 left
	dw 25 ;+6 top
	dd 96 ;+8 w
	dd 144 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_curs: ;���� � ����ࠬ�
.data: dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 0 ;+4 left
	dw 0 ;+6 top
	dd 32 ;+8 w
	dd 32*cursors_count ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_curs_8: ;���� � ����ࠬ�
.data: dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 0 ;+4 left
	dw 0 ;+6 top
	dd 32 ;+8 w
	dd 32*cursors_count ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

cursor_pointer dd 0 ;㪠��⥫� �� ����� ��� �����

el_focus dd 0
tree1 tree_list 32,points_max+2, tl_key_no_edit, 16,16,\
    0x8080ff,0x0000ff,0xffffff, 5,panel_3_coord_top+85,145,170, 0,capt_offs,0,\
    el_focus, wScr1,0
tree2 tree_list el_offs_col-el_offs_nam,100+2, tl_key_no_edit+tl_list_box_mode, 16,16,\
    0x8080ff,0x0000ff,0xffffff, 5,panel_3_coord_top,145,80, 0,0,0,\
    el_focus, wScr2,0

align 4
wScr1:
.x:
.size_x     dw 16 ;+0
.start_x    dw 0 ;+2
.y:
.size_y     dw 150 ;+4
.start_y    dw 0 ;+6
.btn_high   dd 15 ;+8
.type	    dd 1  ;+12
.max_area   dd 100  ;+16
.cur_area   dd 30  ;+20
.position   dd 0  ;+24
.bckg_col   dd 0xeeeeee ;+28
.frnt_col   dd 0xbbddff ;+32
.line_col   dd 0  ;+36
.redraw     dd 0  ;+40
.delta	    dw 0  ;+44
.delta2     dw 0  ;+46
.run_x:
.r_size_x   dw 0  ;+48
.r_start_x  dw 0  ;+50
.run_y:
.r_size_y   dw 0 ;+52
.r_start_y  dw 0 ;+54
.m_pos	    dd 0 ;+56
.m_pos_2    dd 0 ;+60
.m_keys     dd 0 ;+64
.run_size   dd 0 ;+68
.position2  dd 0 ;+72
.work_size  dd 0 ;+76
.all_redraw dd 0 ;+80
.ar_offset  dd 1 ;+84

align 4
wScr2:
.x:
.size_x     dw 16 ;+0
.start_x    dw 0 ;+2
.y:
.size_y     dw 150 ;+4
.start_y    dw 0 ;+6
.btn_high   dd 15 ;+8
.type	    dd 1  ;+12
.max_area   dd 100  ;+16
.cur_area   dd 30  ;+20
.position   dd 0  ;+24
.bckg_col   dd 0xeeeeee ;+28
.frnt_col   dd 0xbbddff ;+32
.line_col   dd 0  ;+36
.redraw     dd 0  ;+40
.delta	    dw 0  ;+44
.delta2     dw 0  ;+46
.run_x:
.r_size_x   dw 0  ;+48
.r_start_x  dw 0  ;+50
.run_y:
.r_size_y   dw 0 ;+52
.r_start_y  dw 0 ;+54
.m_pos	    dd 0 ;+56
.m_pos_2    dd 0 ;+60
.m_keys     dd 0 ;+64
.run_size   dd 0 ;+68
.position2  dd 0 ;+72
.work_size  dd 0 ;+76
.all_redraw dd 0 ;+80
.ar_offset  dd 1 ;+84

align 4
proc str_n_cat uses eax ecx edi esi, str1:dword, str2:dword, n:dword
	mov esi,dword[str2]
	mov ecx,dword[n]
	mov edi,dword[str1]
	stdcall str_len,edi
	add edi,eax
	cld
	repne movsb
	mov byte[edi],0
	ret
endp

align 4
proc str_cat uses eax ecx edi esi, str1:dword, str2:dword
	mov esi,dword[str2]
	stdcall str_len,esi
	mov ecx,eax
	inc ecx
	mov edi,dword[str1]
	stdcall str_len,edi
	add edi,eax
	cld
	repne movsb
	ret
endp

;output:
; eax = strlen
align 4
proc str_len, str1:dword
	mov eax,[str1]
	@@:
		cmp byte[eax],0
		je @f
		inc eax
		jmp @b
	@@:
	sub eax,[str1]
	ret
endp

;input:
; buf - 㪠��⥫� �� ��ப�, �᫮ ������ ���� � 10 ��� 16 �筮� ����
;output:
; eax - �᫮
align 4
proc conv_str_to_int uses ebx ecx esi, buf:dword
	xor eax,eax
	xor ebx,ebx
	mov esi,[buf]

	;�� ��砩 �᫨ ��। �᫮� ��室���� �஡���
	@@:
	cmp byte[esi],' '
	jne @f
		inc esi
		jmp @b
	@@:

	;��।������ ����⥫��� �ᥫ
	xor ecx,ecx
	inc ecx
	cmp byte[esi],'-'
	jne @f
		dec ecx
		inc esi
	@@:

	cmp word[esi],'0x'
	je .load_digit_16

	.load_digit_10: ;���뢠��� 10-���� ���
		mov bl,byte[esi]
		cmp bl,'0'
		jl @f
		cmp bl,'9'
		jg @f
			sub bl,'0'
			imul eax,10
			add eax,ebx
			inc esi
			jmp .load_digit_10
	jmp @f

	.load_digit_16: ;���뢠��� 16-���� ���
		add esi,2
	.cycle_16:
		mov bl,byte[esi]
		cmp bl,'0'
		jl @f
		cmp bl,'f'
		jg @f
		cmp bl,'9'
		jle .us1
			cmp bl,'A'
			jl @f ;��ᥨ���� ᨬ���� >'9' � <'A'
		.us1: ;��⠢��� �᫮���
		cmp bl,'F'
		jle .us2
			cmp bl,'a'
			jl @f ;��ᥨ���� ᨬ���� >'F' � <'a'
			sub bl,32 ;��ॢ���� ᨬ���� � ���孨� ॣ����, ��� ��饭�� �� ��᫥��饩 ��ࠡ�⪨
		.us2: ;��⠢��� �᫮���
			sub bl,'0'
			cmp bl,9
			jle .cor1
				sub bl,7 ;convert 'A' to '10'
			.cor1:
			shl eax,4
			add eax,ebx
			inc esi
			jmp .cycle_16
	@@:
	cmp ecx,0 ;�᫨ �᫮ ����⥫쭮�
	jne @f
		sub ecx,eax
		mov eax,ecx
	@@:
	ret
endp

;��� ��� �� ���, �� �८�ࠧ�� �᫮ � ��ப�
;input:
; eax = value
; edi = string buffer
;output:
align 4
proc convert_int_to_str uses eax ecx edx edi
	mov dword[edi+1],0
	call .str
	ret
endp

align 4
.str:
	mov ecx,0x0a ;�������� ��⥬� ��᫥��� ���������� ॣ����� eax,ecx,edx �室�� ��ࠬ���� eax - �᫮
    ;��ॢ�� �᫠ � ASCII ��ப� ������ ����� ecx=��⥬� ��᫥�� edi ���� �㤠 �����뢠��, �㤥� ��ப�, ��祬 ����� ��६�����
	cmp eax,ecx  ;�ࠢ���� �᫨ � eax ����� 祬 � ecx � ��३� �� @@-1 �.�. �� pop eax
	jb @f
		xor edx,edx  ;������ edx
		div ecx      ;ࠧ������ - ���⮪ � edx
		push edx     ;�������� � �⥪
		;dec edi             ;ᬥ饭�� ����室���� ��� ����� � ���� ��ப�
		call .str ;��३� �� ᠬ� ᥡ� �.�. �맢��� ᠬ� ᥡ� � ⠪ �� ⮣� ������ ���� � eax �� �⠭�� ����� 祬 � ecx
		pop eax
	@@: ;cmp al,10 ;�஢���� �� ����� �� ���祭�� � al 祬 10 (��� ��⥬� ��᫥�� 10 ������ ������� - ��譠�))
	or al,0x30  ;������ ������� ���� 祬 ��� ���
	stosb	    ;������� ����� �� ॣ���� al � �祪� ����� es:edi
	ret	      ;�������� 祭� ������ 室 �.�. ���� � �⥪� �࠭����� ���-�� �맮��� � �⮫쪮 ࠧ �� � �㤥� ��뢠����

align 16
i_end:
	rb 1024
stacktop:
	sys_path rb 1024
	file_name:
		rb 1024 ;4096
	library_path rb 1024
	plugin_path rb 4096
	openfile_path rb 4096
	filename_area rb 256
mem:
