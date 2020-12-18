use32
  org 0
  db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
  dd 1, start, i_end, mem, stacktop, file_name, sys_path

MAX_COLOR_WORD_LEN equ 40
BUF_SIZE equ 4096 ;buffer for copy|paste
CAPT_PATH_WIDTH equ 50 ;�ਭ� ������ ��। ⥪�⮢� �����

include '../../macros.inc'
include '../../proc32.inc'
include '../../KOSfuncs.inc'
include '../../load_img.inc'
include '../../load_lib.mac'
include '../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../system/skincfg/trunk/kglobals.inc'
include '../../system/skincfg/trunk/unpacker.inc'
include 'te_data.inc'
include 'te_work.inc' ;text work functions

@use_library mem.Alloc,mem.Free,mem.ReAlloc,dll.Load

icon_tl_sys dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ��⥬��� ������

align 4
start:
	mcall SF_STYLE_SETTINGS,SSF_GET_COLORS,sc,sizeof.system_colors

	mcall SF_SYS_MISC,SSF_HEAP_INIT
	or eax,eax
	jz button.exit

	mcall SF_KEYBOARD,SSF_SET_INPUT_MODE,1 ;scan code
	mcall SF_SET_EVENTS_MASK,0xC0000027

	load_libraries l_libs_start,load_lib_end

	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ������⥪�
	cmp dword[lib0+ll_struc_size-4],0
	jz @f
		mcall -1 ;exit not correct
	@@:

;---------------------------------------------------------------------
	stdcall [tl_data_init], tree1

; �⠥� 䠩� � ����ࠬ� � ����ﬨ
	include_image_file '..\..\media\log_el\trunk\tl_sys_16.png', icon_tl_sys
	mov eax,[icon_tl_sys]
	mov [tree1.data_img_sys],eax
;---------------------------------------------------------------------
; �⠥� bmp 䠩� � �������� 㧫��
	include_image_file '..\t_edit\tl_nod_16.png', icon_tl_sys
	mov eax,[icon_tl_sys]
	mov [tree1.data_img],eax
;------------------------------------------------------------------------------
	copy_path fn_syntax_dir,sys_path,file_name,0 ;��६ ���� � ����� � 䠩���� ᨭ⠪��
	mcall SF_FILE,tree_file_struct

	cmp ebx,2
	jg @f
		notify_window_run file_name ;ᮮ�饭�� �᫨ �� 㤠���� ������ 䠩��
		jmp .end_dir_init
	@@:
	;cmp ebx,0
	;jle .end_dir_init
		mov eax,dir_mem+32+40
		mov ecx,ebx
		@@:
			cmp byte[eax],'.' ;䨫���㥬 䠩�� � ������� '.' � '..'
			je .filter
			stdcall [tl_node_add],tree1,0x10000,eax ;1*2^16 - ��� 1 ����� ������ � ������
			stdcall [tl_cur_next],tree1
		.filter:
			add eax,304
		loop @b
		stdcall [tl_cur_beg],tree1 ;�⠢�� ����� �� ��砫� ᯨ᪠
		or dword[tree1.style], tl_cursor_pos_limited ;��࠭�稢��� �������� ����� � �।���� ᯨ᪠
	.end_dir_init:

	xor eax,eax
	inc eax
	mov [scrol_w1.type],eax
	mov [scrol_h1.type],eax
	mov [ws_dir_lbox.type],eax

;--- load color option file ---
stdcall [ted_init], tedit0
mov byte[file_name],0

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
  cmp al,6
  jz mouse

  jmp still


align 4
draw_window:
	pushad
	mcall SF_REDRAW,SSF_BEGIN_DRAW

	mov edx,[sc.work]
	or  edx,0x33000000
	mov edi,hed
	mcall SF_CREATE_WINDOW,<10,555>,<10,333>

	mcall SF_THREAD_INFO,procinfo,-1
	mov edi,tedit0 ;���祭�� edi �㦭� ��� EvSize � ted_wnd_t
	call EvSize

	mov esi,[sc.work_button];0xd0
	mcall SF_DEFINE_BUTTON,5*65536+90,195*65536+20,200

	mov ebx,100*65536+85
	mov ecx,195*65536+20
	mov edx,201
	mov esi,0xd00000
	mcall

	mov ecx,[sc.work_button_text]
	or  ecx,0x80000000
	mcall SF_DRAW_TEXT,10*65536+200,,txt_load_f

	mov ecx,0xffff00
	or  ecx,0x80000000
	mcall ,105*65536+200,,txt_save_f

	mov ecx,[sc.work_text]
	or  ecx,0x80000000
	mcall ,195*65536+10,,txt_inp_file

	add ebx,20
	mov edx,txt_out_file
	int 0x40

	stdcall [PathShow_draw], PathShow_data_1
	
	stdcall [edit_box_draw], edit1
	stdcall [tl_draw], tree1

	;scroll 1
	mov [ws_dir_lbox.all_redraw],1
	stdcall [scrollbar_ver_draw],ws_dir_lbox
	stdcall [ted_draw], tedit0

	mcall SF_REDRAW,SSF_END_DRAW
	popad
	ret

MIN_M_WND_H equ 100 ;�������쭠� ���� �������� ����
;input:
; edi = pointer to tedit struct
align 4
EvSize:
	pushad
	mov ebx,ted_scr_h
	mov esi,ted_scr_w

	m2m ted_wnd_w,[procinfo.client_box.width] ;�⠢�� �ਭ� ���� ।���� ࠢ��� �ਭ� �ᥣ� ����
	mov eax,ted_wnd_l
	sub ted_wnd_w,eax ;�⭨���� ����� ᫥��
	movzx eax,word[esi+sb_offs_size_x]
	sub ted_wnd_w,eax ;�⭨���� �ਭ� ����. �஫�����

	m2m ted_wnd_h,[procinfo.client_box.height] ;�⠢�� ����� ���� ।���� ࠢ��� ���� �ᥣ� ����
	cmp ted_wnd_h,MIN_M_WND_H
	jg @f
		mov ted_wnd_h,MIN_M_WND_H
	@@:

	movzx eax,word[ebx+sb_offs_size_y]
	sub ted_wnd_h,eax	      ;�⭨���� ����� ��ਧ. �஫�����
	mov eax,ted_wnd_t
	sub ted_wnd_h,eax	      ;�⭨���� ����� ᢥ���

	stdcall [ted_init_scroll_bars], tedit0,2
	popad
	ret

align 4
mouse:
	stdcall [tl_mouse],tree1
	stdcall [edit_box_mouse],edit1
	stdcall [ted_mouse], tedit0
	jmp still

;output:
; ah = symbol
align 4
proc KeyConvertToASCII, table:dword
	push ebx
	mov ebx,dword[table] ;convert scan to ascii
	shr ax,8
	add bx,ax
	mov ah,byte[ebx]
	pop ebx
	ret
endp

align 4
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
	stdcall [tl_key],tree1

	test word [edit1.flags],10b;ed_focus ; �᫨ �� � 䮪��, ��室��
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
		stdcall [edit_box_key],edit1
		jmp still
	@@:

	stdcall [ted_key], tedit0, conv_tabl,esi
	jmp still

align 4
button:
	mcall SF_GET_BUTTON

	cmp ah,200
	jne @f
		call but_OpenSyntax
	@@:
	cmp ah,201
	jne @f
		call but_SaveSyntax
	@@:

	cmp ah,1
	jne still
.exit:
	stdcall mem.Free,[options_file]
	stdcall mem.Free,[unpac_mem]

	stdcall [tl_data_clear], tree1
	stdcall [ted_delete], tedit0
	mcall SF_TERMINATE_PROCESS ;��室 �� �ணࠬ��

align 4
but_OpenSyntax:
push eax
	stdcall [tl_node_get_data],tree1
	mov [fn_col_option],eax
	call InitColText
pop eax
	ret

align 4
but_SaveSyntax:
	stdcall [ted_save_file], tedit0,run_file_70,[edit1.text]
	ret

;description:
; �㭪�� ��뢠���� �� ����⨨ Ctrl+N,O,F,S,H,G
align 4
proc ted_but_ctrl_all uses eax, opt_key:dword
	mov eax,[opt_key]
	cmp al,'N' ;Ctrl+N
	jne @f
		call but_ctrl_n
		jmp .end0
	@@:
	cmp al,'O' ;Ctrl+O
	jne @f
		call but_ctrl_o
	@@:
	;cmp al,'S' ;Ctrl+S
	;cmp al,'F' ;Ctrl+F
	;cmp al,'G' ;Ctrl+G
	;cmp al,'H' ;Ctrl+H
	.end0:
	ret
endp

align 4
but_ctrl_o:
	push eax
	call get_wnd_in_focus
	or eax,eax
	jz @f
		stdcall [ted_open_file], eax,str_file_70,[edit1.text]
	@@:
	pop eax
	ret

;ᮧ����� ������ 䠩��
align 4
but_ctrl_n:
	push eax
	call get_wnd_in_focus
	or eax,eax
	jz @f
		stdcall [ted_clear], eax,1
		stdcall [ted_draw], eax
	@@:
	pop eax
	ret

;��।��塞 ����� �� ���� ।���� � 䮪��
align 4
get_wnd_in_focus:
	xor eax,eax
	cmp dword[el_focus],tedit0
	jne @f
		mov eax,tedit0
	@@:
	;cmp dword[el_focus],tedit1
	;jne @f
	;       mov eax,tedit1
	;@@:
	ret

hed db 'TextEditor syntax file converter 18.12.20',0 ;������� ����
conv_tabl rb 128 ; ⠡��� ��� �������஢���� scan-���� � ascii-���

txt_load_f db '����. 䠩�',0
txt_save_f db '����. 䠩�',0
txt_inp_file db '���. 䠩�:',0
txt_out_file db '���. 䠩�:',0

;library structures
l_libs_start:
	lib0 l_libs lib_name_0, file_name, system_dir_0, import_box_lib
	lib1 l_libs lib_name_1, file_name, system_dir_1, import_libimg
load_lib_end:

IncludeIGlobals

align 16
i_end:
IncludeUGlobals
	procinfo process_information
	rb 1024
thread:
	rb 1024
stacktop:
	sys_path rb 4096
	file_name rb 4096
	file_name_rez rb 4096
mem:
