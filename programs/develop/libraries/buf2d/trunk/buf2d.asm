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

;input:
; ebx = coord x
; ecx = coord y
; edx = pixel color
; edi = pointer to buffer struct
align 4
draw_pixel:
	;cmp buf2d_bits,24
	;jne @f
	bt ebx,31
	jc @f
	bt ecx,31
	jc @f
	cmp ebx,buf2d_w
	jge @f
	cmp ecx,buf2d_h
	jge @f
	push esi
		mov esi,buf2d_w ;size x
		imul esi,ecx ;size_x*y
		add esi,ebx	 ;size_x*y+x
		cmp buf2d_bits,8
		je .beg8
			lea esi,[esi+esi*2] ;(size_x*y+x)*3
			add esi,buf2d_data  ;ptr+(size_x*y+x)*3
			mov word[esi],dx ;copy pixel color
			ror edx,16
			mov byte[esi+2],dl
			ror edx,16
			jmp .end_draw
		.beg8: ;�ᮢ���� �窨 � 8 ��⭮� ����
			add esi,buf2d_data  ;ptr+(size_x*y+x)
			mov byte[esi],dl
		.end_draw:
	pop esi
	@@:
	ret

;input:
; ebx = coord x
; ecx = coord y
; edi = pointer to buffer struct
;output:
; eax = 梥� �窨
; � ��砥 �訡�� eax = 0xffffffff
align 4
get_pixel_24:
	mov eax,0xffffffff

	bt ebx,31
	jc @f
	bt ecx,31
	jc @f
	cmp ebx,buf2d_w
	jge @f
	cmp ecx,buf2d_h
	jge @f
	push esi
		mov esi,buf2d_w ;size x
		imul esi,ecx ;size_x*y
		add esi,ebx	 ;size_x*y+x
		lea esi,[esi+esi*2] ;(size_x*y+x)*3
		add esi,buf2d_data  ;ptr+(size_x*y+x)*3

		xor eax,eax
		mov ax,word[esi] ;copy pixel color
		ror eax,16
		mov al,byte[esi+2]
		ror eax,16
	pop esi
	@@:
	ret

;input:
; ebx = coord x
; ecx = coord y
; edx = pixel color + transparent
; edi = pointer to buffer struct
; t_prop, m_prop - ����樥��� ����室��� ��� ���᫥��� �⥯��� �஧�筮��
align 4
transp_32 dd 0 ;梥� ��㥬�� �窨 + �஧�筮���
align 4
proc draw_pixel_transp, t_prop:dword, m_prop:dword
	;cmp buf2d_bits,24
	;jne @f
	bt ebx,31
	jc @f
	bt ecx,31
	jc @f
	cmp ebx,buf2d_w
	jge @f
	cmp ecx,buf2d_h
	jge @f
	push eax ebx edx edi esi
		mov esi,buf2d_w ;size x
		imul esi,ecx ;size_x*y
		add esi,ebx	 ;size_x*y+x
		lea esi,[esi+esi*2] ;(size_x*y+x)*3
		add esi,buf2d_data  ;ptr+(size_x*y+x)*3

		mov edi,esi ;㪠��⥫� �� 梥� 䮭�
		mov dword[transp_32],edx ;梥� ��㥬�� �窨

		xor edx,edx
		mov eax,[t_prop]
		shl eax,8 ;*=256
		mov ebx,[m_prop]
		div ebx ;����塞 ����. �஧�筮�� (������ ���� �� 0 �� 255)
		bt ax,8
		jnc .over_255
			;�᫨ ����. �஧�筮�� >=256 � 㬥��蠥� ��� �� 255
			mov al,0xff
		.over_255:

		mov byte[transp_32+3],al ;�஧�筮��� ��㥬�� �窨
		mov esi,dword transp_32 ;㪠��⥫� �� 梥� ��㥬�� �窨

		call combine_colors
	pop esi edi edx ebx eax
	@@:
	ret
endp

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

align 4
proc buf_clear, buf_struc:dword, color:dword ;���⪠ ���� ������ 梥⮬
	pushad
	mov edi,dword[buf_struc]

	mov ecx,buf2d_w
	mov ebx,buf2d_h
	imul ecx,ebx

	cld

	cmp buf2d_bits,8
	jne .end_clear_8
		mov edi,buf2d_data
		mov al,byte[color]
		rep stosb
		jmp .end_clear_32
	.end_clear_8:

	cmp buf2d_bits,24
	jne .end_clear_24
		mov edi,buf2d_data
		mov eax,dword[color]
		mov ebx,eax
		shr ebx,16
		@@:
			stosw
			mov byte[edi],bl
			inc edi
			loop @b
		jmp .end_clear_32
	.end_clear_24:

	cmp buf2d_bits,32
	jne .end_clear_32
		mov edi,buf2d_data
		mov eax,dword[color]
		rep stosd
		;jmp .end_clear_32
	.end_clear_32:
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
proc buf_draw_buf, buf_struc:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .error
		mov eax,7
		mov ebx,buf2d_data

		mov ecx,buf2d_w
		ror ecx,16
		mov edx,buf2d_h
		mov cx,dx

		mov edx,buf2d_size_lt
		ror edx,16
		int 0x40
		jmp .end_draw_24
	.error:
		stdcall print_err,sz_buf2d_draw,txt_err_n24b
	.end_draw_24:
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

align 4
proc buf_resize, buf_struc:dword, new_w:dword, new_h:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .24bit
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
	popad
	ret
endp

align 4
proc buf_line_brs, buf_struc:dword, coord_x0:dword, coord_y0:dword, coord_x1:dword, coord_y1:dword, color:dword
locals
	loc_1 dd ?
	loc_2 dd ?
	napravl db ?
endl
	pushad
		mov eax,dword[coord_x1]
		sub eax,dword[coord_x0]
		bt eax,31
		jae @f
			neg eax
			inc eax
		@@:
		mov ebx,dword[coord_y1]
		sub ebx,dword[coord_y0]
		jnz @f
			;�᫨ ������ ��ਧ��⠫쭠� ����� y0=y1
			stdcall buf_line_h, [buf_struc], [coord_x0], [coord_y0], [coord_x1], [color]
			jmp .coord_end
		@@:
		bt ebx,31
		jae @f
			neg ebx
			inc ebx
		@@:
		mov edx,dword[color]

		mov [napravl],byte 0 ;bool steep=false
		cmp eax,ebx
		jle @f
			mov [napravl],byte 1 ;bool steep=true
			swap dword[coord_x0],dword[coord_y0] ;swap(x0, y0);
			swap dword[coord_x1],dword[coord_y1] ;swap(x1, y1);
		@@:
		mov eax,dword[coord_y0] ;x0
		cmp eax,dword[coord_y1] ;if(x0>x1)
		jle @f
			swap dword[coord_y0],dword[coord_y1] ;swap(x0, x1);
			swap dword[coord_x0],dword[coord_x1] ;swap(y0, y1);
		@@:

; int deltax esi
; int deltay edi
; int error  ebp-6
; int ystep  ebp-8

		mov eax,dword[coord_y0]
		mov esi,dword[coord_y1]
		sub esi,eax ;deltax = y1-y0
		mov ebx,esi
		shr ebx,1
		mov [loc_1],ebx ;error = deltax/2

		mov eax,dword[coord_x0]
		mov edi,dword[coord_x1]
		mov [loc_2],dword -1 ;ystep = -1
		cmp eax,edi ;if (x0<x1) ystep = 1;
		jge @f
			mov [loc_2],dword 1 ;ystep = 1
		@@:
		sub edi,eax ;x1-x0

		bts edi,31
		jae @f
			neg edi
			inc edi
		@@:
		and edi,0x7fffffff ;deltay = abs(x1-x0)

		mov eax,edi
		mov edi,[buf_struc]
		cmp buf2d_bits,8
		je @f
		cmp buf2d_bits,24
		je @f
			jmp .coord_end
		@@:

		cmp [napravl],0
		jne .coord_yx
			mov ebx,dword[coord_x0]
			mov ecx,dword[coord_y0]

			@@: ;for (x=x0 ; x<x1; x++) ;------------------------------------
				cmp ecx,dword[coord_y1]
				jg @f ;jge ???
				call draw_pixel

				sub dword[loc_1],eax ;error -= deltay
				cmp dword[loc_1],0 ;if(error<0)
				jge .if0
					add ebx,[loc_2] ;y += ystep
					add [loc_1],esi ;error += deltax
				.if0:
				inc ecx
				jmp @b
			@@:
			jmp .coord_end
		.coord_yx:
			mov ebx,dword[coord_y0]
			mov ecx,dword[coord_x0]

			@@: ;for (x=x0 ; x<x1; x++) ;------------------------------------
				cmp ebx,dword[coord_y1]
				jg @f ;jge ???
				call draw_pixel

				sub dword[loc_1],eax ;error -= deltay
				cmp dword[loc_1],0 ;if(error<0)
				jge .if1
					add ecx,[loc_2] ;y += ystep
					add [loc_1],esi ;error += deltax
				.if1:
				inc ebx
				jmp @b
			@@:
	.coord_end:
	popad
	ret
endp

;�ᮢ���� ᣫ������� �����
align 4
proc buf_line_brs_sm, buf_struc:dword, coord_x0:dword, coord_y0:dword, coord_x1:dword, coord_y1:dword, color:dword
locals
	loc_1 dd ?
	loc_2 dd ?
	napravl db ?
endl
	pushad
		mov eax,dword[coord_x1]
		sub eax,dword[coord_x0]
		bt eax,31
		jae @f
			neg eax
			inc eax
		@@:
		mov ebx,dword[coord_y1]
		sub ebx,dword[coord_y0]
		jnz @f
			;�᫨ ������ ��ਧ��⠫쭠� ����� y0=y1
			stdcall buf_line_h, [buf_struc], [coord_x0], [coord_y0], [coord_x1], [color]
			jmp .coord_end
		@@:
		bt ebx,31
		jae @f
			neg ebx
			inc ebx
		@@:
		mov edx,dword[color]

		mov [napravl],byte 0 ;bool steep=false
		cmp eax,ebx
		jle @f
			mov [napravl],byte 1 ;bool steep=true
			swap dword[coord_x0],dword[coord_y0] ;swap(x0, y0);
			swap dword[coord_x1],dword[coord_y1] ;swap(x1, y1);
		@@:
		mov eax,dword[coord_y0] ;x0
		cmp eax,dword[coord_y1] ;if(x0>x1)
		jle @f
			swap dword[coord_y0],dword[coord_y1] ;swap(x0, x1);
			swap dword[coord_x0],dword[coord_x1] ;swap(y0, y1);
		@@:

; int deltax esi
; int deltay edi
; int error  ebp-6
; int ystep  ebp-8

		mov eax,dword[coord_y0]
		mov esi,dword[coord_y1]
		sub esi,eax ;deltax = y1-y0
		mov ebx,esi
		shr ebx,1
		mov [loc_1],ebx ;error = deltax/2

		mov eax,dword[coord_x0]
		mov edi,dword[coord_x1]
		mov [loc_2],dword -1 ;ystep = -1
		cmp eax,edi ;if (x0<x1) ystep = 1;
		jge @f
			mov [loc_2],dword 1 ;ystep = 1
		@@:
		sub edi,eax ;x1-x0

		bts edi,31
		jae @f
			neg edi
			inc edi
		@@:
		and edi,0x7fffffff ;deltay = abs(x1-x0)

		mov eax,edi
		mov edi,[buf_struc]
		cmp buf2d_bits,24
		jne .coord_end

		cmp [napravl],0
		jne .coord_yx
			mov ebx,dword[coord_x0]
			mov ecx,dword[coord_y0]

			@@: ;for (x=x0 ; x<x1; x++) ;------------------------------------
				cmp ecx,dword[coord_y1]
				jg @f ;jge ???
				push eax
					mov eax,esi
					sub eax,[loc_1]
					stdcall draw_pixel_transp, eax,esi
				pop eax
				add ebx,[loc_2]
				stdcall draw_pixel_transp, [loc_1],esi
				sub ebx,[loc_2]

				sub dword[loc_1],eax ;error -= deltay
				cmp dword[loc_1],0 ;if(error<0)
				jge .if0
					add ebx,[loc_2] ;y += ystep
					add [loc_1],esi ;error += deltax
				.if0:
				inc ecx
				jmp @b
			@@:
			jmp .coord_end
		.coord_yx:
			mov ebx,dword[coord_y0]
			mov ecx,dword[coord_x0]

			@@: ;for (x=x0 ; x<x1; x++) ;------------------------------------
				cmp ebx,dword[coord_y1]
				jg @f ;jge ???
				push eax
					mov eax,esi
					sub eax,[loc_1]
					stdcall draw_pixel_transp, eax,esi
				pop eax
				add ecx,[loc_2]
				stdcall draw_pixel_transp, [loc_1],esi
				sub ecx,[loc_2]

				sub dword[loc_1],eax ;error -= deltay
				cmp dword[loc_1],0 ;if(error<0)
				jge .if1
					add ecx,[loc_2] ;y += ystep
					add [loc_1],esi ;error += deltax
				.if1:
				inc ebx
				jmp @b
			@@:
	.coord_end:
	popad
	ret
endp

;�ᮢ���� ��ਧ��⠫쭮� �����, ��⮬� ��� ��ࠬ��� coord_y1
align 4
proc buf_line_h, buf_struc:dword, coord_x0:dword, coord_y0:dword, coord_x1:dword, color:dword
	pushad
	pushfd
		mov edi,[buf_struc]
		cmp buf2d_bits,8
		je @f
		cmp buf2d_bits,24
		je @f
			jmp .end24
		@@: ;��।������ ���न��� ����� �⭮�⥫쭮 ����

		mov ecx,dword[coord_y0]
		bt ecx,31
		jc .end24 ;�᫨ ���न��� y0 ����⥫쭠�
		cmp ecx,buf2d_h
		jge .end24 ;�᫨ ���न��� y0 ����� ����� ����

		mov ebx,dword[coord_x0]
		mov esi,dword[coord_x1]
		cmp ebx,esi
		jle @f
			xchg ebx,esi ;�᫨ x0 > x1 � ���塞 ���⠬� x0 � x1
		@@:
		bt ebx,31
		jae @f
			;�᫨ ���न��� x0 ����⥫쭠�
			xor ebx,ebx
		@@:
		cmp esi,buf2d_w
		jl @f
			;�᫨ ���न��� x0 ����� �ਭ� ����
			mov esi,buf2d_w
			dec esi
		@@:
		cmp ebx,esi
		jg .end24 ;�᫨ x0 > x1 ����� ���������� ����� ��� ���न���� x0, x1 ��室����� �� ����� �� �।���� ����

		cmp buf2d_bits,24
		je .beg24
			;�ᮢ���� � 8 ��⭮� ����
			;� edx ����塞 ��砫� 1-� �窨 ����� � ���� ����ࠦ����
			mov edx,buf2d_w ;size x
			imul edx,ecx ;size_x*y
			add edx,ebx	 ;size_x*y+x
			add edx,buf2d_data ;ptr+(size_x*y+x)
			mov edi,edx ;⥯��� ����� ������ 㪠��⥫� �� ����

			mov ecx,esi
			sub ecx,ebx ;� ecx �������⢮ �祪 ����� �뢮����� � ����
			inc ecx ;��-�� ��᫥���� �窠 ����� ⠪�� �⮡ࠦ�����
			mov eax,dword[color] ;�㤥� �ᯮ�짮���� ⮫쪮 ���祭�� � al
			cld
			rep stosb ;横� �� �� x �� x0 �� x1 (������ x1)
			jmp .end24

		.beg24: ;�ᮢ���� � 24 ��⭮� ����
		;� eax ����塞 ��砫� 1-� �窨 ����� � ���� ����ࠦ����
		mov eax,buf2d_w ;size x
		imul eax,ecx ;size_x*y
		add eax,ebx	 ;size_x*y+x
		lea eax,[eax+eax*2] ;(size_x*y+x)*3
		add eax,buf2d_data  ;ptr+(size_x*y+x)*3

		mov ecx,esi
		sub ecx,ebx ;� ecx �������⢮ �祪 ����� �뢮����� � ����
		inc ecx ;��-�� ��᫥���� �窠 ����� ⠪�� �⮡ࠦ�����
		mov edx,dword[color]
		mov ebx,edx ;���न��� x0 � ebx 㦥 �� �㦭�
		ror edx,16 ;�����稢��� ॣ���� �� �� 3-� ���� ����� � dl
		cld
		@@: ;横� �� �� x �� x0 �� x1 (������ x1)
			mov word[eax],bx ;copy pixel color
			mov byte[eax+2],dl
			add eax,3
			loop @b
		.end24:
	popfd
	popad
	ret
