;��஬��� �������୮��� �ᥬ, �� �������: �����/ᮢ�⮬/�������� ...

use32
  org 0x0
  db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
  dd 0x01
  dd start
  dd i_end ; ࠧ��� �ਫ������
  dd mem
  dd stacktop
  dd file_name ; command line
  dd sys_path

MAX_COLOR_WORD_LEN equ 40
maxChars equ 100002 ;(...+2)
BUF_SIZE equ 4096 ;buffer for copy|paste
maxSyntaxFileSize equ 410000
TOOLBAR_ICONS_SIZE equ 1200*20

include '../../proc32.inc'
;include '../../config.inc'
include '../../macros.inc'
include '../../dll.inc'
include '../../develop/libraries/box_lib/load_lib.mac'
include '../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../system/desktop/trunk/kglobals.inc'
include '../../system/desktop/trunk/unpacker.inc'
include 'lang.inc'

include 't_data.inc'
include 't_menu.inc'
include 'strlen.inc'
include 't_draw.inc' ;draw main window functions
include 't_button.inc' ;text work functions
include 'wnd_k_words.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load


;����� ��� ����㧪� ����ࠦ���� � �ᯮ�짮������ ������⥪� libimg.obj
;��� �ᯮ�짮����� ����� �㦭� ��६����:
; - run_file_70 FileInfoBlock
; - image_data dd 0
macro load_image_file path,buf,size
{
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

image_data dd 0 ;㪠��⥫� �� �६����� ������. ��� �㦥� �८�ࠧ������ ����ࠦ����
icon_tl_sys dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ��⥬��� ������

align 4
start:
	mcall 48,3,sc,sizeof.system_colors

	mcall 68,11
	or eax,eax
	jz button.exit

	mcall 66,1,1 ;scan code
	mcall 40,0xC0000027

	mov esi,file_name
	call strlen
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
	mcall -1 ;exit not correct
@@:
	add ebp,ll_struc_size
	cmp ebp,load_lib_end
	jl .test_lib_open

;---------------------------------------------------------------------
	stdcall [ted_init], tedit0
	stdcall dword[tl_data_init], tree1

; OpenDialog initialisation
	stdcall [OpenDialog_Init],OpenDialog_data

; kmenu initialisation	
	stdcall [kmenu_init], sc
	
	stdcall [ksubmenu_new]
	mov [main_menu], eax
	
	stdcall [ksubmenu_new]
	mov [main_menu_file], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_File_New, 3
	stdcall [ksubmenu_add], [main_menu_file], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_File_Open, 4
	stdcall [ksubmenu_add], [main_menu_file], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_File_Save, 5
	stdcall [ksubmenu_add], [main_menu_file], eax
	stdcall [kmenuitem_new], KMENUITEM_SEPARATOR, 0, 0
	stdcall [ksubmenu_add], [main_menu_file], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_File_Exit, 199
	stdcall [ksubmenu_add], [main_menu_file], eax	
	stdcall [kmenuitem_new], KMENUITEM_SUBMENU, sz_main_menu_File, [main_menu_file]
	stdcall [ksubmenu_add], [main_menu], eax

	stdcall [ksubmenu_new]
	mov [main_menu_changes], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Changes_Undo, 16
	stdcall [ksubmenu_add], [main_menu_changes], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Changes_Redo, 17
	stdcall [ksubmenu_add], [main_menu_changes], eax
	stdcall [kmenuitem_new], KMENUITEM_SUBMENU, sz_main_menu_Changes, [main_menu_changes]
	stdcall [ksubmenu_add], [main_menu], eax
	
	stdcall [ksubmenu_new]
	mov [main_menu_buf], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Buf_Cut, 7
	stdcall [ksubmenu_add], [main_menu_buf], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Buf_Copy, 8
	stdcall [ksubmenu_add], [main_menu_buf], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Buf_Paste, 9
	stdcall [ksubmenu_add], [main_menu_buf], eax	
	stdcall [kmenuitem_new], KMENUITEM_SUBMENU, sz_main_menu_Buf, [main_menu_buf]
	stdcall [ksubmenu_add], [main_menu], eax

	stdcall [ksubmenu_new]
	mov [main_menu_search], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Search_Text, 10
	stdcall [ksubmenu_add], [main_menu_search], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Search_Keywords, 12
	stdcall [ksubmenu_add], [main_menu_search], eax
	stdcall [kmenuitem_new], KMENUITEM_SUBMENU, sz_main_menu_Search, [main_menu_search]
	stdcall [ksubmenu_add], [main_menu], eax

	stdcall [ksubmenu_new]
	mov [main_menu_view], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Veiw_UseSyntax, 19
	stdcall [ksubmenu_add], [main_menu_view], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Veiw_ChooseSyntax, 20
	stdcall [ksubmenu_add], [main_menu_view], eax
	stdcall [kmenuitem_new], KMENUITEM_SEPARATOR, 0, 0
	stdcall [ksubmenu_add], [main_menu_view], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Veiw_LineBreak, 18
	stdcall [ksubmenu_add], [main_menu_view], eax	
	stdcall [kmenuitem_new], KMENUITEM_SUBMENU, sz_main_menu_View, [main_menu_view]
	stdcall [ksubmenu_add], [main_menu], eax

	stdcall [ksubmenu_new]
	mov [main_menu_encoding], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Encoding_1251to866, 21
	stdcall [ksubmenu_add], [main_menu_encoding], eax
	stdcall [kmenuitem_new], KMENUITEM_NORMAL, sz_main_menu_Encoding_866to1251, 22
	stdcall [ksubmenu_add], [main_menu_encoding], eax	
	stdcall [kmenuitem_new], KMENUITEM_SUBMENU, sz_main_menu_Encoding, [main_menu_encoding]
	stdcall [ksubmenu_add], [main_menu], eax
	
; init toolbar file
	load_image_file 'te_icon.png', bmp_icon,TOOLBAR_ICONS_SIZE*2 ;㬭������ �� 2 ��� ���� ������
	mov eax,[bmp_icon]
	add eax,TOOLBAR_ICONS_SIZE
	stdcall img_to_gray, [bmp_icon],eax,(TOOLBAR_ICONS_SIZE)/3
;---------------------------------------------------------------------
; �⠥� 䠩� � ����ࠬ� � ����ﬨ
	load_image_file 'tl_sys_16.png', icon_tl_sys,54+3*256*13
	mov eax,dword[icon_tl_sys]
	mov dword[tree1.data_img_sys],eax
;---------------------------------------------------------------------
; �⠥� 䠩� � �������� 㧫��
	load_image_file 'tl_nod_16.png', icon_tl_sys,54+3*256*2
	mov eax,dword[icon_tl_sys]
	mov dword[tree1.data_img],eax
;------------------------------------------------------------------------------
  copy_path fn_syntax_dir,sys_path,file_name,0x0 ;��६ ���� � ����� � 䠩���� ᨭ⠪��
  mov eax,70
  mov ebx,tree_file_struct
  int 0x40

cmp ebx,-1
je .end_dir_init

  mov eax,dir_mem
  add eax,32+4+1+3+4*6+8
mov ecx,ebx
@@:
  cmp byte[eax],'.' ;䨫���㥬 䠩�� � ������� '.' � '..'
  je .filter
    ;0x10000 ;1*2^16 - ��� 1 ����� ������ � ������
    stdcall dword[tl_node_add], eax,0x10000, tree1

    stdcall dword[tl_cur_next], tree1
  .filter:
  add eax,304
  loop @b
  stdcall dword[tl_cur_beg],tree1 ;�⠢�� ����� �� ��砫� ᯨ᪠
.end_dir_init:

;--- load ini file ---
	copy_path ini_name,sys_path,file_name,0
	;window startup pozition
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_window_l,ini_def_window_l
	mov word[wnd_s_pos+2],ax
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_window_w,ini_def_window_w
	mov word[wnd_s_pos],ax
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_window_t,ini_def_window_t
	mov word[wnd_s_pos+6],ax
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_window_h,ini_def_window_h
	mov word[wnd_s_pos+4],ax
	;scrool type
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_scroll_type,ini_def_scroll_type
	mov [wScr.type],eax
	mov [hScr.type],eax
	mov [ws_dir_lbox.type],eax
    mov [w_scr_t3.type],eax
	;symbol size
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_symbol_w,ini_def_symbol_w
	mov dword[tedit0.rec.width],eax
	stdcall dword[ini_get_int],file_name,ini_sec_window,key_symbol_h,ini_def_symbol_h
	mov dword[tedit0.rec.height],eax
	lea eax,[eax+eax*2]
	mov dword[tedit0.rec.top],eax
	;䠩���� ���७��
	xor edx,edx
	mov ebx,synt_auto_open
	@@:
		;��६ ��� 䠩��
		stdcall dword[ini_get_str],file_name,ini_sec_options,key_synt_file,ebx,32,ini_def_synt_f
		cmp byte[ebx],0
		je @f
		inc byte[key_synt_file.numb]
		add ebx,32
		;��६ ���७��
		stdcall dword[ini_get_str],file_name,ini_sec_options,key_synt_ext,ebx,32,ini_def_synt_f
		inc byte[key_synt_ext.numb]
		add ebx,32
		inc edx
		cmp edx,max_synt_auto_open
		jl @b
	@@:

;--- load color option file ---
	mov ebx,dword[fn_col_option]
	call open_unpac_synt_file

;--- get cmd line ---
	cmp byte[openfile_path+3],0 ;openfile_path
	je @f ;if file names exist
		mov esi,openfile_path
		call strlen ;eax=strlen
		call but_no_msg_OpenFile
	@@:



align 4
red_win:
  call draw_window

align 4
still:
	mcall 10
	cmp dword[exit_code],1
	je button.exit

	cmp al,1 ;���������� ��������� ����
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
	mcall 12,1

	mov edx,[sc.work]
	or  edx,0x73000000
	mov edi,hed
	mcall 0,dword[wnd_s_pos],dword[wnd_s_pos+4]

	mcall 9,procinfo,-1
	mov edi,tedit0 ;���祭�� edi �㦭� ��� EvSize � ted_wnd_t
	call EvSize

	mov eax,13 ;���孨� ��אַ㣮�쭨�, ��� ���⪨ ���孥� ������
	xor ebx,ebx
	mov ecx,ted_wnd_t
	mov bx,word[procinfo.client_box.width]
	inc bx
	int 0x40

	call draw_but_toolbar
	
	stdcall [kmainmenu_draw], [main_menu]

	stdcall [ted_draw], tedit0

	mcall 12,2
	ret

align 4
mouse:
	stdcall [kmainmenu_dispatch_cursorevent], [main_menu]
	stdcall [ted_mouse], tedit0

	cmp byte[tedit0.panel_id],TED_PANEL_FIND ;if not panel
	jne @f
		stdcall [edit_box_mouse], dword edit2
	@@:
	cmp byte[tedit0.panel_id],TED_PANEL_SYNTAX ;if not panel
	jne .mouse_end
	stdcall [tl_mouse], tree1
.mouse_end:
	jmp still
;---------------------------------------------------------------------

;output:
; ah = symbol
align 4
proc KeyConvertToASCII, table:dword
  push ebx
  mov ebx,dword[table] ;convert scan to ascii
  ror ax,8
  xor ah,ah
  add bx,ax
  mov ah,byte[ebx]
  pop ebx
  ret
endp

align 4
key:
  mcall 66,3 ;66.3 ������� ���ﭨ� �ࠢ����� ������
  xor esi,esi
  mov ecx,1
  test al,0x03 ;[Shift]
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

  mcall 26,2,,conv_tabl ;26.2 ������� �᪫���� ����������
  mcall 2 ;����砥� ��� ����⮩ ������
  stdcall [tl_key], tree1

  test word [edit2.flags],10b;ed_focus ; �᫨ �� � 䮪��, ��室��
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

    stdcall KeyConvertToASCII, dword conv_tabl
    stdcall [edit_box_key], dword edit2
    jmp still
  @@:

  stdcall [ted_key], tedit0, conv_tabl,esi
  jmp still

align 4
button:

  mcall 17 ;������� ��� ����⮩ ������
  cmp ah,3
  jne @f
    call ted_but_new_file
  @@:
  cmp ah,4
  jne @f
    call ted_but_open_file
  @@:
  cmp ah,5
  jne @f
    call ted_but_save_file
  @@:
  cmp ah,6
  jne @f
    stdcall [ted_but_select_word], tedit0
  @@:
  cmp ah,7
  jne @f
    stdcall [ted_but_cut], tedit0
  @@:
  cmp ah,8
  jne @f
    stdcall [ted_but_copy], tedit0
  @@:
  cmp ah,9
  jne @f
    stdcall [ted_but_paste], tedit0
  @@:
  cmp ah,10
  jne @f
    call ted_but_find
  @@:
  cmp ah,11
  jne @f
    call but_replace
  @@:
  cmp ah,12
  jne @f
    call but_find_key_w
  @@:
  cmp ah,13
  jne @f
    stdcall [ted_but_sumb_upper], tedit0
  @@:
  cmp ah,14
  jne @f
    stdcall [ted_but_sumb_lover], tedit0
  @@:
  cmp ah,15
  jne @f
    stdcall [ted_but_reverse], tedit0
  @@:
  cmp ah,16
  jne @f
    stdcall [ted_but_undo], tedit0
  @@:
  cmp ah,17
  jne @f
    stdcall [ted_but_redo], tedit0
  @@:
  cmp ah,18
  jne @f
    stdcall but_sumb_invis, tedit0
  @@:
  cmp ah,19
  jne @f
    stdcall but_k_words_show, tedit0
  @@:
  cmp ah,20
  jne @f
    stdcall but_synt_show, tedit0
  @@:
  cmp ah,21
  jne @f
    stdcall [ted_but_convert_by_table],tedit0,tbl_1251_866
  @@:
  cmp ah,22
  jne @f
    stdcall [ted_but_convert_by_table],tedit0,tbl_866_1251
  @@:

  cmp ah,200
  jne @f
    stdcall ted_but_open_syntax, tedit0
  @@:
  cmp ah,201
  jne @f
    stdcall [ted_but_find_next], tedit0
  @@:

  cmp ah,1
  je .exit

  cmp ah,199
  je .exit
  
  jmp still
.exit:
	cmp dword[exit_code],1
	je @f
	stdcall [ted_can_save], tedit0
	cmp al,1
	jne @f
		stdcall [mb_create],msgbox_2,thread ;message: save buf in file?
		stdcall [mb_setfunctions],msgbox_2E_funct
		jmp still
	@@:
	stdcall mem.Free,[bmp_icon]
	cmp dword[unpac_mem],0
	je @f
		stdcall mem.Free,[unpac_mem]
	@@:
 
	stdcall [ted_delete], tedit0
	stdcall dword[tl_data_clear], tree1
	mcall -1 ;��室 �� �ணࠬ��

edit2 edit_box TED_PANEL_WIDTH-1, 0, 20, 0xffffff, 0xff80, 0xff0000, 0xff, 0x4080, 300, buf_find, mouse_dd, 0

unpac_mem dd 0

if lang eq ru
  head_f_i:
  head_f_l db '���⥬��� �訡��',0
  err_message_found_lib0 db '�� ������� ������⥪� ',39,'box_lib.obj',39,0
  err_message_import0 db '�訡�� �� ������ ������⥪� ',39,'box_lib.obj',39,0
  err_message_found_lib1 db '�� ������� ������⥪� ',39,'msgbox.obj',39,0
  err_message_import1 db '�訡�� �� ������ ������⥪� ',39,'msgbox.obj',39,0
  err_message_found_lib2 db '�� ������� ������⥪� ',39,'proc_lib.obj',39,0
  err_message_import2 db '�訡�� �� ������ ������⥪� ',39,'proc_lib.obj',39,0
  err_message_found_lib_3 db '�� ������� ������⥪� ',39,'libimg.obj',39,0
  err_message_import_3 db '�訡�� �� ������ ������⥪� ',39,'libimg.obj',39,0
  err_message_found_lib_4 db '�� ������� ������⥪� ',39,'libini.obj',39,0
  err_message_import_4 db '�訡�� �� ������ ������⥪� ',39,'libini.obj',39,0
  err_message_found_lib_5 db '�� ������� ������⥪� ',39,'libkmenu.obj',39,0
  err_message_import_5 db '�訡�� �� ������ ������⥪� ',39,'libkmenu.obj',39,0
else
  head_f_i:
  head_f_l db 'System error',0
  err_message_found_lib0 db 'Sorry I cannot found library ',39,'box_lib.obj',39,0
  err_message_import0 db 'Error on load import library ',39,'box_lib.obj',39,0
  err_message_found_lib1 db 'Sorry I cannot found library ',39,'msgbox.obj',39,0
  err_message_import1 db 'Error on load import library ',39,'msgbox.obj',39,0
  err_message_found_lib2 db 'Sorry I cannot found library ',39,'proc_lib.obj',39,0
  err_message_import2 db 'Error on load import library ',39,'proc_lib.obj',39,0
  err_message_found_lib_3 db 'Sorry I cannot found library ',39,'libimg.obj',39,0
  err_message_import_3 db 'Error on load import library ',39,'libimg.obj',39,0
  err_message_found_lib_4 db 'Sorry I cannot found library ',39,'libini.obj',39,0
  err_message_import_4 db 'Error on load import library ',39,'libini.obj',39,0
  err_message_found_lib_5 db 'Sorry I cannot found library ',39,'libkmenu.obj',39,0
  err_message_import_5 db 'Error on load import library ',39,'libkmenu.obj',39,0
end if

;library structures
l_libs_start:
	lib0 l_libs lib_name_0, sys_path, file_name, system_dir_0,\
		err_message_found_lib0, head_f_l, import_box_lib,err_message_import0, head_f_i
	lib1 l_libs lib_name_1, sys_path, file_name, system_dir_1,\
		err_message_found_lib1, head_f_l, import_msgbox_lib, err_message_import1, head_f_i
	lib2 l_libs lib_name_2, sys_path, file_name, system_dir_2,\
		err_message_found_lib2, head_f_l, import_proclib, err_message_import2, head_f_i
	lib3 l_libs lib_name_3, sys_path, file_name, system_dir_3,\
		err_message_found_lib_3, head_f_l, import_libimg, err_message_import_3, head_f_i
	lib4 l_libs lib_name_4, sys_path, file_name, system_dir_4,\
		err_message_found_lib_4, head_f_l, import_libini, err_message_import_4, head_f_i
	lib5 l_libs lib_name_5, sys_path, file_name, system_dir_5,\
		err_message_found_lib_5, head_f_l, import_libkmenu, err_message_import_5, head_f_i
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
IncludeUGlobals
	align 16
	procinfo process_information
		rb 1024
	thread:
	rb 1024
    thread_coords:
	rb 4096
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
