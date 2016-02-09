format MS COFF
public EXPORTS
section '.flat' code readable align 16

include '../../../../macros.inc'
include '../../../../proc32.inc'

;-----------------------------------------------------------------------------
mem.alloc   dd ? ;�㭪�� ��� �뤥����� �����
mem.free    dd ? ;�㭪�� ��� �᢮�������� �����
mem.realloc dd ? ;�㭪�� ��� �����।������ �����
dll.load    dd ?

BUF_STRUCT_SIZE equ 21
buf2d_data equ dword[edi] ;����� ���� ����ࠦ����
buf2d_w equ dword[edi+8] ;�ਭ� ����
buf2d_h equ dword[edi+12] ;���� ����
buf2d_l equ word[edi+4]
buf2d_t equ word[edi+6] ;����� ᢥ���
buf2d_size_lt equ dword[edi+4] ;����� ᫥�� � �ࠢ� ��� ����
buf2d_color equ dword[edi+16] ;梥� 䮭� ����
buf2d_bits equ byte[edi+20] ;������⢮ ��� � 1-� �窥 ����ࠦ����

struct buf_2d_header
	img_data dd ?
	left dw ? ;+4 left
	top dw ? ;+6 top
	size_x dd ? ;+8 w
	size_y dd ? ;+12 h
	color dd ? ;+16 color
	bit_pp db ? ;+21 bit in pixel
ends

macro swap v1, v2 {
  push v1
  push v2
  pop v1
  pop v2
}

;䫠��, ��� �㭪樨 ��१���� ����
BUF2D_OPT_CROP_TOP equ 1 ;��१�� ᢥ���
BUF2D_OPT_CROP_LEFT equ 2 ;��१�� ᫥��
BUF2D_OPT_CROP_BOTTOM equ 4 ;��१�� ᭨��
BUF2D_OPT_CROP_RIGHT equ 8 ;��१�� �ࠢ�
BUF2D_BIT_OPT_CROP_TOP equ 0
BUF2D_BIT_OPT_CROP_LEFT equ 1
BUF2D_BIT_OPT_CROP_BOTTOM equ 2
BUF2D_BIT_OPT_CROP_RIGHT equ 3

;input:
; eax = 㪠��⥫� �� �㭪�� �뤥����� �����
; ebx = ... �᢮�������� �����
; ecx = ... �����।������ �����
; edx = ... ����㧪� ������⥪� (���� �� �ᯮ������)
align 16
lib_init:
	mov dword[mem.alloc], eax
	mov dword[mem.free], ebx
	mov dword[mem.realloc], ecx
	mov dword[dll.load], edx
	ret

include 'fun_draw.inc' ;�㭪樨 �ᮢ���� � ����

;ᮧ����� ����
align 4
proc buf_create, buf_struc:dword
	pushad
	mov edi,dword[buf_struc]
	mov ecx,buf2d_w
	mov ebx,buf2d_h
	imul ecx,ebx
	cmp buf2d_bits,24
	jne @f
		lea ecx,[ecx+ecx*2] ; 24 bit = 3
		;;;inc ecx ;����᭮� ���� � ���� ����, ��-�� �� ���稫� ������� �㭪樨 �� ����ࠦ����� ����� 4�
	@@:
	cmp buf2d_bits,32
	jne @f
		shl ecx,2 ; 32 bit = 4
	@@:
	invoke mem.alloc,ecx
	mov buf2d_data,eax

	stdcall buf_clear,edi,buf2d_color ;���⪠ ���� 䮭��� 梥⮬
	popad
	ret
endp

;ᮧ����� ���� �� �᭮�� ����ࠦ���� rgb
align 4
proc buf_create_f_img, buf_struc:dword, rgb_data:dword
	pushad
	mov edi,dword[buf_struc]
	mov ecx,buf2d_w
	mov ebx,buf2d_h
	imul ecx,ebx
	cmp buf2d_bits,24
	jne @f
		lea ecx,[ecx+ecx*2] ; 24 bit = 3
		;;;inc ecx ;����᭮� ���� � ���� ����, ��-�� �� ���稫� ������� �㭪樨 �� ����ࠦ����� ����� 4�
	@@:
	cmp buf2d_bits,32
	jne @f
		shl ecx,2 ; 32 bit = 4
	@@:
	invoke mem.alloc,ecx
	mov buf2d_data,eax

	cmp buf2d_bits,24
	jne @f
		cld
		mov esi,[rgb_data]
		mov edi,eax ;eax=buf2d_data
		rep movsb ;�����㥬 ���� ����ࠦ���� � ����
		jmp .end_create
	@@:
		stdcall buf_clear,edi,buf2d_color ;���⪠ ���� 䮭��� 梥⮬
	.end_create:
	popad
	ret
endp

;�㭪�� ��� ��१���� ���஢ 8 � 24 �����, �� �������� 梥��.
;��ࠬ��� opt �������� �������樥� ����⠭�:
; BUF2D_OPT_CROP_TOP - ��१�� ᢥ���
; BUF2D_OPT_CROP_LEFT - ��१�� ᫥��
; BUF2D_OPT_CROP_BOTTOM - ��१�� ᭨��
; BUF2D_OPT_CROP_RIGHT - ��१�� �ࠢ�
align 4
proc buf_crop_color, buf_struc:dword, color:dword, opt:dword
locals
	crop_r dd ?
endl
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .24end_f

	bt dword[opt],BUF2D_BIT_OPT_CROP_BOTTOM
	jae .24no_crop_bottom
		mov eax,dword[color]
		mov edx,eax ;ax = colors - r,g
		shr edx,16 ;dl = color - b
		mov ecx,buf2d_h
		cmp ecx,1
		jle .24no_crop_bottom ;�஢��塞 �� ��砩 �᫨ ���� ���� 1 ���ᥫ�
		mov ebx,buf2d_w
		imul ecx,ebx
		lea esi,[ecx+ecx*2] ;esi=3*ecx
		add esi,buf2d_data
		cld
		@@:
			sub esi,3
			cmp word[esi],ax
			jne @f
			cmp byte[esi+2],dl
			jne @f
			loop @b
		@@:
		lea ebx,[ebx+ebx*2]
		xor edx,edx
		mov eax,buf2d_h
		imul eax,ebx
		add eax,buf2d_data ;eax - 㪠��⥫� �� ����� ���� ����ࠦ����
		@@:
			add esi,ebx
			cmp esi,eax
			jge @f
			inc edx ;����塞 �᫮ ������ ��ப ��� ��१����
			loop @b
		@@:
		cmp edx,0
		je .24no_crop_bottom
			cmp edx,buf2d_h
			jge .24no_crop_bottom ;��-�� �� ������� ���⮩ ����
			sub buf2d_h,edx ;㬥��蠥� ����� ����
			mov ecx,buf2d_h
			imul ecx,ebx ;ecx = ���� ࠧ��� ����ࠦ����
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.24no_crop_bottom:

	bt dword[opt],BUF2D_BIT_OPT_CROP_TOP
	jae .24no_crop_top
		mov eax,dword[color]
		mov edx,eax ;ax = colors - r,g
		shr edx,16 ;dl = color - b
		mov esi,buf2d_data
		mov ecx,buf2d_h
		cmp ecx,1
		jle .24no_crop_top ;�஢��塞 �� ��砩 �᫨ ���� ���� 1 ���ᥫ�
		dec ecx ;�� ��१���� ������ ������� ������ 1-�� ��ப� ���ᥫ��
		mov ebx,buf2d_w
		imul ecx,ebx
		cld
		@@:
			cmp word[esi],ax
			jne @f
			cmp byte[esi+2],dl
			jne @f
			add esi,3
			loop @b
		@@:
		lea ebx,[ebx+ebx*2]
		xor edx,edx
		@@:
			sub esi,ebx
			cmp esi,buf2d_data
			jl @f
			inc edx ;����塞 �᫮ ������ ��ப ��� ��१����
			loop @b
		@@:
		cmp edx,0
		je .24no_crop_top
			xor eax,eax
			sub eax,edx
			mov ebx,buf2d_h
			sub ebx,edx
			stdcall buf_offset_h, edi, eax, edx, ebx ;ᤢ����� ����ࠦ���� � ���� ����� (eax<0)
			sub buf2d_h,edx ;㬥��蠥� ����� ����
			mov ecx,buf2d_h
			add buf2d_t,dx ;ᤢ����� ����� ����, �� �᫮ ��१����� ��ப
			mov ebx,buf2d_w
			imul ecx,ebx
			lea ecx,[ecx+ecx*2]
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.24no_crop_top:

	bt dword[opt],BUF2D_BIT_OPT_CROP_RIGHT
	jae .24no_crop_right
		mov eax,dword[color]
		mov edx,eax ;ax = colors - r,g
		shr edx,16 ;dl = color - b
		mov ebx,buf2d_w
		cmp ebx,1
		jle .24no_crop_right ;�� ��砩 �᫨ �ਭ� ���� 1 ���ᥫ�
		lea ebx,[ebx+ebx*2]
		mov esi,ebx
		imul esi,buf2d_h
		add esi,buf2d_data ;esi - 㪠��⥫� �� ����� ���� ����ࠦ����
		mov dword[crop_r],0
		cld
		.24found_beg_right:
		sub esi,3 ;��������� �� 1-�� ������� �����
		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			cmp word[esi],ax
			jne .24found_right
			cmp byte[esi+2],dl
			jne .24found_right
			sub esi,ebx ;��룠�� �� ������ ��ப�
			loop @b
		inc dword[crop_r]

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp dword[crop_r],ecx
		jge .24found_right

		sub esi,3 ;��������� �� 1-�� ������� �����
		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			add esi,ebx ;��룠�� �� ������ ��ப�
			cmp word[esi],ax
			jne .24found_right
			cmp byte[esi+2],dl
			jne .24found_right
			loop @b
		inc dword[crop_r]

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp dword[crop_r],ecx
		jl .24found_beg_right

		.24found_right:
		cmp dword[crop_r],0
		je .24no_crop_right
			mov ecx,buf2d_w
			sub ecx,dword[crop_r]
			stdcall img_rgb_crop_r, buf2d_data, buf2d_w, ecx, buf2d_h ;��१��� ����, �� ������ ࠧ����
			mov buf2d_w,ecx ;�⠢�� ����� �ਭ� ��� ����
			mov ebx,buf2d_h
			imul ecx,ebx
			lea ecx,[ecx+ecx*2]
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.24no_crop_right:

	bt dword[opt],BUF2D_BIT_OPT_CROP_LEFT
	jae .24no_crop_left
		mov eax,dword[color]
		mov edx,eax ;ax = colors - r,g
		shr edx,16 ;dl = color - b
		mov ebx,buf2d_w
		cmp ebx,1
		jle .24no_crop_left ;�� ��砩 �᫨ �ਭ� ���� 1 ���ᥫ�
		lea ebx,[ebx+ebx*2]
		mov esi,buf2d_data ;esi - 㪠��⥫� �� ��箫� ���� ����ࠦ����
		mov dword[crop_r],0
		cld
		.24found_beg_left:

		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			cmp word[esi],ax
			jne .24found_left
			cmp byte[esi+2],dl
			jne .24found_left
			add esi,ebx ;��룠�� �� ������ ��ப�
			loop @b
		inc dword[crop_r]
		add esi,3 ;��������� �� 1-�� ������� ��ࠢ�

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp dword[crop_r],ecx
		jge .24found_left

		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			sub esi,ebx ;��룠�� �� ������ ��ப�
			cmp word[esi],ax
			jne .24found_left
			cmp byte[esi+2],dl
			jne .24found_left
			loop @b
		inc dword[crop_r]
		add esi,3 ;��������� �� 1-�� ������� ��ࠢ�

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp dword[crop_r],ecx
		jl .24found_beg_left

		.24found_left:
		cmp dword[crop_r],0
		je .24no_crop_left
			mov ecx,buf2d_w
			sub ecx,dword[crop_r]
			stdcall img_rgb_crop_l, buf2d_data, buf2d_w, ecx, buf2d_h ;��१��� ����, �� ������ ࠧ����
			mov buf2d_w,ecx ;�⠢�� ����� �ਭ� ��� ����
			mov ebx,buf2d_h
			imul ecx,ebx
			lea ecx,[ecx+ecx*2]
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
			mov eax,dword[crop_r]
			add buf2d_l,ax
	.24no_crop_left:

	.24end_f:


	cmp buf2d_bits,8
	jne .8end_f

	bt dword[opt],BUF2D_BIT_OPT_CROP_BOTTOM
	jae .8no_crop_bottom
		mov eax,dword[color]
		mov esi,buf2d_data
		mov ecx,buf2d_h
		cmp ecx,1
		jle .8no_crop_bottom ;�஢��塞 �� ��砩 �᫨ ���� ���� 1 ���ᥫ�
		mov ebx,buf2d_w
		imul ecx,ebx
		mov esi,ecx
		add esi,buf2d_data
		cld
		@@:
			dec esi
			cmp byte[esi],al
			jne @f
			loop @b
		@@:
		xor edx,edx
		mov eax,buf2d_h
		imul eax,ebx
		add eax,buf2d_data ;eax - 㪠��⥫� �� ����� ���� ����ࠦ����
		@@:
			add esi,ebx
			cmp esi,eax
			jge @f
			inc edx
			loop @b
		@@:
		cmp edx,0
		je .8no_crop_bottom
			cmp edx,buf2d_h
			jge .8no_crop_bottom ;��-�� �� ������� ���⮩ ����
			sub buf2d_h,edx ;㬥��蠥� ����� ����
			mov ecx,buf2d_h
			imul ecx,ebx ;ecx = ���� ࠧ��� ����ࠦ����
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.8no_crop_bottom:

	bt dword[opt],BUF2D_BIT_OPT_CROP_TOP
	jae .8no_crop_top
		mov eax,dword[color]
		mov esi,buf2d_data
		mov ecx,buf2d_h
		cmp ecx,1
		jle .8no_crop_top ;�஢��塞 �� ��砩 �᫨ ���� ���� 1 ���ᥫ�
		dec ecx ;�� ��१���� ������ ������� ������ 1-�� ��ப� ���ᥫ��
		mov ebx,buf2d_w
		imul ecx,ebx
		cld
		@@:
			cmp byte[esi],al
			jne @f
			inc esi
			loop @b
		@@:
		xor edx,edx
		@@:
			sub esi,ebx
			cmp esi,buf2d_data
			jl @f
			inc edx
			loop @b
		@@:
		cmp edx,0
		je .8no_crop_top
			xor eax,eax
			sub eax,edx
			mov ebx,buf2d_h
			sub ebx,edx
			stdcall buf_offset_h, edi, eax, edx, ebx
			mov ecx,buf2d_h
			sub ecx,edx
			mov buf2d_h,ecx ;㬥��蠥� ����� ����
			add buf2d_t,dx ;ᤢ����� ����� ����, �� �᫮ ��१����� ��ப
			mov ebx,buf2d_w
			imul ecx,ebx
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.8no_crop_top:

	bt dword[opt],BUF2D_BIT_OPT_CROP_RIGHT
	jae .8no_crop_right
		mov eax,dword[color]
		mov ebx,buf2d_w
		cmp ebx,1
		jle .8no_crop_right ;�� ��砩 �᫨ �ਭ� ���� 1 ���ᥫ�
		mov esi,ebx
		imul esi,buf2d_h
		add esi,buf2d_data ;esi - 㪠��⥫� �� ����� ���� ����ࠦ����
		xor edx,edx
		cld

		.8found_beg:
		dec esi ;��������� �� 1-�� ������� �����
		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			cmp byte[esi],al
			jne .8found
			sub esi,ebx ;��룠�� �� ������ ��ப�
			loop @b
		inc edx
		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp edx,ecx
		jge .8found

		dec esi ;��������� �� 1-�� ������� �����
		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			add esi,ebx ;��룠�� �� ������ ��ப�
			cmp byte[esi],al
			jne .8found
			loop @b
		inc edx

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp edx,ecx
		jl .8found_beg

		.8found:
		cmp edx,0
		je .8no_crop_right
			mov ecx,buf2d_w
			sub ecx,edx
			stdcall img_gray_crop_r, buf2d_data, buf2d_w, ecx, buf2d_h ;��१��� ����, �� ������ ࠧ����
			mov buf2d_w,ecx ;�⠢�� ����� �ਭ� ��� ����
			mov ebx,buf2d_h
			imul ecx,ebx
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.8no_crop_right:

	bt dword[opt],BUF2D_BIT_OPT_CROP_LEFT
	jae .8no_crop_left
		mov eax,dword[color]
		mov ebx,buf2d_w
		cmp ebx,1
		jle .8no_crop_left ;�� ��砩 �᫨ �ਭ� ���� 1 ���ᥫ�
		mov esi,buf2d_data ;esi - 㪠��⥫� �� ��箫� ���� ����ࠦ����
		mov edx,0
		cld
		.8found_beg_left:

		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			cmp word[esi],ax
			jne .8found_left
			add esi,ebx ;��룠�� �� ������ ��ப�
			loop @b
		inc edx
		inc esi ;��������� �� 1-�� ������� ��ࠢ�

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp edx,ecx
		jge .8found_left

		mov ecx,buf2d_h ;����⠭������� ecx ��� ������ 横��
		@@:
			sub esi,ebx ;��룠�� �� ������ ��ப�
			cmp word[esi],ax
			jne .8found_left
			loop @b
		inc edx
		inc esi ;��������� �� 1-�� ������� ��ࠢ�

		mov ecx,buf2d_w
		dec ecx ;1 ������� �� �����
		cmp edx,ecx
		jl .8found_beg_left

		.8found_left:
		cmp edx,0
		je .8no_crop_left
			mov ecx,buf2d_w
			sub ecx,edx
			stdcall img_gray_crop_l, buf2d_data, buf2d_w, ecx, buf2d_h ;��१��� ����, �� ������ ࠧ����
			mov buf2d_w,ecx ;�⠢�� ����� �ਭ� ��� ����
			mov ebx,buf2d_h
			imul ecx,ebx
			invoke mem.realloc,buf2d_data,ecx
			mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
			mov eax,edx
			add buf2d_l,ax
	.8no_crop_left:

	.8end_f:

	popad
	ret
endp

;��१��� 梥⭮� ����ࠦ���� � �ࠢ�� ��஭�
;input:
;data_rgb - pointer to rgb data
;size_w_old - width img in pixels
;size_w_new - new width img in pixels
;size_h - height img in pixels
align 4
proc img_rgb_crop_r, data_rgb:dword, size_w_old:dword, size_w_new:dword, size_h:dword
	pushad
	mov eax, dword[size_w_old]
	lea eax, dword[eax+eax*2] ;eax = width(old) * 3(rgb)
	mov ebx, dword[size_w_new]
	lea ebx, dword[ebx+ebx*2] ;ebx = width(new) * 3(rgb)
	mov edx, dword[size_h]
	mov edi, dword[data_rgb] ;edi - ����砥� �����
	mov esi, edi
	add edi, ebx
	add esi, eax
	cld
	@@:
		dec edx ;㬥��蠥� ���稪 ��⠢���� ��ப �� 1
		cmp edx,0
		jle @f
		mov ecx, ebx
		rep movsb ;��७�� (����஢����) ��ப� ���ᥫ��
		add esi,eax ;���室 �� ����� ����� ����ࠦ����
		sub esi,ebx
		jmp @b
	@@:
	popad
	ret
endp

;��१��� �஥ ����ࠦ���� � �ࠢ�� ��஭�
;input:
;data_gray - pointer to gray data
;size_w_old - width img in pixels
;size_w_new - new width img in pixels
;size_h - height img in pixels
align 4
proc img_gray_crop_r, data_gray:dword, size_w_old:dword, size_w_new:dword, size_h:dword
	pushad
	mov eax, dword[size_w_old]
	mov ebx, dword[size_w_new]
	mov edx, dword[size_h]
	mov edi, dword[data_gray] ;edi - ����砥� �����
	mov esi, edi
	add edi, ebx
	add esi, eax
	cld
	@@:
		dec edx ;㬥��蠥� ���稪 ��⠢���� ��ப �� 1
		cmp edx,0
		jle @f
		mov ecx, ebx
		rep movsb ;��७�� (����஢����) ��ப� ���ᥫ��
		add esi,eax ;���室 �� ����� ����� ����ࠦ����
		sub esi,ebx
		jmp @b
	@@:
	popad
	ret
endp

;��१��� 梥⭮� ����ࠦ���� � ����� ��஭�
;input:
;data_rgb - pointer to rgb data
;size_w_old - width img in pixels
;size_w_new - new width img in pixels
;size_h - height img in pixels
align 4
proc img_rgb_crop_l, data_rgb:dword, size_w_old:dword, size_w_new:dword, size_h:dword
	pushad
	mov edi,dword[data_rgb]
	mov esi,edi
	mov eax,dword[size_w_old]
	mov ebx,dword[size_w_new]
	cmp eax,ebx
	jle .end_f ;���� ࠧ��� ����ࠦ���� �� ����� ���� ����� ������ (�� �᫮��� ��१���� ���⨭��)
		lea eax,[eax+eax*2]
		lea ebx,[ebx+ebx*2]
		sub eax,ebx
		mov edx,dword[size_h] ;���� ����ࠦ����
		cld
		@@:
			add esi,eax
			mov ecx,ebx
			rep movsb
			dec edx
			cmp edx,0
			jg @b
	.end_f:
	popad
	ret
endp

;��१��� �஥ ����ࠦ���� � ����� ��஭�
;input:
;data_gray - pointer to gray data
;size_w_old - width img in pixels
;size_w_new - new width img in pixels
;size_h - height img in pixels
align 4
proc img_gray_crop_l, data_gray:dword, size_w_old:dword, size_w_new:dword, size_h:dword
	pushad
	mov edi,dword[data_gray]
	mov esi,edi
	mov eax,dword[size_w_old]
	mov ebx,dword[size_w_new]
	cmp eax,ebx
	jle .end_f ;���� ࠧ��� ����ࠦ���� �� ����� ���� ����� ������ (�� �᫮��� ��१���� ���⨭��)
		sub eax,ebx
		mov edx,dword[size_h] ;���� ����ࠦ����
		cld
		@@:
			add esi,eax
			mov ecx,ebx
			rep movsb
			dec edx
			cmp edx,0
			jg @b
	.end_f:
	popad
	ret
endp

;hoffs - �������⢮ ���ᥫ�� �� ����� �����������/���᪠���� ����ࠦ����
;img_t - ����, � ���ன ��稭����� ���������� ���� ����ࠦ����
align 4
proc buf_offset_h, buf_struc:dword, hoffs:dword, img_t:dword, img_h:dword ;ᤢ����� ����ࠦ���� �� ����
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .end_move_24

	mov eax,[hoffs]
	cmp eax,0
	je .end_move_24
		mov ebx,buf2d_w
		mov edx,dword[img_t]
			mov ecx,dword[img_h] ;ecx - ���� ᤢ������� ������
			cmp ecx,buf2d_h
			jge .end_f ;�訡�筮� �᫮���, ���� ����ࠦ���� ����� 祬 ���� ᤢ�������� ����ࠦ����
			imul ecx,ebx ;ecx - �������⢮ ���ᥫ�� � ᤢ������� ������
			lea ecx,[ecx+ecx*2]
		imul ebx,edx
		lea ebx,[ebx+ebx*2]
		mov esi,buf2d_data
		add esi,ebx

		add edx,eax ;edx = img_t+hoffs (hoffs<0)
		mov ebx,buf2d_w
		imul ebx,edx
		lea ebx,[ebx+ebx*2]
		mov edi,buf2d_data ;������, �㤠 �㤥� ��������� ����ࠦ����
		add edi,ebx

		cmp eax,0
		jg .move_down_24
			;������� ����ࠦ���� �����
			cld
			rep movsb
			jmp .end_f
		.move_down_24:
			;������� ����ࠦ���� ����
			add esi,ecx
			dec esi
			add edi,ecx
			dec edi
			std
			rep movsb
			jmp .end_f
	.end_move_24:

;stdcall print_err,sz_buf2d_offset_h,txt_err_n24b

	cmp buf2d_bits,8
	jne .end_move_8

	mov eax,[hoffs]
	cmp eax,0
	je .end_move_8
		;������� ����ࠦ���� �����
		mov ebx,buf2d_w
		mov edx,dword[img_t]
			mov ecx,dword[img_h] ;ecx - ���� ᤢ������� ������
			cmp ecx,buf2d_h
			jge .end_f ;�訡�筮� �᫮���, ���� ����ࠦ���� ����� 祬 ���� ᤢ�������� ����ࠦ����
			imul ecx,ebx ;ecx - �������⢮ ���ᥫ�� � ᤢ������� ������
		imul ebx,edx
		mov esi,buf2d_data
		add esi,ebx

		add edx,eax ;edx = img_t+hoffs (hoffs<0)
		mov ebx,buf2d_w
		imul ebx,edx
		mov edi,buf2d_data ;������, �㤠 �㤥� ��������� ����ࠦ����
		add edi,ebx

		cmp eax,0
		jg .move_down_8
			cld
			rep movsb
			jmp .end_f
		.move_down_8:
			;������� ����ࠦ���� ����
			add esi,ecx
			dec esi
			add edi,ecx
			dec edi
			std
			rep movsb
			jmp .end_f
	.end_move_8:

	.end_f:
	popad
	ret
endp

align 4
proc buf_delete, buf_struc:dword
	push eax edi
	mov edi,dword[buf_struc]
	invoke mem.free,buf2d_data
	pop edi eax
	ret
endp

;input:
; new_w - ����� �ਭ� (�᫨ 0 � �� �������)
; new_h - ����� ���� (�᫨ 0 � �� �������)
; options - ��ࠬ���� ��������� ���� (1 - �������� ࠧ��� ����,
;    2 - �������� ����ࠦ���� � ����, 3 - �������� ���� � ����ࠦ����)
align 4
proc buf_resize, buf_struc:dword, new_w:dword, new_h:dword, options:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,8
	jne .8bit
		bt dword[options],1 ;ᦠ⨥ �����.
		jnc @f
			;...
		@@:
		bt dword[options],0 ;�����. ����
		jnc .end_f
			;...
		jmp .end_f
	.8bit:
	cmp buf2d_bits,24
	jne .24bit
		bt dword[options],1 ;ᦠ⨥ �����.
		jnc .24_end_r
			mov eax,dword[new_w]
			cmp eax,1
			jl @f
			cmp eax,buf2d_w
			jge @f
				;ᦠ⨥ �� �ਭ�
				stdcall img_rgb24_wresize, buf2d_data,buf2d_w,buf2d_h,eax
				jmp .24_r_h
			@@:
			mov eax,buf2d_w
			.24_r_h: ;eax - �ਭ� ���� ��� �ਭ� ᦠ⮣� ����ࠦ����
			mov ebx,dword[new_h]
			cmp ebx,1
			jl @f
			cmp ebx,buf2d_h
			jge @f
				;ᦠ⨥ �� ����
				stdcall img_rgb24_hresize, buf2d_data,eax,buf2d_h,ebx
			@@:
		.24_end_r:
		bt dword[options],0 ;�����. ����
		jnc .end_f
		mov eax,dword[new_w]
		cmp eax,1
		jl @f
			mov buf2d_w,eax
		@@:
		mov ecx,buf2d_w
		mov eax,dword[new_h]
		cmp eax,1
		jl @f
			mov buf2d_h,eax
		@@:
		mov ebx,buf2d_h
		imul ecx,ebx
		lea ecx,[ecx+ecx*2] ; 24 bit = 3
		invoke mem.realloc,buf2d_data,ecx ;�����塞 ������ ���������� ���஬
		mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
	.24bit:
	.end_f:
	popad
	ret
endp

align 4
rot_table: ;⠡��� ��� 㪠����� �� ����㭪樨 ��� �����⮢
	dd buf_rotate.8b90,buf_rotate.24b90,buf_rotate.32b90,\
	buf_rotate.8b180,buf_rotate.24b180,buf_rotate.32b180

;������ ����ࠦ���� �� 90 ��� 180 �ࠤ�ᮢ
align 4
proc buf_rotate, buf_struc:dword, angle:dword
locals
	n_data dd ?
	dec_h dd ? ;�᫮ ����, ��� 㬥��襭�� ���न���� y
endl
	pushad
	mov edi,[buf_struc]
	mov ebx,buf2d_w
	mov ecx,buf2d_h

	lea eax,[rot_table]
	cmp dword[angle],90 ;�஢�ઠ 㣫� ������
	je .beg_0
	cmp dword[angle],180
	jne @f
		add eax,12
		jmp .beg_0
	@@:
	jmp .end_f
	.beg_0: ;�஢�ઠ ��⭮�� ����
	cmp buf2d_bits,8
	jne @f
		jmp dword[eax]
	@@:
	cmp buf2d_bits,24
	jne @f
		add eax,4
		jmp dword[eax]
	@@:
	cmp buf2d_bits,32
	jne @f
		add eax,8
		jmp dword[eax]
	@@:
	jmp .end_f

	.8b90: ;������ 8 ��⭮�� ���� �� 90 �ࠤ�ᮢ
		mov edx,ecx ;edx - buf_h
		imul ecx,ebx
		invoke mem.alloc,ecx ;�뤥�塞 �६����� ������
		cmp eax,0
		je .end_f
		mov [n_data],eax
		mov [dec_h],ecx
		inc dword[dec_h]

		;copy buf --> mem
		mov edi,[buf_struc]
		mov esi,buf2d_data
		mov edi,eax ;[n_data]
		dec edx ;��४��㥬 edx �� 1 ����, ��� �������樨 ᤢ��� � movsb
		add edi,edx
		xor eax,eax
		cld
		.cycle_0:
			movsb
			add edi,edx
			inc eax
			cmp eax,ebx
			jl @f
				xor eax,eax
				sub edi,[dec_h]
			@@:
			loop .cycle_0

		;change buf_w <---> buf_h
		mov esi,[n_data]
		mov edi,[buf_struc]
		mov edi,buf2d_data
		mov ecx,ebx
		inc edx ;��ࠢ�塞 ᪮४�஢���� edx
		imul ecx,edx
		;copy buf <-- mem
		;cld
		rep movsb
		invoke mem.free,[n_data]
		jmp .change_w_h
	.24b90: ;������ 24 ��⭮�� ���� �� 90 �ࠤ�ᮢ
		mov esi,ecx
		imul esi,ebx
		lea ecx,[ecx+ecx*2]
		mov edx,ecx ;edx - buf_h * 3
		imul ecx,ebx
		invoke mem.alloc,ecx ;�뤥�塞 �६����� ������
		cmp eax,0
		je .end_f
		mov [n_data],eax
		mov [dec_h],ecx
		add dword[dec_h],3

		;copy buf --> mem
		
		mov edi,[buf_struc]
		mov ecx,esi
		mov esi,buf2d_data
		mov edi,eax ;[n_data]
		sub edx,3 ;��४��㥬 edx �� 3 ����, ��� �������樨 ᤢ���
		add edi,edx
		xor eax,eax
		cld
		.cycle_1:
			movsw
			movsb
			add edi,edx
			inc eax
			cmp eax,ebx
			jl @f
				xor eax,eax
				sub edi,[dec_h]
			@@:
			loop .cycle_1

		;copy buf <-- mem
		mov esi,[n_data]
		mov edi,[buf_struc]
		mov edi,buf2d_data
		mov ecx,ebx
		add edx,3 ;��ࠢ�塞 ᪮४�஢���� edx
		imul ecx,edx
		;cld
		rep movsb
		invoke mem.free,[n_data]
		jmp .change_w_h
	.32b90: ;������ 32 ��⭮�� ���� �� 90 �ࠤ�ᮢ
		shl ecx,2
		mov edx,ecx ;edx - buf_h * 4
		imul ecx,ebx
		invoke mem.alloc,ecx ;�뤥�塞 �६����� ������
		cmp eax,0
		je .end_f
		mov [n_data],eax
		mov [dec_h],ecx
		add dword[dec_h],4

		;copy buf --> mem
		mov edi,[buf_struc]
		shr ecx,2
		mov esi,buf2d_data
		mov edi,eax ;[n_data]
		sub edx,4 ;��४��㥬 edx �� 4 ����, ��� �������樨 ᤢ��� � movsd
		add edi,edx
		xor eax,eax
		cld
		.cycle_2:
			movsd
			add edi,edx
			inc eax
			cmp eax,ebx
			jl @f
				xor eax,eax
				sub edi,[dec_h]
			@@:
			loop .cycle_2

		;copy buf <-- mem
		mov esi,[n_data]
		mov edi,[buf_struc]
		mov edi,buf2d_data
		mov ecx,ebx
		add edx,4 ;��ࠢ�塞 ᪮४�஢���� edx
		imul ecx,edx
		shr ecx,2
		;cld
		rep movsd
		invoke mem.free,[n_data]
		;jmp .change_w_h
	.change_w_h: ;change buf_w <---> buf_h
		mov edi,[buf_struc]
		mov eax,buf2d_w
		mov ebx,buf2d_h
		mov buf2d_h,eax
		mov buf2d_w,ebx
		jmp .end_f
	.8b180: ;������ 8 ��⭮�� ���� �� 180 �ࠤ�ᮢ
		mov edi,buf2d_data
		mov esi,edi
		imul ecx,ebx
		add esi,ecx
		dec esi
		shr ecx,1 ;ecx - �᫮ ���ᥫ�� ���� : 2
		std
		@@:
			lodsb
			mov ah,byte[edi]
			mov byte[esi+1],ah
			mov byte[edi],al
			inc edi
			loop @b
			jmp .end_f
	.24b180: ;������ 24 ��⭮�� ���� �� 180 �ࠤ�ᮢ
		mov esi,buf2d_data
		mov edi,esi
		imul ecx,ebx
		mov eax,ecx
		lea ecx,[ecx+ecx*2]
		add edi,ecx
		sub edi,3
		shr eax,1
		mov ecx,eax ;ecx - �᫮ ���ᥫ�� ���� : 2
		cld
		@@:
			lodsw
			mov edx,eax
			lodsb
			mov bx,word[edi]
			mov word[esi-3],bx
			mov bl,byte[edi+2]
			mov byte[esi-1],bl
			mov byte[edi+2],al
			mov word[edi],dx
			sub edi,3
			loop @b
			jmp .end_f
	.32b180: ;������ 32 ��⭮�� ���� �� 180 �ࠤ�ᮢ
		mov edi,buf2d_data
		mov esi,edi
		imul ecx,ebx
		shl ecx,2
		add esi,ecx
		sub esi,4
		shr ecx,3 ;ecx - �᫮ ���ᥫ�� ���� : 2
		std
		@@:
			lodsd
			mov ebx,dword[edi]
			mov dword[esi+4],ebx
			mov dword[edi],eax
			add edi,4
			loop @b
		;jmp .end_f

	.end_f:
	popad
	ret
endp

align 4
proc buf_flip_h, buf_struc:dword
pushad
	mov edi,[buf_struc]
	cmp buf2d_bits,24
	jne .end_24
		mov esi,buf2d_data
		mov eax,buf2d_w
		mov ecx,eax
		shr ecx,1
		dec eax
		lea eax,[eax+eax*2]
		mov ebx,buf2d_h
		mov edi,esi		
		add esi,eax
		add eax,3
		cld
		.cycle_24:
		push ecx edi esi
align 4
		@@:
			;swap word[edi] <-> word[esi]
			mov dx,[edi]
			movsw
			mov [esi-2],dx
			;swap byte[edi] <-> byte[esi]
			mov dl,[edi]
			movsb
			mov [esi-1],dl
			sub esi,6
		loop @b
		pop esi edi ecx
		add edi,eax
		add esi,eax
		dec ebx
		or ebx,ebx
		jnz .cycle_24
		jmp .end_32
	.end_24:
	cmp buf2d_bits,32
	jne .end_32
		mov esi,buf2d_data
		mov eax,buf2d_w
		dec eax
		shl eax,2
		mov ebx,buf2d_h
		mov edi,esi
		add esi,eax
		add eax,4
		cld
		.cycle_32:
		mov ecx,eax
		shr ecx,3
		push edi esi
align 4
		@@:
			;swap dword[edi] <-> dword[esi]
			mov edx,[edi]
			movsd
			mov [esi-4],edx
			sub esi,8
		loop @b
		pop esi edi
		add edi,eax
		add esi,eax
		dec ebx
		or ebx,ebx
		jnz .cycle_32
	.end_32:
popad
	ret
endp

;��ࠧ��� �� ���⨪��� (���� � ��� �������� ���⠬�)
align 4
proc buf_flip_v, buf_struc:dword
locals
	line_pix dd ? ;���. ���ᥫ�� � ����� ����
	line_2byte dd ? ;���. ���� � ����� ���� * 2
endl
	pushad
	mov edi,[buf_struc]
	cmp buf2d_bits,24
	jne .end_24
		mov edx,buf2d_w
		mov [line_pix],edx
		mov ebx,buf2d_h
		lea edx,[edx+edx*2]
		mov esi,edx
		imul esi,ebx
		sub esi,edx
		add esi,buf2d_data ;㪠��⥫� �� ������ �����
		shr ebx,1 ;���. �������� 横���
		shl edx,1
		mov [line_2byte],edx
		mov edi,buf2d_data
		xchg edi,esi
		cld
		.flip_24:
		cmp ebx,0
		jle .end_32 ;����� ��室 �� �㭪樨 (��⮬� .end_24 �� ���室��)
		mov ecx,[line_pix]
align 4
		@@:
			lodsw
			mov dx,word[edi]
			mov word[esi-2],dx
			stosw
			lodsb
			mov ah,byte[edi]
			mov byte[esi-1],ah
			stosb
			loop @b
		sub edi,[line_2byte]
		dec ebx
		jmp .flip_24
	.end_24:
	cmp buf2d_bits,32
	jne .end_32
		mov edx,buf2d_w
		mov [line_pix],edx
		mov ebx,buf2d_h
		shl edx,2
		mov esi,edx
		imul esi,ebx
		sub esi,edx
		add esi,buf2d_data ;㪠��⥫� �� ������ �����
		shr ebx,1 ;���. �������� 横���
		shl edx,1
		mov [line_2byte],edx
		mov edi,buf2d_data
		xchg edi,esi
		cld
		.flip_32:
		cmp ebx,0
		jle .end_32
		mov ecx,[line_pix]
align 4
		@@:
			lodsd
			mov edx,dword[edi]
			mov dword[esi-4],edx
			stosd
			loop @b
		sub edi,[line_2byte]
		dec ebx
		jmp .flip_32
	.end_32:
	popad
	ret
endp

;description:
; ᦠ⨥ ����ࠦ���� �� �ਭ� � 2 ࠧ� (ࠧ���� ���� �� ��������)
align 4
proc buf_img_wdiv2, buf_struc:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,8
	jne @f
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_8b_wdiv2, buf2d_data,ecx
	@@:
	cmp buf2d_bits,24
	jne @f
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_rgb24_wdiv2, buf2d_data,ecx
	@@:
	cmp buf2d_bits,32
	jne @f
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_rgba32_wdiv2, buf2d_data,ecx
	@@:
	popad
	ret
endp

;input:
;data_8b - pointer to rgb data
;size - count img pixels (size img data / 3(rgb) )
align 4
proc img_8b_wdiv2 data_8b:dword, size:dword
	mov eax,dword[data_8b]
	mov ecx,dword[size] ;ecx = size
	cld
	@@: ;��⥬����� 梥� ���ᥫ��
		shr byte[eax],1
		inc eax
		loop @b

	mov eax,dword[data_8b]
	mov ecx,dword[size] ;ecx = size
	shr ecx,1
	@@: ;᫮����� 梥⮢ ���ᥫ��
		mov bl,byte[eax+1] ;�����㥬 梥� �ᥤ���� ���ᥫ�
		add byte[eax],bl
		add eax,2
		loop @b

	mov eax,dword[data_8b]
	inc eax
	mov ebx,eax
	inc ebx
	mov ecx,dword[size] ;ecx = size
	shr ecx,1
	dec ecx ;��譨� ���ᥫ�
	@@: ;�����⨥ ���ᥫ��
		mov dl,byte[ebx]
		mov byte[eax],dl

		inc eax
		add ebx,2
		loop @b
	ret
endp

;input:
;data_rgb - pointer to rgb data
;size - count img pixels (size img data / 3(rgb) )
align 4
proc img_rgb24_wdiv2 data_rgb:dword, size:dword
  mov eax,dword[data_rgb]
  mov ecx,dword[size] ;ecx = size
  lea ecx,[ecx+ecx*2]
  cld
  @@: ;��⥬����� 梥� ���ᥫ��
		shr byte[eax],1
		inc eax
		loop @b

  mov eax,dword[data_rgb]
  mov ecx,dword[size] ;ecx = size
  shr ecx,1
  @@: ;᫮����� 梥⮢ ���ᥫ��
		mov bx,word[eax+3] ;�����㥬 梥� �ᥤ���� ���ᥫ�
		add word[eax],bx
		mov bl,byte[eax+5] ;�����㥬 梥� �ᥤ���� ���ᥫ�
		add byte[eax+2],bl
		add eax,6 ;=2*3
		loop @b

  mov eax,dword[data_rgb]
  add eax,3
  mov ebx,eax
  add ebx,3
  mov ecx,dword[size] ;ecx = size
  shr ecx,1
  dec ecx ;��譨� ���ᥫ�
  @@: ;�����⨥ ���ᥫ��
		mov edx,dword[ebx]
		mov word[eax],dx
		shr edx,16
		mov byte[eax+2],dl

		add eax,3
		add ebx,6
		loop @b
  ret
endp

;input:
;data_rgba - pointer to rgba data
;size - count img pixels (size img data / 4(rgba) )
align 4
proc img_rgba32_wdiv2 data_rgba:dword, size:dword
	mov eax,dword[data_rgba]

	mov eax,dword[data_rgba]
	mov ebx,eax
	add ebx,4
	mov ecx,dword[size] ;ecx = size
	shr ecx,1
	@@: ;ᬥ訢���� 梥⮢ ���ᥫ��
		call combine_colors_1
		mov [eax],edx
		add eax,8 ;=2*4
		add ebx,8
		loop @b

	mov eax,dword[data_rgba]
	add eax,4
	mov ebx,eax
	add ebx,4
	mov ecx,dword[size] ;ecx = size
	shr ecx,1
	dec ecx ;��譨� ���ᥫ�
	@@: ;�����⨥ ���ᥫ��
		mov edx,dword[ebx]
		mov dword[eax],edx

		add eax,4
		add ebx,8
		loop @b
	ret
endp

;description:
; ᦠ⨥ ����ࠦ���� �� ���� � 2 ࠧ� (���� ���� �� �������)
align 4
proc buf_img_hdiv2, buf_struc:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,8
	jne @f
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_8b_hdiv2, buf2d_data,ecx,eax
		jmp .end_f ;edi ������� � �㭪樨, ��⮬� �ᯮ�짮����� buf2d_bits ���᭮
	@@:
	cmp buf2d_bits,24
	jne @f
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_rgb24_hdiv2, buf2d_data,ecx,eax
		jmp .end_f
	@@:
	cmp buf2d_bits,32
	jne @f
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		shl eax,2
		stdcall img_rgba32_hdiv2, buf2d_data,ecx,eax
		;jmp .end_f
	@@:
	.end_f:
	popad
	ret
endp

;input:
;data_8b - pointer to 8 bit data
;size - count img pixels (size img data)
;size_w - width img in pixels
align 4
proc img_8b_hdiv2, data_8b:dword, size:dword, size_w:dword

	mov eax,dword[data_8b] ;eax =
	mov ecx,dword[size]
	cld
	@@: ;��⥬����� 梥� ���ᥫ��
		shr byte[eax],1
		inc eax
		loop @b

	mov eax,dword[data_8b] ;eax =
	mov esi,dword[size_w]
	mov ebx,esi
	add ebx,eax
	mov ecx,dword[size]  ;ecx = size
	shr ecx,1
	xor edi,edi
	@@: ;᫮����� 梥⮢ ���ᥫ��
		mov dl,byte[ebx] ;�����㥬 梥� ������� ���ᥫ�
		add byte[eax],dl

		inc eax
		inc ebx
		inc edi
		cmp edi,dword[size_w]
		jl .old_line
			add eax,esi
			add ebx,esi
			xor edi,edi
		.old_line:
		loop @b


	mov eax,dword[data_8b] ;eax =
	add eax,esi ;esi = width*3(rgb)
	mov ebx,eax
	add ebx,esi
	mov ecx,dword[size] ;ecx = size
	shr ecx,1
	sub ecx,dword[size_w] ;����� ��ப� ���ᥫ��
	xor edi,edi
	@@: ;�����⨥ ���ᥫ��
		mov dl,byte[ebx] ;�����㥬 梥� ������� ���ᥫ�
		mov byte[eax],dl

		inc eax
		inc ebx
		inc edi
		cmp edi,dword[size_w]
		jl .old_line_2
			add ebx,esi
			xor edi,edi
		.old_line_2:
		loop @b
	ret
endp

;input:
;data_rgb - pointer to rgb data
;size - count img pixels (size img data / 3(rgb) )
;size_w - width img in pixels
align 4
proc img_rgb24_hdiv2, data_rgb:dword, size:dword, size_w:dword

  mov eax,dword[data_rgb] ;eax =
  mov ecx,dword[size]	  ;ecx = size
  lea ecx,[ecx+ecx*2]
  cld
  @@: ;��⥬����� 梥� ���ᥫ��
    shr byte[eax],1
    inc eax
    loop @b

  mov eax,dword[data_rgb] ;eax =
  mov esi,dword[size_w]
  lea esi,[esi+esi*2] ;esi = width*3(rgb)
  mov ebx,esi
  add ebx,eax
  mov ecx,dword[size]  ;ecx = size
  shr ecx,1
  xor edi,edi
  @@: ;᫮����� 梥⮢ ���ᥫ��
    mov dx,word[ebx] ;�����㥬 梥� ������� ���ᥫ�
    add word[eax],dx
    mov dl,byte[ebx+2] ;�����㥬 梥� ������� ���ᥫ�
    add byte[eax+2],dl

    add eax,3
    add ebx,3
    inc edi
    cmp edi,dword[size_w]
    jl .old_line
      add eax,esi
      add ebx,esi
      xor edi,edi
    .old_line:
    loop @b

  mov eax,dword[data_rgb] ;eax =
  add eax,esi ;esi = width*3(rgb)
  mov ebx,eax
  add ebx,esi
  mov ecx,dword[size] ;ecx = size
  shr ecx,1
  sub ecx,dword[size_w] ;����� ��ப� ���ᥫ��
  xor edi,edi
  @@: ;�����⨥ ���ᥫ��
    mov edx,dword[ebx] ;�����㥬 梥� ������� ���ᥫ�
    mov word[eax],dx
    shr edx,16
    mov byte[eax+2],dl

    add eax,3
    add ebx,3
    inc edi
    cmp edi,dword[size_w]
    jl .old_line_2
      add ebx,esi
      xor edi,edi
    .old_line_2:
    loop @b
  ret
endp

;input:
;data_rgba - pointer to rgba data
;size - count img pixels (size img data / 4(rgba) )
;size_w_b - width img in bytes
align 4
proc img_rgba32_hdiv2, data_rgba:dword, size:dword, size_w_b:dword

	mov eax,dword[data_rgba] ;eax =
	mov ebx,dword[size_w_b]
	add ebx,eax
	mov ecx,dword[size]  ;ecx = size
	shr ecx,1
	xor edi,edi
	@@: ;ᬥ訢���� 梥⮢ ���ᥫ��
		call combine_colors_1
		mov dword[eax],edx

		add eax,4
		add ebx,4
		add edi,4
		cmp edi,dword[size_w_b]
		jl .old_line
			add eax,dword[size_w_b]
			add ebx,dword[size_w_b]
			xor edi,edi
		.old_line:
		loop @b

	mov eax,dword[data_rgba] ;eax =
	mov ebx,dword[size_w_b]
	add eax,ebx
	add ebx,eax
	mov ecx,dword[size] ;ecx = size
	shl ecx,1
	sub ecx,dword[size_w_b] ;����� ��ப� ���ᥫ��
	shr ecx,2
	xor edi,edi
	@@: ;�����⨥ ���ᥫ��
		mov edx,dword[ebx] ;�����㥬 梥� ������� ���ᥫ�
		mov dword[eax],edx

		add eax,4
		add ebx,4
		add edi,4
		cmp edi,dword[size_w_b]
		jl .old_line_2
			add ebx,dword[size_w_b]
			xor edi,edi
		.old_line_2:
		loop @b
	ret
endp

;input:
; eax - 㪠��⥫� �� 32-���� 梥�
; ebx - 㪠��⥫� �� 32-���� 梥�
;output:
; edx - 32-���� 梥� ᬥ蠭�� � ��⮬ �஧�筮��
;destroy:
; esi
align 4
proc combine_colors_1 uses ecx edi
locals
	c_blye dd ?
	c_green dd ?
	c_red dd ?
endl		
	movzx edi,byte[eax+3]
	cmp edi,255
	je .c0z
	movzx esi,byte[ebx+3]
	cmp esi,255
	je .c1z
	cmp edi,esi
	je .c0_c1

	;��ॢ��稢��� ���祭�� �஧�筮�⥩
	neg edi
	inc edi
	add edi,255
	neg esi
	inc esi
	add esi,255

	movzx ecx,byte[eax]
	imul ecx,edi
	mov [c_blye],ecx
	movzx ecx,byte[ebx]
	imul ecx,esi
	add [c_blye],ecx

	movzx ecx,byte[eax+1]
	imul ecx,edi
	mov [c_green],ecx
	movzx ecx,byte[ebx+1]
	imul ecx,esi
	add [c_green],ecx

	movzx ecx,byte[eax+2]
	imul ecx,edi
	mov [c_red],ecx
	movzx ecx,byte[ebx+2]
	imul ecx,esi
	add [c_red],ecx

push eax ebx
	xor ebx,ebx
	mov eax,[c_red]
	xor edx,edx
	mov ecx,edi
	add ecx,esi
	div ecx
	mov bl,al
	shl ebx,16
	mov eax,[c_green]
	xor edx,edx
	div ecx
	mov bh,al
	mov eax,[c_blye]
	xor edx,edx
	div ecx
	mov bl,al

	shr ecx,1
	;��ॢ��稢��� ���祭�� �஧�筮��
	neg ecx
	inc ecx
	add ecx,255

	shl ecx,24
	add ebx,ecx
	mov edx,ebx
pop ebx eax

	jmp .end_f
	.c0_c1: ;�᫨ �஧�筮�� ����� 梥⮢ ᮢ������
		mov edx,dword[eax]
		shr edx,1
		and edx,011111110111111101111111b
		mov esi,dword[ebx]
		shr esi,1
		and esi,011111110111111101111111b
		add edx,esi
		ror edi,8 ;��६�頥� ���祭�� �஧�筮�� � ���訩 ���� edi
		or edx,edi
		jmp .end_f
	.c0z: ;�᫨ 梥� � eax �஧���
		mov edx,dword[ebx]
		movzx edi,byte[ebx+3]
		jmp @f
	.c1z: ;�᫨ 梥� � ebx �஧���
		mov edx,dword[eax]
	@@:
		add edi,255 ;������ 梥� �� �������� �஧���
		shr edi,1
		cmp edi,255
		jl @f
			mov edi,255 ;���ᨬ��쭠� �஧�筮��� �� ����� 255
		@@:
		shl edi,24
		and edx,0xffffff ;᭨���� ����� �஧�筮���
		add edx,edi
	.end_f:
	ret
endp

;description:
; ᦠ⨥ ����ࠦ���� �� �ਭ� (ࠧ���� ���� �� ��������)
;input:
; data_rgb - pointer to rgb data
; size_w - width img in pixels
; size_h - height img in pixels
; size_w_new - new width img in pixels
align 16
proc img_rgb24_wresize, data_rgb:dword, size_w:dword, size_h:dword, size_w_new:dword
locals
	pr dd 0
	pg dd 0
	pb dd 0
	img_n dd ? ;㪠��⥫� �� ����� ������ ����ࠦ����
	lines dd ?
endl
pushad
;eax - delta for inp. img
;ebx - delta for outp. img
;esi - pointer to data_rgb
	mov esi,[data_rgb]
	mov [img_n],esi
	mov eax,[size_h]
	mov [lines],eax
align 4
	.cycyle_0:
	mov eax,[size_w_new]
	mov ecx,[size_w]
	mov ebx,ecx
align 4
	.cycyle_1:
		cmp eax,ebx
		jg .else_0
			;�����㥬� ���ᥫ� ���ᨬ��쭮 ����� �� १����
			;����������� rgb ��� ���௮��樨 ���ᥫ��
			mov edx,[size_w_new]
			movzx edi,byte[esi]
			imul edi,edx
			add [pb],edi
			movzx edi,byte[esi+1]
			imul edi,edx
			add [pg],edi
			movzx edi,byte[esi+2]
			imul edi,edx
			add [pr],edi
			cmp eax,ebx
			je .d2_add
			jmp .if_0_end
		.else_0:
			;�����㥬� ���ᥫ� ������� �� �࠭��� ���ᥫ��
			mov edx,ebx
			sub edx,eax
			add edx,[size_w_new]
			movzx edi,byte[esi]
			imul edi,edx
			add [pb],edi
			movzx edi,byte[esi+1]
			imul edi,edx
			add [pg],edi
			movzx edi,byte[esi+2]
			imul edi,edx
			add [pr],edi
			;��࠭塞 ��⮢�� rgb
			.d2_add:
			push eax
				mov edi,[img_n]
				mov eax,[pb]
				xor edx,edx
				div dword[size_w] ;eax /= [size_w]
				stosb
				mov eax,[pg]
				xor edx,edx
				div dword[size_w] ;eax /= [size_w]
				stosb
				mov eax,[pr]
				xor edx,edx
				div dword[size_w] ;eax /= [size_w]
				stosb
			pop eax
			add dword[img_n],3 ;next pixel
			;������塞 rgb ��� ������ ���ᥫ�
			mov edx,eax
			sub edx,ebx
			movzx edi,byte[esi]
			imul edi,edx
			mov [pb],edi
			movzx edi,byte[esi+1]
			imul edi,edx
			mov [pg],edi
			movzx edi,byte[esi+2]
			imul edi,edx
			mov [pr],edi
			add ebx,[size_w]
		.if_0_end:
		add eax,[size_w_new]
		add esi,3 ;next pixel
		dec ecx
		jnz .cycyle_1
	dec dword[lines]
	jnz .cycyle_0
popad
	ret
endp

;description:
; ᦠ⨥ ����ࠦ���� �� ���� (ࠧ���� ���� �� ��������)
;input:
; data_rgb - pointer to rgb data
; size_w - width img in pixels
; size_h - height img in pixels
; size_h_new - new height img in pixels
align 16
proc img_rgb24_hresize, data_rgb:dword, size_w:dword, size_h:dword, size_h_new:dword
locals
	pr dd 0
	pg dd 0
	pb dd 0
	img_n dd ? ;㪠��⥫� �� ����� ������ ����ࠦ����
	cols dd ?
	lin_b dd ? ;ࠧ��� ����� ����ࠦ���� � �����
	data_n dd ? ;㪠��⥫� �� ����� ��� ������ �⮫�� ���ᥫ��
endl
pushad
;eax - delta for inp. img
;ebx - delta for outp. img
;esi - pointer to data_rgb
	mov esi,[data_rgb]
	mov [data_n],esi
	mov eax,[size_w]
	mov [cols],eax
	lea eax,[eax+eax*2]
	mov [lin_b],eax
align 4
	.cycyle_0:
	mov eax,[size_h_new]
	mov ecx,[size_h]
	mov ebx,ecx
	mov esi,[data_n]
	mov [img_n],esi
	add dword[data_n],3 ;���室 �� ᫥���騩 �⮫��� ���ᥫ��
align 4
	.cycyle_1:
		cmp eax,ebx
		jg .else_0
			;�����㥬� ���ᥫ� ���ᨬ��쭮 ����� �� १����
			;����������� rgb ��� ���௮��樨 ���ᥫ��
			mov edx,[size_h_new]
			movzx edi,byte[esi]
			imul edi,edx
			add [pb],edi
			movzx edi,byte[esi+1]
			imul edi,edx
			add [pg],edi
			movzx edi,byte[esi+2]
			imul edi,edx
			add [pr],edi
			cmp eax,ebx
			je .d2_add
			jmp .if_0_end
		.else_0:
			;�����㥬� ���ᥫ� ������� �� �࠭��� ���ᥫ��
			mov edx,ebx
			sub edx,eax
			add edx,[size_h_new]
			movzx edi,byte[esi]
			imul edi,edx
			add [pb],edi
			movzx edi,byte[esi+1]
			imul edi,edx
			add [pg],edi
			movzx edi,byte[esi+2]
			imul edi,edx
			add [pr],edi
			;��࠭塞 ��⮢�� rgb
			.d2_add:
			push eax
				mov edi,[img_n]
				mov eax,[pb]
				xor edx,edx
				div dword[size_h] ;eax /= [size_h]
				stosb
				mov eax,[pg]
				xor edx,edx
				div dword[size_h] ;eax /= [size_h]
				stosb
				mov eax,[pr]
				xor edx,edx
				div dword[size_h] ;eax /= [size_h]
				stosb
			pop eax
			mov edx,[lin_b]
			add dword[img_n],edx ;next pixel
			;������塞 rgb ��� ������ ���ᥫ�
			mov edx,eax
			sub edx,ebx
			movzx edi,byte[esi]
			imul edi,edx
			mov [pb],edi
			movzx edi,byte[esi+1]
			imul edi,edx
			mov [pg],edi
			movzx edi,byte[esi+2]
			imul edi,edx
			mov [pr],edi
			add ebx,[size_h]
		.if_0_end:
		add eax,[size_h_new]
		add esi,[lin_b] ;next pixel
		dec ecx
		jnz .cycyle_1
	dec dword[cols]
	jnz .cycyle_0
popad
	ret
endp

;�८�ࠧ������ ���� �� 24-��⭮�� � 8-����
; spectr - ��।���� ����� ᯥ��� ���� �� �८�ࠧ������ 0-ᨭ��, 1-������, 2-����
align 4
proc buf_conv_24_to_8, buf_struc:dword, spectr:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .error
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		mov esi,ecx
		;ebx - ������ �� ���ன ���������
		;edx - ������ �㤠 ���������
		mov edx,buf2d_data
		mov ebx,edx
		cmp [spectr],3
		jge @f
			add ebx,[spectr]
		@@:
			mov al,byte[ebx]
			mov byte[edx],al
			add ebx,3
			inc edx
			loop @b
		mov buf2d_bits,8
		invoke mem.realloc,buf2d_data,esi ;㬥��蠥� ������ ���������� ���஬
		jmp .end_conv
	.error:
		stdcall print_err,sz_buf2d_conv_24_to_8,txt_err_n24b
	.end_conv:
	popad
	ret
endp

;�८�ࠧ������ ���� �� 24-��⭮�� � 32-����
align 4
proc buf_conv_24_to_32, buf_struc:dword, buf_str8:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .error1
		mov ecx,buf2d_w
		mov ebx,buf2d_h
		imul ebx,ecx
		mov ecx,ebx ;ecx = size  8 b
		shl ebx,2   ;ebx = size 32 b
		invoke mem.realloc,buf2d_data,ebx ;㢥��稢��� ������ ���������� ���஬
		mov buf2d_data,eax ;�� ��砩 �᫨ ��������� 㪠��⥫� �� �����
		mov buf2d_bits,32
		mov edx,ebx ;edx = size 32 b
		sub ebx,ecx ;ebx = size 24 b
		mov eax,ecx
		;eax - ࠧ���  8 ����� ������
		;ebx - ࠧ��� 24 ����� ������
		;edx - ࠧ��� 32 ����� ������
		add ebx,buf2d_data
		add edx,buf2d_data
		mov edi,dword[buf_str8]
		cmp buf2d_bits,8
		jne .error2
		add eax,buf2d_data
		mov edi,edx
		;eax - 㪠��⥫� �� �����  8 ����� ������
		;ebx - 㪠��⥫� �� ����� 24 ����� ������
		;edi - 㪠��⥫� �� ����� 32 ����� ������
		@@:
			sub edi,4 ;�⭨���� � ��砫� 横��,
			sub ebx,3 ; ��⮬�, �� 㪠��⥫� ����
			dec eax   ; �� �।����� ���஢
			mov edx,dword[ebx]
			mov dword[edi],edx
			mov dl,byte[eax]
			mov byte[edi+3],dl
			loop @b

		jmp .end_conv
	.error1:
		stdcall print_err,sz_buf2d_conv_24_to_32,txt_err_n24b
		jmp .end_conv
	.error2:
		stdcall print_err,sz_buf2d_conv_24_to_32,txt_err_n8b
	.end_conv:
	popad
	ret
endp

;�㭪�� ������� ����ࠦ���� �� ���� buf_source (24b|32b) � buf_destination (24b)
; 㪠�뢠���� ���न���� ��⠢�� ���� buf_source �⭮�⥫쭮 buf_destination
; �஧�筮��� �� ����஢���� �� ���뢠����
align 4
proc buf_bit_blt, buf_destination:dword, coord_x:dword, coord_y:dword, buf_source:dword
	locals
		right_bytes dd ?
	endl
	pushad

	mov edi,[buf_source]
	cmp buf2d_bits,24
	je .sou24
	cmp buf2d_bits,32
	je .sou32
		jmp .copy_end ;�ଠ� ���� �� �����ন������

	.sou24: ;� ���筨�� 24 ��⭠� ���⨭��
	mov eax,buf2d_w
	mov edx,buf2d_h ;���� �����㥬�� ���⨭��
	mov esi,buf2d_data ;����� �����㥬�� ���⨭��

	mov edi,[buf_destination]
	cmp buf2d_bits,24
	jne .copy_end ;�ଠ� ���� �� �����ন������
	mov ebx,[coord_x] ;� ebx �६���� �⠢�� ����� ����ࠦ���� (��� �஢�ન)
	cmp ebx,buf2d_w   ;�஢��塞 ������ �� ����ࠦ���� �� �ਭ�
	jge .copy_end	  ;�᫨ ����ࠦ���� ��������� �뫠��� �� �ࠢ�� ��஭�
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
		cmp ecx,0
		jge @f
			;�᫨ ���न��� coord_y<0 (1-� ����ன��)
			add edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��
			cmp edx,0
			jle .copy_end ;�᫨ �����㥬�� ����ࠦ���� ��室���� ��������� ��� ���孥� �࠭�楩 ���� (coord_y<0 � |coord_y|>buf_source.h)
			neg ecx
			;inc ecx
			imul ecx,eax
			lea ecx,[ecx+ecx*2] ;�� 3 ���� �� ���ᥫ�
			add esi,ecx ;ᤢ����� 㪠��⥫� � �����㥬묨 ����묨, � ��⮬ �ய�襭�� ���
			xor ecx,ecx ;����塞 ���न���� coord_y
		@@:
		cmp ecx,ebx
		jge .copy_end ;�᫨ ���न��� 'y' ����� ����� ����
		add ecx,edx ;ecx - ������ ���न��� �����㥬�� ���⨭��
		cmp ecx,ebx
		jle @f
			sub ecx,ebx
			sub edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��, � ��� ����� ��� �뫠��� �� ������ �࠭���
		@@:
		mov ebx,buf2d_w
		mov ecx,[coord_y] ;ecx �ᯮ��㥬 ��� �६����� 楫��
		cmp ecx,0
		jg .end_otr_c_y_24
			;�᫨ ���न��� coord_y<=0 (2-� ����ன��)
			mov ecx,[coord_x]
			jmp @f
		.end_otr_c_y_24:
		imul ecx,ebx
		add ecx,[coord_x]
		@@:
		lea ecx,[ecx+ecx*2]
		add ecx,buf2d_data
		sub ebx,eax
		mov edi,ecx ;edi 㪠��⥫� �� ����� ����, �㤠 �㤥� �ந�������� ����஢����

	mov [right_bytes],0
	mov ecx,[coord_x]
	cmp ecx,ebx
	jl @f
		sub ecx,ebx
		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
		lea ecx,[ecx+ecx*2] ;ecx - �᫮ ���� � 1-� ��ப� ���⨭��, ����� �뫠��� �� �ࠢ�� ��஭�
		mov [right_bytes],ecx
	@@:

	lea eax,[eax+eax*2] ;�������⢮ ���� � 1-� ��ப� �����㥬�� ���⨭��
	lea ebx,[ebx+ebx*2] ;�������⢮ ���� � 1-� ��ப� ���� ����� �᫮ ���� � 1-� ��ப� �����㥬�� ���⨭��

	cld
	cmp [right_bytes],0
	jg .copy_1
	.copy_0: ;���⮥ ����஢����
		mov ecx,eax
		rep movsb
		add edi,ebx
		dec edx
		cmp edx,0
		jg .copy_0
	jmp .copy_end
	.copy_1: ;�� ���⮥ ����஢���� (���⨭�� �뫠��� �� �ࠢ�� ��஭�)
		mov ecx,eax
		rep movsb
		add edi,ebx
		add esi,[right_bytes] ;������塞 �����, ����� �뫠��� �� �ࠢ�� �࠭���
		dec edx
		cmp edx,0
		jg .copy_1
	jmp .copy_end

	.sou32: ;� ���筨�� 32 ��⭠� ���⨭��
	mov eax,buf2d_w
	mov edx,buf2d_h ;���� �����㥬�� ���⨭��
	mov esi,buf2d_data ;����� �����㥬�� ���⨭��

	mov edi,[buf_destination]
	cmp buf2d_bits,24
	jne .copy_end ;�ଠ� ���� �� �����ন������
	mov ebx,[coord_x] ;� ebx �६���� �⠢�� ����� ����ࠦ���� (��� �஢�ન)
	cmp ebx,buf2d_w   ;�஢��塞 ������ �� ����ࠦ���� �� �ਭ�
	jge .copy_end	  ;�᫨ ����ࠦ���� ��������� �뫠��� �� �ࠢ�� ��஭�
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
		cmp ecx,0
		jge @f
			;�᫨ ���न��� coord_y<0 (1-� ����ன��)
			add edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��
			cmp edx,0
			jle .copy_end ;�᫨ �����㥬�� ����ࠦ���� ��室���� ��������� ��� ���孥� �࠭�楩 ���� (coord_y<0 � |coord_y|>buf_source.h)
			neg ecx
			;inc ecx
			imul ecx,eax
			shl ecx,2 ;�� 4 ���� �� ���ᥫ�
			add esi,ecx ;ᤢ����� 㪠��⥫� � �����㥬묨 ����묨, � ��⮬ �ய�襭�� ���
			xor ecx,ecx ;����塞 ���न���� coord_y
		@@:
		cmp ecx,ebx
		jge .copy_end ;�᫨ ���न��� 'y' ����� ����� ����
		add ecx,edx ;ecx - ������ ���न��� �����㥬�� ���⨭��
		cmp ecx,ebx
		jle @f
			sub ecx,ebx
			sub edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��, � ��� ����� ��� �뫠��� �� ������ �࠭���
		@@:
		mov ebx,buf2d_w
		;mov ecx,ebx ;ecx �ᯮ��㥬 ��� �६����� 楫��
		;imul ecx,[coord_y]
		;add ecx,[coord_x]
		mov ecx,[coord_y] ;ecx �ᯮ��㥬 ��� �६����� 楫��
		cmp ecx,0
		jg .end_otr_c_y_32
			;�᫨ ���न��� coord_y<=0 (2-� ����ன��)
			mov ecx,[coord_x]
			jmp @f
		.end_otr_c_y_32:
		imul ecx,ebx
		add ecx,[coord_x]
		@@:
		lea ecx,[ecx+ecx*2]
		add ecx,buf2d_data
		sub ebx,eax
		mov edi,ecx ;edi 㪠��⥫� �� ����� ����, �㤠 �㤥� �ந�������� ����஢����

	mov [right_bytes],0
	mov ecx,[coord_x]
	cmp ecx,ebx
	jl @f
		sub ecx,ebx
		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
		shl ecx,2 ;ecx - �᫮ ���� � 1-� ��ப� ���⨭��, ����� �뫠��� �� �ࠢ�� ��஭�
		mov [right_bytes],ecx
	@@:

	;eax - �������⢮ ���ᥫ�� � 1-� ��ப� �����㥬�� ���⨭��
	lea ebx,[ebx+ebx*2] ;�������⢮ ���� � 1-� ��ப� ���� ����� �᫮ ���� � 1-� ��ப� �����㥬�� ���⨭��

	cld
	cmp [right_bytes],0
	jg .copy_3
	.copy_2: ;���⮥ ����஢����
		mov ecx,eax
		@@:
			movsw
			movsb
			inc esi
			loop @b
		add edi,ebx
		dec edx
		cmp edx,0
		jg .copy_2
	jmp .copy_end
	.copy_3: ;�� ���⮥ ����஢���� (���⨭�� �뫠��� �� �ࠢ�� ��஭�)
		mov ecx,eax
		@@:
			movsw
			movsb
			inc esi
			loop @b
		add edi,ebx
		add esi,[right_bytes] ;������塞 �����, ����� �뫠��� �� �ࠢ�� �࠭���
		dec edx
		cmp edx,0
		jg .copy_3

	.copy_end:
	popad
	ret
endp

;input:
; esi = pointer to color1 + transparent
; edi = pointer to background color2
;output:
; [edi] = combine color
align 4
combine_colors_0:
	push ax bx cx dx
	mov bx,0x00ff ;---get transparent---
	movzx cx,byte[esi+3] ;pro
	sub bx,cx ;256-pro
	;---blye---
	movzx ax,byte[esi]
	imul ax,bx
	movzx dx,byte[edi]
	imul dx,cx
	add ax,dx
	mov byte[edi],ah
	;---green---
	movzx ax,byte[esi+1]
	imul ax,bx
	movzx dx,byte[edi+1]
	imul dx,cx
	add ax,dx
	mov byte[edi+1],ah
	;---red---
	movzx ax,byte[esi+2]
	imul ax,bx
	movzx dx,byte[edi+2]
	imul dx,cx
	add ax,dx
	mov byte[edi+2],ah

	pop dx cx bx ax
	ret

;�㭪�� ������� ����ࠦ���� �� ���� buf_source (32b) � buf_destination (24b)
; 㪠�뢠���� ���न���� ��⠢�� ���� buf_source �⭮�⥫쭮 buf_destination
; �� ����஢���� ���뢠���� �஧�筮���
align 4
proc buf_bit_blt_transp, buf_destination:dword, coord_x:dword, coord_y:dword, buf_source:dword
	locals
		lost_bytes dd ?
	endl
	pushad

	mov edi,[buf_source]
	cmp buf2d_bits,32
	jne .copy_end ;�ଠ� ���� �� �����ন������
	mov eax,buf2d_w
	mov edx,buf2d_h ;���� �����㥬�� ���⨭��
	mov esi,buf2d_data ;����� �����㥬�� ���⨭��

	mov edi,[buf_destination]
	cmp buf2d_bits,24
	jne .copy_end ;�ଠ� ���� �� �����ন������
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
		cmp ecx,0
		jge @f
			;�᫨ ���न��� coord_y<0 (1-� ����ன��)
			add edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��
			cmp edx,0
			jle .copy_end ;�᫨ �����㥬�� ����ࠦ���� ��室���� ��������� ��� ���孥� �࠭�楩 ���� (coord_y<0 � |coord_y|>buf_source.h)
			neg ecx
			;inc ecx
			imul ecx,eax
			shl ecx,2 ;�� 4 ���� �� ���ᥫ�
			add esi,ecx ;ᤢ����� 㪠��⥫� � �����㥬묨 ����묨, � ��⮬ �ய�襭�� ���
			xor ecx,ecx ;����塞 ���न���� coord_y
		@@:
		cmp ecx,ebx
		jge .copy_end ;�᫨ ���न��� 'y' ����� ����� ����
		add ecx,edx ;ecx - ������ ���न��� �����㥬�� ���⨭��
		cmp ecx,ebx
		jle @f
			sub ecx,ebx
			sub edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��, � ��� ����� ��� �뫠��� �� ������ �࠭���
		@@:
		mov ebx,buf2d_w
		mov ecx,ebx ;ecx �ᯮ��㥬 ��� �६����� 楫��
		cmp [coord_y],0
		jg .end_otr_c_y
			;�᫨ ���न��� coord_y<=0 (2-� ����ன��)
			mov ecx,[coord_x]
			jmp @f
		.end_otr_c_y:
		imul ecx,[coord_y]
		add ecx,[coord_x]
		@@:
		lea ecx,[ecx+ecx*2]
		add ecx,buf2d_data
		sub ebx,eax
		mov edi,ecx ;edi 㪠��⥫� �� ����� ����, �㤠 �㤥� �ந�������� ����஢����

	mov dword[lost_bytes],0
	mov ecx,[coord_x]
	cmp ecx,0
	jge @f
		neg ecx
		;inc ecx
		cmp eax,ecx ;eax - �ਭ� �����㥬�� ���⨭��
		jle .copy_end ;�᫨ �����㥬�� ����ࠦ���� ��室���� ��������� �� ����� �࠭�楩 ���� (coord_x<0 � |coord_x|>buf_source.w)
		shl ecx,2
		mov [lost_bytes],ecx
		add esi,ecx
		shr ecx,2
		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
		lea ecx,[ecx+ecx*2]
		add edi,ecx ;edi 㪠��⥫� �� ����� ����, �㤠 �㤥� �ந�������� ����஢����
		xor ecx,ecx
	@@:
	cmp ecx,ebx
	jle @f
		sub ecx,ebx
		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
		shl ecx,2 ;ecx - �᫮ ���ᥫ�� � 1-� ��ப� ���⨭��, ����� �뫠��� �� �ࠢ�� ��஭�
		add [lost_bytes],ecx
	@@:

;	mov [right_bytes],0
;	mov ecx,[coord_x]
;	cmp ecx,ebx
;	jl @f
;		sub ecx,ebx
;		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
;		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
;		shl ecx,2 ;ecx - �᫮ ���� � 1-� ��ப� ���⨭��, ����� �뫠��� �� �ࠢ�� ��஭�
;		mov [right_bytes],ecx
;	@@:

	lea ebx,[ebx+ebx*2] ;�������⢮ ���� � 1-� ��ப� ���� ����� �᫮ ���� � 1-� ��ப� �����㥬�� ���⨭��

	cld
	cmp [lost_bytes],0
	jg .copy_1
	.copy_0: ;���⮥ ����஢����
		mov ecx,eax
		@@:
			call combine_colors_0
			add edi,3
			add esi,4
			loop @b
		add edi,ebx
		dec edx
		cmp edx,0
		jg .copy_0
	jmp .copy_end
	.copy_1: ;�� ���⮥ ����஢���� (���⨭�� �뫠��� �� �ࠢ�� ��஭�)
		mov ecx,eax
		@@:
			call combine_colors_0
			add edi,3
			add esi,4
			loop @b
		add edi,ebx
		add esi,[lost_bytes] ;������塞 �����, ����� �뫠��� �� �ࠢ�� �࠭���
		dec edx
		cmp edx,0
		jg .copy_1

	.copy_end:
	popad
	ret
endp

;input:
; ebx - color1
; esi = pointer to transparent
; edi = pointer to background color2
;output:
; [edi] = combine color
align 4
combine_colors_2:
	push ax ebx cx dx si
	mov cl,byte[esi] ;pro
	xor ch,ch
	mov si,0x00ff ;---get transparent---
	sub si,cx ;256-pro

		;---blye---
		movzx ax,bl
		shr ebx,8
		imul ax,si
		movzx dx,byte[edi]
		imul dx,cx
		add ax,dx
		mov byte[edi],ah
		;---green---
		movzx ax,bl
		shr ebx,8
		imul ax,si
		movzx dx,byte[edi+1]
		imul dx,cx
		add ax,dx
		mov byte[edi+1],ah
		;---red---
		movzx ax,bl
		imul ax,si
		movzx dx,byte[edi+2]
		imul dx,cx
		add ax,dx
		mov byte[edi+2],ah

	pop si dx cx ebx ax
	ret

;�㭪�� ������� ����ࠦ���� �� ���� buf_source (8b) � buf_destination (24b)
; 㪠�뢠���� ���न���� ��⠢�� ���� buf_source �⭮�⥫쭮 buf_destination
align 4
proc buf_bit_blt_alpha, buf_destination:dword, coord_x:dword, coord_y:dword, buf_source:dword, color:dword
	locals
		lost_bytes dd ? ;�᫮ ����ﭭ�� ���⮢ � ��ப� �����㥬��� ����ࠦ��� (�� �� �� ������ � ����)
		dest_w_bytes dd ? ;�������⢮ ���� � ���� �ਥ����� �� �ਭ� - �ਭ� ��⠢�塞�� ���⨭��
	endl
	pushad

	mov edi,[buf_source]
	cmp buf2d_bits,8
	jne .error1 ;�ଠ� ���� �� �����ন������
	mov eax,buf2d_w ;�ਭ� �����㥬�� ���⨭��
	mov edx,buf2d_h ;���� �����㥬�� ���⨭��
	mov esi,buf2d_data ;����� �����㥬�� ���⨭��

	mov edi,[buf_destination]
	cmp buf2d_bits,24
	jne .error2 ;�ଠ� ���� �� �����ন������
	mov ebx,[coord_x] ;� ebx �६���� �⠢�� ����� ����ࠦ���� (��� �஢�ન)
	cmp ebx,buf2d_w   ;�஢��塞 ������ �� ����ࠦ���� �� �ਭ�
	jge .copy_end	  ;�᫨ ����ࠦ���� ��������� �뫠��� �� �ࠢ�� ��஭�
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
		cmp ecx,0
		jge @f
			;�᫨ ���न��� coord_y<0 (1-� ����ன��)
			add edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��
			cmp edx,0
			jle .copy_end ;�᫨ �����㥬�� ����ࠦ���� ��室���� ��������� ��� ���孥� �࠭�楩 ���� (coord_y<0 � |coord_y|>buf_source.h)
			neg ecx
			;inc ecx
			imul ecx,eax
			add esi,ecx ;ᤢ����� 㪠��⥫� � �����㥬묨 ����묨, � ��⮬ �ய�襭�� ���
			xor ecx,ecx ;����塞 ���न���� coord_y
		@@:
		cmp ecx,ebx
		jge .copy_end ;�᫨ ���न��� 'y' ����� ����� ����
		add ecx,edx ;ecx - ������ ���न��� �����㥬�� ���⨭��
		cmp ecx,ebx
		jle @f
			sub ecx,ebx
			sub edx,ecx ;㬥��蠥� ����� �����㥬�� ���⨭��, � ��� ����� ��� �뫠��� �� ������ �࠭���
		@@:
		mov ebx,buf2d_w
		mov ecx,[coord_y] ;ecx �ᯮ��㥬 ��� �६����� 楫��
		cmp ecx,0
		jg .end_otr_c_y
			;�᫨ ���न��� coord_y<=0 (2-� ����ன��)
			mov ecx,[coord_x]
			jmp @f
		.end_otr_c_y:
		imul ecx,ebx
		add ecx,[coord_x]
		@@:
		lea ecx,[ecx+ecx*2]
		add ecx,buf2d_data ;buf2d_data ����� �᭮����� ����
		sub ebx,eax ;ebx - �ਭ� �᭮����� ���� ����� �ਭ� ��㥬��� ����
		mov edi,ecx ;edi 㪠��⥫� �� ����� ����, �㤠 �㤥� �ந�������� ����஢����

	mov dword[lost_bytes],0
	mov ecx,[coord_x]
	cmp ecx,0
	jge @f
		neg ecx
		;inc ecx
		cmp eax,ecx ;eax - �ਭ� �����㥬�� ���⨭��
		jle .copy_end ;�᫨ �����㥬�� ����ࠦ���� ��室���� ��������� �� ����� �࠭�楩 ���� (coord_x<0 � |coord_x|>buf_source.w)
		mov [lost_bytes],ecx
		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
		add esi,ecx
		lea ecx,[ecx+ecx*2]
		add edi,ecx ;edi 㪠��⥫� �� ����� ����, �㤠 �㤥� �ந�������� ����஢����
		xor ecx,ecx
	@@:
	cmp ecx,ebx
	jle @f
		sub ecx,ebx
		sub eax,ecx ;㪮�稢��� �����㥬�� ��ப�
		add ebx,ecx ;㤫���塞 ��ப� ��� ᤢ��� ������� ���⨭�� ����
		;ecx - �᫮ ���ᥫ�� � 1-� ��ப� ���⨭��, ����� �뫠��� �� �ࠢ�� ��஭�
		add [lost_bytes],ecx
	@@:

	lea ebx,[ebx+ebx*2] ;�������⢮ ���� � 1-� ��ப� ���� ����� �᫮ ���� � 1-� ��ப� �����㥬�� ���⨭��
	mov [dest_w_bytes],ebx
	mov ebx,[color]

	cld
	cmp dword[lost_bytes],0
	jg .copy_1
	.copy_0: ;���⮥ ����஢����
		mov ecx,eax
		@@:
			call combine_colors_2
			add edi,3
			inc esi
			loop @b
		add edi,[dest_w_bytes]
		dec edx
		cmp edx,0
		jg .copy_0
	jmp .copy_end
	.copy_1: ;�� ���⮥ ����஢���� (���⨭�� �뫠��� �� ����� �/��� �ࠢ�� ��஭�)
		mov ecx,eax
		@@:
			call combine_colors_2
			add edi,3
			inc esi
			loop @b
		add edi,[dest_w_bytes]
		add esi,[lost_bytes] ;������塞 �����, ����� �뫠��� �� �ࠢ�� �࠭���
		dec edx
		cmp edx,0
		jg .copy_1

	jmp .copy_end
	.error1:
		stdcall print_err,sz_buf2d_bit_blt_alpha,txt_err_n8b
		jmp .copy_end
	.error2:
		stdcall print_err,sz_buf2d_bit_blt_alpha,txt_err_n24b
	.copy_end:
	popad
	ret
endp

align 4
proc print_err, fun:dword, mes:dword ;�뢮��� ᮮ�饭�� �� 訡�� �� ���� �⫠���
	pushad
	mov eax,63
	mov ebx,1

	mov esi,[fun]
	@@:
		mov cl,byte[esi]
		int 0x40
		inc esi
		cmp byte[esi],0
		jne @b
	mov cl,':'
	int 0x40
	mov cl,' '
	int 0x40
	mov esi,[mes]
	@@:
		mov cl,byte[esi]
		int 0x40
		inc esi
		cmp byte[esi],0
		jne @b
	popad
	ret
endp

;䨫���
align 4
proc buf_filter_dither, buffer:dword, algor:dword
	pushad
	mov edi,[buffer]
	cmp buf2d_bits,24
	jne .error
		mov edx,buf2d_w
		mov esi,buf2d_h
		mov edi,buf2d_data
;edi - pointer to 24bit bitmap
;edx - x size
;esi - y size
		lea   edx,[edx+edx*2]
		imul  esi,edx

		;��।��塞 ����� ������ �ᯮ�짮����
		cmp dword[algor],0
		jne @f
			call dither_0
			jmp .dither_end
		@@:
		cmp dword[algor],1
		jne @f
			call dither_1
			jmp .dither_end
		@@:
		cmp dword[algor],2
		jne @f
			call dither_2
			jmp .dither_end
		@@:
		cmp dword[algor],3
		jne @f
			call dither_3
			jmp .dither_end
		@@:
		call dither_4
		jmp .dither_end
	.error:
		stdcall print_err,sz_buf2d_filter_dither,txt_err_n24b
	.dither_end:
	popad
	ret
endp

align 16
dither_0: ; Sierra Filter Lite algorithm
newp_0:   ; Dithering cycle
	xor   ebx,ebx ; At first threshold
	movzx ecx,byte[edi]
	cmp   cl,255
	je    newp_0.next
	test  cl,cl
	jz    newp_0.next
	jns   @f
	dec   ebx
	sub   ecx,255
@@:
	mov   [edi],bl               ; putpixel

	sar   ecx,1                  ; error/2
	;adc   ecx,0                  ; round to integer

	movzx eax,byte[edi+3]        ; pixel (x+1;y)
	add   eax,ecx                ; add error/2 to (x+1;y)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok
@@:
	cmp   eax,255
	jle   .ok
	or    al,255
.ok:
	mov   [edi+3],al             ; putpixel

	sar   ecx,1                  ; error/4
	adc   ecx,0                  ; round to integer

	movzx eax,byte[edi+edx-3]    ; pixel (x-1;y+1)
	add   eax,ecx                ; add error/4 to (x-1;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok1
@@:
	cmp   eax,255
	jle   .ok1
	or    al,255
.ok1:
	mov   [edi+edx-3],al         ; putpixel

	movzx eax,byte[edi+edx]      ; pixel (x;y+1)
	add   eax,ecx                ; add error/4 to (x;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok2
@@:
	cmp   eax,255
	jle   .ok2
	or    al,255
.ok2:
	mov   [edi+edx],al           ; putpixel

.next:
	inc   edi
	dec   esi
	jnz   newp_0
	ret

align 16
dither_1: ; Floyd-Steinberg algorithm
newp_1:   ; Dithering cycle
	xor   ebx,ebx ; At first threshold
	movzx ecx,byte[edi]
	cmp   cl,255
	je    newp_1.next
	test  cl,cl
	jz    newp_1.next
	jns   @f
	dec   ebx
	sub   ecx,255
@@:
	mov   [edi],bl               ; putpixel

	sar   ecx,4                  ; error/16
	adc   ecx,0                  ; round to integer
	mov   ebx,ecx

	movzx eax,byte[edi+edx+3]    ; pixel (x+1;y+1)
	add   eax,ecx                ; add error/16 to (x+1;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok
@@:
	cmp   eax,255
	jle   .ok
	or    al,255
.ok:
	mov   [edi+edx+3],al         ;putpixel

	imul  ecx,3
	movzx eax,byte[edi+edx-3]    ; pixel (x-1;y+1)
	add   eax,ecx                ; add 3*error/16 to (x-1;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok1
@@:
	cmp   eax,255
	jle   .ok1
	or    al,255
.ok1:
	mov   [edi+edx-3],al         ;putpixel

	mov   ecx,ebx
	imul  ecx,5
	movzx eax,byte[edi+edx]      ; pixel (x;y+1)
	add   eax,ecx                ; add 5*error/16 to (x;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok2
@@:
	cmp   eax,255
	jle   .ok2
	or    al,255
.ok2:
	mov   [edi+edx],al           ;putpixel

	mov   ecx,ebx
	imul  ecx,7
	movzx eax,byte[edi+3]        ; pixel (x+1;y)
	add   eax,ecx                ; add 7*error/16 to (x+1;y)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok3
@@:
	cmp   eax,255
	jle   .ok3
	or    al,255
.ok3:
	mov   [edi+3],al             ;putpixel

.next:
	inc  edi
	dec  esi
	jnz  newp_1
	ret

align 16
dither_2: ; Burkes algorithm
newp_2:   ; Dithering cycle
	xor   ebx,ebx ; At first threshold
	movsx ecx,byte[edi]
	cmp   cl,255
	je    newp_2.next
	test  cl,cl
	jz    newp_2.next
	jns   @f
	dec   ebx
@@:
	mov   [edi],bl               ; putpixel

	sar   ecx,2                  ; error/4
	adc   ecx,0                  ; round to integer

	movzx eax,byte[edi+3]        ; pixel (x+1;y)
	add   eax,ecx                ; add error/4 to (x+1;y)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok
@@:
	cmp   eax,255
	jle   .ok
	or    al,255
.ok:
	mov   [edi+3],al             ; putpixel

	movzx eax,byte[edi+edx]      ; pixel (x;y+1)
	add   eax,ecx                ; add error/4 to (x;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok1
@@:
	cmp   eax,255
	jle   .ok1
	or    al,255
.ok1:
	mov   [edi+edx],al           ; putpixel

	sar   ecx,1                  ; error/8
	adc   ecx,0                  ; round to integer

	movzx eax,byte[edi+6]        ; pixel (x+2;y)
	add   eax,ecx                ; add error/8 to (x+2;y)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok2
@@:
	cmp   eax,255
	jle   .ok2
	or    al,255
.ok2:
	mov   [edi+6],al             ; putpixel

	movzx eax,byte[edi+edx-3]    ; pixel (x-1;y+1)
	add   eax,ecx                ; add error/8 to (x-1;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok3
@@:
	cmp   eax,255
	jle   .ok3
	or    al,255
.ok3:
	mov   [edi+edx-3],al         ; putpixel

	movzx eax,byte[edi+edx+3]    ; pixel (x+1;y+1)
	add   eax,ecx                ; add error/8 to (x+1;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok4
@@:
	cmp   eax,255
	jle   .ok4
	or    al,255
.ok4:
	mov   [edi+edx+3],al         ; putpixel

	sar   ecx,1                  ; error/16
	;adc   ecx,0                  ; round to integer

	movzx eax,byte[edi+edx-6]    ; pixel (x-2;y+1)
	add   eax,ecx                ; add error/16 to (x-2;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok5
@@:
	cmp   eax,255
	jle   .ok5
	or    al,255
.ok5:
	mov   [edi+edx-6],al         ; putpixel

	movzx eax,byte[edi+edx+6]    ; pixel (x+2;y+1)
	add   eax,ecx                ; add error/16 to (x+2;y+1)
	jge   @f                     ; check_overflow
	xor   eax,eax
	jmp   .ok6
@@:
	cmp   eax,255
	jle   .ok6
	or    al,255
.ok6:
	mov   [edi+edx+6],al         ; putpixel

.next:
	inc   edi
	dec   esi
	jnz   newp_2
	ret


align 16
dither_3:                        ; Heavyiron_mod algorithm
 newp_3:                         ; Dithering cycle
    xor   ebx,ebx                ; At first threshold
    movzx ecx,byte[edi]
    cmp   cl,255
    je   .next
    test  cl,cl
    jz    .next
    jns   @f
    dec   ebx
    sub   ecx,255
  @@:
    mov   [edi],bl               ; putpixel

    sar   ecx,2                  ; error/4

    movzx eax,byte[edi+3]        ; pixel (x+1;y)
    add   eax,ecx                ; add error/4 to (x+1;y)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok
  @@:
    cmp   eax,255
    jle   .ok
    or    al,255
  .ok:
    mov   [edi+3],al             ; putpixel

    movzx eax,byte[edi+edx-3]    ; pixel (x-1;y+1)
    add   eax,ecx                ; add error/4 to (x-1;y+1)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok1
  @@:
    cmp   eax,255
    jle   .ok1
    or    al,255
  .ok1:
    mov   [edi+edx-3],al         ; putpixel

    movzx eax,byte[edi+edx]      ; pixel (x;y+1)
    add   eax,ecx                ; add error/4 to (x;y+1)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok2
  @@:
    cmp   eax,255
    jle   .ok2
    or    al,255
  .ok2:
    mov   [edi+edx],al           ; putpixel

  .next:
    inc   edi
    dec   esi
    jnz   newp_3
    ret

align 16
dither_4:                        ; Atkinson algorithm
 newp_4:                         ; Dithering cycle

    xor   ebx,ebx                ; At first threshold
    movsx ecx,byte[edi]
    cmp   cl,255
    je   .next
    test  cl,cl
    jz    .next
    jns   @f
    dec   ebx
  @@:
    mov   [edi],bl               ; putpixel

    sar   ecx,3                  ; error/8

    movzx eax,byte[edi+3]        ; pixel (x+1;y)
    add   eax,ecx                ; add error/8 to (x+1;y)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok
  @@:
    cmp   eax,255
    jle   .ok
    or    al,255
  .ok:
    mov   [edi+3],al             ; putpixel

    movzx eax,byte[edi+edx]      ; pixel (x;y+1)
    add   eax,ecx                ; add error/8 to (x;y+1)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok1
  @@:
    cmp   eax,255
    jle   .ok1
    or    al,255
  .ok1:
    mov   [edi+edx],al           ; putpixel

    movzx eax,byte[edi+6]        ; pixel (x+2;y)
    add   eax,ecx                ; add error/8 to (x+2;y)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok2
  @@:
    cmp   eax,255
    jle   .ok2
    or    al,255
  .ok2:
    mov   [edi+6],al             ; putpixel

    movzx eax,byte[edi+edx-3]    ; pixel (x-1;y+1)
    add   eax,ecx                ; add error/8 to (x-1;y+1)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok3
  @@:
    cmp   eax,255
    jle   .ok3
    or    al,255
  .ok3:
    mov   [edi+edx-3],al         ; putpixel

    movzx eax,byte[edi+edx+3]    ; pixel (x+1;y+1)
    add   eax,ecx                ; add error/8 to (x+1;y+1)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok4
  @@:
    cmp   eax,255
    jle   .ok4
    or    al,255
  .ok4:
    mov   [edi+edx+3],al         ; putpixel


    movzx eax,byte[edi+edx+edx]    ; pixel (x;y+2)
    add   eax,ecx                ; add error/8 to (x;y+2)
    jge   @f                     ; check_overflow
    xor   eax,eax
    jmp   .ok5
  @@:
    cmp   eax,255
    jle   .ok5
    or    al,255
  .ok5:
    mov   [edi+edx+edx],al         ; putpixel

  .next:
    inc   edi
    dec   esi
    jnz   newp_4
    ret



include 'fun_voxel.inc' ;�㭪樨 ��� ࠡ��� � ���ᥫ쭮� ��䨪��

txt_err_n8b db 'need buffer 8 bit',13,10,0
txt_err_n24b db 'need buffer 24 bit',13,10,0
txt_err_n32b db 'need buffer 32 bit',13,10,0
txt_err_n8_24b db 'need buffer 8 or 24 bit',13,10,0

align 16
EXPORTS:
	dd sz_lib_init, lib_init
	dd sz_buf2d_create, buf_create
	dd sz_buf2d_create_f_img, buf_create_f_img
	dd sz_buf2d_clear, buf_clear
	dd sz_buf2d_draw, buf_draw_buf
	dd sz_buf2d_delete, buf_delete
	dd sz_buf2d_resize, buf_resize
	dd sz_buf2d_rotate, buf_rotate
	dd sz_buf2d_line, buf_line_brs
	dd sz_buf2d_line_sm, buf_line_brs_sm
	dd sz_buf2d_rect_by_size, buf_rect_by_size
	dd sz_buf2d_filled_rect_by_size, buf_filled_rect_by_size
	dd sz_buf2d_circle, buf_circle
	dd sz_buf2d_img_hdiv2, buf_img_hdiv2
	dd sz_buf2d_img_wdiv2, buf_img_wdiv2
	dd sz_buf2d_conv_24_to_8, buf_conv_24_to_8
	dd sz_buf2d_conv_24_to_32, buf_conv_24_to_32
	dd sz_buf2d_bit_blt, buf_bit_blt
	dd sz_buf2d_bit_blt_transp, buf_bit_blt_transp
	dd sz_buf2d_bit_blt_alpha, buf_bit_blt_alpha
	dd sz_buf2d_curve_bezier, buf_curve_bezier
	dd sz_buf2d_convert_text_matrix, buf_convert_text_matrix
	dd sz_buf2d_draw_text, buf_draw_text
	dd sz_buf2d_crop_color, buf_crop_color
	dd sz_buf2d_offset_h, buf_offset_h
	dd sz_buf2d_flood_fill, buf_flood_fill
	dd sz_buf2d_set_pixel, buf_set_pixel
	dd sz_buf2d_get_pixel, buf_get_pixel
	dd sz_buf2d_flip_h, buf_flip_h
	dd sz_buf2d_flip_v, buf_flip_v
	dd sz_buf2d_filter_dither, buf_filter_dither
	dd sz_buf2d_vox_brush_create, vox_brush_create
	dd sz_buf2d_vox_brush_delete, vox_brush_delete
	dd sz_buf2d_vox_obj_get_img_w_3g, buf_vox_obj_get_img_w_3g
	dd sz_buf2d_vox_obj_get_img_h_3g, buf_vox_obj_get_img_h_3g
	dd sz_buf2d_vox_obj_draw_1g, buf_vox_obj_draw_1g
	dd sz_buf2d_vox_obj_draw_3g, buf_vox_obj_draw_3g
	dd sz_buf2d_vox_obj_draw_3g_scaled, buf_vox_obj_draw_3g_scaled
	dd sz_buf2d_vox_obj_draw_pl, buf_vox_obj_draw_pl
	dd sz_buf2d_vox_obj_draw_pl_scaled, buf_vox_obj_draw_pl_scaled
	dd sz_buf2d_vox_obj_draw_3g_shadows, buf_vox_obj_draw_3g_shadows
	dd 0,0
	sz_lib_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0 ;���⪠ ���� 㪠����� 梥⮬
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_resize db 'buf2d_resize',0
	sz_buf2d_rotate db 'buf2d_rotate',0
	sz_buf2d_line db 'buf2d_line',0 ;�ᮢ���� �����
	sz_buf2d_line_sm db 'buf2d_line_sm',0 ;�ᮢ���� ᣫ������� �����
	sz_buf2d_rect_by_size db 'buf2d_rect_by_size',0 ;�ᮢ���� ࠬ�� ��אַ㣮�쭨��, 2-� ���न��� ������ �� ࠧ����
	sz_buf2d_filled_rect_by_size db 'buf2d_filled_rect_by_size',0 ;�ᮢ���� ����⮣� ��אַ㣮�쭨��, 2-� ���न��� ������ �� ࠧ����
	sz_buf2d_circle db 'buf2d_circle',0 ;�ᮢ���� ���㦭���
	sz_buf2d_img_hdiv2 db 'buf2d_img_hdiv2',0 ;ᦠ⨥ ����ࠦ���� �� ���� � 2 ࠧ� (ࠧ��� ���� �� �������)
	sz_buf2d_img_wdiv2 db 'buf2d_img_wdiv2',0 ;ᦠ⨥ ����ࠦ���� �� �ਭ� � 2 ࠧ� (ࠧ��� ���� �� �������)
	sz_buf2d_conv_24_to_8 db 'buf2d_conv_24_to_8',0
	sz_buf2d_conv_24_to_32 db 'buf2d_conv_24_to_32',0 
	sz_buf2d_bit_blt db 'buf2d_bit_blt',0
	sz_buf2d_bit_blt_transp db 'buf2d_bit_blt_transp',0
	sz_buf2d_bit_blt_alpha db 'buf2d_bit_blt_alpha',0
	sz_buf2d_curve_bezier db 'buf2d_curve_bezier',0
	sz_buf2d_convert_text_matrix db 'buf2d_convert_text_matrix',0
	sz_buf2d_draw_text db 'buf2d_draw_text',0
	sz_buf2d_crop_color db 'buf2d_crop_color',0
	sz_buf2d_offset_h db 'buf2d_offset_h',0
	sz_buf2d_flood_fill db 'buf2d_flood_fill',0
	sz_buf2d_set_pixel db 'buf2d_set_pixel',0
	sz_buf2d_get_pixel db 'buf2d_get_pixel',0
	sz_buf2d_flip_h db 'buf2d_flip_h',0
	sz_buf2d_flip_v db 'buf2d_flip_v',0
	sz_buf2d_filter_dither db 'buf2d_filter_dither',0
	sz_buf2d_vox_brush_create db 'buf2d_vox_brush_create',0
	sz_buf2d_vox_brush_delete db 'buf2d_vox_brush_delete',0
	sz_buf2d_vox_obj_get_img_w_3g db 'buf2d_vox_obj_get_img_w_3g',0
	sz_buf2d_vox_obj_get_img_h_3g db 'buf2d_vox_obj_get_img_h_3g',0
	sz_buf2d_vox_obj_draw_1g db 'buf2d_vox_obj_draw_1g',0
	sz_buf2d_vox_obj_draw_3g db 'buf2d_vox_obj_draw_3g',0
	sz_buf2d_vox_obj_draw_3g_scaled db 'buf2d_vox_obj_draw_3g_scaled',0
	sz_buf2d_vox_obj_draw_pl db 'buf2d_vox_obj_draw_pl',0
	sz_buf2d_vox_obj_draw_pl_scaled db 'buf2d_vox_obj_draw_pl_scaled',0
	sz_buf2d_vox_obj_draw_3g_shadows db 'buf2d_vox_obj_draw_3g_shadows',0