endp

align 4
proc buf_rect_by_size, buf_struc:dword, coord_x:dword,coord_y:dword,w:dword,h:dword, color:dword
pushad
	mov edi,[buf_struc]
	cmp buf2d_bits,8
	je @f
	cmp buf2d_bits,24
	je @f
		jmp .coord_end
	@@:

		mov eax,[coord_x]
		mov ebx,[coord_y]
		mov ecx,[w]
		;cmp ecx,1
		;jl .coord_end
		cmp ecx,0
		je .coord_end
		jg @f
			add eax,ecx
			inc eax
			neg ecx
		@@:
		add ecx,eax
		dec ecx
		mov edx,[h]
		;cmp edx,1
		;jl .coord_end
		cmp edx,0
		je .coord_end
		jg @f
			add ebx,edx
			inc ebx
			neg edx
		@@:

		add edx,ebx
		dec edx
		mov esi,dword[color]
		stdcall buf_line_h, edi, eax, ebx, ecx, esi ;����� -
		stdcall buf_line_brs, edi, eax, ebx, eax, edx, esi ;����� |
		stdcall buf_line_h, edi, eax, edx, ecx, esi ;����� -
		stdcall buf_line_brs, edi, ecx, ebx, ecx, edx, esi ;����� |
	.coord_end:
popad
	ret
endp

align 4
proc buf_filled_rect_by_size, buf_struc:dword, coord_x:dword,coord_y:dword,w:dword,h:dword, color:dword
pushad
	mov edi,[buf_struc]
	cmp buf2d_bits,8
	je @f
	cmp buf2d_bits,24
	je @f
		jmp .coord_end
	@@:
		mov eax,[coord_x]
		mov ebx,[coord_y]
		mov edx,[w]
		cmp edx,0
		je .coord_end ;�᫨ ���� 0 ���ᥫ��
		jg @f ;�᫨ ���� ������⥫쭠�
			add eax,edx
			inc eax
			neg edx ;�ਭ� ������ ������⥫쭮�
			;inc edx ;��祬� ��� �� ������塞 1-�� � �� ����, �� � ��� ࠡ�⠥� �� �ࠢ��쭮
		@@:
		add edx,eax
		dec edx
		mov ecx,[h]
		cmp ecx,0
		je .coord_end ;�᫨ ���� 0 ���ᥫ��
		jg @f ;�᫨ ���� ������⥫쭠�
			add ebx,ecx ;ᤢ����� ������ ���न���� ��אַ㣮�쭨��
			inc ebx
			neg ecx ;����� ������ ������⥫쭮�
			;inc ecx ;��祬� ��� �� ������塞 1-�� � �� ����, �� � ��� ࠡ�⠥� �� �ࠢ��쭮
		@@:
		mov esi,dword[color]
		cld
		@@:
			stdcall buf_line_h, edi, eax, ebx, edx, esi ;����� -
			inc ebx
			loop @b
	.coord_end:
popad
	ret
endp

align 4
proc buf_circle, buf_struc:dword, coord_x:dword, coord_y:dword, r:dword, color:dword
locals
	po_x dd ?
	po_y dd ?
endl
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,8
	je @f
	cmp buf2d_bits,24
	je @f
		jmp .error
	@@:
		mov edx,dword[color]

		finit
		fild dword[coord_x]
		fild dword[coord_y]
		fild dword[r]
		fldz ;px=0
		fld st1 ;py=r

		fldpi
		fmul st0,st3
		fistp dword[po_x]
		mov esi,dword[po_x] ;esi=pi*r
		shl esi,1 ;esi=2*pi*r

		;st0 = py
		;st1 = px
		;st2 = r
		;st3 = y
		;st4 = x

		@@:
			;Point(px + x, y - py)
			fld st1 ;st0=px
			fadd st0,st5 ;st0=px+x
			fistp dword[po_x]
			mov ebx,dword[po_x]
			fld st3 ;st0=y
			fsub st0,st1 ;st0=y-py
			fistp dword[po_y]
			mov ecx,dword[po_y]
			call draw_pixel
			;px += py/r
			fld st0 ;st0=py
			fdiv st0,st3 ;st0=py/r
			faddp st2,st0 ;st3+=st0
			;py -= px/r
			fld st1 ;st0=px
			fdiv st0,st3 ;st0=px/r
			fsubp st1,st0 ;st2-=st0

			dec esi
			cmp esi,0
			jge @b
		jmp .exit_fun
	.error:
		stdcall print_err,sz_buf2d_circle,txt_err_n8_24b
	.exit_fun:

	popad
	ret
endp

;�㭪�� ��� ������� ������ ��࠭�� 梥⮬
align 4
proc buf_flood_fill, buf_struc:dword, coord_x:dword, coord_y:dword, mode:dword, color_f:dword, color_b:dword
	pushad
		mov edi,[buf_struc]
		cmp buf2d_bits,24
		jne .end24

			mov ebx,dword[coord_x]
			mov ecx,dword[coord_y]
			mov edx,dword[color_f]
			mov esi,dword[color_b]

			cmp dword[mode],1 ;� ����ᨬ��� �� 'mode' ��।��塞 ����� �����⬮� �㤥� ���짮������
			je @f
				call buf_flood_fill_recurs_0 ;�������� �� ���ᥫ�� 梥� esi
				jmp .end24
			@@:
				call buf_flood_fill_recurs_1 ;�������� ���ᥫ� ����騥 梥� esi

		.end24:
	popad
	ret
endp

;input:
; ebx = coord_x
; ecx = coord_y
; edx = 梥� �������
; esi = 梥� �࠭���, �� ���ன �㤥� �� �������
; edi = buf_struc
;output:
; eax = �������
align 4
buf_flood_fill_recurs_0:
	call get_pixel_24
	cmp eax,0xffffffff ;if error coords
	je .end_fun
	cmp eax,edx ;�᫨ 梥� ���ᥫ� ᮢ��� � 梥⮬ �������, ����� ������� � �⮩ ������ 㦥 �뫠 ᤥ����
	je .end_fun

		call draw_pixel

		dec ebx
		call get_pixel_24
		cmp eax,esi
		je @f
			call buf_flood_fill_recurs_0
		@@:
		inc ebx


		inc ebx
		call get_pixel_24
		cmp eax,esi
		je @f
			call buf_flood_fill_recurs_0
		@@:
		dec ebx

		dec ecx
		call get_pixel_24
		cmp eax,esi
		je @f
			call buf_flood_fill_recurs_0
		@@:
		inc ecx

		inc ecx
		call get_pixel_24
		cmp eax,esi
		je @f
			call buf_flood_fill_recurs_0
		@@:
		dec ecx

	.end_fun:
	ret

;input:
; ebx = coord_x
; ecx = coord_y
; edx = 梥� �������
; esi = 梥� ���ᥫ��, �� ����� �㤥� �� �������
; edi = buf_struc
;output:
; eax = �������
align 4
buf_flood_fill_recurs_1:
	call get_pixel_24
	cmp eax,0xffffffff ;if error coords
	je .end_fun
	cmp eax,edx ;�᫨ 梥� ���ᥫ� ᮢ��� � 梥⮬ �������, ����� ������� � �⮩ ������ 㦥 �뫠 ᤥ����
	je .end_fun
	cmp eax,esi ;�᫨ 梥� ���ᥫ� �� ᮢ��� � ��������� 梥⮬ �������, � �४�頥� �������
	jne .end_fun

		call draw_pixel

		dec ebx
		call get_pixel_24
		cmp eax,esi
		jne @f
			call buf_flood_fill_recurs_1
		@@:
		inc ebx


		inc ebx
		call get_pixel_24
		cmp eax,esi
		jne @f
			call buf_flood_fill_recurs_1
		@@:
		dec ebx

		dec ecx
		call get_pixel_24
		cmp eax,esi
		jne @f
			call buf_flood_fill_recurs_1
		@@:
		inc ecx

		inc ecx
		call get_pixel_24
		cmp eax,esi
		jne @f
			call buf_flood_fill_recurs_1
		@@:
		dec ecx

	.end_fun:
	ret

;�㭪�� ��� �ᮢ���� �窨
align 4
proc buf_set_pixel, buf_struc:dword, coord_x:dword, coord_y:dword, color:dword
	pushad
		mov edi,[buf_struc]
		cmp buf2d_bits,8
		je @f
		cmp buf2d_bits,24
		je @f
			jmp .end24
		@@:
			mov ebx,dword[coord_x]
			mov ecx,dword[coord_y]
			mov edx,dword[color]
			call draw_pixel
		.end24:
	popad
	ret
endp

align 4
proc buf_img_wdiv2, buf_struc:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .end_draw_24
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_rgb24_wdiv2, buf2d_data,ecx
	.end_draw_24:
	popad
	ret
endp

;input:
;data_rgb - pointer to rgb data
;size - count img pixels (size img data / 3(rgb) )
align 4
proc img_rgb24_wdiv2 data_rgb:dword, size:dword
  ;push eax ebx ecx edx
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
  ;pop edx ecx ebx eax
  ret
endp

align 4
proc buf_img_hdiv2, buf_struc:dword
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .end_draw_24
		mov eax,buf2d_w
		mov ecx,buf2d_h
		imul ecx,eax
		stdcall img_rgb24_hdiv2, buf2d_data,ecx,eax
	.end_draw_24:
	popad
	ret
endp

;input:
;data_rgb - pointer to rgb data
;size - count img pixels (size img data / 3(rgb) )
;size_w - width img in pixels
align 4
proc img_rgb24_hdiv2, data_rgb:dword, size:dword, size_w:dword
  ;pushad

  mov eax,dword[data_rgb] ;eax =
  mov ecx,dword[size]	  ;ecx = size
  lea ecx,[ecx+ecx*2]
  cld
  @@: ;��⥬����� 梥� ���ᥫ��
    shr byte[eax],1
    inc eax
    loop @b

  mov eax,dword[data_rgb] ;eax =
  mov edi,dword[size_w]
  lea esi,[edi+edi*2] ;esi = width*3(rgb)
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
  mov ebx,esi
  add ebx,eax
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

  ;popad
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
	jge .copy_end     ;�᫨ ����ࠦ���� ��������� �뫠��� �� �ࠢ�� ��஭�
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
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
		imul ecx,[coord_y]
		add ecx,[coord_x]
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
	jge .copy_end     ;�᫨ ����ࠦ���� ��������� �뫠��� �� �ࠢ�� ��஭�
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
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
		imul ecx,[coord_y]
		add ecx,[coord_x]
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
combine_colors:
	push ax bx cx dx
	mov bx,0x00ff ;---get transparent---
	mov cl,byte[esi+3] ;pro
	xor ch,ch
	sub bx,cx ;256-pro
	;---blye---
	xor ah,ah
	mov al,byte[esi]
	imul ax,bx
	xor dh,dh
	mov dl,byte[edi]
	imul dx,cx
	add ax,dx
	mov byte[edi],ah
	;---green---
	xor ah,ah
	mov al,byte[esi+1]
	imul ax,bx
	xor dh,dh
	mov dl,byte[edi+1]
	imul dx,cx
	add ax,dx
	mov byte[edi+1],ah
	;---red---
	xor ah,ah
	mov al,byte[esi+2]
	imul ax,bx
	xor dh,dh
	mov dl,byte[edi+2]
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
		right_bytes dd ?
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
		imul ecx,[coord_y]
		add ecx,[coord_x]
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

	lea ebx,[ebx+ebx*2] ;�������⢮ ���� � 1-� ��ப� ���� ����� �᫮ ���� � 1-� ��ப� �����㥬�� ���⨭��

	cld
	cmp [right_bytes],0
	jg .copy_1
	.copy_0: ;���⮥ ����஢����
		mov ecx,eax
		@@:
			call combine_colors
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
			call combine_colors
			add edi,3
			add esi,4
			loop @b
		add edi,ebx
		add esi,[right_bytes] ;������塞 �����, ����� �뫠��� �� �ࠢ�� �࠭���
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
		mov al,bl
		xor ah,ah
		shr ebx,8
		imul ax,si
		xor dh,dh
		mov dl,byte[edi]
		imul dx,cx
		add ax,dx
		mov byte[edi],ah
		;---green---
		mov al,bl
		xor ah,ah
		shr ebx,8
		imul ax,si
		xor dh,dh
		mov dl,byte[edi+1]
		imul dx,cx
		add ax,dx
		mov byte[edi+1],ah
		;---red---
		mov al,bl
		xor ah,ah
		imul ax,si
		xor dh,dh
		mov dl,byte[edi+2]
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
		right_bytes dd ?
		dest_w_bytes dd ? ;�������⢮ ���� � ���� �ਥ����� �� �ਭ� - �ਭ� ��⠢�塞�� ���⨭��
	endl
	pushad

	mov edi,[buf_source]
	cmp buf2d_bits,8
	jne .error1 ;�ଠ� ���� �� �����ন������
	mov eax,buf2d_w
	mov edx,buf2d_h ;���� �����㥬�� ���⨭��
	mov esi,buf2d_data ;����� �����㥬�� ���⨭��

	mov edi,[buf_destination]
	cmp buf2d_bits,24
	jne .error2 ;�ଠ� ���� �� �����ন������
	mov ebx,[coord_x] ;� ebx �६���� �⠢�� ����� ����ࠦ���� (��� �஢�ન)
	cmp ebx,buf2d_w   ;�஢��塞 ������ �� ����ࠦ���� �� �ਭ�
	jge .copy_end     ;�᫨ ����ࠦ���� ��������� �뫠��� �� �ࠢ�� ��஭�
		mov ebx,buf2d_h ;ebx - ���� �᭮����� ����
		mov ecx,[coord_y]
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
		imul ecx,[coord_y]
		add ecx,[coord_x]
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
		;ecx - �᫮ ���ᥫ�� � 1-� ��ப� ���⨭��, ����� �뫠��� �� �ࠢ�� ��஭�
		mov [right_bytes],ecx
	@@:

	lea ebx,[ebx+ebx*2] ;�������⢮ ���� � 1-� ��ப� ���� ����� �᫮ ���� � 1-� ��ப� �����㥬�� ���⨭��
	mov [dest_w_bytes],ebx
	mov ebx,[color]

	cld
	cmp [right_bytes],0
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
	.copy_1: ;�� ���⮥ ����஢���� (���⨭�� �뫠��� �� �ࠢ�� ��஭�)
		mov ecx,eax
		@@:
			call combine_colors_2
			add edi,3
			inc esi
			loop @b
		add edi,[dest_w_bytes]
		add esi,[right_bytes] ;������塞 �����, ����� �뫠��� �� �ࠢ�� �࠭���
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

;�८�ࠧ������ 8-��⭮�� ���� ࠧ��஬ 16*16 � ࠧ��� 1*256 ᨬ�����
align 4
proc buf_convert_text_matrix, buf_struc:dword
	locals
		tmp_mem dd ?
		c1 dw ?
		c2 dd ?
		c3 dw ?
	endl
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,8
	jne .error
		mov ecx,buf2d_h
		mov ebx,ecx
		shr ebx,4 ;�।�������� �� � ���� 16 ��ப � ᨬ������, ��⮬� ����� �� 2^4
		mov edx,buf2d_w
		imul ecx,edx ;ecx = size  8 b
		invoke mem.alloc,ecx ;�뤥�塞 �६����� ������
		mov [tmp_mem],eax ;eax - new memory

		shr edx,4 ;�।�������� �� � ���� 16 ������� � ᨬ������, ��⮬� ����� �� 2^4
		mov eax,ebx
		imul ebx,edx ;����塞 �������⢮ ���ᥫ�� �� 1 ᨬ���
		;eax = bhe - ���� �㪢�
		;ebx = bwi*bhe - �������⢮ ���ᥫ�� � 1-� �㪢�
		;edx = bwi - �ਭ� �㪢�
		;ecx,esi,edi - �ᯮ������� � 横�� .c_0
		shr buf2d_w,4
		shl buf2d_h,4 ;�८�ࠧ��뢠�� ࠧ���� ����

		cld
		mov esi,buf2d_data
		mov edi,[tmp_mem]
		mov word[c3],16
		.c_3:
			mov dword[c2],eax
			.c_2:
				mov word[c1],16
				.c_1:
					mov ecx,edx ;.c_0:
					rep movsb
					add edi,ebx
					sub edi,edx ;edi+=(bwi*bhe-bwi)
					dec word[c1]
					cmp word[c1],0
					jg .c_1
				add edi,edx
				shl ebx,4
				sub edi,ebx ;edi-=(16*bwi*bhe-bwi)
				shr ebx,4
				dec dword[c2]
				cmp dword[c2],0
				jg .c_2
			sub edi,ebx
			shl ebx,4
			add edi,ebx ;edi+=(15*bwi*bhe)
			shr ebx,4
			dec word[c3]
			cmp word[c3],0
			jg .c_3

		mov edi,dword[buf_struc] ;����஢���� ����� ������ � �᭮���� ����
		mov edi,buf2d_data
		mov esi,[tmp_mem]
		mov ecx,ebx
		shl ecx,8
		rep movsb
		invoke mem.free,[tmp_mem] ;��⨬ �६����� ������
		jmp .end_conv
	.error:
		stdcall print_err,sz_buf2d_convert_text_matrix,txt_err_n8b
	.end_conv:
	popad
	ret
endp

align 4
buf_s_matr buf_2d_header ? ;�����쭠� ����� ᨬ����

align 4
proc buf_draw_text, buf_struc:dword, buf_t_matr:dword, text:dword, coord_x:dword, coord_y:dword, color:dword
	locals
		buf_t_matr_offs dd ?
	endl
	pushad
	mov edi,dword[buf_struc]
	cmp buf2d_bits,24
	jne .error2
	mov edi,dword[buf_t_matr]
	cmp buf2d_bits,8
	jne .error1
		mov edx,buf2d_data
		mov [buf_t_matr_offs],edx
		mov ecx,BUF_STRUCT_SIZE ;�����㥬 �������� ⥪�⮢�� ������
		mov esi,edi
		lea edi,[buf_s_matr]
		cld
		rep movsb
		lea edi,[buf_s_matr]
		shr buf2d_h,8 ;����� ����� ᨬ���쭮�� ���� �� 256, ��� ��宦����� ����� 1-�� ᨬ����
		mov ebx,buf2d_h ;��६ ����� ᨬ����
		mov ecx,buf2d_w ;��६ �ਭ� ᨬ����

		mov eax,[coord_x]
		mov esi,[text]
		cmp byte[esi],0
		je .end_draw ;�᫨ ����� ��ப�
		@@:
			xor edx,edx
			mov dl,byte[esi] ;��६ ��� ᨬ����
			imul edx,ebx ;㬭����� ��� �� ����� ᨬ����
			imul edx,ecx ;㬭����� �� �ਭ� ᨬ����
			add edx,[buf_t_matr_offs] ;�ਡ���塞 ᬥ饭�� 0-�� ᨬ����, �. �. ����砥��� ᬥ饭�� �뢮������ ᨬ����
			mov buf2d_data,edx ;� ������� ���� ᨬ����, �⠢�� 㪠��⥫� �� �㦭� ᨬ��� �� ���� buf_t_matr
			stdcall buf_bit_blt_alpha, [buf_struc], eax,[coord_y], edi,[color]
			add eax,ecx
			.new_s:
				inc esi
				cmp byte[esi],13
				jne .no_13
					mov eax,[coord_x]
					add [coord_y],ebx
					jmp .new_s
				.no_13:
			cmp byte[esi],0
			jne @b
		jmp .end_draw
	.error1:
		stdcall print_err,sz_buf2d_draw_text,txt_err_n8b
		jmp .end_draw
	.error2:
		stdcall print_err,sz_buf2d_draw_text,txt_err_n24b
	.end_draw:
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

;input:
; ebp+8  = p0
; ebp+12 = p1
align 4
line_len4i:
	push ebp
	mov ebp,esp
		finit
		fild word [ebp+8]
		fisub word [ebp+12]
		fmul st0,st0 ;st0=x^2
		fild word [ebp+10]
		fisub word [ebp+14]
		fmul st0,st0 ;st0=y^2
		fadd st0,st1
		fsqrt
		fstp dword [ebp+12]
	pop ebp
	ret 4 ;8

align 4
proc buf_curve_bezier, buffer:dword, coord_p0:dword,coord_p1:dword,coord_p2:dword, color:dword
	locals
		delt_t dd ?
		opr_param dd ?
		v_poi_0 dd ?
	endl
	pushad

;float t, xt,yt;
;for(t=.0;t<1.;t+=.005){
;  xt=pow(1.-t,2)*x0+2*t*(1.-t)*x1+pow(t,2)*x2;
;  yt=pow(1.-t,2)*y0+2*t*(1.-t)*y1+pow(t,2)*y2;
;  dc.SetPixel(xt,yt,255L);
;}

	mov edx,[color] ;set curve color
	mov edi,[buffer]
	xor ebx,ebx
	xor ecx,ecx

	finit

	; calculate delta t
	stdcall line_len4i, dword[coord_p1],dword[coord_p0]
	fadd dword[esp]
	add esp,4 ;pop ...

	stdcall line_len4i, dword[coord_p2],dword[coord_p1]
	fadd dword[esp]
	add esp,4 ;pop ...

	fadd st0,st0 ; len*=2
	ftst
	fstsw ax

	fld1
	sahf
	jle @f ;�������� ������� �� 0
		fdiv st0,st1
	@@:
	fstp dword[delt_t]

	finit

	;fild word[coord_p2+2] ;y2
	fild word[coord_p1+2] ;y1
	fild word[coord_p0+2] ;y0
	fild word[coord_p2] ;x2
	fild word[coord_p1] ;x1
	fild word[coord_p0] ;x0
	fld dword[delt_t]
	fldz ;t=.0

	@@:
		fld1
		fsub st0,st1 ;1.-t
		fmul st0,st0 ;pow(1.-t,2)
		fmul st0,st3 ;...*x0
		fstp dword[opr_param]

		fld1
		fsub st0,st1 ;1.-t
		fmul st0,st1 ;(1.-t)*t
		fadd st0,st0
		fmul st0,st4 ;...*x1
		mov esi,dword[opr_param]
		fstp dword[opr_param]

		fldz
		fadd st0,st1 ;0+t
		fmul st0,st0 ;t*t
		fmul st0,st5 ;...*x2

		fadd dword[opr_param]
		mov dword[opr_param],esi
		fadd dword[opr_param]
		fistp word[v_poi_0] ;x

		fld1
		fsub st0,st1 ;1.-t
		fmul st0,st0 ;pow(1.-t,2)
		fmul st0,st6 ;...*y0
		fstp dword[opr_param]

		fld1
		fsub st0,st1 ;1.-t
		fmul st0,st1 ;(1.-t)*t
		fadd st0,st0
		fmul st0,st7 ;...*y1
		mov esi,dword[opr_param]
		fstp dword[opr_param]

		fldz
		fadd st0,st1 ;0+t
		fmul st0,st0 ;t*t
		fimul word[coord_p2+2] ;...*y2

		fadd dword[opr_param]
		mov dword[opr_param],esi
		fadd dword[opr_param]
		fistp word[v_poi_0+2] ;y

		mov eax,1
		mov bx,word[v_poi_0+2]
		mov cx,word[v_poi_0]
		call draw_pixel

		fadd st0,st1 ;t+dt

		fld1
		fcomp
		fstsw ax
		sahf
	jae @b

	popad
	ret
endp

txt_err_n8b db 'need buffer 8 bit',13,10,0
txt_err_n24b db 'need buffer 24 bit',13,10,0
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
	dd 0,0
	sz_lib_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0 ;���⪠ ���� 㪠����� 梥⮬
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_resize db 'buf2d_resize',0
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

