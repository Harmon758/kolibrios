; --------------------------------------------------------
; KJ|ABuIIIA  0.9
; --------------------------------------------------------
; ���������� �७���� ��� ����樮���� ��⥬� ������.
;
; ���ࠡ��稪�:
; - ���३ ��堩����� (Dron2004)
;
; ��᫥���� ��������� � ��室��� ����: 24.07.2010 21:15 GMT+6


 use32
 org 0x0

 ;���������
 db 'MENUET01'
 dd 0x01
 dd START
 dd I_END
 dd 0x10000
 dd 0x10000
 dd params
 dd 0x0

;��� �ணࠬ��
START:

      ; call get_screen_params

	mov eax, 0x0000001C
	cmp eax, 0x0
	je start_with_internal_text  ; ��ࠬ���� �� ������

	; ��ࠬ��� �����! ��⠥��� �⨬ ��ᯮ�짮������
	; ����室��� ��।����� ࠧ��� 䠩��... �����!!!
	mov eax, 5
	mov [arg1], eax
	xor eax, eax
	mov [arg2], eax
	mov [arg3], eax
	mov [arg4], eax
	mov eax, filedatastruct
	mov [arg5], eax
	mov eax, [0x0000001C]
	mov [arg7], eax

	mov eax, 70;
	mov ebx, filestruct
	int 0x40

	cmp eax, 0
	jne start_with_internal_text  ;�訡��

	mov eax, dword [size]
	mov [filesize], eax  ;⥯��� ࠧ��� 䠩�� � ��� � filesize

	;���樠�����㥬 ����

	mov eax, 68
	mov ebx, 11
	int 0x40

	cmp eax, 0x0
	je start_with_internal_text   ;�訡��

	;�뤥�塞 ���� ����� ��� 䠩�
	mov eax, 68
	mov ebx, 12
	mov ecx, [filesize]
	inc ecx  ;�뤥��� �������⥫쭮 ���� ���� ��� ⮣�, �⮡� �������� 0xFF
		 ;���� �� "�����" KLA-䠩���
	int 0x40

	mov [datastart], eax	;� ��६����� ��⠑��� ⥯��� ��室���� 㪠��⥫�

	add eax, [filesize]
	mov bh, 0xFF
	mov [eax], bh

	;����⢥���, ���뢠�� 䠩� � ������
	mov eax, 0x00
	mov [arg1], eax   ;����� ����㭪樨
	mov [arg2], eax   ;���饭�� � 䠩��
	mov [arg3], eax   ;���訩 dword ᬥ饭��
	mov eax, [filesize]
	mov [arg4], eax   ;������ 䠩��
	mov eax, [datastart]
	mov [arg5], eax   ;�����⥫� �� �����
	mov eax, par
	mov [arg7], eax   ;�����⥫� �� ASCIIZ-��ப�

	mov eax, 70
	mov ebx, filestruct
	int 0x40

	cmp eax, 0x0
	jne start_with_internal_text   ;�訡��

	;mov edx, [datastart]
       jmp initialize_variables

start_with_internal_text:
	mov edx, string
	mov [datastart], edx
	;mov edx, string         ; �ᯮ��㥬 ���஥��� ⥪��

initialize_variables:




	mov edx, [datastart]
	mov [currentsymb], edx	; ��砫쭠� ���樠������ ��६�����
	mov [startline], edx
	mov [lastsymb], edx
	mov [lessonstart], edx

	mov eax, 0x00000001
	mov [currentline], eax
	call count_lines
	call draw_window


;�᭮���� 横�
event_wait:
	mov eax, 10
	int 0x40

	cmp eax, 1  ;����ᮢ�� ���� �㦭�
	je redraw
	cmp eax, 2  ;������ �����
	je key
	cmp eax, 3  ;�� ������ ����㫨
	je button

	jmp event_wait

redraw:
	call draw_window
	jmp event_wait
key:
	mov eax, 2
	int 0x40    ;������ ��� ����⮩ ������ � ah

	push eax

	;����᪠�� ����稪 �६��� ��� ��᫥���饣�
	; ��।������ ᪮��� �����

	mov eax, [currentsymb]
	cmp eax, [lessonstart]
	jne not_first_symbol
	cmp [mistakes], 0x00
	jne not_first_symbol

	mov eax, 26
	mov ebx, 9
	int 0x40

	mov [typestarttime], eax

not_first_symbol:

	pop eax

	;�஢��塞, �� �����稫�� �� ⥪��
	mov esi, [currentsymb]
	mov al, byte [esi]
	cmp al, 0xFF
	je text_end
	cmp al, 0xFE
	je text_end

	push ebx
	mov ebx, [currentsymb]
	mov al, byte [ebx]
	pop ebx

	cmp al,0x00
	jne not_new_line
	cmp ah, 0x0D
	je correct_key_nl
	cmp ah, 0x20
	je correct_key_nl
	jmp not_this_key

not_new_line:
	cmp ah, al
	jne not_this_key
	jmp correct_key

correct_key_nl:
	mov eax, [currentsymb]
	inc eax
	mov [startline], eax

	mov eax, [currentsymb]
	inc eax
	mov [currentsymb], eax
	mov [lastsymb], eax

	mov eax, [currentline]
	inc eax
	mov [currentline], eax

	mov eax, [symbtyped]	 ;�����稬 �᫮ ������� ᨬ����� �� 1
	inc eax
	mov [symbtyped], eax
	call count_speed
	;call draw_speed

	call draw_window
	jmp event_wait


correct_key:
	mov eax, [currentsymb]
	inc eax
	mov [currentsymb], eax
	mov [lastsymb], eax

	mov eax, [symbtyped]	 ;�����稬 �᫮ ������� ᨬ����� �� 1
	inc eax
	mov [symbtyped], eax
	call count_speed
	;call draw_speed

	call redraw_2_symbols
	jmp event_wait

not_this_key:

	mov esi, [mistakes]
	inc esi
	mov [mistakes], esi
	call redraw_mistakes
	jmp event_wait

text_end:

	; /// �९������ ������� ���� �� ����砭�� �ப�
	; �� ����⨨ ������
	mov esi, [startline]
	cmp esi, lessoncomplete
	je text_end_already_shown
	; ///////////////////////////


	call speed_to_string

	mov esi, lessoncomplete
	mov [startline], esi

	mov esi, lessoncompleteend
	dec esi
	mov [currentsymb], esi

	call draw_window
text_end_already_shown:
	jmp event_wait

button:
	mov eax, 17
	int 0x40     ;�����䨪��� ����⮩ ������ ������� � ah

	cmp ah, 1    ;�� ������ �������
	jne no_close
	mov eax, -1
	int 0x40
no_close:
	;�஢��塞 ��⠫�� ������
	cmp ah, 0x02
	jne no_button_2
	;����� ������ 2
	; ����� �������� ���
	xor eax, eax

	mov [mistakes], 0x0

	mov esi, [lastsymb]
	mov al, [esi]
	;�㦭� �஢����, �� �� ᨬ��� �� ��᫥����. �᫨ 0xFF -
	;����� ���稫��, �㦭� ����� ��� ᭠砫�
	cmp al, 0xFF
	je start_1st_lesson

	cmp al, 0xFE ; �� ��諨 ���� �ப?
	jne not_completed_previous

init_level_after_fe:
	;����� ��, � �஢�ਬ, �� ����� �� ��砩�� ᫥���騬 ᨬ����� 0xFF
	inc esi
	mov al, [esi]
	cmp al, 0xFF
	je start_1st_lesson
	;����� ��� - �� � ���� ᫥���騩 �ப
	jmp set_lesson_start_from_esi

not_completed_previous:
	inc esi
	mov al, [esi]
	cmp al, 0xFF
	je start_1st_lesson
	cmp al, 0xFE
	je init_level_after_fe
	jmp not_completed_previous

start_1st_lesson:
	mov esi, [datastart]
set_lesson_start_from_esi:
	mov [startline], esi
	mov [currentsymb], esi
	mov [lastsymb], esi
	mov [lessonstart], esi

	;CC
	mov eax, 0x00000001
	mov [currentline], eax
	call count_lines
	;CC

	call reset_speed_counters

	call draw_window
	jmp event_wait
	; ����� ��������� ����

no_button_2:
	cmp ah, 0x03
	jne no_button_3

	;��१���� �஢��
	mov edx, [lessonstart]
	mov [currentsymb], edx
	mov [startline], edx
	mov [lastsymb], edx

	mov eax, 0x00000001
	mov [currentline], eax

	mov [mistakes], 0

	call reset_speed_counters

	call count_lines
	call draw_window

	jmp event_wait

no_button_3:
	cmp ah, 0x04
	jne no_button_4

	;��१���� ���� �ࠦ�����

	mov edx, [datastart]

	mov [lessonstart], edx
	mov [currentsymb], edx
	mov [startline], edx
	mov [lastsymb], edx

	mov eax, 0x00000001
	mov [currentline], eax

	mov [mistakes], 0


	call reset_speed_counters

	call count_lines
	call draw_window

	jmp event_wait


no_button_4:
	;� �����頥��� � �������� ᮡ���
	jmp event_wait

;"����" �ணࠬ�� - �㭪�� �ᮢ���� ���� (��� ��⠫쭮� - �㫮��� �-)) )
draw_window:
	mov eax, 12   ;����頥� ��⥬� � ���ﭨ� ����ᮢ�� ����
	mov ebx, 1    ;��砫� ����ᮢ��
	int 0x40


	mov ax,  [areawidth]   ;�� ���� �맮� draw_window?
	cmp ax, 0x0
	jne dw_not_first_call
	call get_screen_params	  ;����砥� ��ࠬ���� �࠭� - �ਭ� � �����

dw_not_first_call:
	xor eax, eax
	mov ax, [windowx]
	mov ecx, 65536
	mul ecx
	add eax, 780
	mov ebx, eax  ;� ebx - X � �ਭ�

	;mov ebx, 10*65536+780

	xor eax, eax
	mov ax, [windowy]
	mov ecx, 65536
	mul ecx
	add eax, 580
	mov ecx, eax  ;� ecx - Y � ����


	;mov ecx, 10*65536+580       ;Y � ����
	mov edx, 0x33CCCCCC	     ;[⨯ ����][梥� ࠡ�祩 ������]
	mov edi, text
	mov eax, 0		     ;��।����� � �뢥�� ����
	int 0x40


;����㥬 ������ ��������� ���� � ⥪�� �� ���
	mov eax, 8
	mov ebx, 295*65536+145
	mov ecx, 8*65536+18
	mov edx, 0x00000002
	mov esi, 0x0099CC99
	int 0x40

	mov eax, 4
	mov ebx,310*65536+14
	mov ecx, 0x80000000
	mov edx, buttontext
	int 0x40

;����㥬 ������ ��������� ����������
	mov eax, 8
	mov ebx, 450*65536+145
	mov ecx, 8*65536+18
	mov edx, 0x00000003
	mov esi, 0x00CC9999
	int 0x40

	mov eax, 4
	mov ebx,465*65536+14
	mov ecx, 0x80000000
	mov edx, retrybuttontext
	int 0x40

;����㥬 ������ ������ ���� ������
	mov eax, 8
	mov ebx, 605*65536+145
	mov ecx, 8*65536+18
	mov edx, 0x00000004
	mov esi, 0x00DD7777
	int 0x40

	mov eax, 4
	mov ebx, 626*65536+14
	mov ecx, 0x80000000
	mov edx, restartbuttontext
	int 0x40

; ����㥬 ����⨯
	mov eax, 65
	mov ebx, logo
	mov ecx, 32*65536+12
	mov edx, 12*65536+12
	mov esi, 1
	mov edi, green_text
	mov ebp, 0
	int 0x40

;����稬 ⥪�騥 ��ࠬ���� ����
	mov eax, 9
	mov ebx, kbbuffer
	mov ecx, -1
	int 0x40

	add ebx, 0x2A
	mov eax, [ebx]
	mov [windowwidth], eax

	mov ebx, kbbuffer
	add ebx, 0x2E
	mov eax, [ebx]
	mov [windowheight], eax

draw_error_count:
	; ����㥬 ������� "������"
	mov eax, 4
	mov ebx, 60*65536+15
	mov ecx, 0x80AA0000
	mov edx, mistakestext
	int 0x40

	; �뢥��� �᫮ �訡��
	mov eax, 47
	mov ebx, 10000000000001000000000000000000b
	mov ecx, [mistakes]
	mov edx, 105*65536+15
	mov esi, 0x00AA0000
	int 0x40

draw_line_number:
	; ����㥬 ������� "��ப�       ��"
	mov eax, 4
	mov ebx, 140*65536+15
	mov ecx, 0x8000AA00
	mov edx, lineistext
	int 0x40

	;�뢥��� ����� ⥪�饩 ��ப�
	mov eax, 47
	mov ebx, 10000000000001000000000000000000b
	mov ecx, [currentline]
	mov edx, 195*65536+15
	mov esi, 0x0000AA00
	int 0x40

	;�뢥��� ⥪���� ��ப�
	mov eax, 47
	mov ebx, 10000000000001000000000000000000b
	mov ecx, [linecount]
	mov edx, 252*65536+15
	mov esi, 0x0000AA00
	int 0x40



draw_text_prepare:
	;�����⮢�� � �뢮�� ⥪��
	mov edx, 40		;��砫쭮� ��������� �뢮������ ⥪�� �� ���⨪���

	mov esi, [startline]
	dec esi 		;��� ���� (�. *1 ����)!
	mov ebx, esi		;������ � edx - ��砫� �뢮����� ��ப�


draw_text:


	;���騢��� 㪠��⥫� �� ⥪�騩 ᨬ��� �� �������
	inc ebx 	  ;(*1)

draw_text_without_inc:
	;��ᬮ�ਬ, �� ��室�� �� ⥪�� �� �࠭��� ����
	mov esi, [windowheight] ;������ � esi ࠧ��� ����
	mov eax, edx
	and eax,0x0000FFFF
	add eax, 64
	cmp eax, esi
	ja end_draw_text     ;�� ��室�� �� �।��� ����. ������ �ᮢ���.

	;��⠭���� ��������� �� ��ਧ��⠫� (+18 � ⥪�饩 ����樨)
	mov esi, 18*65536

	add edx, esi

	; ��ᬮ�ਬ, �� ��襫 �� ⥪�� �� �࠭��� �� ��ਧ��⠫�
	; �᫨ ��襫 - �� ��㥬 ��� ��᮪
	mov esi, [windowwidth] ;������ � esi ࠧ��� ����
	shl esi, 16
	mov eax, edx
	and eax,0xFFFF0000
	add eax, 40*65536
	cmp eax, esi
	jna horizontal_check_ok     ;�᫨ eax>est, � �� ��室�� �� �।��� ����.

skip_line_end_step:

	mov ah, byte [ebx]

	cmp ah, 0x00
	je end_line_to_next_line
	cmp ah, 0xFE
	je end_draw_text
	cmp ah, 0xFF
	je end_draw_text

	inc ebx

	jmp skip_line_end_step

end_line_to_next_line:
	;jmp end_line_to_next_line
	and edx, 0x0000FFFF
	jmp draw_text_without_inc

horizontal_check_ok:

	;�஢�ਬ, �� �����稫�� �� �ப
	mov esi, [startline]
	cmp esi, lessoncomplete
	jne in_process

	;�᫨ �����稫��, ⥪�� �뢮���� �㦭� ������� ������
	mov edi, green_text
	jmp color_set_sucessful

in_process:
	cmp ebx, [currentsymb]	  ; ������� ⥪�騩 ����ࠥ�� ᨬ���?
	je red			  ; ��� �筮!

	ja black		  ; ���, ������� �, �� �� ��� �� ���ࠫ�


	mov edi, gray_text	 ; ���, ������ �, �� �� 㦥 ���ࠫ�
	jmp color_set_sucessful

red:
	mov edi, red_text
	jmp color_set_sucessful

black:
	mov edi, black_text	 ;���� � 梥�

color_set_sucessful:
	xor esi,esi
	movzx si, byte [ebx]
	cmp si, 0x0000
	jne continue_drawing
	call increase_y;
	jmp continue_text_proc

continue_drawing:

	cmp si, 0x00FF
	je end_draw_text
	cmp si, 0x00FE
	je end_draw_text


	;���㥬 �㪢� � ������� �������� ����
	push ebx
	push edx

	movzx eax, byte [ebx]
	mov ecx, 64
	mul ecx
	add eax, big_font
	mov ebx, eax

	pop edx

	mov ecx, 16*65536+32

	; � edx ����� ���न���

	mov esi, 1

	; � edi ����� 㪠��⥫� �� ������� (梥� ����)

	mov ebp, 0

	mov eax, 65

	int 0x40

	pop ebx
	;�� �⮬ ���ᮢ�� �㪢� �����襭�
continue_text_proc:
	;inc edi
	jmp draw_text

end_draw_text:

	mov eax, 12   ;����� ᮮ�頥� � ���ﭨ� ����ᮢ�� ���
	mov ebx, 2   ;, �� �� ��� ࠧ - � ���� ����ᮢ��
	int 0x40

	ret

increase_y:
	; �����稬 ���⨪����� ���न���� �뢮�� �㪢,
	; �᫨ ���稫��� ��ப� (����⨫�� ���� 0x00)
	and edx,0x0000FFFF
	add edx, 33

	ret

;��������� ����������� ���� �������� (���� �������������� �������)
redraw_2_symbols:

	;�஢�ਬ, �� �멤�� �� �� �� �ᮢ����� �� �࠭��� �����⨬��
	;������ ����� ����. �᫨ �멤�� - �஫��� ��ப� �� ��ਧ��⠫�
	mov esi, [windowwidth] ;������ � esi ࠧ��� ����

	mov eax, [currentsymb]
	sub eax, [startline]

	mov ecx, 18

	mul ecx

	add eax, 20

	add eax, 20

	add eax, 40

	cmp eax, esi
	jna r2s_horizontal_check_ok	;�᫨ eax>est, � �� ��室�� �� �।��� ����.

	;�⠪, �� ��室�� �� �࠭��� ����... �� ����...
	;��ப� �ਤ���� �஫����...

	mov eax, [currentsymb]
	dec eax
	mov [startline], eax
	call draw_window
	jmp return_from_redraw_2_symbols

r2s_horizontal_check_ok:

	;���㥬 ���������� ����� �㪢� � ������� �������� ����

	mov ebx, [currentsymb]
	dec ebx

	movzx eax, byte [ebx]
	mov ecx, 64

	mul ecx

	add eax, big_font

	mov ebx, eax


	mov ecx, 16*65536+32

	mov eax, [currentsymb]
	sub eax, [startline]
	dec eax
	mov esi, 18
	mul esi
	add eax, 18
	mov esi, 65536
	mul esi
	add eax, 40

	mov edx, eax


	mov esi, 1

	mov edi, gray_text

	mov ebp, 0

	mov eax, 65

	int 0x40


	;���㥬 ������� �����


	mov ebx, [currentsymb]

	movzx eax, byte [ebx]
	mov ecx, 64
	mul ecx
	add eax, big_font

	mov ebx, eax

	mov ecx, 16*65536+32

	mov eax, [currentsymb]
	sub eax, [startline]
	mov esi, 18
	mul esi
	add eax, 18
	mov esi, 65536
	mul esi
	add eax, 40

	mov edx, eax

	mov esi, 1

	mov edi, red_text

	mov eax, 65

	int 0x40


return_from_redraw_2_symbols:

	ret

redraw_mistakes:
	;����ᨬ ��אַ㣮�쭨�
	mov eax, 13
	mov ebx, 59*65536+75
	mov ecx, 14*65536+10
	mov edx, 0x00CCCCCC
	int 0x40

	; ����㥬 ������� "������"
	mov eax, 4
	mov ebx, 60*65536+15
	mov ecx, 0x80AA0000
	mov edx, mistakestext
	int 0x40

	; �뢥��� �᫮ �訡��
	mov eax, 47
	mov ebx, 10000000000001000000000000000000b
	mov ecx, [mistakes]
	mov edx, 105*65536+15
	mov esi, 0x00AA0000
	int 0x40

	ret

count_lines:
	xor ecx, ecx		; � ecx - ����稪 ��ப
	inc ecx 		; � ��� 1 ��ப�


	; ��稭��� ࠧ���...
	mov eax, [startline]
	dec eax

cl_next_step:
	inc eax
	mov bh, [eax]

	cmp bh, 0x00
	jne cl_not_new_line
	inc ecx
	jmp cl_next_step

cl_not_new_line:
	cmp bh, 0xFE
	je cl_end
	cmp bh, 0xFF
	je cl_end
	jmp cl_next_step

cl_end:
	mov [linecount], ecx
	ret

reset_speed_counters:
	mov eax, 0x00000000

	mov [symbtyped], eax
	mov [typestarttime], eax
	mov [currenttime], eax
	mov [typingspeed], eax

	ret

count_speed:
	mov eax, 26
	mov ebx, 9
	int 0x40

	mov [currenttime], eax

	mov ebx, [typestarttime]

	sub eax, ebx   ;������ � eax - �᫮ ��襤�� ����� ᥪ㭤


	mov ecx, [symbtyped]
	cmp ecx, 0x00
	jne cs_all_ok
	inc ecx

cs_all_ok:
	xor edx, edx;

	div ecx  ;������ � eax - �।��� ��㧠 ����� ᨬ������

	mov ecx, eax
	cmp ecx, 0x00
	jne cs_all_ok_2
	inc ecx

cs_all_ok_2:

	xor edx, edx
	mov eax, 6000

	div ecx

	mov [typingspeed], eax ;��� � ���. � [typingspeed] - �।��� ᪮���� �����


	ret

speed_to_string:
	; �८�ࠧ�� �᫮ � ��ப� � �����뢠�� �� ��६� ��६����� speedbytes ����� ������
	xor edx, edx
	mov eax, [typingspeed]

	mov ecx, 10

	div ecx

	add dl, 30h
	mov [speedbytes + 3], dl

	xor edx, edx
	div ecx
	add dl, 30h
	mov [speedbytes + 2], dl

	xor edx, edx
	div ecx
	add dl, 30h
	mov [speedbytes + 1], dl

	xor edx, edx
	div ecx

	cmp dl, 0x00
	je sts_move_space

	add dl, 30h
	mov [speedbytes], dl
	jmp sts_end
    sts_move_space:
	mov [speedbytes], 0x20

    sts_end:

	ret

get_screen_params:

	mov eax, 14
	int 0x40
	mov [areaheight], ax

	push ax

	shr eax, 16
	mov [areawidth],ax

	;� ax �� �०���� ��室���� �ਭ� ����. ��ᯮ��㥬�� �⨬
	sub ax, 780   ;���⥬ ��砫�� ࠧ��� ����
	shr ax, 1

	mov [windowx], ax


	; ���室�� � ����
	xor ax, ax
	pop ax		; ��⮫���� �� �⥪� ���祭�� �����

	sub ax, 580
	shr ax, 1
	mov [windowy], ax


	ret




;draw_speed:

	;����ᨬ ��אַ㣮�쭨�
;        mov eax, 13
;        mov ebx, 59*65536+340
;        mov ecx, 29*65536+10
;        mov edx, 0x00CCCCCC
;        int 0x40

	; ����㥬 ������� "������� ����� (������ � ������):"
;        mov eax, 4
;        mov ebx, 60*65536+30
;        mov ecx, 0x80008800
;        mov edx, speedtext
;        int 0x40

	; �뢥��� �᫮
;        mov eax, 47
;        mov ebx, 10000000000001000000000000000000b
;        mov ecx, [typingspeed]
;        mov edx, 305*65536+30
;        mov esi, 0x00008800
;        int 0x40

;        ret

;������� )
datastart dd 0x0       ; ���� � �����, �� ���஬� ��室���� ����� ��� �����
currentsymb dd 0x0     ; ����騩 ᨬ���
startline dd 0x0       ; ��砫� ��ࢮ� ������� ��ப�
lessonstart dd 0x0
lastsymb dd 0x0        ; ������ � �����, ���࠭�� ��᫥����
mistakes dd 0x0        ; ������⢮ �訡��

areawidth dw 0x0
areaheight dw 0x0
windowx dw 0x0
windowy dw 0x0

linecount dd 0x00
currentline dd 0x00

symbtyped dd 0x0
typestarttime dd 0x0
currenttime dd 0x0
typingspeed dd 0x0

windowwidth dd 0x00
windowheight dd 0x00

text db '���������� �७���� -�J|A�u|||A- 0.9',0
buttontext db '������饥 �ࠦ�����', 0
retrybuttontext db '������� �ࠦ�����', 0
restartbuttontext db '����� ���� ������', 0

speedtext db '������� ����� (������ � ������):',0
mistakestext db '������:', 0
lessoncomplete db '��ࠦ����� �����襭�!',0x00
db '�।��� ᪮���� �����: '
speedbytes db 0x30,0x30,0x30,0x30
db ' ��./���.', 0xFE, 0xFF
lessoncompleteend:

lineistext db '������        ��', 0

kbbuffer db 1024 DUP (0x00)
filestruct:	 ;������� ������ ��� �맮�� �㭪樨 70
arg1 dd 0x0
arg2 dd 0x0
arg3 dd 0x0
arg4 dd 0x0
arg5 dd 0x0
arg6 db 0x0
arg7 dd 0x0

filedatastruct:   ;��ਡ��� � ����� 䠩��
attrib dd 0x0
fntype db 0x0
db 0x0
db 0x0
db 0x0
dd 0x0	;+8
dd 0x0	;+12
dd 0x0	;+16
dd 0x0	;+20
dd 0x0	;+24
dd 0x0	;+28
size  dq 0x0  ;+32
;size2 dd 0x0  ;+36

filesize dd 0x0

par:
params rb 1024

string db 0x07,' ������������  ____________    ��� �� ',0x07, 0x00
       db 0x07,' ��������     I KJ|ABuIIIA I          ', 0x07, 0x00
       db 0x07,' ������ 0.9   I____________I  ������� ', 0x07, 0x00
       db 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07
       db 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07
       db 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07
       db 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,  0x00

       db 0x07, 0x07, 0x07, '  ����ᠭ� �� ��⮬ ��ᥬ����   ',0x07, 0x07, 0x07, 0x0
       db  ' ',0x0
       db  '���ࠡ��稪�:', 0x00
       db  '- ���३ ��堩����� [Dron2004] ', 0x00
       db  ' ',0x0

       db  '   �ணࠬ�� �뫠 ����饭� ���  ���-', 0x00
       db  '���஢. ��� �����業��� ࠡ��� � ��-', 0x00
       db  '�ࠬ��� ����室�� 䠩� � �ଠ� .KLA,', 0x00
       db  'ᮤ�ঠ騩 �祡�� ������� ��� �����.', 0x00
       db  ' ', 0x00
       db  '(饫���� �� ������ ��������� ����������)', 0xFE

       db  0x07,'  KLA-䠩� ᮤ�ন� ��ப� ��� �����,', 0x00
       db  'ࠧ�����   �㫥��  ᨬ����� (0x00).', 0x00
       db  '�ப� �⤥������ ���� �� ��㣮�� ���⮬', 0x00
       db  '0xFE  (�⠢����   �   ����   ��᫥����', 0x00
       db  '��ப� �ப� ����� 0x00).  ���� ������', 0x00
       db  '�����稢�����  ���⮬   0xFF  (�⠢����', 0x00
       db  '�  ����  ��᫥����  ��ப�  ��᫥�����', 0x00
       db  '�ப� ��᫥ 0xFE).', 0x00
       db  '   ���  KLA-䠩��  ��।����� �ணࠬ��', 0x00
       db  '��  ����᪥   �   ����⢥   ��ࠬ���', 0x00
       db  '���������   ��ப�, ���ਬ��:', 0x00
       db  ' ', 0x00
       db  'klavisha /rd/1/lessons.kla', 0x00
       db  ' ', 0x00
       db  '(饫���� �� ������ ��������� ����������)', 0xFE
       db  0x07,'  ���  �����  -  ������  �।�������', 0x00
       db  '⥪�� � ���������  ������⢮�  �訡��.', 0x00
       db  '���室  � ������  �ࠦ�����   �� ��㣮�', 0x00
       db  '�����⢫����     �    �������    ������', 0x00
       db  '��������� ����������.', 0x00
       db  '�   ⥯���  ����� ���஡����� ��ࠡ����', 0x00
       db  '�  ����஬.  ������� �� ������ ���������', 0x00
       db  '���������� � ��稭��� ������� ⥪��!,', 0x00
       db  '   �� ������:  ��  ⮫쪮 ����������!', 0x00
       db  '���  ���祭�� ����஬� ������ ����室��', 0x00
       db  'ᮮ⢥�����騩 KLA-䠩�!', 0x00
       db  ' ', 0x00
       db  ' ', 0x00
       db  '(饫���� �� ������ ��������� ����������)', 0xFE
       db  '�뢠 �� ���� �뢠 �� ���� �뢠 �� ����', 0x00
       db  '�뢠 �� ���� �뢠 �� ���� �뢠 �� ����', 0x00
       db  '�뢠 �� ���� �뢠 �� ���� �뢠 �� ����', 0x00
       db  '� � � � � � � � � � � � � � � � � � �', 0x00
       db  '� � � � � � � � � � � � � � � � � � �', 0x00
       db  '�뢠 ���� ��� �� ��� ���� ��� ஢', 0x00
       db  '�뢠 ���� ��� �� ��� ���� ��� ஢', 0x00
       db  '�뢠 ���� ��� �� ��� ���� ��� ஢', 0x00
       db  '��� ��� ���� ��� ��� �� �� ��� த', 0x00
       db  '��� ��� ���� ��� ��� �� �� ��� த', 0x00
       db  '��� ��� ���� ��� ��� �� �� ��� த', 0x00
       db  '�� �� ��� ��� �� � �� �� �� �� ��',0x00
       db  '�� �� ��� ��� �� � �� �� �� �� ��',0x00
       db  '�� �� ��� ��� �� � �� �� �� �� ��',0xFE
       db  '��ᬨ ��� ��ᬨ ��� ��ᬨ ��� �� ��', 0x00
       db  '��ᬨ ��� ��ᬨ ��� ��ᬨ ��� �� ��', 0x00
       db  '�ଠ ��� ��� ���� ��� ���쬠 ﬠ', 0x00
       db  '���� ���� ���� ���� ���� ���� ����',0x00
       db  '���� ���� ���� ���� ���� ���� ����',0x00
       db  'ᨫ� ᮬ ���� ⮬� ஬ ��� �ࠢ�� ���', 0x00
       db  '��� ��� ��� ��ந�� �⮨� ������ ����', 0x00
       db  '᮫��� ���� �說� �� ������ ��ந��', 0xFE
       db  '��㪥 ������ ��㪥 ������', 0x00
       db  '��㪥 ������ ��㪥 ������', 0x00
       db  '��த ����஢� �⮨� �� ४� ⮬�', 0x00
       db  '�� ���뢠���� �� ⮬� ��⮬� ��', 0x00
       db  '⮬� �� ��㣮� ��த ᨡ��', 0x00
       db  '������� ����� �ᯮ�짮���� ������', 0x00
       db  '��� �冷� ����������', 0xFE
       db  '������ - �� ������⭠� �����-', 0x00
       db  '����筠� ����樮���� ��⥬�,', 0x00
       db  '����ᠭ��� �� ��⮬ ��ᥬ����.', 0x00
       db  '��⥬� �����砩�� ����� � ������⭠.', 0x00
       db  '᫠�� ������! ��!', 0x00
       db  '�७�஢�� ����� ����諠 � �����', 0xFE, 0xFF

stringend:

big_font:
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 252, 63, 252, 192, 3, 192, 3
db 204, 51, 204, 51, 192, 3, 192, 3, 192, 3
db 192, 3, 207, 243, 207, 243, 195, 195, 195, 195
db 192, 3, 192, 3, 192, 3, 192, 3, 63, 252
db 63, 252, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 252, 63, 252
db 255, 255, 255, 255, 243, 207, 243, 207, 255, 255
db 255, 255, 255, 255, 255, 255, 240, 15, 240, 15
db 252, 63, 252, 63, 255, 255, 255, 255, 255, 255
db 255, 255, 63, 252, 63, 252, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 60, 240
db 60, 240, 255, 252, 255, 252, 255, 252, 255, 252
db 255, 252, 255, 252, 255, 252, 255, 252, 63, 240
db 63, 240, 15, 192, 15, 192, 3, 0, 3, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 0, 3, 0, 15, 192, 15, 192
db 63, 240, 63, 240, 255, 252, 255, 252, 63, 240
db 63, 240, 15, 192, 15, 192, 3, 0, 3, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 15, 240, 15, 240
db 15, 240, 15, 240, 252, 63, 252, 63, 252, 63
db 252, 63, 252, 63, 252, 63, 3, 192, 3, 192
db 3, 192, 3, 192, 15, 240, 15, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 3, 192, 3, 192
db 15, 240, 15, 240, 63, 252, 63, 252, 255, 255
db 255, 255, 255, 255, 255, 255, 63, 252, 63, 252
db 3, 192, 3, 192, 3, 192, 3, 192, 15, 240
db 15, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 15, 240, 15, 240
db 15, 240, 15, 240, 3, 192, 3, 192, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 252, 63, 252, 63
db 240, 15, 240, 15, 240, 15, 240, 15, 252, 63
db 252, 63, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 15, 240, 15, 240
db 60, 60, 60, 60, 48, 12, 48, 12, 48, 12
db 48, 12, 60, 60, 60, 60, 15, 240, 15, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 240, 15, 240, 15, 195, 195, 195, 195, 207, 243
db 207, 243, 207, 243, 207, 243, 195, 195, 195, 195
db 240, 15, 240, 15, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 252, 3, 252, 0, 252, 0, 252
db 3, 204, 3, 204, 15, 12, 15, 12, 63, 192
db 63, 192, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 63, 192
db 63, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 15, 240, 15, 240
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 15, 240, 15, 240
db 3, 192, 3, 192, 63, 252, 63, 252, 3, 192
db 3, 192, 3, 192, 3, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 15, 255, 15, 255, 15, 15, 15, 15, 15, 255
db 15, 255, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 63, 0
db 63, 0, 255, 0, 255, 0, 252, 0, 252, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 255, 63, 255, 60, 15
db 60, 15, 63, 255, 63, 255, 60, 15, 60, 15
db 60, 15, 60, 15, 60, 15, 60, 15, 60, 15
db 60, 15, 60, 63, 60, 63, 252, 63, 252, 63
db 252, 60, 252, 60, 240, 0, 240, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 243, 207, 243, 207, 15, 240, 15, 240, 252, 63
db 252, 63, 15, 240, 15, 240, 243, 207, 243, 207
db 3, 192, 3, 192, 3, 192, 3, 192, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 192, 0
db 192, 0, 240, 0, 240, 0, 252, 0, 252, 0
db 255, 0, 255, 0, 255, 192, 255, 192, 255, 252
db 255, 252, 255, 192, 255, 192, 255, 0, 255, 0
db 252, 0, 252, 0, 240, 0, 240, 0, 192, 0
db 192, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 12, 0, 12, 0, 60, 0, 60
db 0, 252, 0, 252, 3, 252, 3, 252, 15, 252
db 15, 252, 255, 252, 255, 252, 15, 252, 15, 252
db 3, 252, 3, 252, 0, 252, 0, 252, 0, 60
db 0, 60, 0, 12, 0, 12, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 3, 192, 3, 192, 15, 240, 15, 240, 63, 252
db 63, 252, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 63, 252, 63, 252, 15, 240
db 15, 240, 3, 192, 3, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 0, 0, 0, 0, 60, 60, 60, 60
db 60, 60, 60, 60, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 255
db 63, 255, 243, 207, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 63, 207, 63, 207, 3, 207
db 3, 207, 3, 207, 3, 207, 3, 207, 3, 207
db 3, 207, 3, 207, 3, 207, 3, 207, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 60, 0, 60, 0
db 15, 192, 15, 192, 60, 240, 60, 240, 240, 60
db 240, 60, 240, 60, 240, 60, 60, 240, 60, 240
db 15, 192, 15, 192, 0, 240, 0, 240, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 255, 252, 255, 252, 255, 252
db 255, 252, 255, 252, 255, 252, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 3, 192, 3, 192, 15, 240, 15, 240, 63, 252
db 63, 252, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 63, 252, 63, 252, 15, 240
db 15, 240, 3, 192, 3, 192, 63, 252, 63, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 15, 240
db 15, 240, 63, 252, 63, 252, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 63, 252, 63, 252
db 15, 240, 15, 240, 3, 192, 3, 192, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 0, 240
db 0, 240, 255, 252, 255, 252, 0, 240, 0, 240
db 3, 192, 3, 192, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 15, 0
db 15, 0, 60, 0, 60, 0, 255, 252, 255, 252
db 60, 0, 60, 0, 15, 0, 15, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 0, 240, 0
db 240, 0, 240, 0, 240, 0, 240, 0, 255, 252
db 255, 252, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 12, 192, 12, 192
db 60, 240, 60, 240, 255, 252, 255, 252, 60, 240
db 60, 240, 12, 192, 12, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 3, 0, 3, 0
db 15, 192, 15, 192, 15, 192, 15, 192, 63, 240
db 63, 240, 63, 240, 63, 240, 255, 252, 255, 252
db 255, 252, 255, 252, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 255, 252, 255, 252, 63, 240
db 63, 240, 63, 240, 63, 240, 15, 192, 15, 192
db 15, 192, 15, 192, 3, 0, 3, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 3, 192, 3, 192, 15, 240, 15, 240, 15, 240
db 15, 240, 15, 240, 15, 240, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 12, 48, 12, 48, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 60, 240, 60, 240, 60, 240, 60, 240
db 255, 252, 255, 252, 60, 240, 60, 240, 60, 240
db 60, 240, 60, 240, 60, 240, 255, 252, 255, 252
db 60, 240, 60, 240, 60, 240, 60, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 3, 192
db 3, 192, 63, 240, 63, 240, 240, 60, 240, 60
db 240, 12, 240, 12, 240, 0, 240, 0, 63, 240
db 63, 240, 0, 60, 0, 60, 0, 60, 0, 60
db 192, 60, 192, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 3, 192, 3, 192, 3, 192, 3, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 240, 12, 240, 12, 240, 60
db 240, 60, 0, 240, 0, 240, 3, 192, 3, 192
db 15, 0, 15, 0, 60, 0, 60, 0, 240, 60
db 240, 60, 192, 60, 192, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 15, 192, 15, 192, 60, 240, 60, 240, 60, 240
db 60, 240, 15, 192, 15, 192, 63, 60, 63, 60
db 243, 240, 243, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 63, 60, 63, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 60, 0, 60, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 240
db 0, 240, 3, 192, 3, 192, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 3, 192, 3, 192, 0, 240, 0, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 15, 0, 15, 0, 3, 192, 3, 192
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 3, 192, 3, 192, 15, 0
db 15, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 60, 60
db 60, 60, 15, 240, 15, 240, 255, 255, 255, 255
db 15, 240, 15, 240, 60, 60, 60, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 63, 252, 63, 252, 3, 192, 3, 192, 3, 192
db 3, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 15, 0, 15, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 252
db 255, 252, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 3, 192
db 3, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 12, 0, 12, 0, 60
db 0, 60, 0, 240, 0, 240, 3, 192, 3, 192
db 15, 0, 15, 0, 60, 0, 60, 0, 240, 0
db 240, 0, 192, 0, 192, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 240, 63, 240, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 252, 240, 252, 243, 60, 243, 60
db 252, 60, 252, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 63, 240, 63, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 15, 192
db 15, 192, 63, 192, 63, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 63, 252, 63, 252, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 0, 60, 0, 60
db 0, 240, 0, 240, 3, 192, 3, 192, 15, 0
db 15, 0, 60, 0, 60, 0, 240, 0, 240, 0
db 240, 60, 240, 60, 255, 252, 255, 252, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 240, 63, 240, 240, 60, 240, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 15, 240
db 15, 240, 0, 60, 0, 60, 0, 60, 0, 60
db 0, 60, 0, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 240, 0, 240
db 3, 240, 3, 240, 15, 240, 15, 240, 60, 240
db 60, 240, 240, 240, 240, 240, 255, 252, 255, 252
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 3, 252, 3, 252, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 240, 0, 240, 0, 240, 0
db 240, 0, 240, 0, 240, 0, 255, 240, 255, 240
db 0, 60, 0, 60, 0, 60, 0, 60, 0, 60
db 0, 60, 240, 60, 240, 60, 63, 240, 63, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 15, 192, 15, 192, 60, 0
db 60, 0, 240, 0, 240, 0, 240, 0, 240, 0
db 255, 240, 255, 240, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 63, 240, 63, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 252
db 255, 252, 240, 60, 240, 60, 0, 60, 0, 60
db 0, 60, 0, 60, 0, 240, 0, 240, 3, 192
db 3, 192, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 240, 63, 240, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 240, 63, 240
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 63, 252, 63, 252, 0, 60, 0, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 0, 240
db 0, 240, 63, 192, 63, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 3, 192, 3, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 3, 192, 3, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 15, 0, 15, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 60, 0, 60, 0, 240, 0, 240
db 3, 192, 3, 192, 15, 0, 15, 0, 60, 0
db 60, 0, 15, 0, 15, 0, 3, 192, 3, 192
db 0, 240, 0, 240, 0, 60, 0, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 252, 63, 252, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 252, 63, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 60, 0, 60, 0, 15, 0, 15, 0, 3, 192
db 3, 192, 0, 240, 0, 240, 0, 60, 0, 60
db 0, 240, 0, 240, 3, 192, 3, 192, 15, 0
db 15, 0, 60, 0, 60, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 240, 63, 240, 240, 60, 240, 60, 240, 60
db 240, 60, 0, 240, 0, 240, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 240, 60, 240, 60
db 243, 252, 243, 252, 243, 252, 243, 252, 243, 252
db 243, 252, 243, 240, 243, 240, 240, 0, 240, 0
db 63, 240, 63, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 0
db 3, 0, 15, 192, 15, 192, 60, 240, 60, 240
db 240, 60, 240, 60, 240, 60, 240, 60, 255, 252
db 255, 252, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 240, 255, 240, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 63, 240
db 63, 240, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 255, 240
db 255, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 15, 240, 15, 240
db 60, 60, 60, 60, 240, 12, 240, 12, 240, 0
db 240, 0, 240, 0, 240, 0, 240, 0, 240, 0
db 240, 0, 240, 0, 240, 12, 240, 12, 60, 60
db 60, 60, 15, 240, 15, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 192, 255, 192, 60, 240, 60, 240, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 240, 60, 240, 255, 192, 255, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 255, 252, 255, 252, 60, 60
db 60, 60, 60, 12, 60, 12, 60, 192, 60, 192
db 63, 192, 63, 192, 60, 192, 60, 192, 60, 0
db 60, 0, 60, 12, 60, 12, 60, 60, 60, 60
db 255, 252, 255, 252, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 252
db 255, 252, 60, 60, 60, 60, 60, 12, 60, 12
db 60, 192, 60, 192, 63, 192, 63, 192, 60, 192
db 60, 192, 60, 0, 60, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 255, 0, 255, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 15, 240, 15, 240, 60, 60, 60, 60
db 240, 12, 240, 12, 240, 0, 240, 0, 240, 0
db 240, 0, 243, 252, 243, 252, 240, 60, 240, 60
db 240, 60, 240, 60, 60, 60, 60, 60, 15, 204
db 15, 204, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 255, 252, 255, 252, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 15, 240, 15, 240, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 15, 240, 15, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 252, 3, 252, 0, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 63, 192, 63, 192, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 252, 60
db 252, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 240, 60, 240, 63, 192, 63, 192, 63, 192
db 63, 192, 60, 240, 60, 240, 60, 60, 60, 60
db 60, 60, 60, 60, 252, 60, 252, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 0, 255, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 0
db 60, 12, 60, 12, 60, 60, 60, 60, 255, 252
db 255, 252, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 60, 240, 60
db 252, 252, 252, 252, 255, 252, 255, 252, 255, 252
db 255, 252, 243, 60, 243, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 240, 60, 240, 60, 252, 60, 252, 60, 255, 60
db 255, 60, 255, 252, 255, 252, 243, 252, 243, 252
db 240, 252, 240, 252, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 240, 63, 240, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 63, 240, 63, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 240
db 255, 240, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 63, 240, 63, 240, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 255, 0, 255, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 240, 63, 240, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 243, 60, 243, 60, 243, 252, 243, 252, 63, 240
db 63, 240, 0, 240, 0, 240, 0, 252, 0, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 255, 240, 255, 240
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 63, 240, 63, 240, 60, 240, 60, 240
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 252, 60, 252, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 240, 63, 240, 240, 60, 240, 60, 240, 60
db 240, 60, 60, 0, 60, 0, 15, 192, 15, 192
db 0, 240, 0, 240, 0, 60, 0, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 63, 240, 63, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 252, 63, 252, 63, 252
db 63, 252, 51, 204, 51, 204, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 15, 240, 15, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 60, 240, 60, 240, 15, 192, 15, 192, 3, 0
db 3, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 255, 252, 255, 252, 252, 252
db 252, 252, 60, 240, 60, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 240, 60, 240, 60, 240, 60, 240, 60, 60, 240
db 60, 240, 63, 240, 63, 240, 15, 192, 15, 192
db 15, 192, 15, 192, 63, 240, 63, 240, 60, 240
db 60, 240, 240, 60, 240, 60, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 15, 240, 15, 240, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 15, 240, 15, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 252
db 255, 252, 240, 60, 240, 60, 192, 60, 192, 60
db 0, 240, 0, 240, 3, 192, 3, 192, 15, 0
db 15, 0, 60, 0, 60, 0, 240, 12, 240, 12
db 240, 60, 240, 60, 255, 252, 255, 252, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 15, 240, 15, 240, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 240
db 15, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 192, 0, 192, 0, 240, 0, 240, 0, 252, 0
db 252, 0, 63, 0, 63, 0, 15, 192, 15, 192
db 3, 240, 3, 240, 0, 252, 0, 252, 0, 60
db 0, 60, 0, 12, 0, 12, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 15, 240, 15, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 15, 240, 15, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 3, 0, 3, 0
db 15, 192, 15, 192, 60, 240, 60, 240, 240, 60
db 240, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 255, 255, 255, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 15, 0
db 15, 0, 3, 192, 3, 192, 0, 240, 0, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 192
db 63, 192, 0, 240, 0, 240, 63, 240, 63, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 63, 60, 63, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 252, 0, 252, 0, 60, 0, 60, 0, 60, 0
db 60, 0, 63, 192, 63, 192, 60, 240, 60, 240
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 63, 240, 63, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 240, 63, 240
db 240, 60, 240, 60, 240, 0, 240, 0, 240, 0
db 240, 0, 240, 0, 240, 0, 240, 60, 240, 60
db 63, 240, 63, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 240
db 3, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 15, 240, 15, 240, 60, 240, 60, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 63, 60, 63, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 240, 63, 240, 240, 60
db 240, 60, 255, 252, 255, 252, 240, 0, 240, 0
db 240, 0, 240, 0, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 3, 240, 3, 240
db 15, 60, 15, 60, 15, 12, 15, 12, 15, 0
db 15, 0, 63, 192, 63, 192, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 63, 192, 63, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 60, 63, 60, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 63, 240, 63, 240
db 0, 240, 0, 240, 240, 240, 240, 240, 63, 192
db 63, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 252, 0, 252, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 240, 60, 240
db 63, 60, 63, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 252, 60, 252, 60, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 3, 192, 3, 192, 0, 0, 0, 0
db 15, 192, 15, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 15, 240, 15, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 60, 0, 60, 0, 60, 0, 60
db 0, 0, 0, 0, 0, 252, 0, 252, 0, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 0, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 0, 60
db 0, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 15, 240, 15, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 252, 0, 252, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 60
db 60, 60, 60, 240, 60, 240, 63, 192, 63, 192
db 63, 192, 63, 192, 60, 240, 60, 240, 60, 60
db 60, 60, 252, 60, 252, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 15, 192, 15, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 15, 240, 15, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 252, 240, 252, 240
db 255, 252, 255, 252, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 240, 60, 240, 60, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 243, 240, 243, 240, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 240, 63, 240, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 243, 240
db 243, 240, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 63, 240, 63, 240, 60, 0, 60, 0
db 60, 0, 60, 0, 255, 0, 255, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 60, 63, 60, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 63, 240, 63, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 3, 252
db 3, 252, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 243, 240, 243, 240
db 63, 60, 63, 60, 60, 60, 60, 60, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 0
db 255, 0, 255, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 240, 63, 240, 240, 60, 240, 60, 60, 0
db 60, 0, 15, 192, 15, 192, 0, 240, 0, 240
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 0, 3, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 255, 240, 255, 240, 15, 0
db 15, 0, 15, 0, 15, 0, 15, 0, 15, 0
db 15, 0, 15, 0, 15, 60, 15, 60, 3, 240
db 3, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 63, 60, 63, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 60, 240, 60, 240, 15, 192, 15, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 60, 240, 60
db 240, 60, 240, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 255, 252, 255, 252
db 60, 240, 60, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 240, 60, 240, 60, 60, 240, 60, 240, 15, 192
db 15, 192, 15, 192, 15, 192, 15, 192, 15, 192
db 60, 240, 60, 240, 240, 60, 240, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 252
db 63, 252, 0, 60, 0, 60, 0, 240, 0, 240
db 255, 192, 255, 192, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 252
db 255, 252, 240, 240, 240, 240, 3, 192, 3, 192
db 15, 0, 15, 0, 60, 0, 60, 0, 240, 60
db 240, 60, 255, 252, 255, 252, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 252, 0, 252, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 63, 0, 63, 0
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 0, 252, 0, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 0
db 63, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 0, 252, 0, 252, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 63, 0, 63, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 60
db 63, 60, 243, 240, 243, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 0, 3, 0, 15, 192
db 15, 192, 60, 240, 60, 240, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 255, 252
db 255, 252, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 3, 252, 3, 252, 15, 60, 15, 60, 60, 60
db 60, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 255, 252, 255, 252, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 255, 252, 255, 252, 60, 12
db 60, 12, 60, 0, 60, 0, 60, 0, 60, 0
db 63, 240, 63, 240, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 255, 240, 255, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 240
db 255, 240, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 63, 240, 63, 240, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 255, 240, 255, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 252, 255, 252, 60, 60, 60, 60
db 60, 12, 60, 12, 60, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 255, 0
db 255, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 3, 252, 3, 252
db 15, 60, 15, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 255, 255, 255, 255, 240, 15, 240, 15
db 192, 3, 192, 3, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 60, 60, 60, 60, 60, 12
db 60, 12, 60, 192, 60, 192, 63, 192, 63, 192
db 60, 192, 60, 192, 60, 0, 60, 0, 60, 12
db 60, 12, 60, 60, 60, 60, 255, 252, 255, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 243, 207, 243, 207, 243, 207
db 243, 207, 51, 204, 51, 204, 51, 204, 51, 204
db 63, 252, 63, 252, 63, 252, 63, 252, 51, 204
db 51, 204, 243, 207, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 0, 60, 0, 60
db 0, 60, 0, 60, 15, 240, 15, 240, 0, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 0, 60
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 252, 240, 252, 243, 252
db 243, 252, 255, 60, 255, 60, 252, 60, 252, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 60, 240
db 60, 240, 15, 192, 15, 192, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 252
db 240, 252, 243, 252, 243, 252, 255, 60, 255, 60
db 252, 60, 252, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 252, 60, 252, 60, 60, 60, 60, 60, 60, 240
db 60, 240, 60, 240, 60, 240, 63, 192, 63, 192
db 63, 192, 63, 192, 60, 240, 60, 240, 60, 240
db 60, 240, 60, 60, 60, 60, 252, 60, 252, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 255, 3, 255, 15, 60
db 15, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 240, 255, 240, 255, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 60
db 240, 60, 252, 252, 252, 252, 255, 252, 255, 252
db 255, 252, 255, 252, 243, 60, 243, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 255, 252
db 255, 252, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 240, 63, 240
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 255, 240, 255, 240, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 63, 240, 63, 240, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 60, 0
db 255, 0, 255, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 0, 240, 0, 240, 0, 240, 0, 240, 0
db 240, 0, 240, 0, 240, 0, 240, 12, 240, 12
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 255, 255, 255, 243, 207, 243, 207
db 195, 195, 195, 195, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 15, 240
db 15, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 63, 252, 63, 252
db 0, 60, 0, 60, 0, 60, 0, 60, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 252, 63, 252, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 243, 207, 63, 252
db 63, 252, 3, 192, 3, 192, 15, 240, 15, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 240, 60, 240, 60, 240, 60
db 240, 60, 60, 240, 60, 240, 63, 240, 63, 240
db 15, 192, 15, 192, 15, 192, 15, 192, 63, 240
db 63, 240, 60, 240, 60, 240, 240, 60, 240, 60
db 240, 60, 240, 60, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 255, 255, 255, 255, 0, 15
db 0, 15, 0, 15, 0, 15, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 63, 252, 63, 252, 0, 60, 0, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 0, 60
db 0, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 255, 252, 255, 252, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 255, 255, 255, 255
db 0, 15, 0, 15, 0, 15, 0, 15, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 255, 192, 255, 192, 255, 0
db 255, 0, 207, 0, 207, 0, 15, 0, 15, 0
db 15, 252, 15, 252, 15, 15, 15, 15, 15, 15
db 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
db 63, 252, 63, 252, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 15
db 240, 15, 240, 15, 240, 15, 240, 15, 240, 15
db 240, 15, 240, 15, 255, 15, 255, 15, 243, 207
db 243, 207, 243, 207, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 255, 15, 255, 15, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 0, 255, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 60, 0, 63, 240
db 63, 240, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 255, 240
db 255, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 240, 63, 240
db 240, 60, 240, 60, 0, 60, 0, 60, 12, 60
db 12, 60, 15, 252, 15, 252, 12, 60, 12, 60
db 0, 60, 0, 60, 0, 60, 0, 60, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 240, 252, 240, 252, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 255, 207, 255, 207
db 243, 207, 243, 207, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 240, 252, 240, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 15, 255, 15, 255, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 15, 252, 15, 252, 15, 252, 15, 252, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 252, 63, 252, 63, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 192, 63, 192, 0, 240, 0, 240, 63, 240
db 63, 240, 240, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 63, 60, 63, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 12
db 0, 12, 0, 60, 0, 60, 63, 240, 63, 240
db 240, 0, 240, 0, 240, 0, 240, 0, 255, 240
db 255, 240, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 240
db 255, 240, 60, 60, 60, 60, 60, 60, 60, 60
db 63, 240, 63, 240, 60, 60, 60, 60, 60, 60
db 60, 60, 255, 240, 255, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 252, 255, 252, 60, 12, 60, 12
db 60, 12, 60, 12, 60, 0, 60, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 255, 0, 255, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 3, 252, 3, 252
db 15, 60, 15, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 255, 255, 255, 255, 240, 15, 240, 15, 240, 15
db 240, 15, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 240, 63, 240, 240, 60, 240, 60, 240, 60
db 240, 60, 255, 252, 255, 252, 240, 0, 240, 0
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 243, 60, 243, 60, 243, 60
db 243, 60, 51, 48, 51, 48, 63, 240, 63, 240
db 51, 48, 51, 48, 243, 60, 243, 60, 243, 60
db 243, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 0, 60, 0, 60
db 15, 240, 15, 240, 0, 60, 0, 60, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 252, 240, 252, 243, 60, 243, 60, 252, 60
db 252, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 60, 240
db 60, 240, 15, 192, 15, 192, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 252, 240, 252, 243, 60
db 243, 60, 252, 60, 252, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 252, 60, 252, 60, 60, 240, 60, 240, 63, 192
db 63, 192, 63, 192, 63, 192, 60, 240, 60, 240
db 60, 60, 60, 60, 252, 60, 252, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 252, 3, 252, 15, 60
db 15, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 252, 60
db 252, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 60
db 240, 60, 252, 252, 252, 252, 255, 252, 255, 252
db 255, 252, 255, 252, 243, 60, 243, 60, 243, 60
db 243, 60, 240, 60, 240, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 255, 252, 255, 252, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 240, 63, 240
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 63, 240, 63, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 3, 3, 3, 48, 48
db 48, 48, 3, 3, 3, 3, 48, 48, 48, 48
db 3, 3, 3, 3, 48, 48, 48, 48, 3, 3
db 3, 3, 48, 48, 48, 48, 3, 3, 3, 3
db 48, 48, 48, 48, 3, 3, 3, 3, 48, 48
db 48, 48, 3, 3, 3, 3, 48, 48, 48, 48
db 3, 3, 3, 3, 48, 48, 48, 48, 51, 51
db 51, 51, 204, 204, 204, 204, 51, 51, 51, 51
db 204, 204, 204, 204, 51, 51, 51, 51, 204, 204
db 204, 204, 51, 51, 51, 51, 204, 204, 204, 204
db 51, 51, 51, 51, 204, 204, 204, 204, 51, 51
db 51, 51, 204, 204, 204, 204, 51, 51, 51, 51
db 204, 204, 204, 204, 51, 51, 51, 51, 204, 204
db 204, 204, 243, 243, 243, 243, 63, 63, 63, 63
db 243, 243, 243, 243, 63, 63, 63, 63, 243, 243
db 243, 243, 63, 63, 63, 63, 243, 243, 243, 243
db 63, 63, 63, 63, 243, 243, 243, 243, 63, 63
db 63, 63, 243, 243, 243, 243, 63, 63, 63, 63
db 243, 243, 243, 243, 63, 63, 63, 63, 243, 243
db 243, 243, 63, 63, 63, 63, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 255, 192
db 255, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 255, 192, 255, 192, 3, 192
db 3, 192, 255, 192, 255, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 255, 60, 255, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 252, 255, 252, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 255, 192, 255, 192
db 3, 192, 3, 192, 255, 192, 255, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 255, 60, 255, 60, 0, 60, 0, 60, 255, 60
db 255, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 252
db 255, 252, 0, 60, 0, 60, 255, 60, 255, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 255, 60, 255, 60, 0, 60, 0, 60
db 255, 252, 255, 252, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 255, 252, 255, 252, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 255, 192, 255, 192, 3, 192, 3, 192, 255, 192
db 255, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 192, 255, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 255, 3, 255
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 255, 255, 255, 255, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 255, 255, 255, 255, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 255
db 3, 255, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 255, 255, 255, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 255, 255, 255, 255
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 255, 3, 255, 3, 192, 3, 192
db 3, 255, 3, 255, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 63, 15, 63, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 63, 15, 63, 15, 0, 15, 0, 15, 255
db 15, 255, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 15, 255, 15, 255, 15, 0
db 15, 0, 15, 63, 15, 63, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 255, 63
db 255, 63, 0, 0, 0, 0, 255, 255, 255, 255
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 255, 255, 255, 255, 0, 0, 0, 0
db 255, 63, 255, 63, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 63, 15, 63
db 15, 0, 15, 0, 15, 63, 15, 63, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 255, 255, 255, 255, 0, 0, 0, 0, 255, 255
db 255, 255, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 255, 63, 255, 63, 0, 0
db 0, 0, 255, 63, 255, 63, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 255, 255
db 255, 255, 0, 0, 0, 0, 255, 255, 255, 255
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 255, 255, 255, 255, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 255, 255, 255, 255
db 0, 0, 0, 0, 255, 255, 255, 255, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 255
db 255, 255, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 255, 15, 255, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 255
db 3, 255, 3, 192, 3, 192, 3, 255, 3, 255
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 3, 255, 3, 255, 3, 192, 3, 192
db 3, 255, 3, 255, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 15, 255, 15, 255, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 255, 255
db 255, 255, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 15, 60, 15, 60, 15, 60
db 15, 60, 15, 60, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 255, 255, 255, 255, 3, 192
db 3, 192, 255, 255, 255, 255, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 255, 192, 255, 192
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 3, 255, 3, 255, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 0, 255, 0, 255, 0
db 255, 0, 255, 0, 255, 0, 255, 0, 255, 0
db 255, 0, 255, 0, 255, 0, 255, 0, 255, 0
db 255, 0, 255, 0, 255, 0, 255, 0, 255, 0
db 255, 0, 255, 0, 255, 0, 255, 0, 255, 0
db 255, 0, 255, 0, 255, 0, 255, 0, 255, 0
db 255, 0, 255, 0, 255, 0, 255, 0, 0, 255
db 0, 255, 0, 255, 0, 255, 0, 255, 0, 255
db 0, 255, 0, 255, 0, 255, 0, 255, 0, 255
db 0, 255, 0, 255, 0, 255, 0, 255, 0, 255
db 0, 255, 0, 255, 0, 255, 0, 255, 0, 255
db 0, 255, 0, 255, 0, 255, 0, 255, 0, 255
db 0, 255, 0, 255, 0, 255, 0, 255, 0, 255
db 0, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 243, 240, 243, 240
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 60, 60, 60, 60, 60, 60, 60, 60, 60, 60
db 63, 240, 63, 240, 60, 0, 60, 0, 60, 0
db 60, 0, 255, 0, 255, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 63, 240, 63, 240, 240, 60, 240, 60, 240, 0
db 240, 0, 240, 0, 240, 0, 240, 0, 240, 0
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 63, 252, 63, 252, 51, 204
db 51, 204, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 15, 240
db 15, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 252
db 63, 252, 0, 60, 0, 60, 0, 60, 0, 60
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 15, 240, 15, 240, 3, 192
db 3, 192, 63, 252, 63, 252, 243, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 243, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 63, 252, 63, 252
db 3, 192, 3, 192, 3, 192, 3, 192, 15, 240
db 15, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 60, 240, 60
db 60, 240, 60, 240, 15, 192, 15, 192, 15, 192
db 15, 192, 15, 192, 15, 192, 60, 240, 60, 240
db 240, 60, 240, 60, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 255, 255, 255, 255, 0, 15
db 0, 15, 0, 15, 0, 15, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 63, 252, 63, 252, 0, 60, 0, 60, 0, 60
db 0, 60, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 255, 252, 255, 252, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 243, 60, 243, 60
db 243, 60, 243, 60, 243, 60, 255, 252, 255, 252
db 0, 15, 0, 15, 0, 15, 0, 15, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 255, 192, 255, 192
db 207, 0, 207, 0, 207, 0, 207, 0, 15, 252
db 15, 252, 15, 15, 15, 15, 15, 15, 15, 15
db 63, 252, 63, 252, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 255, 60, 255, 60, 243, 252, 243, 252
db 243, 252, 243, 252, 255, 60, 255, 60, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 255, 0, 255, 0, 60, 0
db 60, 0, 60, 0, 60, 0, 63, 240, 63, 240
db 60, 60, 60, 60, 60, 60, 60, 60, 255, 240
db 255, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 63, 240
db 63, 240, 240, 60, 240, 60, 0, 60, 0, 60
db 15, 252, 15, 252, 0, 60, 0, 60, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 240, 252, 240, 252, 243, 207, 243, 207
db 243, 207, 243, 207, 255, 207, 255, 207, 243, 207
db 243, 207, 243, 207, 243, 207, 240, 252, 240, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 255, 63, 255
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 252
db 63, 252, 15, 60, 15, 60, 60, 60, 60, 60
db 252, 63, 252, 63, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 60, 240, 60, 240, 0, 0, 0, 0, 255, 252
db 255, 252, 60, 60, 60, 60, 60, 12, 60, 12
db 60, 192, 60, 192, 63, 192, 63, 192, 60, 192
db 60, 192, 60, 0, 60, 0, 60, 12, 60, 12
db 60, 60, 60, 60, 255, 252, 255, 252, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 60, 240, 60, 240
db 0, 0, 0, 0, 63, 240, 63, 240, 240, 60
db 240, 60, 240, 60, 240, 60, 255, 240, 255, 240
db 240, 0, 240, 0, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 63, 240, 63, 240
db 240, 60, 240, 60, 240, 0, 240, 0, 240, 192
db 240, 192, 255, 192, 255, 192, 240, 192, 240, 192
db 240, 0, 240, 0, 240, 0, 240, 0, 240, 60
db 240, 60, 63, 240, 63, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 240, 63, 240, 240, 60, 240, 60
db 240, 0, 240, 0, 255, 192, 255, 192, 240, 0
db 240, 0, 240, 60, 240, 60, 63, 240, 63, 240
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 60, 60, 60, 60
db 0, 0, 0, 0, 15, 240, 15, 240, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 15, 240, 15, 240, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 60, 240, 60, 240, 0, 0, 0, 0
db 15, 192, 15, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 3, 192, 3, 192, 3, 192
db 3, 192, 3, 192, 15, 240, 15, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 60, 240, 60, 240, 15, 192
db 15, 192, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 63, 252, 63, 252, 0, 60, 0, 60
db 0, 60, 0, 60, 240, 60, 240, 60, 63, 240
db 63, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 60, 240, 60, 240, 15, 192, 15, 192, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 63, 252
db 63, 252, 0, 60, 0, 60, 0, 60, 0, 60
db 240, 60, 240, 60, 63, 240, 63, 240, 0, 0
db 0, 0, 0, 0, 0, 0, 15, 192, 15, 192
db 60, 240, 60, 240, 60, 240, 60, 240, 15, 192
db 15, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 3, 192, 3, 192, 3, 192
db 3, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 3, 192
db 3, 192, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 252
db 0, 252, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 0, 240, 0, 240
db 0, 240, 0, 240, 0, 240, 252, 240, 252, 240
db 60, 240, 60, 240, 15, 240, 15, 240, 3, 240
db 3, 240, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 240, 255, 240, 255
db 240, 243, 240, 243, 252, 255, 252, 255, 252, 240
db 252, 240, 255, 255, 255, 255, 243, 240, 243, 240
db 243, 240, 243, 240, 240, 240, 240, 240, 240, 240
db 240, 240, 240, 240, 240, 240, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 240, 60, 240, 60, 63, 240
db 63, 240, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 240, 60, 240, 60, 240, 60, 240, 60
db 240, 60, 63, 240, 63, 240, 240, 60, 240, 60
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 63, 252, 63, 252, 63, 252, 63, 252
db 63, 252, 63, 252, 63, 252, 63, 252, 63, 252
db 63, 252, 63, 252, 63, 252, 63, 252, 63, 252
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
db 0, 0, 0, 0

black_text:
dd 0x00CCCCCC, 0x00000000
green_text:
dd 0x00CCCCCC, 0x0000AA00
red_text:
dd 0x00CCCCCC, 0x00AA0000
gray_text:
dd 0x00CCCCCC, 0x00777777

logo:
db 00000000b, 00000000b, 01000000b, 00000000b
db 00000000b, 00000001b, 11000000b, 00000000b
db 00000000b, 00000001b, 00000000b, 00000000b
db 11111111b, 11111111b, 11111111b, 11111111b
db 10000000b, 00000000b, 00000000b, 00000001b
db 10101010b, 10101010b, 10101010b, 01010101b
db 10000000b, 00000000b, 00001010b, 00000001b
db 10101010b, 10101010b, 10100000b, 01010101b
db 10000000b, 00000000b, 00000100b, 00000001b
db 10101010b, 10101010b, 10101110b, 01010101b
db 10000000b, 00000000b, 00000000b, 00000001b
db 11111111b, 11111111b, 11111111b, 11111111b


I_END: