; �ணࠬ�� ��� �८�ࠧ������ �ᥫ �� ��ப�
;   � �ଠ� float, double, � ⠪�� �� 10 ��� 16 �筮��
;   �� float.
; ������� �� �᭮�� hex2dec2bin � �ਬ�஢ �� 䠩�� list2_05.inc
;   (���� �㫠��� �������� ������쥢�� 24.05.2002),
;   ����� ChE ��।���� � 16 �� 32 ��� �� ��ᥬ���� fasm.
; �ணࠬ�� �������� ���� ⠪�� �����:
; 1) �᫮ � ��ப���� ���� ��ॢ��� � 4 ���� (float) � ��設�� ���
; 2) �᫮ � ��ப���� ���� ��ॢ��� � 8 ���� (double) � ��設�� ���
; 3) �᫮ � ��設��� ���� (float) ��ॢ��� � ��ப��� ��� (5 ������ ��᫥ ����⮩)

use32
    org 0x0
    db	'MENUET01'
    dd	0x01,start,i_end,e_end,e_end,0,sys_path

include '../../../proc32.inc'
include '../../../macros.inc'
include '../../../develop/libraries/box_lib/load_lib.mac'
include '../../../develop/libraries/box_lib/trunk/box_lib.mac'
include 'lang.inc'

@use_library

align 4
start:
	load_libraries l_libs_start,l_libs_end
	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ��� ������⥪�
	mov	ebp,lib_0
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:
	mcall 40,0x27
	mcall 48, 3, sys_colors, 40
	edit_boxes_set_sys_color edit1,editboxes_end,sys_colors
	option_boxes_set_sys_color sys_colors,Option_boxes1

align 4
red:
    call draw_window

align 4
still:
    mcall 10		; �㭪�� 10 - ����� ᮡ���

    cmp  eax,1		; ����ᮢ��� ���� ?
    je	 red		; �᫨ �� - �� ���� red
    cmp  eax,2		; ����� ������ ?
    je	 key		; �᫨ �� - �� key
    cmp  eax,3		; ����� ������ ?
    je	 button 	; �᫨ �� - �� button
    cmp  eax,6
    je	 mouse

    jmp  still		; �᫨ ��㣮� ᮡ�⨥ - � ��砫� 横��

;---------------------------------------------------------------------
key: ; ����� ������ �� ���������
	mcall 2
	;cmp ah,13
	stdcall [edit_box_key], dword edit1
	jmp  still ; �������� � ��砫� 横��

; ������⢮ ������ �᫠ ��᫥ ����⮩ (1-17)
NumberSymbolsAD DW 5
; ����⠭�� (10 � �⥯��� N)
MConst DQ 1.0E1,1.0E2,1.0E3,1.0E4,1.0E5
       DQ 1.0E6,1.0E7,1.0E8,1.0E9,1.0E10
       DQ 1.0E11,1.0E12,1.0E13,1.0E14,1.0E15
       DQ 1.0E16,1.0E17,1.0E18,1.0E19,1.0E20
       DQ 1.0E21,1.0E22,1.0E23,1.0E24,1.0E25
       DQ 1.0E26,1.0E27,1.0E28,1.0E29,1.0E30
       DQ 1.0E31,1.0E32,1.0E33,1.0E34,1.0E35
       DQ 1.0E36,1.0E37,1.0E38,1.0E39,1.0E40
       DQ 1.0E41,1.0E42,1.0E43,1.0E44,1.0E45
       DQ 1.0E46,1.0E47,1.0E48,1.0E49,1.0E50
       DQ 1.0E51,1.0E52,1.0E53,1.0E54,1.0E55
       DQ 1.0E56,1.0E57,1.0E58,1.0E59,1.0E60
       DQ 1.0E61,1.0E62,1.0E63,1.0E64,1.0E65
       DQ 1.0E66,1.0E67,1.0E68,1.0E69,1.0E70
       DQ 1.0E71,1.0E72,1.0E73,1.0E74,1.0E75
       DQ 1.0E76,1.0E77,1.0E78,1.0E79,1.0E80
       DQ 1.0E81,1.0E82,1.0E83,1.0E84,1.0E85
       DQ 1.0E86,1.0E87,1.0E88,1.0E89,1.0E90
       DQ 1.0E91,1.0E92,1.0E93,1.0E94,1.0E95
       DQ 1.0E96,1.0E97,1.0E98,1.0E99,1.0E100
       DQ 1.0E101,1.0E102,1.0E103,1.0E104,1.0E105
       DQ 1.0E106,1.0E107,1.0E108,1.0E109,1.0E110
       DQ 1.0E111,1.0E112,1.0E113,1.0E114,1.0E115
       DQ 1.0E116,1.0E117,1.0E118,1.0E119,1.0E120
       DQ 1.0E121,1.0E122,1.0E123,1.0E124,1.0E125
       DQ 1.0E126,1.0E127,1.0E128
; ��᫮ � ������饩 ����⮩ ������� �筮��
Data_Double   DQ ?
; ��᫮ � BCD-�ଠ� 
Data_BCD      DT ?
; �ᯮ����⥫�� 䫠�
Data_Flag     DB ?
; ���� १���� (�᫨ �� 0 - ����⥫쭮� �᫮)
Data_Sign     DB ?
; ��ப� ��� �࠭���� �᫠ � ���� ASCII
Data_String   DB 32 DUP (?)

string1 db 32 dup (0)
string1_end:



;*******************************************************
;*  �������������� ����� � ��������� ������� � ������  *
;* ��᫮ ����� �ଠ� � 㤢������ �筮����, १���� *
;* �뤠���� � �����筮� ����, � "��⮢��" �ଠ� �   *
;* 䨪�஢���� ������⢮� ������ ��᫥ ����⮩.     *
;* �室�� ��ࠬ����:                                  *
;* Data_Double - �८�ࠧ㥬�� �᫮;                  *
;* NumberSymbolsAD - ������⢮ ������ ��᫥           *
;*                   ����⮩ (0-17).                   *
;* ��室�� ��ࠬ����:                                 *
;* Data_String - ��ப�-१����.                     *
;*******************************************************
DoubleFloat_to_String:
	pushad
	; ������� �����뢠�� � ��ப� Data_String
	mov	EDI, Data_String

	; �������� �᫮ ����� �� NumberSymbolsAD
	; �������� ࠧ�冷�
	fninit		       ;��� ᮯ�����
	fld	[Data_Double]  ;����㧨�� �᫮
	xor ebx,ebx
	mov	BX,[NumberSymbolsAD]
	cmp	BX, 0
	je	.NoShifts     ;��� ��� ��᫥ ����⮩
	jl	.Error	      ;�訡��
	dec	BX
	shl	BX, 3		;㬭����� �� 8
	add	EBX, MConst
	fmul	qword [EBX] ;㬭����� �� ����⠭��
.NoShifts:
	; ������� �᫮ � ���� BCD
	fbstp	[Data_BCD]
; �஢���� १���� �� ��९�������
	mov	AX,word [Data_BCD + 8]
	cmp	AX,0FFFFh  ;"�����筮�" ��९�������?
	je	.Overflow
; �뤥���� ���� �᫠ � ������� ��� � ASCII-����
	mov	AL, byte [Data_BCD + 9]
	and	AL,AL
	jz	.NoSign
	mov	AL,'-'
	stosb
.NoSign:
; ��ᯠ������ �᫮ � ��� ASCII
	mov	ebx,8	 ;ᬥ饭�� ��᫥���� ���� ���
	mov	ecx,9	 ;���稪 ��� ���
	; ��।����� ������ �����筮� �窨 � �᫥
	mov	DX,18
	sub	DX,[NumberSymbolsAD]
	js	.Error	;�訡��, �᫨ ����⥫쭠�
	jz	.Error	;��� �㫥��� ������
.NextPair:
	; ����㧨�� ��।��� ���� ࠧ�冷�
	mov	AL, byte [ebx + Data_BCD]
	mov	AH,AL
	; �뤥����, ��ॢ��� � ASCII �
	; ��࠭��� ������ ��ࠤ�
	shr	AL,4
	add	AL,'0'
	stosb
	dec	DX
	jnz	.N0
	mov	AL,'.'
	stosb
.N0:   ; �뤥����, ��ॢ��� � ASCII �
	; ��࠭��� ������� ��ࠤ�
	mov	AL,AH
	and	AL,0Fh
	add	AL,'0'
	stosb
	dec	DX
	jnz	.N1
	mov	AL,'.'
	stosb
.N1:
	dec  BX
	loop .NextPair
	mov  AL,0
	stosb

; ����� ������騥 �㫨 ᫥��
	mov	EDI, Data_String
	mov	ESI, Data_String
	; �ய����� ���� �᫠, �᫨ �� ����
	cmp	byte [ESI],'-'
	jne	.N2
	inc	ESI
	inc	EDI
.N2:   ; ����㧨�� � ���稪 横�� ������⢮ ࠧ�冷�
	; �᫠ ���� 1 (���� �����筮� �窨)
	mov	ecx,18+1+1
	; �ய����� ������騥 �㫨
.N3:
	cmp byte [ESI],'0'
	jne .N4
	cmp byte [ESI+1],'.'
	je .N4
	inc ESI
	loop .N3
	; �訡�� - ��� ������ ���
	jmp	.Error
; �����஢��� ������� ���� �᫠ � ��砫� ��ப�
.N4:	rep movsb
	jmp    .End

; �訡��
.Error:
	mov	AL,'E'
	stosb
	mov	AL,'R'
	stosb
	mov	AL,'R'
	stosb
	xor	AL,AL
	stosb
	jmp	.End
; ��९������� ࠧ�來�� �⪨
.Overflow:
	mov	AL,'#'
	stosb
	xor	AL,AL
	stosb
; ����� ��楤���
.End:
	popad
	ret

;****************************************************
;* ������������� ������ � ����� � ��������� ������� *
;*      (�᫮ ����� �����, "��⮢��" �ଠ�)     *
;* �室�� ��ࠬ����:                               *
;* Data_String - �᫮ � ���� ASCII.                *
;* ��室�� ��ࠬ����:                              *
;* Data_Double - �᫮ � ����筮� ����.             *
;****************************************************
String_to_DoubleFloat:
	pushad
	cld
	; ��頥� Data_BCD 
	mov dword [Data_BCD],0
	mov dword [Data_BCD+4],0
	mov  word [Data_BCD+8],0
	; ��頥� ���� �����
	mov	[Data_Sign],0
	; ����ᨬ � SI 㪠��⥫� �� ��ப�
	mov	ESI, Data_String
	; �ய�᪠�� �஡��� ��। �᫮�
	mov	ecx,64 ;���� �� ��横�������
.ShiftIgnore:
	lodsb
	cmp	AL,' '
	jne	.ShiftIgnoreEnd
	loop	.ShiftIgnore
	jmp	.Error
.ShiftIgnoreEnd:
	; �஢��塞 ���� �᫠
	cmp	AL,'-'
	jne	.Positive
	mov	[Data_Sign],80h
	lodsb
.Positive:
	mov	[Data_Flag],0 ;�ਧ��� ������ �窨
	mov	DX,0	      ;������ �窨
	mov	ecx,18	      ;����. �᫮ ࠧ�冷�
.ASCIItoBCDConversion:
	cmp	AL,'.'	      ;�窠?
	jne	.NotDot
	cmp	[Data_Flag],0 ;�窠 �� ����砫���?
	jne	.Error
	mov	[Data_Flag],1
	lodsb
	cmp	AL,0	      ;����� ��ப�?
	jne	.NotDot
	jmp	.ASCIItoBCDConversionEnd
.NotDot:
	; �������� �� 1 ���祭�� ����樨 �窨,
	; �᫨ ��� �� �� ����砫���
	cmp	[Data_Flag],0
	jnz	.Figures
	inc	DX
.Figures:
	; ������� �᫠ ������ ���� ��ࠬ�
	cmp	AL,'0'
	jb	.Error
	cmp	AL,'9'
	ja	.Error
	; ��襬 ��।��� ���� � ������� ��ࠤ� BCD
	and	AL,0Fh
	or	byte [Data_BCD],AL
	; �஢�ઠ �� ����� ��ப�
	cmp	byte [ESI],0
	je	.ASCIItoBCDConversionEnd
	; �������� BCD �� 4 ࠧ�鸞 �����
	; (ᤢ����� ���訥 2 ����)
	mov	AX,word [Data_BCD+6]
	shld	word [Data_BCD+8],AX,4
	; (ᤢ����� �।��� 4 ����)
	mov	EAX, dword [Data_BCD]
	shld	dword [Data_BCD+4],EAX,4
	; (ᤢ����� ����訥 4 ����)
	shl	dword [Data_BCD],4
	; ����㦠�� ᫥���騩 ᨬ��� � AL
	lodsb
	loop	.ASCIItoBCDConversion
	; �᫨ 19-� ᨬ��� �� 0 � �� �窠,
	; � �訡�� ��९�������
	cmp	AL,'.'
	jne	.NotDot2
	inc	ecx
	lodsb
.NotDot2:
	cmp	AL,0
	jne	.Error ;��९������� ࠧ�來�� �⪨

; ������������� ����� �� ���� BCD � ������������ �����
.ASCIItoBCDConversionEnd:
	; ������ ���� � ���訩 ����
	mov	AL,[Data_Sign]
	mov	byte [Data_BCD+9],AL
	; ������ ॣ����� ᮯ�����
	fninit
	; ����㧨�� � ᮯ����� �᫮ � BCD-�ଠ�
	fbld	[Data_BCD]
	; ���᫨�� ����� ����⥫�
	mov	EBX,18+1
	sub	BX,CX
	sub	BX,DX
	cmp	EBX,0
	je	.NoDiv
	dec	EBX
	shl	EBX,3		;㬭����� �� 8
	add	EBX, MConst
	fdiv	qword [EBX] ;ࠧ������ �� ����⠭��
.NoDiv:; ���㧨�� �᫮ � ����筮� �ଠ�
	fstp	[Data_Double]
	jmp	.End

.Error:; �� �� �訡�� ���㫨�� १����
	fldz	;������ ���� � �⥪ ᮯ�����
	fstp	[Data_Double]
.End:
	popad
	ret



;input:
; buf - 㪠��⥫� �� ��ப�, �᫮ ������ ���� � 10 ��� 16 �筮� ����
;output:
; eax - �᫮
align 4
proc conv_str_to_int, buf:dword
	xor eax,eax
	push ebx ecx esi
	xor ebx,ebx
	mov esi,[buf]
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
	pop esi ecx ebx
	ret
endp



;---------------------------------------------------------------------
align 4
button:
	mcall 17		; 17 - ������� �����䨪��� ����⮩ ������
	cmp   ah, 1	; �᫨ �� ����� ������ � ����஬ 1,
	jne   @f
		mcall -1
	@@:
	cmp ah, 5
	jne @f
		cmp dword[option_group1],opt3
		jne .opt_3_end
			stdcall conv_str_to_int,dword[edit1.text]
			mov dword[Data_Double],eax
			finit
			fld dword[Data_Double]
			fstp qword[Data_Double]

			; Data_Double - �८�ࠧ㥬�� �᫮
			; NumberSymbolsAD - ������⢮ ������ ��᫥ ����⮩ (0-17)
			call DoubleFloat_to_String
			mov dword[Data_Double],eax ;����⠭�������� ���祭�� � �ଠ� float
			jmp .opt_all_end
		.opt_3_end:

		mov esi,string1
		mov edi,Data_String
		cld
		mov ecx,32
		rep movsb

		call String_to_DoubleFloat
		cmp dword[option_group1],opt1
		jne .opt_all_end ;�᫨ ��࠭ float, � �८�ࠧ㥬 �� ࠭�� ����祭���� double
			finit
			fld  qword[Data_Double] ;�⠥� �� double
			fstp dword[Data_Double] ;� �����頥� �� float
		.opt_all_end:
		jmp red
	@@:
	jmp still

mouse:
	stdcall [edit_box_mouse], edit1
	stdcall [option_box_mouse], Option_boxes1
	jmp still

;------------------------------------------------
align 4
draw_window:
	mcall 48, 3, sys_colors, sizeof.system_colors

	mcall 12, 1
	mov edx, 0x14000000
	or  edx, [sys_colors.work]
	mcall 0, 200*65536+300, 200*65536+175, ,,title

	mcall 8, (300-53)*65536+38,145*65536+ 15, 5,[sys_colors.work_button] ; ������ Ok

	mov ecx, 0x80000000
	or  ecx, [sys_colors.work_text]
	mcall 4, 15*65536 +30,, binstr,
	mcall  , 15*65536 +58,, decstr,
		mcall  , (240-56*3)*65536 +58,, Data_String,
	mcall  , 15*65536 +72,, hexstr,
	mcall  , 15*65536+150,, numstr,

	mov ecx, 0x80000000
	or  ecx, [sys_colors.work_button_text]
	mcall  , (300-42)*65536+149,	, Okstr,3

	cmp dword[option_group1],opt1
	je @f ;�᫨ ��࠭ float, � ���訥 4 ���� (�� double) �� ���⠥�
	cmp dword[option_group1],opt3
	je @f ;�᫨ ��࠭ float, � ���訥 4 ���� (�� double) �� ���⠥�
		mov ecx, dword[Data_Double+4]
		mcall  47, 8*65536+256,,185*65536+72,[sys_colors.work_text]    ; 16-���

		mov ecx, dword[Data_Double+4]
		mcall	 ,8*65536+512,,240*65536+30,	  ; 2-���
		ror ecx, 8
		mcall	 ,,,(240-56)*65536+30,
		ror ecx, 8
		mcall	 ,,,(240-56*2)*65536+30,
		ror ecx, 8
		mcall	 ,,,(240-56*3)*65536+30,
		ror ecx, 8
	@@:

	mov ecx,dword[Data_Double]
	mcall  47, 8*65536+256,,240*65536+72,[sys_colors.work_text]	 ; 16-���

	mov ecx,dword[Data_Double]
	mcall	 , 8*65536+512,,240*65536+44,	  ; 2-���
	ror ecx, 8
	mcall	 ,,,(240-56)*65536+44,
	ror ecx, 8
	mcall	 ,,,(240-56*2)*65536+44,
	ror ecx, 8
	mcall	 ,,,(240-56*3)*65536+44,
	ror ecx, 8

	mcall 38, 15*65536+300-15, 137*65536+137, [sys_colors.work_graph]
	stdcall [edit_box_draw], edit1
	stdcall [option_box_draw], Option_boxes1
	mcall 12, 2		   ; �㭪�� 12: ᮮ���� �� �� ���ᮢ�� ����

ret


;-------------------------------------------------
title db 'string to double 07.09.11',0
hexstr db 'hex:',0
decstr db 'dec:',0
binstr db 'bin:',0

if lang eq ru
	numstr db '��᫮:',0
	Okstr db '����',0
	head_f_i:
	head_f_l db '���⥬��� �訡��',0
else
	numstr db 'Number:',0
	Okstr db 'Ok',0
	head_f_i:
	head_f_l db 'System error',0
end if

mouse_dd dd 0
edit1 edit_box 182, 59, 146, 0xffffff, 0xff, 0x80ff, 0, 0x8000, (string1_end-string1), string1, mouse_dd, 0
editboxes_end:

;option_boxes
opt1 option_box option_group1, 15,  90, 8, 12, 0xffffff, 0x80ff, 0, op_text.1, 17
opt2 option_box option_group1, 15, 106, 8, 12, 0xffffff, 0x80ff, 0, op_text.2, 18
opt3 option_box option_group1, 15, 122, 8, 12, 0xffffff, 0x80ff, 0, op_text.3, 21

op_text: ;⥪�� ��� ࠤ�� ������
  .1 db 'str(dec) -> float'
  .2 db 'str(dec) -> double'
  .3 db 'float(dec,hex) -> str'
;㪠��⥫� ��� option_box
option_group1 dd opt1
Option_boxes1 dd opt1, opt2, opt3, 0

system_dir_0 db '/sys/lib/'
lib_name_0 db 'box_lib.obj',0
err_msg_found_lib_0 db '�� ������� ������⥪� ',39,'box_lib.obj',39,0
err_msg_import_0 db '�訡�� �� ������ ������⥪� ',39,'box_lib',39,0

l_libs_start:
	lib_0 l_libs lib_name_0, sys_path, library_path, system_dir_0,\
		err_msg_found_lib_0,head_f_l,import_box_lib,err_msg_import_0,head_f_i
l_libs_end:

align 4
import_box_lib:
	;dd sz_init1
	edit_box_draw dd sz_edit_box_draw
	edit_box_key dd sz_edit_box_key
	edit_box_mouse dd sz_edit_box_mouse
	;edit_box_set_text dd sz_edit_box_set_text
	option_box_draw dd aOption_box_draw
	option_box_mouse dd aOption_box_mouse
	;version_op dd aVersion_op
dd 0,0
	;sz_init1 db 'lib_init',0
	sz_edit_box_draw db 'edit_box',0
	sz_edit_box_key db 'edit_box_key',0
	sz_edit_box_mouse db 'edit_box_mouse',0
	;sz_edit_box_set_text db 'edit_box_set_text',0
	aOption_box_draw db 'option_box_draw',0
	aOption_box_mouse db 'option_box_mouse',0
	;aVersion_op db 'version_op',0

i_end:
	sys_colors system_colors
	rb 0x400 ;stack
	sys_path rb 4096
	library_path rb 4096
e_end: ; ��⪠ ���� �ணࠬ��
