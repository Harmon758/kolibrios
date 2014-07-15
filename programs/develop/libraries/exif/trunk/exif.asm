format MS COFF
public EXPORTS
section '.flat' code readable align 16

include '../../../../macros.inc'
include '../../../../proc32.inc'



;---------
offs_m_or_i    equ 8 ;ᬥ饭�� ��ࠬ��� 'MM' ��� 'II' (Motorola, Intel)
offs_tag_count equ 16 ;ᬥ饭�� ������⢠ ⥣��
offs_tag_0     equ 18 ;ᬥ饭�� 0-�� ⥣�
tag_size       equ 12 ;ࠧ��� �������� ⥣�
;�ଠ�� ������
tag_format_ui1b  equ  1 ;unsigned integer 1 byte
tag_format_text  equ  2 ;ascii string
tag_format_ui2b  equ  3 ;unsigned integer 2 byte
tag_format_ui4b  equ  4 ;unsigned integer 4 byte
tag_format_ui8b  equ  5 ;unsigned integer 8 byte
tag_format_si1b  equ  6 ;signed integer 1 byte
tag_format_undef equ  7 ;undefined
tag_format_si2b  equ  8 ;signed integer 2 byte
tag_format_si4b  equ  9 ;signed integer 4 byte
tag_format_si8b  equ 10 ;signed integer 8 byte
tag_format_f4b	 equ 11 ;float 4 byte
tag_format_f8b	 equ 12 ;float 8 byte

align 4
txt_dp db ': ',0

;
align 4
exif_tag_numbers:

db 0x01,0x0e,'Image description',0
db 0x01,0x0f,'Manufacturer of digicam',0
db 0x01,0x10,'Model',0
db 0x01,0x12,'Orientation',0
db 0x01,0x1a,'X resolution',0
db 0x01,0x1b,'Y resolution',0
db 0x01,0x28,'Resolution unit',0
db 0x01,0x31,'Software',0
db 0x01,0x32,'Date time',0
db 0x01,0x3e,'White point',0
db 0x01,0x3f,'Primary chromaticities',0
db 0x02,0x11,'YCbCrCoefficients',0
db 0x02,0x13,'YCbCrPositioning',0
db 0x02,0x14,'Reference black white',0
db 0x82,0x98,'Copyright',0
db 0x87,0x69,'Exif offset',0

db 0x88,0x25,'GPS Info',0

dw 0


;input:
; bof - 㪠��⥫� �� ��砫� 䠩��
; app1 - 㪠��⥫� ��� ���������� exif.app1
;output:
; app1 - 㪠��⥫� �� ��砫� exif.app1 (��� 0 �᫨ �� ������� ��� �ଠ� 䠩�� �� �����ন������)
align 4
proc exif_get_app1 uses eax ebx edi, bof:dword, app1:dword
	mov eax,[bof]
	mov edi,[app1]

	;䠩� � �ଠ� jpg?
	cmp word[eax],0xd8ff
	jne .no_exif
	add eax,2

	;䠩� ᮤ�ন� exif.app0?
	cmp word[eax],0xe0ff
	jne @f
		add eax,2
		movzx ebx,word[eax]
		ror bx,8 ;�ᥣ�� �� ⠪ ����?
		add eax,ebx
	@@:

	;䠩� ᮤ�ন� exif.app1?
	cmp word[eax],0xe1ff
	jne .no_exif

	add eax,2
	mov [edi],eax

	jmp @f
	.no_exif:
		mov dword[edi],0
	@@:
	ret
endp

;input:
; app1 - 㪠��⥫� �� ��砫� exif.app1
; num - ���浪��� ����� ⥣� (��稭����� � 1)
; txt - 㪠��⥫� �� ⥪��, �㤠 �㤥� ����ᠭ� ���祭��
; t_max - ���ᨬ���� ࠧ��� ⥪��
align 4
proc exif_get_app1_tag, app1:dword, num:dword, txt:dword, t_max:dword
pushad
	mov eax,[app1]
	mov edi,[txt]
	mov ecx,[num]

	xor edx,edx
	cmp eax,edx
	je .end_f ;�᫨ �� ������ 㪠��⥫� �� ��砫� exif.app1
	cmp ecx,edx
	jle .end_f ;�᫨ ���浪��� ����� ⥣� <= 0

	mov byte[edi],0
	cmp word[eax+offs_m_or_i],'II'
	je @f
		inc edx ;if 'MM' edx=1
	@@:

	;�஢��塞 �᫮ ⥣��
	movzx ebx,word[eax+offs_tag_count]
	bt edx,0
	jnc @f
		ror bx,8
	@@:
	cmp ecx,ebx
	jg .end_f ;�᫨ ����� ⥣� ����� 祬 �� ���� � 䠩��

	;���室�� �� ������� ⥣
	dec ecx
	imul ecx,tag_size
	add eax,offs_tag_0
	add eax,ecx

	;�⠥� �����祭�� ⥣�
	push exif_tag_numbers
	pop esi
	.next_tag:
	mov bx,word[esi]
	cmp bx,0
	je .tag_unknown ;⥣ �� �������
	bt edx,0
	jc @f
		ror bx,8
	@@:
	cmp word[eax],bx
	je .found
	inc esi
	@@:
		inc esi
		cmp byte[esi],0
		jne @b
	inc esi
	jmp .next_tag
	.found:

	;�����㥬 ��ப�
	add esi,2
	stdcall str_n_cat,edi,esi,[t_max]

	;�⠥� ���ଠ�� � ⥣�
	mov bx,tag_format_text
	bt edx,0
	jnc @f
		ror bx,8
	@@:
	cmp word[eax+2],bx
	jne .tag_02
		stdcall str_n_cat,edi,txt_dp,[t_max]
		;�஢��塞 ������ ��ப�
		mov ebx,dword[eax+4]
		bt edx,0
		jnc @f
			ror bx,8
			ror ebx,16
			ror bx,8
		@@:
		cmp ebx,4
		jg @f
			;�᫨ ��ப� ����頥��� � 4 ᨬ����
			mov esi,eax
			add esi,8
			stdcall str_n_cat,edi,esi,[t_max]
			jmp .end_f
		;�᫨ ��ப� �� ����頥��� � 4 ᨬ����
		@@:
		mov esi,dword[eax+8]
		bt edx,0
		jnc @f
			ror si,8
			ror esi,16
			ror si,8
		@@:
		add esi,offs_m_or_i
		add esi,[app1]
		stdcall str_n_cat,edi,esi,[t_max]
		;;;jmp .end_f
	.tag_02:

	jmp .end_f
	.tag_unknown:
		mov dword[edi],'???'
		mov byte[edi+3],0
	.end_f:
popad
	ret
endp

align 4
proc exif_get_image_160_120 uses edi, app1:dword
	mov edi,[app1]

	ret
endp

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

align 16
EXPORTS:
	dd sz_exif_get_app1, exif_get_app1
	dd sz_exif_get_app1_tag, exif_get_app1_tag
	;dd sz_exif_get_image_160_120, exif_get_image_160_120
	dd 0,0
	sz_exif_get_app1 db 'exif_get_app1',0
	sz_exif_get_app1_tag db 'exif_get_app1_tag',0
	;sz_exif_get_image_160_120 db 'exif_get_image_160_120',0
	
