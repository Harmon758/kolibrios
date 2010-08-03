use32
	org 0x0
	db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
	dd 0x1
	dd start
	dd i_end ;ࠧ��� �ਫ������
	dd mem
	dd stacktop
	dd 0x0
	dd sys_path

include '../../../macros.inc'
include '../../../proc32.inc'
include '../../../develop/libraries/box_lib/load_lib.mac'
include 'mem.inc'
include 'dll.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc, dll.Load

;䫠��, ��� �㭪樨 ��१���� ����
BUF2D_OPT_CROP_TOP equ 1 ;��१�� ᢥ���
BUF2D_OPT_CROP_LEFT equ 2 ;��१�� ᫥��
BUF2D_OPT_CROP_BOTTOM equ 4 ;��१�� ᭨��
BUF2D_OPT_CROP_RIGHT equ 8 ;��१�� �ࠢ�
BUF2D_BIT_OPT_CROP_TOP equ 0
BUF2D_BIT_OPT_CROP_LEFT equ 1
BUF2D_BIT_OPT_CROP_BOTTOM equ 2
BUF2D_BIT_OPT_CROP_RIGHT equ 3

BUF_STRUCT_SIZE equ 21
buf2d_data equ dword[edi] ;����� ���� ����ࠦ����
buf2d_w equ dword[edi+8] ;�ਭ� ����
buf2d_h equ dword[edi+12] ;���� ����
buf2d_l equ word[edi+4] ;����� ᫥��
buf2d_t equ word[edi+6] ;����� ᢥ���
buf2d_size_lt equ dword[edi+4] ;����� ᫥�� � �ࠢ� ��� ����
buf2d_color equ dword[edi+16] ;梥� 䮭� ����
buf2d_bits equ byte[edi+20] ;������⢮ ��� � 1-� �窥 ����ࠦ����

struct FileInfoBlock
	Function dd ?
	Position dd ?
	Flags	 dd ?
	Count	 dd ?
	Buffer	 dd ?
		db ?
	FileName dd ?
ends

displ_w dd ? ;�ਭ� ����
displ_h dd ? ;���� ����
displ_bytes dd ? ;ࠧ��� 1-�� 䠩�� � ����ࠦ�����
;displ_bytes equ 315*210*3 ;ࠧ��� 1-�� 䠩�� � ����ࠦ�����

OFFS_SHADOW_X equ 2 ;ᤢ�� ⥭�� �� �� 'x'
OFFS_SHADOW_Y equ 2 ;ᤢ�� ⥭�� �� �� 'y'
IMAGE_FONT_SIZE equ 128*144*3

use_but equ 1

if use_but eq 1
BUT1_T equ 10 ;����� ᢥ���
BUT1_L equ 15 ;����� ᫥��
BUT1_W equ 50 ;�ਭ�
BUT1_H equ 20 ;����
BUT1_NEXT_TOP equ (BUT1_T+BUT1_H)*65536
end if

fn_icon0 db 'curici.png',0 ;��� 䠩�� � ������ﬨ
fn_icon1 db 'wolf.png',0 ;��� 䠩�� � ������ � ���楬
fn_icon2 db 'eggs.png',0 ;��� 䠩�� � �栬�
fn_icon3 db 'chi.png',0 ;��� 䠩�� � 樯��⠬�
fn_font db 'font8x9.bmp',0

ini_name db 'nu_pogod.ini',0
ini_sec_files db 'Files',0
key_displ_w db 'displ_w',0
key_displ_h db 'displ_h',0
ini_sec_color db 'Colors',0
key_color_unit db 'unit',0

;梥� � ���
color_fon dd 0xffffff
color_shadows dd 0xd0d0d0 ;梥� ⥭��
color_trees dd 0x008000 ;梥� �ࠢ�
color_wolf dd 0x800000 ;梥� ����� � ����
color_egg dd 0x404080 ;梥� ��
color_chick dd 0x00d0d0 ;梥� 樯�����
color_curici dd 0x8080d0 ;梥� �����
color_perilo dd 0x000080 ;梥� ��ਫ� (�ॡ��)
;梥� ����䥩�
color_but_sm dd 0x808080 ;梥� �����쪨� ������
color_but_te dd 0xffffff ;梥� ⥪�� �� �������

macro load_image_file path,buf,size { ;����� ��� ����㧪� ����ࠦ����
	copy_path path,sys_path,file_name,0x0 ;�ନ�㥬 ����� ���� � 䠩�� ����ࠦ����, ���ࠧ㬥���� �� �� � ����� ����� � �ணࠬ���

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

;����� ����
bit_zaac equ 2 ;��� ����
val_zaac equ 4 ;��� �⢥���� �� ��� ����
bit_mig equ 3 ;��� �������
val_mig equ 8 ;��� �⢥���� �� ��� �������
mask_lot_lu    equ 1b ;��᪠ ��� ������ ���孥�� ��⪠
mask_lot_ld    equ 100000b ;��᪠ ��� ������ ������� ��⪠
mask_lot_ru    equ 10000000000b ;��᪠ ��� �ࠢ��� ���孥�� ��⪠
mask_lot_rd    equ 1000000000000000b ;��᪠ ��� �ࠢ��� ������� ��⪠
mask_clear_all equ 11111011111011110111101111011110b ;��᪠ ��� ���⪨ ������� �� � �ਡ������ 樯���
mask_fail_eggs equ 100001000010000100000b ;��᪠ ������� ��
mask_chi_left  equ 11111000000000000000000000b ;��᪠ ����� 樯���
mask_chi_right equ 11111000000000000000000000000000b ;��᪠ �ࠢ�� 樯���
mask_chi_cr_l  equ 1000000000000000000000b ;��᪠ ��� ᮧ����� ������
mask_chi_cr_r  equ 1000000000000000000000000000b ;��᪠ ��� ᮧ����� �ࠢ���
bit_chi_left  equ 21 ;1-� ��� ����� �⢥砥� �� ����饣� ᫥��
bit_chi_right equ 27 ;1-� ��� ����� �⢥砥� �� ����饣� �ࠢ�
val_zaac_time_y equ 5 ;�������⢮ ⠪⮢, ���஥ ��易⥫쭮 ������ �஢���� ����
val_zaac_time_n equ 7 ;�������⢮ ⠪⮢, ���஥ ��易⥫쭮 ������ ���� ���⠭�� ����

txt_game_a db '��� �',0
txt_game_b db '��� �',0

zaac_status db 0
pos_wolf db 0 ;������ ����� 0-� ��� ᫥��/�ࠢ�, 1-� ��� ᢥ���/����
;rb 1
pos_eggs dd 0 ;����樨 �ᯮ������� �� � 樯���
eggs_count dw 0 ;�������⢮ �������� ��
game_text db '��� _',13
some_text db '0'
	rb 8 ;⥪�� � �᫮� ��������� ��
count_last db 0 ;���稪 �ய�饭��� ��
game_spd dd 0 ;����প� ����

;��� �����樨 ��砩��� �ᥫ
rand_x dd 0

align 4
rand_next:
;x(k+1) = (a*x(k)+c) mod m
; a=22695477, c=1, m=2^32
push eax
	mov eax,dword[rand_x]
	imul eax,22695477
	inc eax
	mov dword[rand_x],eax
pop eax
	ret

;ᮧ���� �஧��� ���� �� 8 ��� (������), ��� �ᮢ����
;��� buf - ���� �� �᭮�� ���ண� �㤥� ᮧ��� �����
;䮭��� 梥� ���� ������ ���� 0xffffff, ���� �� �ந������ ��१��
;�� 䮭����� 梥�� � ����� �㤥� ������� 쬭��� ���� � �����
align 4
proc CreateTrapharetBuffer, buf:dword, img_data:dword
	push eax edi
	mov edi,dword[buf]

	;���������� ������ ����
	mov buf2d_size_lt,0
	mov eax,dword[displ_w]
	mov buf2d_w,eax
	mov eax,dword[displ_h]
	mov buf2d_h,eax
	mov buf2d_color,0xffffff
	mov buf2d_bits,24

	stdcall [buf2d_create_f_img], edi,[img_data] ;ᮧ���� ����
	stdcall [buf2d_conv_24_to_8], edi,1 ;������ ���� �஧�筮�� 8���
	;��१��� ��譨� ��� ����, ��� ����� ����ண� �ᮢ����
	stdcall [buf2d_crop_color], edi,buf2d_color,BUF2D_OPT_CROP_TOP+BUF2D_OPT_CROP_BOTTOM+BUF2D_OPT_CROP_RIGHT+BUF2D_OPT_CROP_LEFT
	pop edi eax
	ret
endp

align 4
InitBackgroundBuffer: ;ᮧ����� 䮭����� ����ࠦ����
	pushad
	mov edi,buf_fon
	mov eax,[color_fon]
	mov buf2d_color,eax
	stdcall [buf2d_clear], edi,eax
	mov esi,edi

	xor eax,eax
	xor ebx,ebx

	mov edi,buf_decor
	mov ax,buf2d_t
	add eax,OFFS_SHADOW_Y
	mov bx,buf2d_l
	add ebx,OFFS_SHADOW_X
	stdcall [buf2d_bit_blt_alpha], esi, ebx,eax, edi,[color_shadows] ;��㥬 ⥭� �������
	add edi,BUF_STRUCT_SIZE
	mov ax,buf2d_t
	add eax,OFFS_SHADOW_Y
	mov bx,buf2d_l
	add ebx,OFFS_SHADOW_X
	stdcall [buf2d_bit_blt_alpha], esi, ebx,eax, edi,[color_shadows] ;��㥬 ⥭� ����
	add edi,BUF_STRUCT_SIZE
	mov ax,buf2d_t
	add eax,OFFS_SHADOW_Y
	mov bx,buf2d_l
	add ebx,OFFS_SHADOW_X
	stdcall [buf2d_bit_blt_alpha], esi, ebx,eax, edi,[color_shadows] ;��㥬 ⥭� ��ॢ쥢

	mov edi,buf_decor
	mov ax,buf2d_t
	stdcall [buf2d_bit_blt_alpha], esi, 0,eax, edi,[color_perilo] ;��㥬 ������
	add edi,BUF_STRUCT_SIZE
	mov ax,buf2d_t
	stdcall [buf2d_bit_blt_alpha], esi, 0,eax, edi,[color_curici] ;��㥬 �����
	add edi,BUF_STRUCT_SIZE
	mov ax,buf2d_t
	stdcall [buf2d_bit_blt_alpha], esi, 0,eax, edi,[color_trees] ;��㥬 ��ॢ��
	popad
	ret

;���� ���⨭�� � ���� buf �� ���ᨢ� ���஢ buf_img � �����ᮬ ind
;�. �. buf <- buf_img[ind]
align 4
proc DrawArrayImg, buf:dword, buf_img:dword, ind:dword, color:dword
	pushad
		mov edi,dword[ind]
		imul edi,BUF_STRUCT_SIZE
		add edi,dword[buf_img]

		mov esi,dword[buf]
		xor eax,eax
		mov ax,buf2d_t
		xor ebx,ebx
		mov bx,buf2d_l
		stdcall [buf2d_bit_blt_alpha], esi, ebx,eax, edi,[color]
	popad
	ret
endp

align 4
DrawZaac: ;�ᮢ���� ����
	push eax
	bt word[pos_wolf],bit_zaac
	jae .end_mig

	stdcall DrawArrayImg, buf_displ,buf_wolf,6,[color_wolf] ;��㥬 ⥫� ����
	mov eax,dword[pos_eggs]
	and eax,mask_chi_right+mask_chi_left
	cmp eax,0
	je .end_mig ;�᫨ ��� ������� 樯���, � ���� �㪮� �� ����
	bt word[pos_wolf],bit_mig
	jc @f
		stdcall DrawArrayImg, buf_displ,buf_wolf,7,[color_wolf] ;��㥬 ��� ���� ������
		jmp .end_mig
	@@:
		stdcall DrawArrayImg, buf_displ,buf_wolf,8,[color_wolf] ;��㥬 ��� ���� �����
	.end_mig:
	pop eax
	ret

align 4
DrawWolf: ;���� ����� � ��২����
	bt word[pos_wolf],0
	jc @f
		stdcall DrawArrayImg, buf_displ,buf_wolf,0,[color_wolf]
		bt word[pos_wolf],1
		jc .corz_ldn
			stdcall DrawArrayImg, buf_displ,buf_wolf,1,[color_wolf]
			jmp .corz_lend
		.corz_ldn:
			stdcall DrawArrayImg, buf_displ,buf_wolf,2,[color_wolf]
		.corz_lend:
		jmp .wolf_b
	@@:
		stdcall DrawArrayImg, buf_displ,buf_wolf,3,[color_wolf]
		bt word[pos_wolf],1
		jc .corz_rdn
			stdcall DrawArrayImg, buf_displ,buf_wolf,4,[color_wolf]
			jmp .corz_rend
		.corz_rdn:
			stdcall DrawArrayImg, buf_displ,buf_wolf,5,[color_wolf]
		.corz_rend:
	.wolf_b:
	ret

align 4
DrawEggs: ;���� ��
	pushad
	cld

	mov eax,dword[pos_eggs]
	xor ebx,ebx
	mov ecx,20 ;横� ��� �ᮢ���� �������� ��
	@@:
		bt eax,ebx
		jae .draw_egg
			stdcall DrawArrayImg, buf_displ,buf_egg,ebx,[color_egg]
		.draw_egg:
		inc ebx
		loop @b

	;mov eax,dword[pos_eggs]
	and eax,mask_chi_left
	cmp eax,0
	je @f
		stdcall DrawArrayImg, buf_displ,buf_egg,20,[color_egg] ;ࠧ��⮥ �� ᫥��
	@@:
	mov eax,dword[pos_eggs]
	and eax,mask_chi_right
	cmp eax,0
	je @f
		stdcall DrawArrayImg, buf_displ,buf_egg,21,[color_egg] ;ࠧ��⮥ �� �ࠢ�
	@@:

	mov eax,dword[pos_eggs]
	xor edx,edx
	mov ebx,bit_chi_left
	mov ecx,5 ;横� ��� �ᮢ���� ����� 樯���
	@@:
		bt eax,ebx
		jae .draw_chick_l
			stdcall DrawArrayImg, buf_displ,buf_chi,edx,[color_chick]
		.draw_chick_l:
		inc ebx
		inc edx
		loop @b
	mov ebx,bit_chi_right
	mov ecx,5 ;横� ��� �ᮢ���� �ࠢ�� 樯���
	@@:
		bt eax,ebx
		jae .draw_chick_r
			stdcall DrawArrayImg, buf_displ,buf_chi,edx,[color_chick]
		.draw_chick_r:
		inc ebx
		inc edx
		loop @b

	xor eax,eax
	mov al,byte[count_last]
	mov ecx,eax ;横� ��� �ᮢ���� ������ �窮�
	shr ecx,1
	mov edx,10 ;������ ��砫� ������ ������ � ���� buf_chi
	cmp ecx,0
	je .no_unit_last
	@@:
		stdcall DrawArrayImg, buf_displ,buf_chi,edx,[color_chick]
		inc edx
		loop @b
	.no_unit_last:

	bt ax,0 ;�஢��塞 �����饥 �� �⭮� �窮
	jae @f
		bt word[pos_wolf],bit_mig
		jc @f
			stdcall DrawArrayImg, buf_displ,buf_chi,edx,[color_chick] ;�����饥 ���䭮� �窮
	@@:

	popad
	ret

align 4
CountEggsInc: ;㢥��稢��� ���稪 �� �� 1
	push eax edi
		xor eax,eax
		inc word[eggs_count] ;㢥��稢��� ���稪 ��
		cmp word[eggs_count],200 ;������ �窨
		je @f
		cmp word[eggs_count],500 ;������ �窨
		je @f
			jmp .no_bonus
		@@:
			mov byte[count_last],0
		.no_bonus:

		mov ax,word[eggs_count]
		mov edi,some_text
		call convert_to_str ;������塞 ⥪�⮢�� ��ப�
		and ax,0xf ;�१ ����� 16 �� 㬥��蠥� �६�
		cmp ax,0
		jne @f
			cmp dword[game_spd],15 ;�������쭠� ����প�
			jle @f
				dec dword[game_spd]
		@@:
	pop edi eax
	ret

;input:
; eax - ��᪠, 㪠�뢠��� � ����� ��஭� 㯠�� ��
align 4
CountLastInc: ;���᫥��� ������ �窮�
	inc byte[count_last] ;��������� ���䭮�� �窠
	bt word[pos_wolf],bit_zaac
	jc @f
		inc byte[count_last] ;�᫨ ��� ���� � �� 1-�� �������� ���䭮�� �窠
	@@:
	cmp byte[count_last],6
	jle @f
		mov byte[count_last],6 ;�⠢�� ��࠭�祭� �� �᫮ ������ �窮�
	@@:
	or dword[pos_eggs],eax ;ᮧ���� ����饣� 樯�����
	ret

align 4
MoveEggs:
	pushad
	xor byte[pos_wolf],val_mig ;��� ��� �������

	cmp byte[count_last],6 ;���ᨬ��쭮� �᫮ ������ �窮�
	je .end_fun

	rol dword[pos_eggs],1
	mov ecx,dword[pos_eggs]
	and ecx,mask_fail_eggs
	cmp ecx,0
	je .no_fail ;��� ������� ��
		;���᫥��� �� ������� � ��২��
		;��� ���᫥��� ������ �窮�
		xor ebx,ebx
		mov bl,byte[pos_wolf] ;��६ ��ࠬ���� (������) �����
		and bl,3 ;�� ��直� ��砩

		bt ecx,5 ;�஢��塞 ���孥� ����� ��ਫ�
		jae .perilo_lu
			cmp bx,0 ;�஢��塞 ����稥 ��২��
			jne @f
				call CountEggsInc
				jmp .perilo_lu
			@@:
				mov eax,mask_chi_cr_l
				call CountLastInc
			.perilo_lu:
		bt ecx,10 ;�஢��塞 ������ ����� ��ਫ�
		jae .perilo_ld
			cmp bx,2 ;�஢��塞 ����稥 ��২��
			jne @f
				call CountEggsInc
				jmp .perilo_ld
			@@:
				mov eax,mask_chi_cr_l
				call CountLastInc
		.perilo_ld:
		bt ecx,15 ;�஢��塞 ���孥� �ࠢ�� ��ਫ�
		jae .perilo_ru
			cmp bx,1 ;�஢��塞 ����稥 ��২��
			jne @f
				call CountEggsInc
				jmp .perilo_ru
			@@:
				mov eax,mask_chi_cr_r
				call CountLastInc
		.perilo_ru:
		bt ecx,20 ;�஢��塞 ������ �ࠢ�� ��ਫ�
		jae .perilo_rd
			cmp bx,3 ;�஢��塞 ����稥 ��২��
			jne @f
				call CountEggsInc
				jmp .perilo_rd
			@@:
				mov eax,mask_chi_cr_r
				call CountLastInc
		.perilo_rd:
	.no_fail:

	and dword[pos_eggs],mask_clear_all ;���⪠ 㯠��� �� � ��������� ��३

	call rand_next
	cmp byte[zaac_status],0
	jle @f
		dec byte[zaac_status]
		jmp .no_zaac_move ;���� ���� �� ���������
	@@:
	
	bt dword[rand_x],6 ;���� �� 䮭��� ����� �����
	jc @f
		xor byte[pos_wolf],val_zaac ;��ᮢ뢠��/��ᮢ뢠�� ����
		bt word[pos_wolf],val_zaac
		jc .zaac_n
			mov byte[zaac_status],val_zaac_time_y ;�⠢�� �������쭮� �६� ��� ᬥ�� �����
			jmp @f
		.zaac_n:
			mov byte[zaac_status],val_zaac_time_n ;�⠢�� �������쭮� �६� ��� ᬥ�� �����
	@@:
	.no_zaac_move:

	;ᮧ����� ����� ��
	bt dword[rand_x],4 ;�஢��塞 �㤥� �� ᮧ������ ����� ��
	jc .end_creat
	bt dword[rand_x],5 ;�஢��塞 � ����� ��஭� �㤥� ᮧ������ ����� ��
	jc .creat_r
		bt dword[rand_x],7
		jc @f
			or dword[pos_eggs],mask_lot_lu
			jmp .end_creat
		@@:
			or dword[pos_eggs],mask_lot_ld
			jmp .end_creat
	.creat_r:
		bt dword[rand_x],7
		jc @f
			or dword[pos_eggs],mask_lot_ru
			jmp .end_creat
		@@:
			or dword[pos_eggs],mask_lot_rd
			;jmp .end_creat
	.end_creat:

	.end_fun:
	popad
	ret

align 4
proc InitGame, b:dword ;��ࢮ��砫�� ����ன�� ����
	mov word[eggs_count],0 ;�������⢮ �������� ��
	mov byte[some_text],'0'
	mov byte[some_text+1],0 ;⥪�� � �᫮� ��������� ��
	mov byte[count_last],0
	mov dword[pos_eggs],0
	mov byte[zaac_status],0

	cmp dword[b],0
	jne @f
		mov byte[game_text+5],'�'
		mov dword[game_spd],65 ;����প� ����
		jmp .end_init
	@@:
		mov byte[game_text+5],'�'
		mov dword[game_spd],35 ;����প� ����
	.end_init:

	push eax ebx
		mcall 26,9
		mov dword[rand_x],eax ;������塞 1-� ��砩��� �᫮
	pop ebx eax

	ret
endp

align 4
proc LoadArrayBuffer, f_name:dword, buf_start:dword, count:dword
	pushad
	mov edx,dword[displ_bytes]
	mov ecx,edx
	imul ecx,dword[count]
	mov eax,dword[f_name]
	load_image_file eax,image_data_gray,ecx
		mov edx,dword[displ_bytes]
		mov eax,[image_data_gray]
		mov edi,dword[buf_start]
		mov ecx,dword[count]
		cld
		@@: ;���뢠�� 3 ���� � ������ﬨ
			stdcall CreateTrapharetBuffer,edi,eax
			add eax,edx
			add edi,BUF_STRUCT_SIZE
			loop @b
	stdcall mem.Free,[image_data_gray] ;�᢮������� ������
	popad
	ret
endp

align 4
start:
	load_libraries l_libs_start,load_lib_end

	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ��� ����
	mov	ebp,lib0
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:
	mov	ebp,lib1
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:

	mcall 40,0x27
	mcall 48,3,sc,sizeof.system_colors ;����砥� ��⥬�� 梥�

	;ࠡ�� � 䠩��� ����஥�
	copy_path ini_name,sys_path,file_name,0x0
	stdcall dword[ini_get_int],file_name,ini_sec_files,key_displ_w,210
	mov	dword[displ_w],eax
	stdcall dword[ini_get_int],file_name,ini_sec_files,key_displ_h,140
	mov	dword[displ_h],eax
	stdcall dword[ini_get_color],file_name,ini_sec_color,key_color_unit,0
	mov	dword[color_wolf],eax

	mov edx,dword[displ_w]
	imul edx,dword[displ_h]
	lea edx,[edx+edx*2]
	mov dword[displ_bytes],edx ;����塞 ࠧ��� ��஢��� ����

	stdcall LoadArrayBuffer, fn_icon0, buf_decor,3 ;���뢠�� 3 ���� � ������ﬨ
	stdcall LoadArrayBuffer, fn_icon1, buf_wolf,9 ;���뢠�� 9 ���஢ � ������ � ���楬
	stdcall LoadArrayBuffer, fn_icon2, buf_egg,22 ;���뢠�� 22 ���஢ � �栬�
	stdcall LoadArrayBuffer, fn_icon3, buf_chi,13 ;���뢠�� 13 ���஢ � 樯��⠬�

	load_image_file fn_font, image_data_gray,IMAGE_FONT_SIZE
	stdcall [buf2d_create_f_img], buf_font,[image_data_gray] ;ᮧ���� ����
	stdcall mem.Free,[image_data_gray] ;�᢮������� ������

	stdcall [buf2d_conv_24_to_8], buf_font,1 ;������ ���� �஧�筮�� 8 ���
	stdcall [buf2d_convert_text_matrix], buf_font

;�஢�ઠ ����������� ���஬ ����
;mov edi,buf_wolf
;add edi,BUF_STRUCT_SIZE ;���室�� �� ���� ��২��
;stdcall [buf2d_clear],edi,0x808080 ;�������� ��� ��� 梥⮬

	mov ebx,dword[displ_w]
	mov edx,dword[displ_h]

	mov edi,buf_displ
	mov buf2d_w,ebx
	mov buf2d_h,edx
	stdcall [buf2d_create], buf_displ ;ᮧ���� ���� ��� �뢮�� �� �࠭

	mov edi,buf_fon
	mov buf2d_w,ebx
	mov buf2d_h,edx
	stdcall [buf2d_create], buf_fon ;ᮧ���� ���� � 䮭��묨 ������ﬨ

	call InitBackgroundBuffer ;������塞 ���� � 䮭��묨 ������ﬨ
	stdcall InitGame,0
	mcall 26,9
	mov [last_time],ebx



align 4
red_win:
	call draw_window

align 4
still: ;������ 横�
	mcall 26,9
	mov ebx,[last_time]
	add ebx,dword[game_spd] ;delay
	sub ebx,eax
	cmp ebx,dword[game_spd] ;delay
	ja it_is_time_now
	test ebx,ebx
	jz it_is_time_now
	mcall 23

	cmp eax,0
	je it_is_time_now

	;mcall 10

	cmp al,0x1 ;���������� ��������� ����
	jz red_win
	cmp al,0x2
	jz key
	cmp al,0x3
	jz button

	jmp still

align 4
it_is_time_now:
	mcall 26,9
	mov [last_time],eax

	;...����� ���� ����⢨�, ��뢠��� ����� delay ���� ����� ᥪ㭤...
	call MoveEggs
	call draw_display
	jmp still

align 4
key:
	push eax ebx
	mcall 2

	cmp ah,176 ;Left
	jne @f
		and byte[pos_wolf],0xfe
	@@:
	cmp ah,179 ;Right
	jne @f
		or byte[pos_wolf],1
	@@:
	cmp ah,178 ;Up
	jne @f
		and byte[pos_wolf],0xff-2
	@@:
	cmp ah,177 ;Down
	jne @f
		or byte[pos_wolf],2
	@@:

	cmp ah,97 ;a
	jne @f
		and byte[pos_wolf],0xff-3 ;00
	@@:
	cmp ah,122 ;z
	jne @f
		and byte[pos_wolf],0xff-1
		or byte[pos_wolf],2 ;10
	@@:
	cmp ah,39 ;'
	jne @f
		and byte[pos_wolf],0xff-2
		or byte[pos_wolf],1 ;01
	@@:
	cmp ah,47 ;/
	jne @f
		or byte[pos_wolf],3 ;11
	@@:

	pop ebx eax
	jmp still


align 4
draw_window:
	pushad
	mcall 12,1

	xor eax,eax
	mov ebx,20*65536+480
	mov ecx,20*65536+270
	mov edx,[sc.work]
	;or edx,0x33000000
	or edx,0x73000000
	mov edi,hed
	mcall ;ᮧ����� ����

	mcall 9,procinfo,-1
	mov edi,buf_displ
	mov eax,dword[procinfo.client_box.width]
	cmp eax,dword[displ_w]
	jle @f
		sub eax,dword[displ_w]
		shr eax,1
		mov buf2d_l,ax ;��ࠢ������ ���� �� 業��� ����
	@@:

	call draw_display

	mov eax,13 ;�ᮢ���� ��אַ㣮�쭨��
	mov edx,[sc.work]
	xor esi,esi
	mov si,buf2d_l
	add esi,dword[displ_w]
	mov ebx,dword[procinfo.client_box.width]
	inc ebx
	cmp esi,ebx
	jge @f
		sub ebx,esi
		rol ebx,16
		mov bx,si
		rol ebx,16
		mov ecx,dword[procinfo.client_box.height]
		inc ecx
		int 0x40 ;�ᮢ���� �ࠢ��� �������� ����
		jmp .draw_s
	@@:
		mov esi,dword[procinfo.client_box.width] ;����� �� �ਭ� �� ������
		inc esi
	.draw_s:

if use_but eq 1
	; *** �ᮢ���� ������ ***
push esi
	mov eax,8
	xor ebx,ebx
	mov bx,buf2d_l
	add ebx,buf2d_w
	add ebx,BUT1_L
	shl ebx,16
	mov bx,BUT1_W
	mov ecx,BUT1_T*65536+BUT1_H
	mov edx,5
	;or edx,0x40000000
	mov esi,dword[color_but_sm]
	int 0x40

	inc edx
	add ecx,BUT1_NEXT_TOP
	int 0x40
pop esi

	mov eax,4 ;��R��-�? �?��
	mov bx,BUT1_H
	add ebx,3*65536;+3
	mov ecx,dword[color_but_te]
	or  ecx,0x80000000
	mov edx,txt_game_a
	int 0x40

	ror ebx,16
	add ebx,BUT1_NEXT_TOP
	ror ebx,16
	mov edx,txt_game_b
	int 0x40

	; *** ����⠭������� ��ࠬ��஢ ***
	mov eax,13 ;�ᮢ���� ��אַ㣮�쭨��
	mov edx,[sc.work]
end if

	mov ebx,esi
	mov ecx,dword[procinfo.client_box.height]
	inc ecx
	mov esi,dword[displ_h]
	cmp esi,ebx
	jge @f
		sub ecx,esi
		rol ecx,16
		mov cx,si
		rol ecx,16
		int 0x40 ;�ᮢ���� ������� ����
	@@:
	
	xor ebx,ebx
	mov bx,buf2d_l
	mov ecx,dword[displ_h]
	int 0x40 ;�ᮢ���� ������ �������� ����

	mcall 12,2
	popad
	ret

align 4
draw_display:

	stdcall mem_copy, dword[buf_fon],dword[buf_displ],315*210*3;dword[displ_bytes] ;����஢���� ����ࠦ���� �� 䮭����� ����
	call DrawZaac ;��㥬 ����
	call DrawWolf ;��㥬 �����
	call DrawEggs ;��㥬 ��

push eax
	mov eax,dword[displ_w]
	shr eax,1
	stdcall [buf2d_draw_text], buf_displ, buf_font,game_text,eax,OFFS_SHADOW_X,[color_curici] ;��㥬 ��ப� � ⥪�⮬
pop eax
	stdcall [buf2d_draw], buf_displ
	ret

align 4
button:
	mcall 17 ;������� ��� ����⮩ ������
	if use_but eq 1
	cmp ah,5
	jne @f
		stdcall InitGame,0
	@@:
	cmp ah,6
	jne @f
		stdcall InitGame,1
	@@:	end if
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_fon ;㤠�塞 ����
	stdcall [buf2d_delete],buf_displ ;㤠�塞 ����

	stdcall [buf2d_delete],buf_font

	cld
	mov ecx,3
	mov edi,buf_decor
	@@: ;㤠�塞 3 ����
		stdcall [buf2d_delete],edi
		add edi,BUF_STRUCT_SIZE
		loop @b
	mov ecx,9
	mov edi,buf_wolf
	@@: ;㤠�塞 9 ���஢ � ������ � ���楬
		stdcall [buf2d_delete],edi
		add edi,BUF_STRUCT_SIZE
		loop @b
	mov ecx,13
	mov edi,buf_chi
	@@: ;㤠�塞 13 ���஢
		stdcall [buf2d_delete],edi
		add edi,BUF_STRUCT_SIZE
		loop @b
	mov ecx,22
	mov edi,buf_egg
	@@: ;㤠�塞 22 ����
		stdcall [buf2d_delete],edi
		add edi,BUF_STRUCT_SIZE
		loop @b

	mcall -1 ;��室 �� �ணࠬ��

head_f_i:
head_f_l  db '���⥬��� �訡��',0

system_dir0 db '/sys/lib/'
name_buf2d db 'buf2d.obj',0
err_message_found_lib0 db '�� 㤠���� ���� ������⥪� buf2d.obj',0
err_message_import0 db '�訡�� �� ������ ������⥪� buf2d.obj',0

system_dir1 db '/sys/lib/'
name_libimg db 'libimg.obj',0
err_message_found_lib1 db '�� 㤠���� ���� ������⥪� libimg.obj',0
err_message_import1 db '�訡�� �� ������ ������⥪� libimg.obj',0

system_dir2 db '/sys/lib/'
libini_name db 'libini.obj',0
err_message_found_lib2 db '�� 㤠���� ���� ������⥪� libini.obj',0
err_message_import2 db '�訡�� �� ������ ������⥪� libini.obj',0

;library structures
l_libs_start:
	lib0 l_libs name_buf2d,  sys_path, file_name, system_dir0, err_message_found_lib0, head_f_l, import_buf2d_lib, err_message_import0, head_f_i
	lib1 l_libs name_libimg, sys_path, file_name, system_dir1, err_message_found_lib1, head_f_l, import_libimg, err_message_import1, head_f_i
	lib2 l_libs libini_name, sys_path, file_name, system_dir2, err_message_found_lib2, head_f_l, libini_import, err_message_import2, head_f_i
load_lib_end:

align 4
proc mem_copy, source:dword, destination:dword, len:dword
  push ecx esi edi
    cld
    mov esi, dword[source]
    mov edi, dword[destination]
    mov ecx, dword[len]
    rep movsb
  pop edi esi ecx
  ret
endp

align 4
convert_to_str:
	pushad
	;mov eax,dword[value]
	;mov edi,dword[text]
	mov dword[edi+1],0
	cld
	call .str
	popad
	ret


align 4
.str:
  mov ecx,0x0a ;�������� ��⥬� ��᫥��� ���������� ॣ����� ebx,eax,ecx,edx �室�� ��ࠬ���� eax - �᫮
    ;��ॢ�� �᫠ � ASCII ��ப� ������ ����� ecx=��⥬� ��᫥�� edi ���� �㤠 �����뢠��, �㤥� ��ப�, ��祬 ����� ��६����� 
  cmp eax,ecx  ;�ࠢ���� �᫨ � eax ����� 祬 � ecx � ��३� �� @@-1 �.�. �� pop eax
  jb @f
  xor edx,edx  ;������ edx
  div ecx      ;ࠧ������ - ���⮪ � edx
  push edx     ;�������� � �⥪
  ;dec edi             ;ᬥ饭�� ����室���� ��� ����� � ���� ��ப�
  call .str;��३� �� ᠬ� ᥡ� �.�. �맢��� ᠬ� ᥡ� � ⠪ �� ⮣� ������ ���� � eax �� �⠭�� ����� 祬 � ecx
  pop eax
  @@: ;cmp al,10 ;�஢���� �� ����� �� ���祭�� � al 祬 10 (��� ��⥬� ��᫥�� 10 ������ ������� - ��譠�))
  ;sbb al,$69  ;- ��⭮ ������ �������� ���� ���⠢��� ���㬠���� �.�. � �� ���� ��� �� ࠡ�⠥�
  ;das        ;��᫥ ������ ������� ��� �� �ந�室�� 㬥��襭�� al �� 66h  (� ����� ����ᠭ� ��㣮�)
  or al,0x30  ;������ ������� ����  祬 ��� ��� 
  stosb       ;������� ����� �� ॣ���� al � �祪� ����� es:edi
  ret	      ;�������� 祭� ������ 室 �.�. ���� � �⥪� �࠭����� ���-�� �맮��� � �⮫쪮 ࠧ �� � �㤥� ��뢠����


last_time dd ?
image_data dd 0 ;������ ��� �८�ࠧ������ ���⨭�� �㭪�ﬨ libimg
image_data_gray dd 0 ;������ � �६���묨 ��묨 ����ࠦ���ﬨ � �ଠ� 24-bit, �� ������ ���� ᮧ�������� ������

run_file_70 FileInfoBlock
hed db 'Nu pogodi 03.08.10',0 ;������� ����
sc system_colors  ;��⥬�� 梥�

align 4
buf_font: ;���� � ���⮬
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 25 ;+4 left
	dw 25 ;+6 top
	dd 128 ;+8 w
	dd 144 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_displ:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 25,0
	dd ? ;+8 w
	dd ? ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_fon: ;䮭��� ����
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 0 ;+4 left
	dw 0 ;+6 top
	dd ? ;+8 w
	dd ? ;+12 h
	dd 0xffffff ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_decor: ;���� � ������ﬨ: �������� � ३����; � ���栬�; � ��⥭�ﬨ
	rb 3*BUF_STRUCT_SIZE

align 4
buf_wolf:
	rb 9*BUF_STRUCT_SIZE

align 4
buf_egg:
	rb 22*BUF_STRUCT_SIZE

align 4
buf_chi:
	rb 13*BUF_STRUCT_SIZE



align 4
libini_import:
	dd alib_init0
	ini_get_str   dd aini_get_str
	ini_get_int   dd aini_get_int
	ini_get_color dd aini_get_color
dd 0,0
	alib_init0     db 'lib_init',0
	aini_get_str   db 'ini_get_str',0
	aini_get_int   db 'ini_get_int',0
	aini_get_color db 'ini_get_color',0

align 4
import_libimg:
  dd alib_init1
  img_is_img  dd aimg_is_img
  img_info    dd aimg_info
  img_from_file dd aimg_from_file
  img_to_file dd aimg_to_file
  img_from_rgb dd aimg_from_rgb
  img_to_rgb  dd aimg_to_rgb
  img_to_rgb2 dd aimg_to_rgb2
  img_decode  dd aimg_decode
  img_encode  dd aimg_encode
  img_create  dd aimg_create
  img_destroy dd aimg_destroy
  img_destroy_layer dd aimg_destroy_layer
  img_count   dd aimg_count
  img_lock_bits dd aimg_lock_bits
  img_unlock_bits dd aimg_unlock_bits
  img_flip    dd aimg_flip
  img_flip_layer dd aimg_flip_layer
  img_rotate  dd aimg_rotate
  img_rotate_layer dd aimg_rotate_layer
  img_draw    dd aimg_draw

  dd 0,0
  alib_init1   db 'lib_init',0
  aimg_is_img  db 'img_is_img',0 ;��।���� �� �����, ����� �� ������⥪� ᤥ���� �� ��� ����ࠦ����
  aimg_info    db 'img_info',0
  aimg_from_file db 'img_from_file',0
  aimg_to_file db 'img_to_file',0
  aimg_from_rgb db 'img_from_rgb',0
  aimg_to_rgb  db 'img_to_rgb',0 ;�८�ࠧ������ ����ࠦ���� � ����� RGB
  aimg_to_rgb2 db 'img_to_rgb2',0
  aimg_decode  db 'img_decode',0 ;��⮬���᪨ ��।���� �ଠ� ����᪨� ������
  aimg_encode  db 'img_encode',0
  aimg_create  db 'img_create',0
  aimg_destroy db 'img_destroy',0
  aimg_destroy_layer db 'img_destroy_layer',0
  aimg_count   db 'img_count',0
  aimg_lock_bits db 'img_lock_bits',0
  aimg_unlock_bits db 'img_unlock_bits',0
  aimg_flip    db 'img_flip',0
  aimg_flip_layer db 'img_flip_layer',0
  aimg_rotate  db 'img_rotate',0
  aimg_rotate_layer db 'img_rotate_layer',0
  aimg_draw    db 'img_draw',0

align 4
import_buf2d_lib:
	dd sz_lib_init
	buf2d_create dd sz_buf2d_create
	buf2d_create_f_img dd sz_buf2d_create_f_img
	buf2d_clear dd sz_buf2d_clear
	buf2d_draw dd sz_buf2d_draw
	buf2d_delete dd sz_buf2d_delete
	buf2d_line dd sz_buf2d_line
	buf2d_circle dd sz_buf2d_circle
	buf2d_img_hdiv2 dd sz_buf2d_img_hdiv2
	buf2d_img_wdiv2 dd sz_buf2d_img_wdiv2
	buf2d_conv_24_to_8 dd sz_buf2d_conv_24_to_8
	buf2d_conv_24_to_32 dd sz_buf2d_conv_24_to_32
	buf2d_bit_blt dd sz_buf2d_bit_blt
	buf2d_bit_blt_transp dd sz_buf2d_bit_blt_transp
	buf2d_bit_blt_alpha dd sz_buf2d_bit_blt_alpha
	buf2d_cruve_bezier dd sz_buf2d_cruve_bezier
	buf2d_convert_text_matrix dd sz_buf2d_convert_text_matrix
	buf2d_draw_text dd sz_buf2d_draw_text
	buf2d_crop_color dd sz_buf2d_crop_color
	buf2d_offset_h dd sz_buf2d_offset_h	
	dd 0,0
	sz_lib_init db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_line db 'buf2d_line',0
	sz_buf2d_circle db 'buf2d_circle',0 ;�ᮢ���� ���㦭���
	sz_buf2d_img_hdiv2 db 'buf2d_img_hdiv2',0
	sz_buf2d_img_wdiv2 db 'buf2d_img_wdiv2',0
	sz_buf2d_conv_24_to_8 db 'buf2d_conv_24_to_8',0
	sz_buf2d_conv_24_to_32 db 'buf2d_conv_24_to_32',0
	sz_buf2d_bit_blt db 'buf2d_bit_blt',0
	sz_buf2d_bit_blt_transp db 'buf2d_bit_blt_transp',0
	sz_buf2d_bit_blt_alpha db 'buf2d_bit_blt_alpha',0
	sz_buf2d_cruve_bezier db 'buf2d_cruve_bezier',0
	sz_buf2d_convert_text_matrix db 'buf2d_convert_text_matrix',0
	sz_buf2d_draw_text db 'buf2d_draw_text',0
	sz_buf2d_crop_color db 'buf2d_crop_color',0
	sz_buf2d_offset_h db 'buf2d_offset_h',0

i_end:
	rb 1024
	align 16
	procinfo process_information
stacktop:
	sys_path rb 4096
	file_name:
		rb 4096
	plugin_path:
		rb 4096
	openfile_path:
		rb 4096
	filename_area:
		rb 256
mem: