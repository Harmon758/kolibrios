;
;   Screenshooter for Kolibri
;
; version:	1.0
; last update:  08/09/2010
; written by:   Marat Zakiyanov aka Mario79, aka Mario
; changes:      select path for save with OpenDialog,
;               bag fix for threads stacks
;---------------------------------------------------------------------
; 01.06.09 - ���������� �������� �� ��⥬��� ������⥪� <Lrz>
; ���� �ணࠬ�� ���客 ���ᨬ (Maxxxx32)
; 24.07.2008 <Lrz> �������� editbox
; 01.02.07 - ������� editbox
; 31.01.07 - ��� ⥯��� ������ �⭮�⥫쭮 ������᪮� ������

title equ 'Screenshooter v 1.0' ; ��������� ����
include '../../develop/libraries/box_lib/load_lib.mac'
include '../../develop/libraries/box_lib/trunk/box_lib.mac'
;include '../../develop/examples/editbox/trunk/editbox.inc'
include '../../macros.inc'
include 'txtbut.inc'
include 'label.inc'
include 'textwork.inc'
include 'scrshoot.mac'

;include 'macros.inc'  ; ��⠢�塞 ������
;        meos_header  cmdstr ; ��⠢�塞 ��������� �ணࠬ��
use32		     
    org 0x0
    db 'MENUET01'
    dd 0x1
    dd start
    dd IM_END
    dd i_end
    dd stacktop
    dd cmdstr
    dd cur_dir_path

align 4
	@use_library
	use_txt_button				;  |
	use_label				;  |-- GUI ���������� � ������� ��楤���
	use_text_work				; /
include 'scrwin.inc'  ; ��⠢�塞 ��� ���� �य�ᬮ��
include 'scrsavef.inc'; ��⠢�塞 ��楤��� ��࠭���� 䠩��

include 'gp.inc'
include 'srectwin.inc'
include 'lang.inc'

macro get_sys_colors col_buf
{
	;mov     eax,48
	push	48
	pop	eax
	;mov     ebx,3
	push	3
	pop	ebx
	;mov     ecx,col_buf
	push	col_buf
	pop	ecx
	;mov     edx,40
	push	40
	pop	edx
	mcall
}

;--- ��砫� �ணࠬ�� ---
align 4
start:
;sys_load_library  library_name, cur_dir_path, library_path, system_path, \
;err_message_found_lib, head_f_l, myimport, err_message_import, head_f_i

load_libraries l_libs_start,end_l_libs

;if return code =-1 then exit, else nornary work
;        cmp     eax,-1
	inc	eax
	test	eax,eax
	jz	close
;;;;;;;;;;;;;;;; init memory 68/11
	mov	eax,68
	mov	ebx,11
	mcall
	test	eax,eax
	jz	close

;---------------------------------------------------------------------	
	mov	edi,filename_area
	mov	esi,start_temp_file_name
	xor	eax,eax
	cld
@@:
	lodsb
	stosb
	test	eax,eax
	jnz	@b


	mov	edi,fname_buf
	mov	esi,ed_buffer.1
	xor	eax,eax
	cld
@@:
	lodsb
	stosb
	test	eax,eax
	jnz	@b
	
;OpenDialog	initialisation
	push    dword OpenDialog_data
	call    [OpenDialog_Init]

; prepare for PathShow
	push	dword PathShow_data_1
	call	[PathShow_prepare]
;---------------------------------------------------------------------	
	;mov     al,[gs:1280*4*1024]
		    ; ��⠭�������� ipc �����
	push	60
	pop	eax
	;mov     ebx,1
	xor	ebx,ebx
	inc	ebx
	mov	ecx,app_ipc
	mov	edx,32
	mcall

	push	66   ; ��⠭�������� ��������� ������� �������
	pop	eax
	push	eax
	mov	ebx,4
	mov	cl,55	 ; 55 - PrintScrn
	xor	edx,edx
	mcall

	mov	ebx,app
	call	get_slot_n
	mov	[slot_n],ecx

	; ��⠭�������� ���� ᮡ�⨩
	set_events_mask (evm_redraw+evm_key+evm_button+evm_mouse+evm_ipc)

; � ��룠�� �� ������ ����ᮢ��
red:
	get_procinfo app   ; ����砥� ���ଠ�� � �����
	get_sys_colors sc  ; ����砥� ��⥬�� 梥�

	; ��⠭�������� ��⥬�� 梥� � GUI ��������⮢
	txt_but_set_sys_color buttons,buttons_end,sc		 ; \
	labels_set_sys_color labels,labels_end,sc		 ; |
	check_boxes_set_sys_color check_boxes,check_boxes_end,sc ; |
	edit_boxes_set_sys_color editboxes,editboxes_end,sc	 ; /

	get_screen_prop scr ; ����砥� ���ଠ�� �� �࠭�
	call	draw_window ; ����ᮢ뢠�� ����
still:
	wait_event red,key,button,mouse,ipc,still ; ���� ᮡ�⨩

key:
	get_key
	cmp	al,2
	jne	@f
	cmp	ah,55
	jne	still
	mov	[PrintScreen],1
	call	shoot
	mov	[PrintScreen],0
	jmp	still
@@:
;        key_edit_boxes editboxes,editboxes_end
;	push	dword edit1
;	call	[edit_box_key]

	push	dword edit2
	call	[edit_box_key]

	push	dword edit3
	call	[edit_box_key]

	push	dword edit4
	call	[edit_box_key]

	jmp	still
button:
	get_pressed_button
	cmp	ah,1
	je	p_close
	txt_but_ev buttons,buttons_end
	jmp	still
mouse:
	get_active_window
	cmp	eax,[slot_n]
	jne	still
;----------------------------------
;	push	dword edit1
;	call	[edit_box_mouse]
	push	dword edit2
	call	[edit_box_mouse]
	push	dword edit3
	call	[edit_box_mouse]
	push	dword edit4
	call	[edit_box_mouse]
;----------------------------------
	push	dword ch1
	call	[check_box_mouse]
	push	dword ch2
	call	[check_box_mouse]
	push	dword ch3
	call	[check_box_mouse]
	push	dword ch4
	call	[check_box_mouse]
	push	dword ch5
	call	[check_box_mouse]
	push	dword ch6
	call	[check_box_mouse]
	push	dword use_rect
	call	[check_box_mouse]
;-----------------------------------
;        mouse_check_boxes check_boxes,check_boxes_end
;        mouse_edit_boxes editboxes,editboxes_end
	jmp	still
ipc:
	cmp	word [app_ipc+8],2
	jne	@f
	min_window
	jmp	.clear_ipc
@@:
	cmp	word [app_ipc+8],3
	jne	@f
	mov	ecx,[slot_n]
	activ_window
	jmp	.clear_ipc
@@:
	call	draw_number
	call	dr_st

.clear_ipc:
	cld
	xor	eax,eax
	mov	ecx,32/4
	mov	edi,app_ipc
	rep	stosd
	jmp	still

p_close:
	btr	dword [flags],1

	bt	dword [flags],3
	jnc	@f
	mov    eax,18
	mov	ebx,eax
	mov	ecx,[set_rect_window_pid]
	mcall
@@:

close:
	app_close

;---------------------------------------------------------------------
draw_PathShow:
	pusha
	mcall	13,<4,302>,<35,15>,0xffffff
; draw for PathShow
	push	dword PathShow_data_1
	call	[PathShow_draw]
	popa
	ret
;--------------------------------------------------------------------
;--- �ம楤�� ����ᮢ�� �������� ���� ---------------------------
;--------------------------------------------------------------------
draw_window:
start_draw_window	 ; ��砫� ����ᮢ��
	; ��।��塞 ����
	get_skin_height
	mov	ebx,100*65536+320
	mov	ecx,100*65536+220
	add	cx,ax
	mov	edx,[sc.work]
	or	edx,0x34000000;0x33000000
	xor	esi,esi
	;xor     edi,edi
	mov	edi,grab_text
	xor	eax,eax
	mcall

	mov	eax,47		   ; �뢮��� �᫠ � ����
	mov	bx,4
	shl	ebx,16
	movsx	ecx,word [scr.width] ; �ਭ� �࠭�
	mov	dx,95
	shl	edx,16
	mov	dx,[label1.top]
	push	dx
	mov	esi,[sc.work_text]
	mcall
	movsx	ecx,word [scr.height]	; ���� �࠭�
	mov	dx,[label2.top]
	mcall
	mov	bx,2
	shl	ebx,16
	movsx	ecx,word [scr.bitspp]	; ��� �� ���ᥫ�
	mov	dx,240
	shl	edx,16
	pop	dx
	mcall
	mov	bx,6
	shl	ebx,16
	mov	ecx,[scr.bytesps]	; ���� �� ��ப�
	mov	dx,[label2.top]
	mcall

	call	draw_number	 ; ���ᮢ뢠�� ����� ᭨���
	call	dr_st

	draw_labels labels,labels_end		 ; ��⪨
;        draw_edit_boxes editboxes,editboxes_end  ; edit_box
;------ show check editbox -----------
;	push	dword edit1
;	call	[edit_box_draw]
	push	dword edit2												     
	call	[edit_box_draw]
	push	dword edit3
	call	[edit_box_draw]
	push	dword edit4
	call	[edit_box_draw]
;------ end check all editbox -------

	call	draw_PathShow

	draw_txt_buttons buttons,buttons_end	 ; ������
;        draw_check_boxes check_boxes,check_boxes_end ; 䫠���
;------ check all checkbox ---------
	push	dword ch1
	call	[check_box_draw]
	push	dword ch2
	call	[check_box_draw]	
	push	dword ch3
	call	[check_box_draw]
	push	dword ch4
	call	[check_box_draw]
	push	dword ch5
	call	[check_box_draw]
	push	dword ch6
	call	[check_box_draw]
	push	dword use_rect
	call	[check_box_draw]
;------ end check all checkbox ------

stop_draw_window      ; ����� ����ᮢ��
ret

shoot:
	bt	dword [ch4.flags],1   ; ����祭� �� ����প� ?
	jnc	@f
	draw_status delay_now
	mov	edi,ed_buffer.2
	call	zstr_to_int
	mov	ebx,eax
	delay
@@:
	call	get_mem_for_shoot; ����砥� ������ ��� ᭨���


	bts	dword [flags],0       ; �㫥��� 䫠� - ᭨��� ���� � �����

	; ������ ᭨���
	draw_status shooting
	mov	esi,scr
	mov	edi,[scr_buf.ptr]
	call	copy_screen_to_image

	bt	dword [use_rect.flags],1
	jc	.use_rect
	push	dword [scr]
	jmp	@f
.use_rect:
	push	dword [rect.height]
@@:
	pop	dword [scr_buf.size]

	cmp	[autoshoot_flag],1
	jne	.2
	
	bt	dword [ch5.flags],1  ; ����祭� �� �����࠭���� ?
	jnc	@f
	call	save_file
@@:
	draw_status	shooted_ok
.2:
	bt	dword [flags],1
	jnc	@f
	ret
@@:

	bt	dword [ch2.flags],1  ; �������� ���� �।��ᬮ�� ?
	jnc	@f
	cmp	[PrintScreen],0
	jne	@f
	call	show_scr_window
	ret
@@:
	bt	dword [ch5.flags],1  ; ����祭� �� �����࠭���� ?
	jnc	@f
	
; invoke OpenDialog
	push    dword OpenDialog_data
	call    [OpenDialog_Start]
	cmp	[OpenDialog_data.status],1
	je	.1
	ret
.1:
; prepare for PathShow
	push	dword PathShow_data_1
	call	[PathShow_prepare]

	call	save_file
@@:
	
	bt	word [ch3.flags],1   ; ����⠭����� ���� ?
	jnc	@f

	delay 100
	mov	ecx,[slot_n]
	activ_window
@@:
ret

;--- ������� ������ ��� ᭨��� ---
get_mem_for_shoot:
;clean memory
	mov	ecx,dword [scr_buf.ptr]
	mov	ebx,13
	mov	eax,68
	mcall

	bt	dword [use_rect.flags],1 ; �⪠�� ������� �࠭�
	jc	.use_area
	movzx	ecx, word [scr.width]
	movzx	ebx, word [scr.height]
	jmp	@f
.use_area:
	call	read_rect
	movzx	ecx, word [rect.width]
	movzx	ebx, word [rect.height]
@@:
	imul	ecx,ebx
	lea	ecx,[ecx*3]
;        add     ecx,i_end
	mov	ebx,12
	mov	eax,68
	mcall
	mov	[scr_buf.ptr],eax
	add	eax,ecx
	mov	[scr_buf.end_ptr],ecx
ret

;--- ᮤ��� ���� �஥���ᬮ�� ---
show_scr_window:
pusha
	bt	dword [flags],0
	jnc	@f
	mov	eax,51
	xor	ebx,ebx
	inc	ebx
	mov	ecx,scr_window
	mov	edx,i_end_tread-512
	mcall
@@:
popa
ret

;--- ������ � ���ᮢ��� ⥪�騩 ����� ---
apply_number:
	mov	edi,ed_buffer.3
	call	zstr_to_int
	mov	[cur_number],eax
	call	draw_number
ret

;--- ��楤�� ���ᮢ�� ⥪�饣� ����� ---
draw_number:
	mov	edi,sign_n_input
	call	zstr_to_int
	mov	[sign_n],al
	mov	eax,13
	mov	ebx,150*65536+96
	mov	cx,[label9.top]
	shl	ecx,16
	mov	cx,10
	mov	edx,[sc.work]
	mcall
	mov	eax,47
	movsx	bx,byte [sign_n]
	shl	ebx,16
	mov	ecx,[cur_number]
	mov	dx,150
	shl	edx,16
	mov	dx,[label9.top]
	mov	esi,[sc.work_text]
	mcall
ret

;--- ��楤��, ����᪠��� ��⮪ ����ꥬ�� ---
start_autoshoot:
	bts	dword [flags],1
	jc	@f
	mov	eax,51
	;mov     ebx,1
	xor	ebx,ebx
	inc	ebx
	mov	ecx,autoshoot
	mov	edx,i_end_tread
	mcall
@@:
ret

;--- ��⠭�������� ��� ��⮪ ---
stop_autoshoot:
	btr	dword [flags],1
ret

;--- 横� ��⮪� ����ꥬ�� ---
autoshoot:
	mov	[autoshoot_flag],1
	mov	ecx,[slot_n]
	activ_window
.next:
	bt	dword [flags],1
	jnc	close
	mov	eax,60
	mov	ebx,2
	mov	ecx,[app.pid]
	mov	edx,messages.draw_number
	mov	esi,2
	mcall
	call	shoot
	jmp	autoshoot.next
.close:
	mov	[autoshoot_flag],0
	jmp	close
;--- ��楤�� ���ᮢ�� ��ப� ���ﭨ� ---
; (������ ��뢠���� ��⮪�� �������� ����)
dr_st:
	mov	eax,38		    ; ��१��
	mov	ebx,0*65536+310
	mov	ecx,198*65536+198
	mov	edx,[sc.work_graph]
	mcall

	mov	eax,13		    ; ����᪠
	mov	bx,310
	mov	ecx,199*65536+15
	mov	edx,[sc.work]
	mcall

	mov	edi,status
	call	draw_label
ret

;--- ��楤�� ����祭�� ����� ᫮� ��⮪� ---
; �室 ebx - ����� 1024 ����
; ��室 ecx - ����� ᫮�
get_slot_n:
	mov	eax,9
	xor	ecx,ecx
	dec	ecx
	mcall

	mov	edx,[ebx+30]
	xor	ecx,ecx
@@:
	mov	eax,9
	inc	ecx
	mcall
	cmp	[ebx+30],edx
	je	@f
	jmp	@b
@@:
ret

;--- ��楤��, ����᪠��� ��⮪, �����騩 1 ᭨��� ---
one_shoot:
	mov	ecx,one_shoot_thread
	mov	edx,shoot_esp
	jmp	@f
;--- ��楤�, ����᪠��� ��⮪, ��࠭��騩 ᭨��� ---
save_shoot:
; invoke OpenDialog
	push    dword OpenDialog_data
	call    [OpenDialog_Start]
	cmp	[OpenDialog_data.status],1
	je	.1
	ret
.1:
; prepare for PathShow
	push	dword PathShow_data_1
	call	[PathShow_prepare]

	call	draw_PathShow

	mov	ecx,save_shoot_thread
	mov	edx,shoot_esp
@@:
	bts	dword [flags],2
	jc	.running
	bt	dword [flags],1
	jc	.running

	mov	eax,51
	;mov     ebx,1
	xor	ebx,ebx
	inc	ebx
	mcall
.running:
ret

;--- ��⮪, �����騩 1 ᭨��� ---
one_shoot_thread:
	mov	ecx,[slot_n]
	activ_window
	bt	dword [ch1.flags],1   ; ��������஢��� ���� ?
	jnc	 @f
	mov	eax,60
	mov	ebx,2
	mov	ecx,[app.pid]
	mov	edx,messages.min_window
	mov	esi,2
	mcall
@@:
	call	shoot
	btr	dword [flags],2
	jmp	close

;--- ��楤��, ��ࠢ����� �������� ���� ᮮ�饭�� � ����ᮢ��
; ��ப� ���ﭨ� ---
send_draw_status:
	mov	eax,60
	mov	ebx,2
	mov	ecx,[app.pid]
	mov	edx,messages.draw_status
	mov	esi,2
	mcall
ret

;--- ��⮪, ��࠭� 䠩� ---
save_shoot_thread:
	mov	ecx,[slot_n]
	activ_window
	call	save_file
	btr	dword [flags],2
	jmp	close

;--- ��楤��, ����᪠��� ��⮪ ���� ��⠭���� ������ �ꥬ�� ---
show_set_rect_window:
	bts	dword [flags],3
	jc	@f
	mov	eax,51
	xor	ebx,ebx
	inc	ebx
	mov	ecx,set_rect_window
	mov	edx,set_rect_window_esp
	mcall

	mov	[set_rect_window_pid],eax
ret

@@:
	mov	eax,18
	mov	ebx,eax
	mov	ecx,[set_rect_window_pid]
	mcall
	btr	dword [flags],3
ret

;--- ����祭�� ���ଠ樨 �� ��⨢��� ���� ---
get_active_window_info:
	mov    eax,18
	mov    ebx,7
	mcall

	mov	ecx,eax
	mov	eax,9
	mov	ebx,active_app
	mcall
ret

;====================================================================
;=== ����� �ணࠬ�� ===============================================
;====================================================================
messages:
.draw_number dw 0
.draw_status dw 1
.min_window  dw 2
.act_window  dw 3

grab_text:
	db	title,0

labels:
label1 label 5,5,0,text.1   ; �ਭ� �࠭�
label2 label 5,15,0,text.2   ; ���� �࠭�
label3 label 5,25,0,text.3   ; ������ ��� 䠩��
label4 label 150,5,0,text.4  ; ��� �� ���ᥫ�
label5 label 150,15,0,text.5  ; ���� �� ��ப�
label6 label 115,138,0,text.6  ; 100 = 1 ᥪ.
;label7 label 10,190,0,text.7
;label8 label 10,225,0,text.8
label9 label 5,52,0,text.9   ; ����� ⥪�饣� ᨬ��
label10 label 5,185,0,text.10
status label 5,201,0,no_shoot
labels_end:

;---------------------------------------------------------------------
l_libs_start:

library01  l_libs system_dir_Boxlib+9, cur_dir_path, library_path, system_dir_Boxlib, \
err_message_found_lib1, head_f_l, Box_lib_import, err_message_import1, head_f_i

library02  l_libs system_dir_ProcLib+9, cur_dir_path, library_path, system_dir_ProcLib, \
err_message_found_lib2, head_f_l, ProcLib_import, err_message_import2, head_f_i

end_l_libs:
;---------------------------------------------------------------------
editboxes:
;edit1 edit_box 300,5,35,cl_white,0,0,0,0,300,ed_buffer.1,mouse_dd,ed_focus,10,10	; ���� � 䠩��
edit2 edit_box 35,75,134,cl_white,0,0,0,0,9,ed_buffer.2,mouse_dd,ed_figure_only,3,3	    ; ����প�
edit3 edit_box 35,165,164,cl_white,0,0,0,0,9,ed_buffer.3,mouse_dd,ed_figure_only    ; ��⮭㬥���
edit4 edit_box 16,165,181,cl_white,0,0,0,0,1,sign_n_input,mouse_dd,ed_figure_only,1
editboxes_end:
;---------------------------------------------------------------------
buttons:
but1 txt_button 150,5,15,65,2,0,0,but_text.1,one_shoot		   ; ᤥ���� ᭨���
but2 txt_button 145,160,15,65,3,0,0,but_text.2,save_shoot	   ; ��࠭��� ᭨���
but3 txt_button 140,115,12,120,4,0,0,but_text.3,show_scr_window    ; �������� ᭨���
but4 txt_button 80,205,15,163,5,0,0,but_text.4,apply_number	   ; �ਬ����� �����
but5 txt_button 150,5,15,85,6,0,0,but_text.5,start_autoshoot	   ; ����� �������
but6 txt_button 145,160,15,85,7,0,0,but_text.6,stop_autoshoot	   ; ��⠭����� �������
but7 txt_button 40,205,10,150,8,0,0,but_text.7,show_set_rect_window ; ������ �������
buttons_end:
;---------------------------------------------------------------------
check_boxes:
ch1 check_box 5,105,5,11,cl_white,0,0,ch_text.1,(ch_text.2-ch_text.1),ch_flag_en  ; ᢥ���� ����
ch2 check_box 5,120,5,11,cl_white,0,0,ch_text.2,(ch_text.3-ch_text.2),ch_flag_en  ; ��⥬ ᤥ���� ��⨢��
ch3 check_box 145,105,5,11,cl_white,0,0,ch_text.3,(ch_text.4-ch_text.3),ch_flag_en ; �������� ᭨���
ch4 check_box 5,135,5,11,cl_white,0,0,ch_text.4,(ch_text.5-ch_text.4),ch_flag_en   ; ����প�
ch5 check_box 5,150,5,11,cl_white,0,0,ch_text.5,(ch_text.6-ch_text.5),ch_flag_en
ch6 check_box 5,165,5,11,cl_white,0,0,ch_text.6,(ch_text.7-ch_text.6),ch_flag_en
use_rect check_box 145,150,5,11,cl_white,0,0,ch_text.7,(ch_text.8-ch_text.7) ; ��. �������
; ��⮭㬥���
check_boxes_end:
;---------------------------------------------------------------------
if lang eq ru
text:
.2 db '���� �࠭�:',0
.1 db '��ਭ� �࠭�:',0
.3 db '������ ����� ���� � 䠩��:',0
.4 db '��� �� ���ᥫ�:',0
.5 db '���� �� ��ப�:',0
.6 db '100 = 1 ᥪ㭤�',0
;.7: db '������ ��� 䠩��:',0
;.8: db '����� "*" � ����� 䠩�� �㤥� ��⠢������ �����.',0
.9 db '����� ⥪�饣� ᭨���:',0
.10 db '������⢮ ������ � �����:',0

but_text:
.1 db '������� ᭨��� �࠭�',0
.2 db '���࠭��� ᭨��� �࠭�',0
.3 db '�������� ᭨��� ᥩ��',0
.4 db '�ਬ�����',0
.5 db '����� ����ꥬ��',0
.6 db '��⠭����� ����ꥬ��',0
.7 db '������',0

ch_text:
.1 db '������� ����';,0
.2 db '�������� ᭨���';,0
.3 db '��⥬ ᤥ���� ��⨢��';,0
.4 db '����প�:';,0
.5 db '�����࠭����';,0
.6 db '��⮭㬥���, ��稭�� �';,0
.7 db '�������'
.8:

no_shoot db '������ �� ᤥ���',0
shooting db '��⮣��஢����...',0
shooted_ok db '������ ᤥ���',0
saving db '���࠭����...',0
saved_ok db '������ ��࠭��',0
delay_now db '����প�...',0
bad_file_name db '��� 䠩�� ������� ����୮',0
disk_filled db '��� ��������',0
bad_fat_table db '������ FAT ࠧ��襭�',0
ac_den db '����� ����饭',0
device_er db '�訡�� ���ன�⢠',0
not_shooted db '������� ᭨��� !!!',0
no_file_name db '������ ��� 䠩�� !!!',0
invalid_rect db '�������⨬� ࠧ���� ������',0

else
text:
.2 db 'Screen width:',0
.1 db 'Screen height:',0
.3 db 'Enter full path to the file:',0
.4 db 'Bits per pixel:',0
.5 db 'Bytes per line:',0
.6 db '100 = 1 second',0
;.7: db 'Enter name of file:',0
;.8: db 'Instead of "*" in filename the number will be included.',0
.9 db 'Current photo number:',0
.10 db 'Amount of digits in number:',0

but_text:
.1 db 'Make screen photo',0
.2 db 'Save screen photo',0
.3 db 'Show photo now',0
.4 db 'Apply',0
.5 db 'Start autoshooting',0
.6 db 'Stop autoshooting',0
.7 db 'Set',0

ch_text:
.1 db 'Minimize window';,0
.2 db 'Show photo';,0
.3 db 'then make active';,0
.4 db 'Delay:';,0
.5 db 'Autosave';,0
.6 db 'Start numeration from';,0
.7 db 'Area'
.8:

no_shoot db 'There is no photo',0
shooting db 'Photographing...',0
shooted_ok db 'Photo created',0
saving db 'Saving...',0
saved_ok db 'Photo saved',0
delay_now db 'Delay...',0
bad_file_name db 'File name is wrong',0
disk_filled db 'Disk is full',0
bad_fat_table db 'FAT table destroyed',0
ac_den db 'Access denied',0
device_er db 'Device error',0
not_shooted db 'Make a photo !!!',0
no_file_name db 'Enter file name !!!',0
invalid_rect db 'Wrong area size',0

end if

;---------------------------------------------------------------------
PathShow_data_1:
.type			dd 0	;+0
.start_y		dw 38	;+4
.start_x		dw 6	;+6
.font_size_x		dw 6	;+8	; 6 - for font 0, 8 - for font 1
.area_size_x		dw 300	;+10
.font_number		dd 0	;+12	; 0 - monospace, 1 - variable
.background_flag	dd 0	;+16
.font_color		dd 0x0	;+20
.background_color	dd 0x0	;+24
.text_pointer		dd fname_buf	;+28
.work_area_pointer	dd text_work_area	;+32
.temp_text_length	dd 0	;+36
;---------------------------------------------------------------------
OpenDialog_data:
.type			dd 1	; Save
.procinfo		dd procinfo	;+4
.com_area_name		dd communication_area_name	;+8
.com_area		dd 0	;+12
.opendir_pach		dd temp_dir_pach	;+16
.dir_default_pach	dd communication_area_default_pach	;+20
.start_path		dd open_dialog_path	;+24
.draw_window		dd draw_window	;+28
.status			dd 0	;+32
.openfile_pach 		dd fname_buf	;+36
.filename_area		dd filename_area	;+40
.filter_area		dd Filter
.x:
.x_size			dw 420 ;+48 ; Window X size
.x_start		dw 10 ;+50 ; Window X position
.y:
.y_size			dw 320 ;+52 ; Window y size
.y_start		dw 10 ;+54 ; Window Y position

communication_area_name:
	db 'FFFFFFFF_open_dialog',0
open_dialog_path:
	db '/sys/File Managers/opendial',0
communication_area_default_pach:
	db '/sys',0

Filter:
dd	Filter.end - Filter
.1:
db	'BMP',0
.end:
db	0

start_temp_file_name:	db '1.bmp',0

;---------------------------------------------------------------------
sign_n_input:
	db	'2',0

PrintScreen	db  0
autoshoot_flag	db  0
	
app_ipc ipc_buffer 32
align 4

mouse_flag: dd 0x0
;---------------------------------------------------------------------
align 4

ed_buffer:
.1: db '/sys/1.bmp',0
;rb 287
.2:
	db '100',0
	rb 6
.3:
	rb 10
;---------------------------------------------------------------------	
IM_END:
;---------------------------------------------------------------------
structure_of_potock:
rb 100
;---------------------------------------------------------------------
align 4

cur_number	  dd	  ?

sign_n	      db      ?

slot_n	      dd ?

flags	     dd ?

scr_buf:
.ptr	    dd	    ?
.end_ptr	dd	?
.size:
.height        dw      ?
.width	      dw      ?

fs_struc:
.funk_n        dd      ?
	dd	?
	dd	?
.bytes_to_write  dd	 ?
.data_ptr	 dd    ?,?
.fn_ptr        dd      ?

sf_buf:
.bmp_header   dd      ?
.bmp_area     dd      ?
.end	      dd      ?

set_rect_window_pid dd ?
set_rect_window_slot dd ?
;---------------------------------------------------------------------
align 4
rect_input_buffer:
.left rb 6
.top  rb 6
.width	rb 6
.height rb 6

cmdstr rb 257
;---------------------------------------------------------------------
align 4

file_name:
	rb	1058

scr screen_prop
rect:
.left dw ?
.top dw ?
.height dw ?
.width dw ?

sc sys_color_table
app procinfo	    ; ���ଠ�� � ������� ����
active_app procinfo ; ���ଠ�� �� ��⨢��� ����
set_rect_window_procinfo procinfo  ; ���ଠ�� �� ���� ������
;---------------------------------------------------------------------
	rb 512		   ; �⥪ ��⮪� �⪠���
shoot_esp:
;---------------------------------------------------------------------
	rb 512	   ; �⥪ ���� ������
set_rect_window_esp:
;---------------------------------------------------------------------
;        app_end    ; ����� �ணࠬ��
mouse_dd	rd 1
;---------------------------------------------------------------------
align 4
cur_dir_path:
	rb 4096
;---------------------------------------------------------------------
library_path:
	rb 4096
;---------------------------------------------------------------------
temp_dir_pach:
	rb 4096
;---------------------------------------------------------------------
text_work_area:
	rb 1024
;---------------------------------------------------------------------
fname_buf:
	rb 4096
;---------------------------------------------------------------------
procinfo:
	rb 1024
;---------------------------------------------------------------------
filename_area:
	rb 256
;---------------------------------------------------------------------
	rb 1024
i_end_tread:
;---------------------------------------------------------------------
	rb 1024
stacktop:
;---------------------------------------------------------------------
i_end: