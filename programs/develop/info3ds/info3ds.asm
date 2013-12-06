use32
	org 0x0
	db 'MENUET01' ;������. �ᯮ��塞��� 䠩�� �ᥣ�� 8 ����
	dd 0x1
	dd start
	dd i_end ;ࠧ��� �ਫ������
	dd mem
	dd stacktop
	dd 0
	dd sys_path

include '../../macros.inc'
include '../../proc32.inc'
include '../../develop/libraries/box_lib/load_lib.mac'
include '../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../dll.inc'
include 'lang.inc'
include 'info_fun_float.inc'

debug equ 0

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load

struct FileInfoBlock
	Function dd ?
	Position dd ?
	Flags	 dd ?
	Count	 dd ?
	Buffer	 dd ?
		db ?
	FileName dd ?
ends

run_file_70 FileInfoBlock
image_data dd 0 ;㪠��⥫� �� �६����� ������. ��� �㦥� �८�ࠧ������ ����ࠦ����
open_file_lif dd 0 ;㪠��⥫� �� ������ ��� ������ 䠩��� 3ds
open_file_size dd 0 ;ࠧ��� ����⮣� 䠩��

;
fn_toolbar db 'toolbar.png',0
IMAGE_TOOLBAR_ICON_SIZE equ 16*16*3
IMAGE_TOOLBAR_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*7
image_data_toolbar dd 0
;
TREE_ICON_SYS16_BMP_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*11+54 ;ࠧ��� bmp 䠩�� � ��⥬�묨 ��������
icon_tl_sys dd 0 ;㪠��⥥�� �� ������ ��� �࠭���� ��⥬��� ������
icon_toolbar dd 0 ;㪠��⥥�� �� ������ ��� �࠭���� ������ ��ꥪ⮢
TOOLBAR_ICON_BMP_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*8+54 ;ࠧ��� bmp 䠩�� � �������� ��ꥪ⮢
;
IMAGE_FILE1_SIZE equ 128*144*3+54 ;ࠧ��� 䠩�� � ����ࠦ�����

macro load_image_file path,buf,size { ;����� ��� ����㧪� ����ࠦ����
	;path - ����� ���� ��६����� ��� ��ப��� ��ࠬ��஬
	if path eqtype '' ;�஢��塞 ����� �� ��ப�� ��ࠬ��� path
		jmp @f
			local .path_str
			.path_str db path ;�ନ�㥬 �������� ��६�����
			db 0
		@@:
		;32 - �⠭����� ���� �� ���஬� ������ ���� ���� � ��⥬�� ��⥬
		copy_path .path_str,[32],file_name,0x0
	else
		copy_path path,[32],file_name,0x0 ;�ନ�㥬 ����� ���� � 䠩�� ����ࠦ����, ���ࠧ㬥���� �� �� � ����� ����� � �ணࠬ���
	end if

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

;--------------------------------------
include 'data.inc'

level_stack dd 0
offs_last_timer dd 0 ;��᫥���� ᤢ�� �������� � �㭪樨 ⠩���

ID_ICON_CHUNK_MAIN equ 0 ;������ �������� �����
ID_ICON_CHUNK_NOT_FOUND equ 1 ;������ �� �����⭮�� �����
ID_ICON_DATA equ 2 ;������ ��� ������ �����, �� ��।������� ��������

FILE_ERROR_CHUNK_SIZE equ -3 ;�訡�� � ࠧ��� �����

align 4
file_3ds: ;��६���� �ᯮ��㥬� �� ����⨨ 䠩��
.offs: dd 0 ;+0 㪠��⥫� �� ��砫� �����
.size: dd 0 ;+4 ࠧ��� ����� (��� 1-�� ��ࠬ��� = ࠧ��� 䠩�� 3ds)
rb 8*MAX_FILE_LEVEL

size_one_list equ 42
list_offs_chunk_del equ 8 ;����� �� ���� 㤠�����
list_offs_chunk_lev equ 9 ;�஢��� ����� (�ய�ᠭ � ����� 㧫�)
list_offs_p_data equ 10 ;㪠��⥫� �� ������� �����
list_offs_text equ 14 ;ᤢ�� ��砫� ⥪�� � ����
buffer rb size_one_list ;���� ��� ���������� ������� � ᯨ᮪ tree1

txt_3ds_symb db 0,0
;--------------------------------------

include 'info_wnd_coords.inc'

align 4
start:
	load_libraries l_libs_start,l_libs_end
	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ��� ����
	mov	ebp,lib_7
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:
	mcall 48,3,sc,sizeof.system_colors
	mcall 40,0x27
	stdcall [OpenDialog_Init],OpenDialog_data ;�����⮢�� �������

	stdcall dword[tl_data_init], tree1
	;��⥬�� ������ 16*16 ��� tree_list
	load_image_file 'tl_sys_16.png', icon_tl_sys,TREE_ICON_SYS16_BMP_SIZE
	;�᫨ ����ࠦ���� �� ���뫮��, � � icon_tl_sys ����
	;�� ���樠����஢���� �����, �� �訡�� �� �㤥�, �. �. ���� �㦭��� ࠧ���
	mov eax,dword[icon_tl_sys]
	mov dword[tree1.data_img_sys],eax

	load_image_file 'objects.png', icon_toolbar,TOOLBAR_ICON_BMP_SIZE
	mov eax,dword[icon_toolbar]
	mov dword[tree1.data_img],eax

	stdcall [buf2d_create], buf_0 ;ᮧ����� ����

	load_image_file 'font8x9.bmp', image_data_toolbar,IMAGE_FILE1_SIZE
	stdcall [buf2d_create_f_img], buf_1,[image_data_toolbar] ;ᮧ���� ����
	stdcall mem.Free,[image_data_toolbar] ;�᢮������� ������
	stdcall [buf2d_conv_24_to_8], buf_1,1 ;������ ���� �஧�筮�� 8 ���
	stdcall [buf2d_convert_text_matrix], buf_1

	load_image_file fn_toolbar, image_data_toolbar,IMAGE_TOOLBAR_SIZE

	stdcall mem.Alloc,MAX_FILE_SIZE
	mov dword[open_file_lif],eax

	mcall 26,9
	mov [last_time],eax

align 4
red_win:
	call draw_window

align 4
still:
	mcall 26,9
	mov ebx,[last_time]
	add ebx,10 ;����প�
	cmp ebx,eax
	jge @f
		mov ebx,eax
	@@:
	sub ebx,eax
	;cmp ebx,10 ;����প�
	;ja timer_funct
	;test ebx,ebx
	;jz timer_funct
	mcall 23
	cmp eax,0
	je timer_funct

	cmp al,1
	jz red_win
	cmp al,2
	jz key
	cmp al,3
	jz button
	cmp al,6
	jne @f
		mcall 9,procinfo,-1
		cmp ax,word[procinfo+4]
		jne @f ;���� �� ��⨢��
		call mouse
	@@:
	jmp still

align 4
mouse:
	stdcall [tl_mouse], dword tree1
	ret

align 4
timer_funct:
	pushad
if debug
	mov eax,4
	mov ebx,(5 shl 16)+8
	mov ecx,0xff+0x80000000
	mov edx,txt_0002
	int 0x40
end if
	mcall 26,9
	mov [last_time],eax

	;
	stdcall [tl_node_get_data],tree1
	pop ebx
	cmp ebx,0
	je @f
		mov eax,dword[ebx] ;����砥� ���祭�� ᤢ��� ��࠭���� ����� �⭮�⥫쭮 ��砫� 䠩��
		mov ecx,dword[ebx+4] ;ࠧ��� �����
		stdcall hex_in_str, txt_3ds_offs.dig, eax,8
		stdcall hex_in_str, txt_3ds_offs.siz, ecx,8

		add eax,dword[open_file_lif] ;����砥� ���祭�� ᤢ��� � �����
		cmp dword[offs_last_timer],eax
		je @f
			mov dword[offs_last_timer],eax
			call buf_draw_beg
			stdcall [buf2d_draw_text], buf_0, buf_1,txt_3ds_offs,5,35,0xb000
			mov edx,dword[ebx+list_offs_p_data]
			cmp edx,0 ;ᬮ�ਬ ���� �� ���ᠭ�� �����
			je .no_info
				stdcall [buf2d_draw_text], buf_0, buf_1,edx,5,45,0xb000
			.no_info:
			add ecx,eax ;����砥� ࠧ��� �����
			stdcall buf_draw_hex_table,eax,ecx ;���������� 16-���� ������
			stdcall [buf2d_draw], buf_0 ;������塞 ���� �� �࠭�
	@@:
	popad
	jmp still

align 4
buf_draw_beg:
	stdcall [buf2d_clear], buf_0, [buf_0.color] ;��⨬ ����
	stdcall [buf2d_draw_text], buf_0, buf_1,txt_open_3ds,5,5,0xff
	stdcall [buf2d_draw_text], buf_0, buf_1,openfile_path,5,15,0xff
	cmp dword[level_stack],FILE_ERROR_CHUNK_SIZE ;�������� �訡�� 䠩��
	jne @f
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_3ds_err_sizes,5,25,0xff0000
	@@:
	ret

align 4
proc buf_draw_hex_table, offs:dword, size_line:dword
	pushad
	locals
		coord_y dd 55 ;���न��� y ��� ��砫� �뢮�� ⠡����
	endl
		mov esi,dword[offs]
		mov edi,dword[open_file_lif]
		add edi,dword[file_3ds.size] ;edi - 㪠��⥫� �� ����� 䠩�� � �����
		mov dword[txt_3ds_offs.dig],0
		cld
		.cycle_rows:
			mov ebx,5 ;����� ᫥�� ��� ���
			mov edx,5+10*24 ;����� ᫥�� ��� ⥪��
			mov ecx,10
			@@:
				stdcall hex_in_str, txt_3ds_offs.dig, dword[esi],2
				stdcall [buf2d_draw_text], buf_0, buf_1,txt_3ds_offs.dig,ebx,[coord_y],0

				mov al,byte[esi]
				mov byte[txt_3ds_symb],al
				stdcall [buf2d_draw_text], buf_0, buf_1,txt_3ds_symb,edx,[coord_y],0x808080
				inc esi
				cmp esi,dword[size_line]
				jne .end_block
					stdcall draw_block_end_line, dword[coord_y]
				.end_block:
				cmp esi,edi
				jge @f ;jg ???
				add ebx,24
				add edx,9 ;�ਭ� 1-�� ᨬ���� +1pix
				loop @b
			add dword[coord_y],10 ;���� 1-�� ᨬ���� (��� ���ࢠ� ����� ��ப���)
			mov ebx,dword[buf_0.h]
			cmp dword[coord_y],ebx
			jl .cycle_rows
		@@:
	popad
	ret
endp

align 4
proc draw_block_end_line, coord_y:dword
	push eax ebx ecx
		add ebx,20 ;20 = width 2.5 symbols
		mov eax,[coord_y]
		sub eax,2
		mov ecx,eax
		add ecx,10
		stdcall [buf2d_line], buf_0, 0,ecx,ebx,ecx ,0xff
		stdcall [buf2d_line], buf_0, ebx,ecx,ebx,eax ,0xff
		stdcall [buf2d_line], buf_0, ebx,eax,5+10*24-4,eax ,0xff
	pop ecx ebx eax
	ret
endp

align 4
draw_window:
pushad
	mcall 12,1
	xor eax,eax
	mov ebx,(20 shl 16)+560
	mov ecx,(20 shl 16)+315
	mov edx,[sc.work]
	or  edx,(3 shl 24)+0x10000000+0x20000000
	mov edi,capt
	int 0x40

	mcall 9,procinfo,-1
	mov eax,dword[procinfo.box.height]
	cmp eax,250
	jge @f
		mov eax,250
	@@:
	sub eax,65
	mov dword[tree1.box_height],eax
	mov word[w_scr_t1+4],ax ;���� ࠧ���� �஫�����
	cmp eax,dword[buf_0.h] ;㢥��稢��� ����� ����
	jle @f
		stdcall [buf2d_resize],buf_0,0,eax
		mov dword[offs_last_timer],0 ;��� ���������� ���� � ⠩���
	@@:

	mov eax,dword[procinfo.box.width]
	cmp eax,400
	jge @f
		mov eax,400
	@@:
	sub eax,[buf_0.w]
	sub eax,41
	mov dword[tree1.box_width],eax
	add ax,word[tree1.box_left]
	mov word[w_scr_t1.start_x],ax
	add ax,16+5
	mov word[buf_0.l],ax

	mov eax,8
	mov ebx,(5 shl 16)+20
	mov ecx,(5 shl 16)+20
	mov edx,3
	mov esi,[sc.work_button]
	int 0x40

	mov ebx,(30 shl 16)+20
	mov edx,4
	int 0x40

	cmp byte[can_save],0
	je @f
		mov ebx,(55 shl 16)+20
		mov edx,5
		int 0x40
	@@:

	mov ebx,(85 shl 16)+20
	mov edx,6 ;���� � ���न��⠬�
	int 0x40

	mov ebx,(110 shl 16)+20
	mov edx,7 ;㤠����� �����
	int 0x40

	mov eax,7
	mov ebx,[image_data_toolbar]
	mov ecx,(16 shl 16)+16
	mov edx,(7 shl 16)+7 ;new
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	mov edx,(32 shl 16)+7 ;open
	int 0x40

	cmp byte[can_save],0
	je @f
		add ebx,IMAGE_TOOLBAR_ICON_SIZE
		mov edx,(57 shl 16)+7 ;save
		int 0x40
		sub ebx,IMAGE_TOOLBAR_ICON_SIZE
	@@:

	add ebx,4*IMAGE_TOOLBAR_ICON_SIZE
	mov edx,(87 shl 16)+7
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	mov edx,(112 shl 16)+7
	int 0x40

	mov dword[w_scr_t1.all_redraw],1
	stdcall [tl_draw],dword tree1

	stdcall [buf2d_draw], buf_0

	mcall 12,2
popad
	ret

align 4
key:
	mcall 2
	stdcall [tl_key], dword tree1
	jmp still


align 4
button:
	mcall 17
	cmp ah,3
	jne @f
		call but_new_file
	@@:
	cmp ah,4
	jne @f
		call but_open_file
	@@:
	cmp ah,5
	jne @f
		call but_save_file
	@@:
	cmp ah,6
	jne @f
		call but_wnd_coords
	@@:
	cmp ah,7
	jne @f
		call but_delete_chunk
	@@:

	cmp ah,1
	jne still
.exit:
	mov dword[tree1.data_img],0
	mov dword[tree1.data_img_sys],0
	stdcall dword[tl_data_clear], tree1
	stdcall [buf2d_delete],buf_0
	stdcall [buf2d_delete],buf_1 ;㤠�塞 ����
	stdcall mem.Free,[image_data_toolbar]
	stdcall mem.Free,[open_file_lif]
	mcall -1


align 4
but_new_file:
	mov byte[can_save],0
	stdcall [tl_info_clear], tree1 ;���⪠ ᯨ᪠ ��ꥪ⮢
	stdcall [buf2d_clear], buf_0, [buf_0.color] ;��⨬ ����
	stdcall [tl_draw], tree1
	stdcall [buf2d_draw], buf_0 ;������塞 ���� �� �࠭�
	ret

align 4
but_open_file:
	pushad
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],0
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],2
	je .end_open_file
	;��� �� 㤠筮� ����⨨ �������

	mov eax,70 ;70-� �㭪�� ࠡ�� � 䠩����
	mov [run_file_70.Function], 0
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov dword[run_file_70.Count], MAX_FILE_SIZE
	m2m [run_file_70.Buffer], dword[open_file_lif]
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mov ebx,run_file_70
	int 0x40 ;����㦠�� 䠩� ����ࠦ����
	cmp ebx,0xffffffff
	je .end_open_file

	mov [open_file_size],ebx
	;mcall 71,1,openfile_path

	mov byte[can_save],0
	call init_tree
	stdcall [buf2d_draw], buf_0 ;������塞 ���� �� �࠭�

	.end_open_file:
	popad
	ret

align 4
init_tree:
	stdcall [tl_info_clear], tree1 ;���⪠ ᯨ᪠ ��ꥪ⮢

	mov esi,dword[open_file_lif]
	cmp word[esi],CHUNK_MAIN
	je @f
		call buf_draw_beg
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_no_3ds,5,25,0xff0000 ;��㥬 ��ப� � ⥪�⮬
		jmp .end_open
	@@:
	cmp dword[esi+2],MAX_FILE_SIZE
	jle @f
		call buf_draw_beg
		stdcall [buf2d_draw_text], buf_0, buf_1,txt_3ds_big_file,5,25,0xff0000 ;��㥬 ��ப� � ⥪�⮬
		jmp .end_open
	@@:
	;--- ��ࠡ�⪠ ����⮣� *.3ds 䠩��
	mov eax,file_3ds
	mov dword[level_stack],0 ;����塞 �஢��� �⥪�
	mov dword[offs_last_timer],0
	;--- ���������� �������� ����� � ᯨ᮪
	stdcall add_3ds_object, ID_ICON_CHUNK_MAIN,0,dword[esi+2],0
	call block_children ;�室 � ���୨� ����

	mov edi,dword[file_3ds.offs]
	add edi,dword[file_3ds.size]
	.cycle_main:
		cmp dword[level_stack],0
		jle .end_cycle
		
		cmp esi,edi ;�᫨ ����� 䠩��
		jge .end_cycle

		mov edx,dword[esi+2] ;ࠧ��� �����
		call block_analiz
		cmp dword[bl_found],0
		jne @f
			;��ꥪ� �� �����⭮�� ����
			stdcall add_3ds_object, ID_ICON_CHUNK_NOT_FOUND,dword[level_stack],edx,0
			call block_next
			jmp .cycle_main
		@@:
			;��ꥪ� �����⭮�� ����
			mov ecx,dword[bl_found]
			mov bx,word[ecx+2] ;����� ������ ��� ��ꥪ�
			stdcall add_3ds_object, ebx,dword[level_stack],edx,dword[ecx+5]
			cmp byte[ecx+4],1
			je .bl_data
				;���� ᮤ�ন� ���୨� �����
				call block_children ;�室 � ���୨� ����
				jmp .cycle_main
			.bl_data:
				;���� ᮤ�ন� �����
				call block_analiz_data
				jmp .cycle_main
	.end_cycle:
		stdcall [tl_cur_beg], tree1
		stdcall [tl_draw], tree1
	.end_open:
	ret

;������ ������ �����
;input:
; esi - memory pointer
;output:
; eax - new stack pointer
; esi - new memory pointer
align 4
block_analiz_data:
	push ebx ecx edx edi
		mov dx,word[esi]
		mov ecx,dword[esi+2]
		sub ecx,6 ;ࠧ��� ������ � �����
		add esi,6
		mov ebx,dword[level_stack]
		inc ebx
		; *** ������ ������ � ࠧ�묨 ����묨 � �뤥������ ���������
		cmp dx,CHUNK_OBJBLOCK ;��ꥪ�
		jne @f
			push ax
				cld
				xor al,al
				mov edi,esi
				repne scasb
			pop ax
			sub edi,esi ;edi - strlen
			stdcall add_3ds_object, ID_ICON_DATA,ebx,edi,0 ;�������� ��ꥪ�
			add esi,edi
			;sub ecx,edi ;㦥 ᤥ���� � repne
			jmp .next_bl
		@@:
		cmp dx,CHUNK_VERTLIST ;ᯨ᮪ ���設
		je .vertexes
		cmp dx,0x4111 ;䫠�� ���設
		je .vertexes
		cmp dx,CHUNK_MAPLIST ;⥪����� ���न����
		je .vertexes
		jmp @f
		.vertexes: ;��ࠡ�⪠ ������, ᮤ�ঠ�� ����� ���設
			stdcall add_3ds_object, ID_ICON_DATA,ebx,2,txt_count ;�᫮ ���設
			add esi,2
			sub ecx,2
			stdcall add_3ds_object, ID_ICON_DATA,ebx,ecx,0 ;����� ���設
			sub esi,8 ;����⠭������� esi
			call block_next
			jmp .end_f		
		@@:
		cmp dx,CHUNK_FACELIST ;ᯨ᮪ �࠭��
		jne @f
			stdcall add_3ds_object, ID_ICON_DATA,ebx,2,txt_count ;�᫮ �࠭��
			push eax
			xor eax,eax
			mov ax,[esi]
			shl eax,3
			add esi,2
			sub ecx,2
			stdcall add_3ds_object, ID_ICON_DATA,ebx,eax,0 ;����� ���設

			sub ecx,eax
			cmp ecx,1
			jl .data_3 ;�஢��塞 ���� �� ���� ����뢠�騩 ���ਠ�, �ਬ��塞� � ��ꥪ��
				add esi,eax
				pop eax
				jmp .next_bl
				;stdcall add_3ds_object, ID_ICON_DATA,ebx,ecx,0 ;����� ���ਠ��
				;sub esi,eax ;����⠭������� esi
			.data_3:

			sub esi,8 ;����⠭������� esi
			pop eax
			call block_next
			jmp .end_f		
		@@:
		cmp dx,CHUNK_FACEMAT ;���ਠ�� �࠭��
		jne @f
			push ax
				cld
				xor al,al
				mov edi,esi
				repne scasb
			pop ax
			sub edi,esi ;edi - strlen
			stdcall add_3ds_object, ID_ICON_DATA,ebx,edi,0 ;�������� ��ꥪ�
			add esi,edi
			;sub ecx,edi ;㦥 ᤥ���� � repne
			stdcall add_3ds_object, ID_ICON_DATA,ebx,2,txt_count ;�᫮ �࠭��
			add esi,2
			sub ecx,2
			stdcall add_3ds_object, ID_ICON_DATA,ebx,ecx,0 ;����� �࠭��, � ����� �ਬ���� ���ਠ�
			sub esi,edi ;����⠭������� esi (1)
			sub esi,8   ;����⠭������� esi (2)
			call block_next
			jmp .end_f
		@@:
		; *** ������ ����� � ����묨 �� 㬮�砭�� (��� �뤥����� ���������)
			stdcall add_3ds_object, ID_ICON_DATA,ebx,ecx,0
			sub esi,6 ;����⠭������� esi
			call block_next
			jmp .end_f
		.next_bl:
		; *** ����ன�� ��� ������� ��⠢���� ���������
			mov dword[eax],esi ;㪠��⥫� �� ��砫� �����
			mov ebx,dword[esi+2]
			mov dword[eax+4],ebx ;ࠧ��� �����
			inc dword[level_stack]
			add eax,8
		.end_f:
	pop edi edx ecx ebx
	ret

;�室 � 1-� ���୨� ����
;output:
; edx - destroy
align 4
block_children:
	push ecx
		;�஢�ઠ �ࠢ��쭮�� ࠧ��஢ ���୥�� �����
		mov ebx,esi ;ᬥ饭�� ��砫� த�⥫�᪮�� �����
		add ebx,6 ;���室 �� ��砫� ���୥�� �����
		add ebx,dword[ebx+2] ;������塞 ࠧ��� ���୥�� �����
		mov ecx,esi ;ᬥ饭�� ��砫� த�⥫�᪮�� �����
		add ecx,dword[esi+2] ;������塞 ࠧ��� த�⥫�᪮�� �����
		cmp ebx,ecx ;���뢠�� ��������� �� �㦭�, �. �. �ࠢ�������� ⮫쪮 ����� ������
		jle @f
			;��������஢��� �訡�� 䠩��, ���୨� ���� ��室�� �� �।��� த�⥫�᪮��
			mov dword[level_stack],FILE_ERROR_CHUNK_SIZE
			jmp .end_f
		@@:
		mov dword[eax],esi ;㪠��⥫� �� ��砫� �����
		mov ebx,dword[esi+2]
		mov dword[eax+4],ebx ;ࠧ��� �����
		add esi,6 ;���室�� � ����� �����
		inc dword[level_stack]
		add eax,8
	.end_f:
	pop ecx
	ret

;���室 � ᫥��饬� ����� ⥪�饣� �஢��
;input:
; eax - ���� �������� � ��६���묨
align 4
block_next:
push ebx
	add esi,dword[esi+2] ;�ய�᪠�� ����� �����

	;�஢�ઠ ࠧ��஢ த�⥫�᪮�� �����, ��� ���������� ��室� �� ���孨� �஢��� �᫨ ����� �����
	@@:
	mov ebx,dword[eax-8]
	add ebx,dword[eax-4]
	cmp esi,ebx
	jl @f
		dec dword[level_stack]
		sub eax,8
		cmp dword[level_stack],0
		jg @b
	@@:
pop ebx
	ret

;�㭪�� ���᪠ �������� ����뢠�饩 ����
;input:
;esi - memory pointer
;output:
;dword[bl_found] - pointer to chunk struct (= 0 if not found)
align 4
bl_found dd 0
block_analiz:
pushad
	mov dword[bl_found],0
	mov ecx,type_bloks
	@@:
		mov bx,word[ecx]
		cmp word[esi],bx
		je .found
		add ecx,sizeof.block_3ds
		cmp ecx,type_bloks.end
		jl @b
	jmp .no_found
	.found:
		mov dword[bl_found],ecx
	.no_found:
popad
	ret

;input:
; esi - 㪠��⥫� �� ��������㥬� �����
; level - �஢��� ���������� 㧫�
; size_bl - ࠧ��� �����
align 4
proc add_3ds_object, icon:dword,level:dword,size_bl:dword,info_bl:dword
	pushad
		mov bx,word[icon]
		shl ebx,16
		mov bx,word[level]

		mov eax,esi
		sub eax,dword[open_file_lif]
		mov dword[buffer],eax ;ᬥ饭�� �����
		mov ecx,dword[size_bl]
		mov dword[buffer+4],ecx ;ࠧ��� ����� (�ᯮ������ � �㭪樨 buf_draw_hex_table ��� �ᮢ���� �����)
		mov ecx,dword[bl_found]
		cmp ecx,0
		je @f
			;... ����� �㦥� ��㣮� ������ ����� �� 㤠�����
			mov cl,byte[ecx+4]
		@@:
		mov byte[buffer+list_offs_chunk_del],cl
		mov ecx,[level]
		mov byte[buffer+list_offs_chunk_lev],cl
		mov ecx,dword[info_bl]
		mov dword[buffer+list_offs_p_data],ecx
		stdcall hex_in_str, buffer+list_offs_text,dword[esi+1],2
		stdcall hex_in_str, buffer+list_offs_text+2,dword[esi],2 ;��� 3ds �����
		cmp ecx,0
		jne @f
			mov byte[buffer+list_offs_text+4],0 ;0 - ᨬ��� ���� ��ப�
			jmp .no_capt
		@@:
			mov byte[buffer+list_offs_text+4],' '
			mov esi,ecx
			mov edi,buffer+list_offs_text+5
			mov ecx,size_one_list-(list_offs_text+5)
			cld
			rep movsb
			mov byte[buffer+size_one_list-1],0 ;0 - ᨬ��� ����� ��ப�
		.no_capt:
		stdcall [tl_node_add], buffer, ebx, tree1
		stdcall [tl_cur_next], tree1
		if debug
			stdcall print_err,sz_add_3ds_object,buffer+list_offs_text
		end if
	popad
	ret
endp

if debug
sz_add_3ds_object db 13,10,'3ds_object',0

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
end if

align 4
proc hex_in_str, buf:dword,val:dword,zif:dword
	pushad
		mov edi,dword[buf]
		mov ecx,dword[zif]
		add edi,ecx
		dec edi
		mov ebx,dword[val]

		.cycle:
			mov al,bl
			and al,0xf
			cmp al,10
			jl @f
				add al,'a'-'0'-10
			@@:
			add al,'0'
			mov byte[edi],al
			dec edi
			shr ebx,4
		loop .cycle

	popad
	ret
endp

align 4
but_save_file:
	pushad
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],1
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],2
	je .end_save_file
	;��� �� 㤠筮� ����⨨ �������

	mov eax,70 ;70-� �㭪�� ࠡ�� � 䠩����
	mov [run_file_70.Function], 2
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov ebx, dword[open_file_lif]
	mov [run_file_70.Buffer], ebx
	mov ebx,dword[ebx+2]
	mov dword[run_file_70.Count], ebx ;ࠧ��� 䠩��
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mov ebx,run_file_70
	int 0x40 ;����㦠�� 䠩� ����ࠦ����
	cmp ebx,0xffffffff
	je .end_save_file

	.end_save_file:
	popad
	ret

align 4
but_wnd_coords:
	cmp byte[prop_wnd_run],0
	jne @f
		pushad
		mcall 51,1,prop_start,thread_coords
		popad
	@@:
	ret

;description:
; 㤠����� ��࠭���� ����� �� ����⮣� 䠩��
align 4
but_delete_chunk:
	pushad
	stdcall [tl_node_get_data],tree1
	pop ebx
	cmp ebx,0
	je .end_f
	cmp byte[ebx+list_offs_chunk_del],0 ;�᫨ ���� ���饭 �� 㤠�����
	jne .notify

	;(1) ����஢���� ������ ��� 䠩��
	mov edx,dword[ebx+4] ;ࠧ��� �����
	sub [open_file_size],edx ;��������� ࠧ��஢ 䠩��
	mov ecx,[open_file_size]
	mov eax,dword[ebx] ;����砥� ���祭�� ᤢ��� ��࠭���� ����� �⭮�⥫쭮 ��砫� 䠩��
	sub ecx,eax ;ecx - ࠧ��� ������ ᤢ������� ��� 䠩��
	add eax,dword[open_file_lif] ;����砥� ���祭�� ᤢ��� � �����
	mov edi,eax
	mov esi,eax
	add esi,edx
	mov al,byte[ebx+list_offs_chunk_lev] ;��६ �஢��� ⥪�饣� 㧫�
	rep movsb
	mov byte[can_save],1

	;(2) ��������� ࠧ��஢ த�⥫�᪨� ������
	cmp al,0
	je .end_2
	.cycle_2:
	stdcall [tl_cur_perv], tree1
	stdcall [tl_node_get_data],tree1
	pop ebx
	cmp ebx,0
	je .end_2
		cmp byte[ebx+list_offs_chunk_lev],al
		jge .cycle_2
		mov al,byte[ebx+list_offs_chunk_lev]
		mov ecx,[ebx]
		add ecx,[open_file_lif]
		sub dword[ecx+2],edx
		cmp al,0 ;�᫨ ᠬ� ���孨� 㧥�, � al=0
		jne .cycle_2
	.end_2:
	
	;(3) ���������� ᯨ᪠ tree1
	call init_tree
	call draw_window

	jmp .end_f
	.notify:
	notify_window_run txt_not_delete
	.end_f:
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

;����� ��� ������� ������ 䠩���
align 4
OpenDialog_data:
.type			dd 0 ;0 - ������, 1 - ��࠭���, 2 - ����� ��४���
.procinfo		dd procinfo	;+4
.com_area_name		dd communication_area_name	;+8
.com_area		dd 0	;+12
.opendir_path		dd plugin_path	;+16
.dir_default_path	dd default_dir ;+20
.start_path		dd file_name ;+24 ���� � ������� ������ 䠩���
.draw_window		dd draw_window	;+28
.status 		dd 0	;+32
.openfile_path		dd openfile_path	;+36 ���� � ���뢠����� 䠩��
.filename_area		dd filename_area	;+40
.filter_area		dd Filter
.x:
.x_size 		dw 420 ;+48 ; Window X size
.x_start		dw 10 ;+50 ; Window X position
.y:
.y_size 		dw 320 ;+52 ; Window y size
.y_start		dw 10 ;+54 ; Window Y position

default_dir db '/rd/1',0

communication_area_name:
	db 'FFFFFFFF_open_dialog',0
open_dialog_name:
	db 'opendial',0
communication_area_default_path:
	db '/rd/1/File managers/',0

Filter:
dd Filter.end - Filter.1
.1:
db '3DS',0
db 'TXT',0
.end:
db 0



head_f_i:
head_f_l db '���⥬��� �訡��',0

system_dir_0 db '/sys/lib/'
lib_name_0 db 'proc_lib.obj',0
err_message_found_lib_0 db '�� ������� ������⥪� ',39,'proc_lib.obj',39,0
err_message_import_0 db '�訡�� �� ������ ������⥪� ',39,'proc_lib.obj',39,0

system_dir_1 db '/sys/lib/'
lib_name_1 db 'libimg.obj',0
err_message_found_lib_1 db '�� ������� ������⥪� ',39,'libimg.obj',39,0
err_message_import_1 db '�訡�� �� ������ ������⥪� ',39,'libimg.obj',39,0

system_dir_2 db '/sys/lib/'
lib_name_2 db 'box_lib.obj',0
err_msg_found_lib_2 db '�� ������� ������⥪� ',39,'box_lib.obj',39,0
err_msg_import_2 db '�訡�� �� ������ ������⥪� ',39,'box_lib',39,0

system_dir_7 db '/sys/lib/'
lib_name_7 db 'buf2d.obj',0
err_msg_found_lib_7 db '�� ������� ������⥪� ',39,'buf2d.obj',39,0
err_msg_import_7 db '�訡�� �� ������ ������⥪� ',39,'buf2d',39,0

l_libs_start:
	lib0 l_libs lib_name_0, sys_path, file_name, system_dir_0,\
		err_message_found_lib_0, head_f_l, proclib_import,err_message_import_0, head_f_i
	lib1 l_libs lib_name_1, sys_path, file_name, system_dir_1,\
		err_message_found_lib_1, head_f_l, import_libimg, err_message_import_1, head_f_i
	lib_2 l_libs lib_name_2, sys_path, library_path, system_dir_2,\
		err_msg_found_lib_2,head_f_l,import_box_lib,err_msg_import_2,head_f_i
	lib_7 l_libs lib_name_7, sys_path, library_path, system_dir_7,\
		err_msg_found_lib_7,head_f_l,import_buf2d,err_msg_import_7,head_f_i
l_libs_end:

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
proclib_import: ;���ᠭ�� �ᯮ���㥬�� �㭪権
	OpenDialog_Init dd aOpenDialog_Init
	OpenDialog_Start dd aOpenDialog_Start
dd 0,0
	aOpenDialog_Init db 'OpenDialog_init',0
	aOpenDialog_Start db 'OpenDialog_start',0

align 4
import_buf2d:
	dd sz_init0
	buf2d_create dd sz_buf2d_create
	buf2d_create_f_img dd sz_buf2d_create_f_img
	buf2d_clear dd sz_buf2d_clear
	buf2d_draw dd sz_buf2d_draw
	buf2d_delete dd sz_buf2d_delete
	buf2d_resize dd sz_buf2d_resize
	buf2d_line dd sz_buf2d_line
	buf2d_rect_by_size dd sz_buf2d_rect_by_size
	buf2d_filled_rect_by_size dd sz_buf2d_filled_rect_by_size
	buf2d_circle dd sz_buf2d_circle
	buf2d_img_hdiv2 dd sz_buf2d_img_hdiv2
	buf2d_img_wdiv2 dd sz_buf2d_img_wdiv2
	buf2d_conv_24_to_8 dd sz_buf2d_conv_24_to_8
	buf2d_conv_24_to_32 dd sz_buf2d_conv_24_to_32
	buf2d_bit_blt dd sz_buf2d_bit_blt
	buf2d_bit_blt_transp dd sz_buf2d_bit_blt_transp
	buf2d_bit_blt_alpha dd sz_buf2d_bit_blt_alpha
	buf2d_curve_bezier dd sz_buf2d_curve_bezier
	buf2d_convert_text_matrix dd sz_buf2d_convert_text_matrix
	buf2d_draw_text dd sz_buf2d_draw_text
	buf2d_crop_color dd sz_buf2d_crop_color
	buf2d_offset_h dd sz_buf2d_offset_h
	buf2d_flood_fill dd sz_buf2d_flood_fill
	buf2d_set_pixel dd sz_buf2d_set_pixel
	dd 0,0
	sz_init0 db 'lib_init',0
	sz_buf2d_create db 'buf2d_create',0
	sz_buf2d_create_f_img db 'buf2d_create_f_img',0
	sz_buf2d_clear db 'buf2d_clear',0
	sz_buf2d_draw db 'buf2d_draw',0
	sz_buf2d_delete db 'buf2d_delete',0
	sz_buf2d_resize db 'buf2d_resize',0
	sz_buf2d_line db 'buf2d_line',0
	sz_buf2d_rect_by_size db 'buf2d_rect_by_size',0
	sz_buf2d_filled_rect_by_size db 'buf2d_filled_rect_by_size',0
	sz_buf2d_circle db 'buf2d_circle',0
	sz_buf2d_img_hdiv2 db 'buf2d_img_hdiv2',0
	sz_buf2d_img_wdiv2 db 'buf2d_img_wdiv2',0
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

align 4
import_box_lib:
	dd sz_init1
	init_checkbox dd sz_init_checkbox
	check_box_draw dd sz_check_box_draw
	edit_box_draw dd sz_edit_box_draw
	edit_box_key dd sz_edit_box_key
	edit_box_mouse dd sz_edit_box_mouse
	edit_box_set_text dd sz_edit_box_set_text
	scrollbar_ver_draw dd sz_scrollbar_ver_draw
	scrollbar_hor_draw dd sz_scrollbar_hor_draw

	tl_data_init dd sz_tl_data_init
	tl_data_clear dd sz_tl_data_clear
	tl_info_clear dd sz_tl_info_clear
	tl_key dd sz_tl_key
	tl_mouse dd sz_tl_mouse
	tl_draw dd sz_tl_draw
	tl_info_undo dd sz_tl_info_undo
	tl_info_redo dd sz_tl_info_redo
	tl_node_add dd sz_tl_node_add
	tl_node_set_data dd sz_tl_node_set_data
	tl_node_get_data dd sz_tl_node_get_data
	tl_node_delete dd sz_tl_node_delete
	tl_node_move_up dd sz_tl_node_move_up
	tl_node_move_down dd sz_tl_node_move_down
	tl_cur_beg dd sz_tl_cur_beg
	tl_cur_next dd sz_tl_cur_next
	tl_cur_perv dd sz_tl_cur_perv
	tl_node_close_open dd sz_tl_node_close_open
	tl_node_lev_inc dd sz_tl_node_lev_inc
	tl_node_lev_dec dd sz_tl_node_lev_dec
	tl_node_poi_get_info dd sz_tl_node_poi_get_info
	tl_node_poi_get_next_info dd sz_tl_node_poi_get_next_info
	tl_node_poi_get_data dd sz_tl_node_poi_get_data

	dd 0,0
	sz_init1 db 'lib_init',0
	sz_init_checkbox db 'init_checkbox2',0
	sz_check_box_draw db 'check_box_draw2',0
	sz_edit_box_draw db 'edit_box',0
	sz_edit_box_key db 'edit_box_key',0
	sz_edit_box_mouse db 'edit_box_mouse',0
	sz_edit_box_set_text db 'edit_box_set_text',0
	sz_scrollbar_ver_draw db 'scrollbar_v_draw',0
	sz_scrollbar_hor_draw db 'scrollbar_h_draw',0

	sz_tl_data_init db 'tl_data_init',0
	sz_tl_data_clear db 'tl_data_clear',0
	sz_tl_info_clear db 'tl_info_clear',0
	sz_tl_key db 'tl_key',0
	sz_tl_mouse db 'tl_mouse',0
	sz_tl_draw db 'tl_draw',0
	sz_tl_info_undo db 'tl_info_undo',0
	sz_tl_info_redo db 'tl_info_redo',0
	sz_tl_node_add db 'tl_node_add',0
	sz_tl_node_set_data db 'tl_node_set_data',0
	sz_tl_node_get_data db 'tl_node_get_data',0
	sz_tl_node_delete db 'tl_node_delete',0
	sz_tl_node_move_up db 'tl_node_move_up',0
	sz_tl_node_move_down db 'tl_node_move_down',0
	sz_tl_cur_beg db 'tl_cur_beg',0
	sz_tl_cur_next db 'tl_cur_next',0
	sz_tl_cur_perv db 'tl_cur_perv',0
	sz_tl_node_close_open db 'tl_node_close_open',0
	sz_tl_node_lev_inc db 'tl_node_lev_inc',0
	sz_tl_node_lev_dec db 'tl_node_lev_dec',0
	sz_tl_node_poi_get_info db 'tl_node_poi_get_info',0
	sz_tl_node_poi_get_next_info db 'tl_node_poi_get_next_info',0
	sz_tl_node_poi_get_data db 'tl_node_poi_get_data',0



mouse_dd dd 0x0
sc system_colors 
last_time dd 0

align 16
procinfo process_information 

align 4
buf_0: dd 0 ;㪠��⥫� �� ���� ����ࠦ����
.l: dw 205 ;+4 left
.t: dw 35 ;+6 top
.w: dd 340 ;+8 w
.h: dd 250 ;+12 h
.color: dd 0xffffd0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_1:
	dd 0 ;㪠��⥫� �� ���� ����ࠦ����
	dw 25 ;+4 left
	dw 25 ;+6 top
	dd 128 ;+8 w
	dd 144 ;+12 h
	dd 0 ;+16 color
	db 24 ;+20 bit in pixel

el_focus dd tree1
tree1 tree_list size_one_list,300+2, tl_key_no_edit+tl_draw_par_line,\
	16,16, 0xffffff,0xb0d0ff,0xd000ff, 5,35,195-16,250, 16,list_offs_text,0, el_focus,\
	w_scr_t1,0

align 4
w_scr_t1:
.size_x     dw 16 ;+0
.start_x    dw 0
rb 2+2
.btn_high   dd 15 ;+8
.type	    dd 1  ;+12
.max_area   dd 100  ;+16
rb 4+4
.bckg_col   dd 0xeeeeee ;+28
.frnt_col   dd 0xbbddff ;+32
.line_col   dd 0  ;+36
rb 4+2+2
.run_x:
rb 2+2+2+2+4+4+4+4+4+4
.all_redraw dd 0 ;+80
.ar_offset  dd 1 ;+84



;��� ��� �� ���, �� �८�ࠧ�� �᫮ � ��ப�
;input:
; eax = value
; edi = string buffer
;output:
align 4
tl_convert_to_str:
	pushad
		mov dword[edi+1],0;0x20202020
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
		call .str ;��३� �� ᠬ� ᥡ� �.�. �맢��� ᠬ� ᥡ� � ⠪ �� ⮣� ������ ���� � eax �� �⠭�� ����� 祬 � ecx
		pop eax
	@@: ;cmp al,10 ;�஢���� �� ����� �� ���祭�� � al 祬 10 (��� ��⥬� ��᫥�� 10 ������ ������� - ��譠�))
	or al,0x30  ;������ ������� ����  祬 ��� ���
	stosb	    ;������� ����� �� ॣ���� al � �祪� ����� es:edi
	ret	      ;�������� 祭� ������ 室 �.�. ���� � �⥪� �࠭����� ���-�� �맮��� � �⮫쪮 ࠧ �� � �㤥� ��뢠����

i_end:
	rb 1024
thread_coords:
	rb 1024
stacktop:
	sys_path rb 1024
	file_name:
		rb 1024 ;4096 
	library_path rb 1024
	plugin_path rb 4096
	openfile_path rb 4096
	filename_area rb 256
mem:
