; --------------------------------------------------------
; KJ|ABuIIIA - ���������� �७���� ��� ����樮���� ��⥬� ������.
;---------------------------------------------------------------------
; version:	0.95
; last update:  19/08/2011
; changed by:   Marat Zakiyanov aka Mario79, aka Mario
; changes:      1) Checking for "rolled up" window
;               2) Code optimization
;               3) Clearing source
;---------------------------------------------------------------------
; version:	0.90
; last update:  24/07/2010
; changed by:   ���३ ��堩����� (Dron2004)
;
; ��᫥���� ��������� � ��室��� ����: 24.07.2010 21:15 GMT+6
;---------------------------------------------------------------------
	use32
	org 0x0
	;���������
	db 'MENUET01'
	dd 0x01
	dd START
	dd IM_END
	dd I_END
	dd stacktop
	dd params
	dd 0x0
;---------------------------------------------------------------------
include 'lang.inc'
include '../../../macros.inc'
;---------------------------------------------------------------------
;��� �ணࠬ��
START:
;���樠�����㥬 ����
	mcall	68,11
;	call	get_screen_params
;	mov	eax,0x0000001C
;	cmp	eax,0x0
;	je	start_with_internal_text  ; ��ࠬ���� �� ������

; ��ࠬ��� �����! ��⠥��� �⨬ ��ᯮ�짮������
; ����室��� ��।����� ࠧ��� 䠩��... �����!!!
	mov	eax,5
	mov	[arg1],eax
	xor	eax,eax
	mov	[arg2],eax
	mov	[arg3],eax
	mov	[arg4],eax
	mov	eax,filedatastruct
	mov	[arg5],eax
	mov	eax,[0x0000001C]
	mov	[arg7],eax
	mcall	70,filestruct
	test	eax,eax
	jne	start_with_internal_text	;�訡��
	mov	eax, dword [size]
	mov	[filesize], eax  ;⥯��� ࠧ��� 䠩�� � ��� � filesize
;�뤥�塞 ���� ����� ��� 䠩�
	mov	ecx, [filesize]
	inc	ecx  ;�뤥��� �������⥫쭮 ���� ���� ��� ⮣�, �⮡� �������� 0xFF
		 ;���� �� "�����" KLA-䠩���
	mcall	68,12
	mov	[datastart], eax	;� ��६����� ��⠑��� ⥯��� ��室���� 㪠��⥫�
	add	eax, [filesize]
	mov	bh, 0xFF
	mov	[eax], bh
;����⢥���, ���뢠�� 䠩� � ������
	xor	eax,eax
	mov	[arg1],eax   ;����� ����㭪樨
	mov	[arg2],eax   ;���饭�� � 䠩��
	mov	[arg3],eax   ;���訩 dword ᬥ饭��
	mov	eax,[filesize]
	mov	[arg4],eax   ;������ 䠩��
	mov	eax,[datastart]
	mov	[arg5],eax   ;�����⥫� �� �����
	mov	eax,par
	mov	[arg7],eax   ;�����⥫� �� ASCIIZ-��ப�
	mcall	70,filestruct
	test	eax,eax
	je	initialize_variables
start_with_internal_text:
	mov	edx, string
	mov	[datastart], edx
;	mov	edx, string         ; �ᯮ��㥬 ���஥��� ⥪��
initialize_variables:
	mov	edx,[datastart]
	mov	[currentsymb],edx	; ��砫쭠� ���樠������ ��६�����
	mov	[startline],edx
	mov	[lastsymb],edx
	mov	[lessonstart],edx
	xor	eax,eax
	inc	eax
	mov	[currentline], eax
	call	count_lines
;---------------------------------------------------------------------
redraw:
	call	draw_window
;---------------------------------------------------------------------
;�᭮���� 横�
event_wait:
	mcall	10
	cmp	eax, 1  ;����ᮢ�� ���� �㦭�
	je	redraw
	cmp	eax, 2  ;������ �����
	je	key
	cmp	eax, 3  ;�� ������ ����㫨
	je	button
	jmp	event_wait
;---------------------------------------------------------------------
key:
	mcall	2	;������ ��� ����⮩ ������ � ah
	push	eax
;����᪠�� ����稪 �६��� ��� ��᫥���饣�
; ��।������ ᪮��� �����
	mov	eax, [currentsymb]
	cmp	eax, [lessonstart]
	jne	not_first_symbol
	mov	eax,[mistakes]
	test	eax,eax
	jne	not_first_symbol
	mcall	26,9
	mov	[typestarttime], eax
not_first_symbol:
	pop	eax
;�஢��塞, �� �����稫�� �� ⥪��
	mov	esi, [currentsymb]
	mov	al, byte [esi]
	cmp	al, 0xFF
	je	text_end
	cmp	al, 0xFE
	je	text_end
	push	ebx
	mov	ebx, [currentsymb]
	mov	al, byte [ebx]
	pop	ebx
	test	al,al
	jne	not_new_line
	cmp	ah, 0x0D
	je	correct_key_nl
	cmp	ah, 0x20
	je	correct_key_nl
	jmp	not_this_key
;---------------------------------------------------------------------
correct_key_nl:
	mov	eax, [currentsymb]
	inc	eax
	mov	[startline], eax
	mov	eax, [currentsymb]
	inc	eax
	mov	[currentsymb], eax
	mov	[lastsymb], eax
	mov	eax, [currentline]
	inc	eax
	mov	[currentline], eax
	mov	eax, [symbtyped]	 ;�����稬 �᫮ ������� ᨬ����� �� 1
	inc	eax
	mov	[symbtyped], eax
	call	count_speed
;	call	draw_speed
	jmp	redraw
;---------------------------------------------------------------------
not_new_line:
	cmp	ah, al
	jne	not_this_key
correct_key:
	mov	eax, [currentsymb]
	inc	eax
	mov	[currentsymb], eax
	mov	[lastsymb], eax
	mov	eax, [symbtyped]	 ;�����稬 �᫮ ������� ᨬ����� �� 1
	inc	eax
	mov	[symbtyped], eax
	call	count_speed
;	call	draw_speed
	call	redraw_2_symbols
	jmp	event_wait
;---------------------------------------------------------------------
not_this_key:
	mov	esi, [mistakes]
	inc	esi
	mov	[mistakes], esi
	call	redraw_mistakes
	jmp	event_wait
;---------------------------------------------------------------------
text_end:
; /// �९������ ������� ���� �� ����砭�� �ப�
; �� ����⨨ ������
	mov	esi, [startline]
	cmp	esi, lessoncomplete
	je	text_end_already_shown
	; ///////////////////////////
	call	speed_to_string
	mov	esi, lessoncomplete
	mov	[startline], esi
	mov	esi, lessoncompleteend
	dec	esi
	mov	[currentsymb], esi
	call	draw_window
text_end_already_shown:
	jmp	event_wait
;---------------------------------------------------------------------
button:
	mcall	17	;�����䨪��� ����⮩ ������ ������� � ah
	cmp	ah, 1    ;�� ������ �������
	jne	no_close
	mcall	-1
no_close:
;�஢��塞 ��⠫�� ������
	cmp	ah, 0x02
	jne	no_button_2
;����� ������ 2
;---------------------------------------------------------------------
; ����� �������� ���
;---------------------------------------------------------------------
	xor	eax, eax
	mov	[mistakes], 0x0
	mov	esi, [lastsymb]
	mov	al, [esi]
;�㦭� �஢����, �� �� ᨬ��� �� ��᫥����. �᫨ 0xFF -
;����� ���稫��, �㦭� ����� ��� ᭠砫�
	cmp	al, 0xFF
	je	start_1st_lesson
	cmp	al, 0xFE ; �� ��諨 ���� �ப?
	jne	not_completed_previous
init_level_after_fe:
;����� ��, � �஢�ਬ, �� ����� �� ��砩�� ᫥���騬 ᨬ����� 0xFF
	inc	esi
	mov	al, [esi]
	cmp	al, 0xFF
;����� ��� - �� � ���� ᫥���騩 �ப
	jne	set_lesson_start_from_esi
start_1st_lesson:
	mov	esi, [datastart]
set_lesson_start_from_esi:
	mov	[startline], esi
	mov	[currentsymb], esi
	mov	[lastsymb], esi
	mov	[lessonstart], esi
	xor	eax,eax
	jmp	no_button_3.2
;	inc	eax
;	mov	[currentline], eax
;	call	count_lines
;	call	reset_speed_counters
;	jmp	redraw
;---------------------------------------------------------------------
not_completed_previous:
	inc	esi
	mov	al, [esi]
	cmp	al, 0xFF
	je	start_1st_lesson
	cmp	al, 0xFE
	je	init_level_after_fe
	jmp	not_completed_previous
;---------------------------------------------------------------------
; ����� ��������� ����
;---------------------------------------------------------------------
no_button_2:
	cmp	ah,0x03
	jne	no_button_3
;��१���� �஢��
	mov	edx,[lessonstart]
	jmp	no_button_3.1
;	mov	[currentsymb],edx
;	mov	[startline],edx
;	mov	[lastsymb],edx
;	xor	eax,eax
;	mov	[mistakes],eax
;	inc	eax
;	mov	[currentline],eax
;	call	reset_speed_counters
;	call	count_lines
;	jmp	redraw
;---------------------------------------------------------------------
no_button_3:
	cmp	ah,0x04
	jne	event_wait
;��१���� ���� �ࠦ�����
	mov	edx,[datastart]
	mov	[lessonstart],edx
.1:
	mov	[currentsymb],edx
	mov	[startline],edx
	mov	[lastsymb],edx
	xor	eax,eax
	mov	[mistakes],eax
.2:
	inc	eax
	mov	[currentline],eax
	call	reset_speed_counters
	call	count_lines
	jmp	redraw
;---------------------------------------------------------------------
;����稬 ⥪�騥 ��ࠬ���� ����
get_window_param:
	mcall	9,procinfo,-1
	ret
;---------------------------------------------------------------------
;"����" �ணࠬ�� - �㭪�� �ᮢ���� ���� (��� ��⠫쭮� - �㫮��� �-)) )
draw_window:
	mcall	12,1
	mov	ax,[areawidth]   ;�� ���� �맮� draw_window?
	test	ax,ax
	jne	.dw_not_first_call
	call	get_screen_params	  ;����砥� ��ࠬ���� �࠭� - �ਭ� � �����
.dw_not_first_call:
;� ebx - X � �ਭ�
	mov	bx,[windowx]
	shl	ebx,16
	mov	bx,780
;	mov	ebx,10*65536+780
;� ecx - Y � ����
	mov	cx, [windowy]
	shl	ecx,16
	mov	cx,580
;	mov	ecx,10*65536+580
	xor	esi,esi
;��।����� � �뢥�� ����
	mcall	0,,,0x33CCCCCC,,text
	call	get_window_param
	mov	eax,[procinfo+70] ;status of window
	test	eax,100b
	jne	.end
;����㥬 ������ ��������� ���� � ⥪�� �� ���
	mcall	8,<295,145>,<8,18>,2,0x0099CC99
;����㥬 ������ ��������� ����������
	mcall	,<450,145>,,3,0x00CC9999
;����㥬 ������ ������ ���� ������
	mcall	,<605,145>,,4,0x00DD7777
;�뢮��� ⥪�� �� �������
	mcall	4,<310,14>,0x80000000,buttontext
	mcall	,<465,14>,,retrybuttontext	
	mcall	,<626,14>,,restartbuttontext
; ����㥬 ����⨯
	xor	ebp,ebp
	mcall	65,logo,<32,12>,<12,12>,1,green_text
;����稬 ⥪�騥 ��ࠬ���� ����
	call	get_window_param
	add	ebx,0x2A
	mov	eax,[ebx]
	mov	[windowwidth], eax
	mov	ebx,procinfo
	add	ebx,0x2E
	mov	eax,[ebx]
	mov	[windowheight], eax
;draw_error_count:
; ����㥬 ������� "������"
	mcall	4,<60,15>,0x80AA0000,mistakestext
; �뢥��� �᫮ �訡��
	mcall	47,0x80040000,[mistakes],<105,15>,0x00AA0000
;draw_line_number:
; ����㥬 ������� "��ப�       ��"
	mcall	4,<140,15>,0x8000AA00,lineistext
;�뢥��� ����� ⥪�饩 ��ப�
	mcall	47,80040000,[currentline],<195,15>,0x0000AA00
;�뢥��� ⥪���� ��ப�
	mcall	,,[linecount],<252,15>,
;�뢥�� ࠡ�稩 ⥪��
	call	draw_text
.end:
	mcall	12,2
	ret
;---------------------------------------------------------------------
draw_text:
;�����⮢�� � �뢮�� ⥪��
	mov	edx,40		;��砫쭮� ��������� �뢮������ ⥪�� �� ���⨪���
	mov	esi,[startline]
	dec	esi 		;��� ���� (�. *1 ����)!
	mov	ebx,esi		;������ � edx - ��砫� �뢮����� ��ப�
.start:
;���騢��� 㪠��⥫� �� ⥪�騩 ᨬ��� �� �������
	inc	ebx 	  ;(*1)
.draw_text_without_inc:
;��ᬮ�ਬ,�� ��室�� �� ⥪�� �� �࠭��� ����
	mov	esi,[windowheight] ;������ � esi ࠧ��� ����
	mov	eax,edx
	and	eax,0x0000FFFF
	add	eax,64
	cmp	eax,esi
	ja	end_draw_text     ;�� ��室�� �� �।��� ����. ������ �ᮢ���.
;��⠭���� ��������� �� ��ਧ��⠫� (+18 � ⥪�饩 ����樨)
	mov	esi,18*65536
	add	edx,esi
; ��ᬮ�ਬ, �� ��襫 �� ⥪�� �� �࠭��� �� ��ਧ��⠫�
; �᫨ ��襫 - �� ��㥬 ��� ��᮪
	mov	esi,[windowwidth] ;������ � esi ࠧ��� ����
	shl	esi,16
	mov	eax,edx
	and	eax,0xFFFF0000
	add	eax,40*65536
	cmp	eax,esi
	jna	.horizontal_check_ok     ;�᫨ eax>est, � �� ��室�� �� �।��� ����.
.skip_line_end_step:
	mov	ah,byte [ebx]
	cmp	ah,0x00
	je	.end_line_to_next_line
	cmp	ah,0xFE
	je	end_draw_text
	cmp	ah,0xFF
	je	end_draw_text
	inc	ebx
	jmp	.skip_line_end_step
;-------------------------------------------
.end_line_to_next_line:
	and	edx,0x0000FFFF
	jmp	.draw_text_without_inc
;-------------------------------------------
.horizontal_check_ok:
;�஢�ਬ, �� �����稫�� �� �ப
	mov	esi, [startline]
	cmp	esi, lessoncomplete
	jne	.in_process
;�᫨ �����稫��, ⥪�� �뢮���� �㦭� ������� ������
	mov	edi, green_text
	jmp	.color_set_sucessful
;--------------------------------------------
.in_process:
	cmp	ebx, [currentsymb]	  ; ������� ⥪�騩 ����ࠥ�� ᨬ���?
	je	.red			  ; ��� �筮!
	ja	.black		  ; ���, ������� �, �� �� ��� �� ���ࠫ�
	mov	edi, gray_text	 ; ���, ������ �, �� �� 㦥 ���ࠫ�
	jmp	.color_set_sucessful
;--------------------------------------------
.red:
	mov	edi, red_text
	jmp	.color_set_sucessful
;---------------------------------------------
.black:
	mov	edi, black_text	 ;���� � 梥�
.color_set_sucessful:
	xor	esi,esi
	movzx	si, byte [ebx]
	cmp	si, 0x0000
	jne	.continue_drawing
;	call	increase_y;
; �����稬 ���⨪����� ���न���� �뢮�� �㪢,
; �᫨ ���稫��� ��ப� (����⨫�� ���� 0x00)
	and	edx,0x0000FFFF
	add	edx, 33
	jmp	.continue_text_proc
;----------------------------------------------
.continue_drawing:
	cmp	si, 0x00FF
	je	end_draw_text
	cmp	si, 0x00FE
	je	end_draw_text
;���㥬 �㪢� � ������� �������� ����
	push	ebx
	push	edx
	movzx	eax, byte [ebx]
	shl	eax,6
	add	eax, big_font
	mov	ebx, eax
	pop	edx
	mov	ecx, 16*65536+32
; � edx ����� ���न���
	mov	esi, 1
; � edi ����� 㪠��⥫� �� ������� (梥� ����)
	xor	ebp,ebp
	mcall	65
	pop	ebx
;�� �⮬ ���ᮢ�� �㪢� �����襭�
.continue_text_proc:
;	inc	edi
	jmp	.start
end_draw_text:
	ret
;---------------------------------------------------------------------
;��������� ����������� ���� �������� (���� �������������� �������)
redraw_2_symbols:
;�஢�ਬ, �� �멤�� �� �� �� �ᮢ����� �� �࠭��� �����⨬��
;������ ����� ����. �᫨ �멤�� - �஫��� ��ப� �� ��ਧ��⠫�
	mov	esi, [windowwidth] ;������ � esi ࠧ��� ����
	mov	eax, [currentsymb]
	sub	eax, [startline]
	mov	ecx, 18
	mul	ecx
	add	eax, 20
	add	eax, 20
	add	eax, 40
	cmp	eax, esi
	jna	r2s_horizontal_check_ok	;�᫨ eax>est, � �� ��室�� �� �।��� ����.
;�⠪, �� ��室�� �� �࠭��� ����... �� ����...
;��ப� �ਤ���� �஫����...
	mov	eax, [currentsymb]
	dec	eax
	mov	[startline], eax
	call	draw_window
	jmp	return_from_redraw_2_symbols
;---------------------------------------------------------------------
r2s_horizontal_check_ok:
;���㥬 ���������� ����� �㪢� � ������� �������� ����
	mov	ebx, [currentsymb]
	dec	ebx
	movzx	eax, byte [ebx]
	shl	eax,6
	add	eax, big_font
	mov	ebx, eax
	mov	ecx, 16*65536+32
	mov	eax, [currentsymb]
	sub	eax, [startline]
	dec	eax
	imul	eax,18
	add	eax, 18
	shl	eax,16
	add	eax, 40
	mov	edx, eax
	xor	ebp,ebp
	mcall	65,,,,1,gray_text
;���㥬 ������� �����
	mov	ebx, [currentsymb]
	movzx	eax, byte [ebx]
	shl	eax,6
	add	eax, big_font
	mov	ebx, eax
	mov	ecx, 16*65536+32
	mov	eax, [currentsymb]
	sub	eax, [startline]
	imul	eax,18
	add	eax, 18
	shl	eax,16
	add	eax, 40
	mov	edx, eax
	mcall	65,,,,1,red_text
return_from_redraw_2_symbols:
	ret
;---------------------------------------------------------------------
redraw_mistakes:
;����ᨬ ��אַ㣮�쭨�
	mcall	13,<59,75>,<14,10>,0x00CCCCCC
; ����㥬 ������� "������"
	mcall	4,<60,15>,0x80AA0000,mistakestext
; �뢥��� �᫮ �訡��
	mcall	47,0x80040000,[mistakes],<105,15>,0x00AA0000
	ret
;---------------------------------------------------------------------
count_lines:
	xor	ecx, ecx		; � ecx - ����稪 ��ப
	inc	ecx 		; � ��� 1 ��ப�
; ��稭��� ࠧ���...
	mov	eax, [startline]
	dec	eax
cl_next_step:
	inc	eax
	mov	bh, [eax]
	cmp	bh, 0x00
	jne	cl_not_new_line
	inc	ecx
	jmp	cl_next_step
;---------------------------------------------------------------------
cl_not_new_line:
	cmp	bh, 0xFE
	je	cl_end
	cmp	bh, 0xFF
	je	cl_end
	jmp	cl_next_step
;---------------------------------------------------------------------
cl_end:
	mov	[linecount],ecx
	ret
;---------------------------------------------------------------------
reset_speed_counters:
	xor	eax,eax
	mov	[symbtyped],eax
	mov	[typestarttime],eax
	mov	[currenttime],eax
	mov	[typingspeed],eax
	ret
;---------------------------------------------------------------------
count_speed:
	mcall	26,9
	mov	[currenttime], eax
	mov	ebx, [typestarttime]
	sub	eax, ebx   ;������ � eax - �᫮ ��襤�� ����� ᥪ㭤
	mov	ecx, [symbtyped]
	cmp	ecx, 0x00
	jne	cs_all_ok
	inc	ecx
cs_all_ok:
	xor	edx, edx;
	div	ecx  ;������ � eax - �।��� ��㧠 ����� ᨬ������
	mov	ecx, eax
	cmp	ecx, 0x00
	jne	cs_all_ok_2
	inc	ecx
cs_all_ok_2:
	xor	edx, edx
	mov	eax, 6000
	div	ecx
	mov	[typingspeed], eax ;��� � ���. � [typingspeed] - �।��� ᪮���� �����
	ret
;---------------------------------------------------------------------
speed_to_string:
; �८�ࠧ�� �᫮ � ��ப� � �����뢠�� �� ��६� ��६����� speedbytes ����� ������
	xor	edx, edx
	mov	eax, [typingspeed]
	mov	ecx, 10
	div	ecx
	add	dl, 30h
	mov	[speedbytes + 3], dl
	xor	edx, edx
	div	ecx
	add	dl, 30h
	mov	[speedbytes + 2], dl
	xor	edx, edx
	div	ecx
	add	dl, 30h
	mov	[speedbytes + 1], dl
	xor	edx, edx
	div	ecx
	cmp	dl, 0x00
	je	sts_move_space
	add	dl, 30h
	mov	[speedbytes], dl
	jmp	sts_end
;---------------------------------------------------------------------
sts_move_space:
	mov	[speedbytes], 0x20
sts_end:
	ret
;---------------------------------------------------------------------
get_screen_params:
	mcall	14
	mov	[areaheight], ax
	push	ax
	shr	eax, 16
	mov	[areawidth],ax
;� ax �� �०���� ��室���� �ਭ� ����. ��ᯮ��㥬�� �⨬
	sub	ax, 780   ;���⥬ ��砫�� ࠧ��� ����
	shr	ax, 1
	mov	[windowx], ax
; ���室�� � ����
	xor	ax, ax
	pop	ax		; ��⮫���� �� �⥪� ���祭�� �����
	sub	ax, 580
	shr	ax, 1
	mov	[windowy], ax
	ret
;---------------------------------------------------------------------
;draw_speed:
;����ᨬ ��אַ㣮�쭨�
;	mcall	13,<59,340>,<29,10>,0x00CCCCCC
; ����㥬 ������� "������� ����� (������ � ������):"
;	mcall	4,<60,30>,0x80008800,speedtext
; �뢥��� �᫮
;	mcall	47,0x80040000,[typingspeed],<305,30>,0x00008800
;	ret
;---------------------------------------------------------------------
include 'data.inc'
;---------------------------------------------------------------------
IM_END:
;---------------------------------------------------------------------
par:
params:
	rb 1024
;---------------------------------------------------------------------
procinfo:
	rb 1024
;---------------------------------------------------------------------
	rb 1024
stacktop:
;---------------------------------------------------------------------
I_END: