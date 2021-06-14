;��஬��� �������୮��� �ᥬ, �� �������: �����/ᮢ�⮬/�������� ...

use32
  org 0
  db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
  dd 1, start, i_end, mem, stacktop, file_name, sys_path

MAX_COLOR_WORD_LEN equ 40
maxChars equ 100002 ;(�������⢮ ᨬ����� � ����� ���㬥�� + 2)
BUF_SIZE equ 4096 ;buffer for copy|paste
maxSyntaxFileSize equ 410000
TOOLBAR_ICONS_SIZE equ 1200*21

include '../../proc32.inc'
include '../../macros.inc'
include '../../KOSfuncs.inc'
include '../../load_img.inc'
include '../../load_lib.mac'
include '../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../system/skincfg/trunk/kglobals.inc'
include '../../system/skincfg/trunk/unpacker.inc'
include 'lang.inc'

include 't_data.inc'
include 't_button.inc'
include 't_menu.inc'
include 'strlen.inc'
include 't_draw.inc' ;draw main window functions
include 'wnd_k_words.inc'

@use_library mem.Alloc,mem.Free,mem.ReAlloc,dll.Load

align 4
icon_tl_sys dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ��⥬��� ������
run_file_70 FileInfoBlock

align 4
start:
	mcall SF_STYLE_SETTINGS,SSF_GET_COLORS,sc,sizeof.system_colors

	mcall SF_SYS_MISC,SSF_HEAP_INIT
	or eax,eax
	jnz @f
		call ted_Exit
	@@:

	mcall SF_KEYBOARD,SSF_SET_INPUT_MODE,1 ;scan code
	mcall SF_SET_EVENTS_MASK,0xC0000027

	mov esi,file_name
	stdcall str_len,esi
	mov ecx,eax
	mov edi,openfile_path
	cld
	rep movsb ;�����㥬 ��� 䠩�� � ���� openfile_path

load_libraries l_libs_start,load_lib_end

;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ������⥪�
mov	ebp,lib0
.test_lib_open:
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
	mcall SF_TERMINATE_PROCESS ;exit not correct
@@:
	add ebp,ll_struc_size
	cmp ebp,load_lib_end
	jl .test_lib_open

;---------------------------------------------------------------------
	stdcall [ted_init], tedit0
	stdcall [tl_data_init], tree1
	option_boxes_set_sys_color sc,opt_grlist1
	init_checkboxes2 check_boxes,check_boxes_end
	check_boxes_set_sys_color2 check_boxes,check_boxes_end,sc

; OpenDialog initialisation
	stdcall [OpenDialog_Init],OpenDialog_data

; kmenu initialisation
	call tedit_menu_init

; init toolbar file
	include_image_file 'te_icon.png', bmp_icon,,,6 ;6 ��� ���� ������
	mov eax,[bmp_icon]
	add eax,TOOLBAR_ICONS_SIZE
	stdcall img_to_gray, [bmp_icon],eax,(TOOLBAR_ICONS_SIZE)/3
;---------------------------------------------------------------------
; �����塞 䠩� � ����ࠬ� � ����ﬨ
	include_image_file '..\..\media\log_el\trunk\tl_sys_16.png', icon_tl_sys
	mov eax,dword[icon_tl_sys]
	mov dword[tree1.data_img_sys],eax
; �����塞 䠩� � �������� 㧫��
	include_image_file 'tl_nod_16.png', icon_tl_sys
	mov eax,dword[icon_tl_sys]
	mov dword[tree1.data_img],eax
;------------------------------------------------------------------------------
	copy_path fn_syntax_dir,sys_path,file_name,0 ;��६ ���� � ����� � 䠩���� ᨭ⠪��
	xor eax,eax
	mov ecx,4096
	mov edi,file_name
	repnz scasb
	mov byte[edi-2],al ;㡨ࠥ� '/' � ���� ����� ����� (�㦭� ��� �⥭�� � CD)
	mcall SF_FILE,tree_file_struct

	or ebx,ebx
	jz .end_dir_init
		mov eax,dir_mem+32+40
		mov ecx,ebx
		@@:
			cmp byte[eax],'.' ;䨫���㥬 䠩�� � ������� '.' � '..'
			je .filter
			;0x10000 ;1*2^16 - ��� 1 ����� ������ � ������
			stdcall [tl_node_add], tree1,0x10000,eax 
			stdcall [tl_cur_next], tree1
			.filter:
			add eax,304
			loop @b
		stdcall [tl_cur_beg],tree1 ;�⠢�� ����� �� ��砫� ᯨ᪠
		or dword[tree1.style], tl_cursor_pos_limited ;��࠭�稢��� �������� ����� � �।���� ᯨ᪠
	.end_dir_init:

;--- load ini file ---
	copy_path ini_name,sys_path,file_name,0
	;window startup pozition
	stdcall [ini_get_int],file_name,ini_sec_window,key_window_l,ini_def_window_l
	mov word[wnd_s_pos+2],ax
	stdcall [ini_get_int],file_name,ini_sec_window,key_window_w,ini_def_window_w
	mov word[wnd_s_pos],ax
	stdcall [ini_get_int],file_name,ini_sec_window,key_window_t,ini_def_window_t
	mov word[wnd_s_pos+6],ax
	stdcall [ini_get_int],file_name,ini_sec_window,key_window_h,ini_def_window_h
	mov word[wnd_s_pos+4],ax
	;scrool type
	stdcall [ini_get_int],file_name,ini_sec_window,key_scroll_type,ini_def_scroll_type
	mov [wScr.type],eax
	mov [hScr.type],eax
	mov [ws_dir_lbox.type],eax
    mov [w_scr_t3.type],eax
	;symbol size
	stdcall [ini_get_int],file_name,ini_sec_window,key_symbol_w,ini_def_symbol_w
	mov dword[tedit0.rec.width],eax
	stdcall [ini_get_int],file_name,ini_sec_window,key_symbol_h,ini_def_symbol_h
	mov dword[tedit0.rec.height],eax
	;lea eax,[eax+eax*2]
	;mov dword[tedit0.rec.top],eax
	;font size
	stdcall [ini_get_int],file_name,ini_sec_window,key_font_s,ini_def_font_s
	shl eax,24
	mov dword[tedit0.font_size],eax
	;������ �� ������
	ini_panel key_but_new,	  ID_BUT_NEW
	ini_panel key_but_open,   ID_BUT_OPEN
	ini_panel key_but_save,   ID_BUT_SAVE
	ini_panel key_but_save_as,ID_BUT_SAVE_AS
	ini_panel key_but_select, ID_BUT_SELECT
	ini_panel key_but_cut,	  ID_BUT_CUT
	ini_panel key_but_copy,   ID_BUT_COPY
	ini_panel key_but_paste,  ID_BUT_PASTE
	ini_panel key_but_find,   ID_BUT_FIND
	ini_panel key_but_replace,ID_BUT_REPLACE
	ini_panel key_but_key_words,ID_BUT_KEY_WORDS
	ini_panel key_but_upper,  ID_BUT_UPPER
	ini_panel key_but_lower,  ID_BUT_LOWER
	ini_panel key_but_reverse,ID_BUT_REVERSE
	ini_panel key_but_undo,   ID_BUT_UNDO
	ini_panel key_but_redo,   ID_BUT_REDO
	ini_panel key_but_invisible,ID_BUT_INVISIBLE
	ini_panel key_but_syntax_list,ID_BUT_SYNTAX_LIST
	ini_panel key_but_syntax_mode,ID_BUT_SYNTAX_MODE
	ini_panel key_but_convert_1251_866,ID_BUT_CONVERT_1251_866
	ini_panel key_but_convert_866_1251,ID_BUT_CONVERT_866_1251
	;䠩���� ���७��
	xor edx,edx
	mov ebx,synt_auto_open
	@@:
		;��६ ��� 䠩��
		stdcall [ini_get_str],file_name,ini_sec_options,key_synt_file,ebx,32,ini_def_synt_f
		cmp byte[ebx],0
		je @f
		inc byte[key_synt_file.numb]
		add ebx,32
		;��६ ���७��
		stdcall [ini_get_str],file_name,ini_sec_options,key_synt_ext,ebx,32,ini_def_synt_f
		inc byte[key_synt_ext.numb]
		add ebx,32
		inc edx
		cmp edx,max_synt_auto_open
		jl @b
	@@:

;--- load color option file ---
	stdcall open_unpac_synt_file,[fn_col_option]

;--- get cmd line ---
	cmp byte[openfile_path+3],0 ;openfile_path
	je @f ;if file names exist
		mov esi,openfile_path
		stdcall auto_open_syntax,esi
		call but_no_msg_OpenFile
	@@:



align 16
red_win:
	call draw_window

align 16
still:
	mcall SF_WAIT_EVENT
	cmp dword[exit_code],1
	jne @f
		call ted_Exit
		jmp still
	@@:

	cmp al,1 ;���������� ��������� ����
	jz red_win
	cmp al,2
	jz key
	cmp al,3
	jz button
	cmp al,6 ;����
	jne @f
		call mouse
	@@:
	jmp still

align 16
mouse:
	stdcall [kmainmenu_dispatch_cursorevent], [main_menu]

	mcall SF_MOUSE_GET,SSF_WINDOW_POSITION
	cmp word[tedit0.wnd.top],ax
	jg .no_edit
	shr eax,16
	cmp word[tedit0.wnd.left],ax
	jg .no_edit
	mcall SF_MOUSE_GET,SSF_BUTTON_EXT
	bt eax,24 ;������� 饫�� ����� �������
	jnc @f
		stdcall [ted_but_select_word], tedit0
		ret
	@@:
		stdcall [ted_mouse], tedit0
	.no_edit:

	;�஢�ઠ ������� �������
	cmp byte[tedit0.panel_id],TED_PANEL_FIND
	jne @f
		stdcall [edit_box_mouse], edit_find
		stdcall [option_box_mouse], opt_grlist1
		stdcall [check_box_mouse], ch1
	@@:
	cmp byte[tedit0.panel_id],TED_PANEL_REPLACE
	jne @f
		stdcall [edit_box_mouse], edit_find
		stdcall [edit_box_mouse], edit_replace
		stdcall [option_box_mouse], opt_grlist1
	@@:
	cmp byte[tedit0.panel_id],TED_PANEL_SYNTAX
	jne @f
		stdcall [tl_mouse], tree1
	@@:
	cmp byte[tedit0.panel_id],TED_PANEL_GOTO
	jne @f
		stdcall [edit_box_mouse], edit_goto
	@@:
	ret
;---------------------------------------------------------------------

;output:
; ah = symbol
align 16
proc KeyConvertToASCII uses ebx, table:dword
	mov ebx,dword[table] ;convert scan to ascii
	shr ax,8
	add bx,ax
	mov ah,byte[ebx]
	ret
endp

align 16
key:
	mcall SF_KEYBOARD,SSF_GET_CONTROL_KEYS ;66.3 ������� ���ﭨ� �ࠢ����� ������
	xor esi,esi
	mov ecx,1
	test al,3 ;[Shift]
	jz @f
		mov cl,2
		or esi,KM_SHIFT
	@@:
	test al,0x0c ;[Ctrl]
	jz @f
		or esi,KM_CTRL
	@@:
	test al,0x30 ;[Alt]
	jz @f
		mov cl,3
		or esi,KM_ALT
	@@:
	test al,0x80 ;[NumLock]
	jz @f
		or esi,KM_NUMLOCK
	@@:

	mcall SF_SYSTEM_GET,SSF_KEYBOARD_LAYOUT,,conv_tabl ;26.2 ������� �᪫���� ����������
	mcall SF_GET_KEY
	stdcall [tl_key], tree1

	test word[edit_replace.flags],ed_focus ;�᫨ �� � 䮪��, ��室��
	je @f
		cmp ah,0x80 ;if key up
		ja still
		cmp ah,42 ;[Shift] (left)
		je still
		cmp ah,54 ;[Shift] (right)
		je still
		cmp ah,56 ;[Alt]
		je still
		cmp ah,29 ;[Ctrl]
		je still
		cmp ah,69 ;[Pause Break]
		je still

		stdcall KeyConvertToASCII, conv_tabl
		stdcall [edit_box_key], edit_replace
		jmp still
	@@:
	test word[edit_find.flags],ed_focus ;�᫨ �� � 䮪��, ��室��
	je @f
		cmp ah,0x80 ;if key up
		ja still
		cmp ah,42 ;[Shift] (left)
		je still
		cmp ah,54 ;[Shift] (right)
		je still
		cmp ah,56 ;[Alt]
		je still
		cmp ah,29 ;[Ctrl]
		je still
		cmp ah,69 ;[Pause Break]
		je still

		stdcall KeyConvertToASCII, conv_tabl
		stdcall [edit_box_key], edit_find
		jmp still
	@@:
	test word[edit_goto.flags],ed_focus ;�᫨ �� � 䮪��, ��室��
	je @f
		cmp ah,0x80 ;if key up
		ja still
		cmp ah,42 ;[Shift] (left)
		je still
		cmp ah,54 ;[Shift] (right)
		je still
		cmp ah,56 ;[Alt]
		je still
		cmp ah,29 ;[Ctrl]
		je still
		cmp ah,69 ;[Pause Break]
		je still

		stdcall KeyConvertToASCII, conv_tabl
		stdcall [edit_box_key], edit_goto
		jmp still
	@@:

	stdcall [ted_key], tedit0, conv_tabl,esi
	jmp still

align 4
edit_replace edit_box TED_PANEL_WIDTH-1, 0, 20, 0xffffff, 0xff80, 0xff0000, 0xff, 0x4080, 300, buf_replace, mouse_dd, 0
edit_find edit_box TED_PANEL_WIDTH-1, 0, 20, 0xffffff, 0xff80, 0xff0000, 0xff, 0x4080, 300, buf_find, mouse_dd, 0
edit_goto edit_box TED_PANEL_WIDTH-1, 0, 20, 0xffffff, 0xff80, 0xff0000, 0xff, 0x4080, 10, buf_goto, mouse_dd, 0

unpac_mem dd 0

;library structures
l_libs_start:
	lib0 l_libs lib_name_0, file_name, system_dir_0, import_box_lib
	lib1 l_libs lib_name_1, file_name, system_dir_1, import_msgbox_lib
	lib2 l_libs lib_name_2, file_name, system_dir_2, import_proclib
	lib3 l_libs lib_name_3, file_name, system_dir_3, import_libimg
	lib4 l_libs lib_name_4, file_name, system_dir_4, import_libini
	lib5 l_libs lib_name_5, file_name, system_dir_5, import_libkmenu
load_lib_end:

IncludeIGlobals
hed db 'TextEdit '
i_end:
	openfile_path: ;����� ���� � 䠩�� � ����� ���� ࠡ��
		rb 4096
	dir_mem rb 32+304*count_of_dir_list_files
	wnd_s_pos: ;���� ��� ����஥� ���⮢�� ����樨 ����
		rq 1
	last_open_synt_file rb 32 ;��� ��᫥����� ������祭���� 䠩�� ᨭ⠪��
	buf rb BUF_SIZE ;���� ��� ����஢���� � ��⠢��
	buf_find rb 302 ;���� ��� ���᪠ ⥪��
	buf_replace rb 302 ;���� ��� ������ ⥪��
	buf_goto rb 12 ;���� ��� ���室� �� ��ப�
	sc system_colors
IncludeUGlobals
	align 16
	procinfo process_information
		rb 1024
	thread:
		rb 4096
align 16
    thread_coords:
	rb 4096
align 16
stacktop:
	sys_path: ;���� ��㤠 �����⨫�� �ᯮ��塞� 䠩�
		rb 4096
	file_name: ;��ࠬ���� ����᪠
		rb 4096
	syntax_path: ;��� ������砥���� 䠩�� ᨭ⠪��
		rb 4096
	plugin_path:
		rb 4096
	text_work_area: ;���� � 䠩��, ����� �����뢠���� � ����
		rb 4096
	filename_area: ;��� 䠩�� ��� ������� ������/�������
		rb 256
	file_info:
		rb 40
mem:
