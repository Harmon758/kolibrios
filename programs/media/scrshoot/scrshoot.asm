; ���� �ணࠬ�� ���客 ���ᨬ (Maxxxx32)
; ��� ��᫥����� ���������: 20.07.06 18:05
macro draw_status text_ptr
{
	mov	[status.text],dword text_ptr
	call	send_draw_status
}


title equ 'Maxxxx32 Screenshooter v 0.78' ; ��������� ����
include 'lang.inc'
include 'macros.inc'  ; ��⠢�塞 ������
	meos_header  cmdstr ; ��⠢�塞 ��������� �ணࠬ��
include 'scrwin.inc'  ; ��⠢�塞 ��� ���� �य�ᬮ��
include 'scrsavef.inc'; ��⠢�塞 ��楤��� ��࠭���� 䠩��
	use_edit_box   ; \
	use_txt_button ;  |
	use_label      ;  |
	use_gp	       ;  |-- GUI ���������� � ������� ��楤���
	use_check_box  ;  |
	use_text_work  ; /
;--- ��砫� �ணࠬ�� ---
	app_start
		    ; ��⠭�������� ipc �����
	push	60
	pop	eax
	;mov     ebx,1
	xor	ebx,ebx
	inc	ebx
	mov	ecx,app_ipc
	mov	edx,32
	int	0x40

	push	66   ; ��⠭�������� ��������� ������� �������
	pop	eax
	push	eax
	mov	ebx,4
	mov	cl,55	 ; 55 - PrintScrn
	xor	edx,edx
	int	0x40

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
	jne	@f
	call	shoot
	jmp	still
@@:
	key_edit_boxes editboxes,editboxes_end
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
	mouse_check_boxes check_boxes,check_boxes_end
	mouse_edit_boxes editboxes,editboxes_end
	jmp	still
ipc:
	cmp	word [app_ipc+8],2
	jne	@f
	min_window
	jmp	.clear_ipc
@@:

	call	draw_number
	call	dr_st

.clear_ipc:
	cld
	xor	eax,eax
	movr	 ecx,32
	mov	edi,app_ipc
	rep	stosb
	jmp	still

p_close:
	btr	dword [flags],1
close:
	app_close

;--------------------------------------------------------------------
;--- �ம楤�� ����ᮢ�� �������� ���� ---------------------------
;--------------------------------------------------------------------
draw_window:
start_draw_window	 ; ��砫� ����ᮢ��
	xor	eax,eax 	       ; ��।��塞 ����
	mov	ebx,100*65536+320
	mov	ecx,100*65536+240
	mov	edx,[sc.work]
	add	edx,0x03000000
	xor	esi,esi
	xor	edi,edi
	int	0x40

	get_scin_height  ; ����砥� ����� ��������� ���� � ����뢠��
	sub	ax,4	 ; ��������� ⥪�� ���������
	shr	ax,1
	mov	bx,10
	shl	ebx,16
	mov	bx,ax

	mov	eax,4		  ; ��㥬 ⥪�� ����������
	mov	ecx,[sc.grab_text]
	bts	ecx,28
	mov	edx,grab_text
	mov	esi,grab_text_end-grab_text
	int	0x40

	movr	 eax,47 	    ; �뢮��� �᫠ � ����
	mov	bx,4
	shl	ebx,16
	movsx	ecx,word [scr.width] ; �ਭ� �࠭�
	mov	dx,95
	shl	edx,16
	mov	dx,[label1.top]
	push	dx
	mov	esi,[sc.work_text]
	int	0x40
	movsx	ecx,word [scr.height]	; ���� �࠭�
	mov	dx,[label2.top]
	int	0x40
	mov	bx,2
	shl	ebx,16
	movsx	ecx,word [scr.bitspp]	; ��� �� ���ᥫ�
	mov	dx,240
	shl	edx,16
	pop	dx
	int	0x40
	mov	bx,6
	shl	ebx,16
	mov	ecx,[scr.bytesps]	; ���� �� ��ப�
	mov	dx,[label2.top]
	int	0x40

	call	draw_number	 ; ���ᮢ뢠�� ����� ᭨���
	call	dr_st

	draw_labels labels,labels_end		 ; ��⪨
	draw_edit_boxes editboxes,editboxes_end  ; edit_box
	draw_txt_buttons buttons,buttons_end	 ; ������
	draw_check_boxes check_boxes,check_boxes_end ; 䫠���
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

	push	dword [scr]
	pop	dword [scr_buf.size]

	bt	dword [ch5.flags],1  ; ����祭� �� �����࠭���� ?
	jnc	@f
	call	save_file
@@:
	draw_status	shooted_ok

	bt	dword [flags],1
	jnc	@f
	ret
@@:

	bt	dword [ch2.flags],1  ; �������� ���� �।��ᬮ�� ?
	jnc	@f
	call	show_scr_window
@@:
	bt	word [ch3.flags],1   ; ����⠭����� ���� ?
	jnc	@f

	delay 100
	mov	ecx,[slot_n]
	activ_window
@@:
ret

get_mem_for_shoot:
	mov	[scr_buf.ptr],dword __app_end
	movsx	ecx, word [scr.width]
	movsx	ebx, word [scr.height]
	imul	ecx,ebx
	lea	ecx,[ecx*3]
	add	ecx,__app_end
	mov	[scr_buf.end_ptr],ecx
	xor	ebx,ebx
	inc	ebx
	mov	eax,64
	int	0x40
ret

;--- ᮤ��� ���� �஥���ᬮ�� ---
show_scr_window:
pusha
	bt	dword [flags],0
	jnc	@f
	mov	eax,51
	xor	ebx,ebx
	inc	ebx
	movr	 ecx,scr_window
	mov	edx,__app_end-1000
	int	0x40
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
	movr	 eax,13
	mov	ebx,140*65536+96
	mov	cx,[label9.top]
	shl	ecx,16
	mov	cx,10
	mov	edx,[sc.work]
	int	0x40
	movr	 eax,47
	movsx	bx,byte [sign_n]
	shl	ebx,16
	mov	ecx,[cur_number]
	mov	dx,140
	shl	edx,16
	mov	dx,[label9.top]
	mov	esi,[sc.work_text]
	int	0x40
ret

;--- ��楤��, ����᪠��� ��⮪ ����ꥬ�� ---
start_autoshoot:
	bts	dword [flags],1
	jc	@f
	movr	 eax,51
	;mov     ebx,1
	xor	ebx,ebx
	inc	ebx
	mov	ecx,autoshoot
	mov	edx,__app_end-512
	int	0x40
@@:
ret

;--- ��⠭�������� ��� ��⮪ ---
stop_autoshoot:
	btr	dword [flags],1
ret

;--- 横� ��⮪� ����ꥬ�� ---
autoshoot:
	mov	ecx,[slot_n]
	activ_window
.next:
	bt	dword [flags],1
	jnc	close
	movr	 eax,60
	movr	 ebx,2
	mov	ecx,[app.pid]
	mov	edx,messages.draw_number
	mov	esi,2
	int	0x40
	call	shoot
	jmp	autoshoot.next

;--- ��楤�� ���ᮢ�� ��ப� ���ﭨ� ---
; (������ ��뢠���� ��⮪�� �������� ����)
dr_st:
	movr	 eax,38 	     ; ��१��
	mov	ebx,5*65536+315
	mov	ecx,222*65536+222
	mov	edx,[sc.work_graph]
	int	0x40

	movr	 eax,13 	     ; ����᪠
	mov	bx,310
	mov	ecx,223*65536+12
	mov	edx,[sc.work]
	int	0x40

	mov	edi,status
	call	draw_label
ret

;--- ��楤�� ����祭�� ����� ᫮� ��⮪� ---
; �室 ebx - ����� 1024 ����
; ��室 ecx - ����� ᫮�
get_slot_n:
	movr	 eax,9
	xor	ecx,ecx
	dec	ecx
	int	0x40

	mov	edx,[app.pid]
	xor	ecx,ecx
@@:
	movr	 eax,9
	inc	ecx
	int	0x40
	cmp	[app.pid],edx
	je	@f
	jmp	@b
@@:
ret

one_shoot:
	mov	ecx,one_shoot_thread
	mov	edx,shoot_esp
	jmp	@f
save_shoot:
	mov	ecx,save_shoot_thread
	mov	edx,shoot_esp
@@:
	bts	dword [flags],2
	jc	.running
	bt	dword [flags],1
	jc	.running

	movr	 eax,51
	;mov     ebx,1
	xor	ebx,ebx
	inc	ebx
	int	0x40
.running:
ret

one_shoot_thread:
	mov	ecx,[slot_n]
	activ_window
	bt	dword [ch1.flags],1   ; ��������஢��� ���� ?
	jnc	 @f
	movr	 eax,60
	movr	 ebx,2
	mov	ecx,[app.pid]
	mov	edx,messages.min_window
	mov	esi,2
	int	0x40
@@:
	call	shoot
	btr	dword [flags],2
	jmp	close

send_draw_status:
	movr	 eax,60
	movr	 ebx,2
	mov	ecx,[app.pid]
	mov	edx,messages.draw_status
	mov	esi,2
	int	0x40
ret

save_shoot_thread:
	mov	ecx,[slot_n]
	activ_window
	call	save_file
	btr	dword [flags],2
	jmp	close

;====================================================================
;=== ����� �ணࠬ�� ===============================================
;====================================================================
messages:
.draw_number dw 0
.draw_status dw 1
.min_window  dw 2


grab_text:
	db	title
grab_text_end:
labels:
label1 label 10,30,0,text.1   ; ���� �࠭�
label2 label 10,40,0,text.2   ; �ਭ� �࠭�
label3 label 10,50,0,text.3   ; ������ ��� 䠩��
label4 label 150,30,0,text.4  ; ��� �� ���ᥫ�
label5 label 150,40,0,text.5  ; ���� �� ��ப�
label6 label 120,163,0,text.6
;label7 label 10,190,0,text.7
;label8 label 10,225,0,text.8
label9 label 10,78,0,text.9   ; ⥪�騩
label10 label 10,210,0,text.10
status label 10,226,0,no_shoot
labels_end:

editboxes:
edit1 edit_box 300,10,60,cl_white,0,0,0,1024,ed_buffer.1,ed_focus
edit2 edit_box 35,80,159,cl_white,0,0,0,9,ed_buffer.2,ed_figure_only
edit3 edit_box 35,170,189,cl_white,0,0,0,9,ed_buffer.3,ed_figure_only
edit4 edit_box 16,170,206,cl_white,0,0,0,1,sign_n_input,ed_figure_only,1
editboxes_end:

buttons:
but1 txt_button 150,10,15,90,2,0,0,but_text.1,one_shoot
but2 txt_button 145,165,15,90,3,0,0,but_text.2,save_shoot
but3 txt_button 140,120,12,145,4,0,0,but_text.3,show_scr_window
but4 txt_button 80,210,15,188,5,0,0,but_text.4,apply_number
but5 txt_button 150,10,15,110,6,0,0,but_text.5,start_autoshoot
but6 txt_button 145,165,15,110,7,0,0,but_text.6,stop_autoshoot
buttons_end:

check_boxes:
ch1 check_box 10,130,cl_white,0,0,ch_text.1,(ch_text.2-ch_text.1)
ch2 check_box 10,145,cl_white,0,0,ch_text.2,(ch_text.3-ch_text.2)
ch3 check_box 150,130,cl_white,0,0,ch_text.3,(ch_text.4-ch_text.3)
ch4 check_box 10,160,cl_white,0,0,ch_text.4,(ch_text.5-ch_text.4)
ch5 check_box 10,175,cl_white,0,0,ch_text.5,(ch_text.6-ch_text.5)
ch6 check_box 10,190,cl_white,0,0,ch_text.6,(ch_text.7-ch_text.6)
; ��⮭㬥���
check_boxes_end:

if lang eq ru
text:
.2: db '���� �࠭�:',0
.1: db '��ਭ� �࠭�:',0
.3: db '������ ����� ���� � 䠩��:',0
.4: db '��� �� ���ᥫ�:',0
.5: db '���� �� ��ப�:',0
.6: db '100 = 1 ᥪ㭤�',0
;.7: db '������ ��� 䠩��:',0
;.8: db '����� "*" � ����� 䠩�� �㤥� ��⠢������ �����.',0
.9: db '����騩 ����� ᭨���:',0
.10: db '������⢮ ������ � �����:',0

but_text:
.1: db '������� ᭨��� �࠭�',0
.2: db '���࠭��� ᭨��� �࠭�',0
.3: db '�������� ᭨��� ᥩ��',0
.4: db '�ਬ�����',0
.5: db '����� ����ꥬ��',0
.6: db '��⠭����� ����ꥬ��',0

ch_text:
.1 db '������� ����';,0
.2 db '�������� ᭨���';,0
.3 db '��⥬ ᤥ���� ��⨢��';,0
.4: db '����প�:';,0
.5: db '�����࠭����';,0
.6: db '��⮭㬥���, ��稭�� �';,0
.7:

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
end if

sign_n_input:
	db	'2',0

app_ipc ipc_buffer 32

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

cmdstr rb 257

ed_buffer:
.1: rb 1025
.2: rb 10
.3: rb 10

file_name:
	rb	1058

scr screen_prop
sc sys_color_table
app procinfo
shoot_esp rb 512
	app_end    ; ����� �ணࠬ��