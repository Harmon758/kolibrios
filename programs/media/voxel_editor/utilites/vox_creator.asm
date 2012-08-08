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

include '../../../../programs/macros.inc'
include '../../../../programs/proc32.inc'
include '../../../../programs/develop/libraries/box_lib/load_lib.mac'
include '../trunk/mem.inc'
include '../trunk/dll.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load
caption db 'Voxel creator 8.08.12',0 ;������� ����

struct FileInfoBlock
	Function dd ?
	Position dd ?
	Flags	 dd ?
	Count	 dd ?
	Buffer	 dd ?
		db ?
	FileName dd ?
ends

BUF_STRUCT_SIZE equ 21
buf2d_data equ dword[edi] ;����� ���� ����ࠦ����
buf2d_w equ dword[edi+8] ;�ਭ� ����
buf2d_h equ dword[edi+12] ;���� ����
buf2d_l equ word[edi+4]
buf2d_t equ word[edi+6] ;����� ᢥ���
buf2d_size_lt equ dword[edi+4] ;����� ᫥�� � �ࠢ� ��� ����
buf2d_color equ dword[edi+16] ;梥� 䮭� ����
buf2d_bits equ byte[edi+20] ;������⢮ ��� � 1-� �窥 ����ࠦ����

vox_offs_tree_table equ 4
vox_offs_data equ 12

run_file_70 FileInfoBlock
image_data dd 0 ;㪠��⥫� �� �६����� ������. ��� �㦥� �८�ࠧ������ ����ࠦ����
vox_obj_size dd 0 ;ࠧ��� ���ᥫ쭮�� ��ꥪ� (��� �᪮७�� ��⠢��)
txt_space db ' ',0
txt_pref db ' � ',0,' ��',0,' ��',0,' ��',0 ;���⠢��: ����, ����, ����
txt_f_size: db '������: '
.size: rb 16

fn_toolbar db 'toolbar.png',0
IMAGE_TOOLBAR_ICON_SIZE equ 16*16*3
IMAGE_TOOLBAR_SIZE equ IMAGE_TOOLBAR_ICON_SIZE*9
image_data_toolbar dd 0

max_open_file_size equ 1024*1024 ;1 Mb

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



align 4
start:
	load_libraries l_libs_start,l_libs_end
	;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ������⥪�
	mov	ebp,lib_0
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
		mcall -1 ;exit not correct
	@@:
	mcall 48,3,sc,sizeof.system_colors
	mcall 40,0x27
	stdcall [OpenDialog_Init],OpenDialog_data ;�����⮢�� �������

	stdcall [buf2d_create], buf_0 ;ᮧ����� ����
	stdcall [buf2d_create], buf_0z
	stdcall [buf2d_vox_brush_create], buf_vox, vox_6_7_z

	load_image_file fn_toolbar, image_data_toolbar,IMAGE_TOOLBAR_SIZE

	stdcall mem.Alloc,max_open_file_size
	mov dword[open_file_vox],eax
	stdcall mem.Alloc,max_open_file_size
	mov dword[open_file_img],eax

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

	jmp still

align 4
timer_funct:
	cmp byte[calc],0
	je still

	pushad
	mcall 26,9
	mov [last_time],eax

	; ᪨�뢠�� 㪠��⥫� ���஢ buf_npl_p, buf_npl, buf_npl_n
	mov edi,buf_npl_p
	mov eax,buf2d_data
	mov edi,buf_npl
	mov ebx,buf2d_data
	mov edi,buf_npl_n
	mov ecx,buf2d_data
	; change buffer data pointers
	mov buf2d_data,eax
	mov edi,buf_npl_p
	mov buf2d_data,ebx
	mov edi,buf_npl
	mov buf2d_data,ecx

	mov eax,[n_plane]
	mov ebx,buf2d_w
	dec ebx

	cmp ebx,eax
	jg @f
		stdcall create_obj_from_plane,buf_npl,eax ;ᮧ���� �������饥 �祭��
		;�뢮� ����筮�� १����
		call draw_object
		mov byte[calc],0
		jmp .end_f
	@@:

	inc eax
	stdcall create_plane, buf_npl_n,eax

	; ᮧ����� ��᪥�쭮�� �祭�� �� �᭮�� ���� buf_npl
	mov edi,buf_npl
	mov edx,[bby_min] ;0
	.cycle_0:
	mov ecx,[btx_min] ;0
	.cycle_1:
		stdcall [buf2d_get_pixel], edi,ecx,edx
		cmp eax,buf2d_color
		je @f
			mov esi,eax
			call need_node
			cmp eax,buf2d_color
			jne @f ;��ᥨ���� ����७��� ���ᥫ�, ��� ��⨬���樨 ������
			mov eax,ebx
			sub eax,edx
			stdcall buf2d_vox_obj_create_node, [open_file_vox],ecx,[n_plane],\
				eax,[k_scale],esi
		@@:
		inc ecx
		cmp ecx,ebx
		jle .cycle_1
		inc edx
		cmp edx,ebx
		jle .cycle_0

	stdcall [buf2d_draw], buf_npl
	inc dword[n_plane] ;��६�頥� ���᪮��� �祭��
	call draw_pok
	.end_f:
	popad
	jmp still

align 4
proc create_plane, buf_img:dword, n_plane:dword
	pushad
	; �����㥬 ��楢�� ����ࠦ���� � ���� buf_img
	; bby_min - �ᯮ��㥬 ��� ��⨬���樨 (�᫨ ᢥ��� ����ࠦ���� ���⮥)
	mov eax,[bby_min]
	mov esi,buf_i2
	mov esi,[esi] ;buf2d_data
	mov edi,[buf_img]
	mov ecx,buf2d_w
	imul eax,ecx
	mov ebx,ecx
	imul ecx,ebx
	sub ecx,eax
	lea ecx,[ecx+ecx*2]
	mov edi,buf2d_data
	lea eax,[eax+eax*2]
	add edi,eax
	add esi,eax
	cld
	rep movsb

	mov ecx,ebx
	dec ebx
	mov edi,buf_i0
	.cycle_0:
		mov eax,ebx
		sub eax,[n_plane] ;eax - ��ॢ����� ���न��� n_plane
		stdcall [buf2d_get_pixel], edi,ecx,eax ;[n_plane]
		cmp eax,buf2d_color
		jne @f
			;���ન����� ���⨪��쭮� ����� �� �祭�� buf_img
			stdcall [buf2d_line], [buf_img],ecx,[bby_min],ecx,ebx, buf2d_color
			jmp .end_1
		@@:
			mov edx,[bby_min] ;xor edx,edx
			mov esi,eax
			.cycle_1: ;横� ��� ��������� ���孥� ⥪�����
			stdcall [buf2d_get_pixel], [buf_img],ecx,edx
			cmp eax, buf2d_color
			je .end_0
				stdcall [buf2d_set_pixel], [buf_img],ecx,edx, esi ;��������� ���孥� ⥪�����
				jmp .end_1
			.end_0:
			inc edx
			cmp edx,ebx
			jle .cycle_1
		.end_1:
		loop .cycle_0

	;��ਧ��⠫�� ����� �� �᭮�� ������� �࠭�
	mov ecx,[bby_min]
	mov edi,buf_i1
	.cycle_2:
		stdcall [buf2d_get_pixel], edi,[n_plane],ecx
		cmp eax,buf2d_color
		jne @f
			;���ન����� ��ਧ��⠫쭮� ����� �� �祭�� buf_img
			stdcall [buf2d_line], [buf_img],[btx_min],ecx,[btx_max],ecx, buf2d_color
			jmp .end_3
		@@:
			mov edx,[btx_max] ;ebx
			mov esi,eax
			.cycle_3: ;横� ��� ��������� ������� ⥪�����
			stdcall [buf2d_get_pixel], [buf_img],edx,ecx
			cmp eax, buf2d_color
			je .end_2
				stdcall [buf2d_set_pixel], [buf_img],edx,ecx, esi ;��������� ������� ⥪�����
				jmp .end_3
			.end_2:
			dec edx
			cmp edx,[btx_min]
			jge .cycle_3
		.end_3:
		inc ecx
		cmp ecx,ebx
		jle .cycle_2
	popad
	ret
endp

align 4
proc create_obj_from_plane, buf_img:dword, n_plane:dword
pushad
	; ᮧ����� ��᪥�쭮�� �祭�� �� �᭮�� ���� buf_img
	mov edi,[buf_img]
	mov ebx,buf2d_w
	dec ebx
	mov edx,[bby_min] ;0
	.cycle_0:
	mov ecx,[btx_min] ;0
	.cycle_1:
		stdcall [buf2d_get_pixel], edi,ecx,edx
		cmp eax,buf2d_color
		je @f
			mov esi,eax
			;call need_node
			;cmp eax,buf2d_color
			;jne @f ;��ᥨ���� ����७��� ���ᥫ�, ��� ��⨬���樨 ������
			mov eax,ebx
			sub eax,edx
			stdcall buf2d_vox_obj_create_node, [open_file_vox],ecx,[n_plane],\
				eax,[k_scale],esi
		@@:
		inc ecx
		cmp ecx,ebx
		jle .cycle_1
		inc edx
		cmp edx,ebx
		jle .cycle_0
	ret
popad
endp

;description:
; �㭪�� �஢���� �㦭� �� ��ᥨ���� ���ᥫ� � ���न��⠬� [n_plane],ecx,edx
;input:
; ebx = max size y - 1
;output:
; eax = buf2d_color if node need
align 4
need_node:
	mov eax,buf2d_color
	cmp ecx,[btx_min] ;0
	jle .end_f
	cmp ecx,[btx_max] ;0
	jge .end_f
	cmp edx,[bby_min] ;0
	jle .end_f
	cmp edx,ebx ;max-1
	jge .end_f
		push ecx edx
		stdcall [buf2d_get_pixel], buf_npl_p,ecx,edx ;�஢�ઠ �।��饣� �祭��
		cmp eax,buf2d_color
		je @f
		stdcall [buf2d_get_pixel], buf_npl_n,ecx,edx ;�஢�ઠ ��᫥���饣� �祭��
		cmp eax,buf2d_color
		je @f
		dec ecx
		stdcall [buf2d_get_pixel], edi,ecx,edx
		cmp eax,buf2d_color
		je @f
		add ecx,2
		stdcall [buf2d_get_pixel], edi,ecx,edx
		cmp eax,buf2d_color
		je @f
		dec ecx
		dec edx
		stdcall [buf2d_get_pixel], edi,ecx,edx
		cmp eax,buf2d_color
		je @f
		add edx,2
		stdcall [buf2d_get_pixel], edi,ecx,edx
		;cmp eax,buf2d_color
		;je @f
		@@:
		pop edx ecx
	.end_f:
	ret

;
;x0y0 - x1y0
;x1y0 - x1y1
;x0y1 - x0y0
;x1y1 - x0y1
align 4
proc vox_obj_rot_z uses eax ebx ecx, v_obj:dword
	mov ebx,[v_obj]
	add ebx,vox_offs_tree_table
	mov ecx,2
	cld
	@@:
		mov eax,dword[ebx]
		mov byte[ebx+1],al
		mov byte[ebx+3],ah
		shr eax,16
		mov byte[ebx],al
		mov byte[ebx+2],ah
		add ebx,4
		loop @b
	ret
endp

align 4
draw_window:
pushad
	mcall 12,1

	; *** �ᮢ���� �������� ���� (�믮������ 1 ࠧ �� ����᪥) ***
	xor eax,eax
	mov ebx,(20 shl 16)+410
	mov ecx,(20 shl 16)+520
	mov edx,[sc.work]
	or  edx,(3 shl 24)+0x30000000
	mov edi,caption
	int 0x40

	; *** ᮧ����� ������ �� ������ ***
	mov eax,8
	mov ebx,(5 shl 16)+20
	mov ecx,(5 shl 16)+20
	mov edx,3
	mov esi,[sc.work_button]
	int 0x40

	mov ebx,(30 shl 16)+20
	mov edx,4
	int 0x40

	mov ebx,(55 shl 16)+20
	mov edx,5
	int 0x40

	mov ebx,(85 shl 16)+20
	mov edx,6
	int 0x40

	add ebx,25 shl 16
	mov edx,7
	int 0x40

	add ebx,25 shl 16
	mov edx,8
	int 0x40

	add ebx,25 shl 16
	mov edx,9
	int 0x40

	add ebx,25 shl 16
	mov edx,10
	int 0x40

	add ebx,25 shl 16
	mov edx,11
	int 0x40

	add ebx,25 shl 16
	mov edx,12
	int 0x40

	; *** �ᮢ���� ������ �� ������� ***
	mov eax,7
	mov ebx,[image_data_toolbar]
	mov ecx,(16 shl 16)+16
	mov edx,(7 shl 16)+7 ;icon new
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon open
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;icon save
	int 0x40

	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(30 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40
	add ebx,IMAGE_TOOLBAR_ICON_SIZE
	add edx,(25 shl 16) ;
	int 0x40

	call draw_buffers
	call draw_pok

	mcall 12,2
popad
	ret

align 4
draw_buffers:
	; *** �ᮢ���� ���� ***
	stdcall [buf2d_draw], buf_0
	stdcall [buf2d_draw], buf_i0
	stdcall [buf2d_draw], buf_i1
	stdcall [buf2d_draw], buf_i2
	ret

align 4
draw_pok:
	;���������� ������ ࠧ��� 䠩��
	mov edi,txt_f_size.size
	mov dword[edi],0
	mov eax,dword[vox_obj_size]
	mov ebx,txt_pref
	.cycle:
		cmp eax,1024
		jl @f
		shr eax,10
		add ebx,4
		jmp .cycle
	@@:
	call convert_int_to_str
	stdcall str_cat, edi,ebx
	stdcall str_cat, edi,txt_space ;�������騩 �஡��

	mov eax,4 ;�ᮢ���� ⥪��
	mov ebx,(275 shl 16)+7
	mov ecx,[sc.work_text]
	or  ecx,0x80000000 or (1 shl 30)
	mov edx,txt_f_size
	mov edi,[sc.work] ;梥� 䮭� ����
	int 0x40

	ret

align 4
key:
	mcall 2
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
		call but_1
	@@:
	cmp ah,7
	jne @f
		call but_2
	@@:
	cmp ah,8
	jne @f
		call but_3
	@@:
	cmp ah,9
	jne @f
		stdcall but_run, 0
	@@:
	cmp ah,10
	jne @f
		stdcall but_run, 1
	@@:
	cmp ah,11
	jne @f
		call but_5
	@@:
	cmp ah,12
	jne @f
		call but_rot_z
	@@:
	cmp ah,1
	jne still
.exit:
	stdcall [buf2d_delete],buf_0
	stdcall [buf2d_delete],buf_0z
	stdcall [buf2d_vox_brush_delete],buf_vox
	stdcall [buf2d_delete],buf_i0
	stdcall [buf2d_delete],buf_i1
	stdcall [buf2d_delete],buf_i2
	stdcall [buf2d_delete],buf_npl_p
	stdcall [buf2d_delete],buf_npl
	stdcall [buf2d_delete],buf_npl_n
	stdcall mem.Free,[image_data_toolbar]
	stdcall mem.Free,[open_file_vox]
	stdcall mem.Free,[open_file_img]
	mcall -1


align 4
vox_new_data:
	db 2,0,0,0
	db 000b,001b,010b,011b, 100b,101b,110b,111b ;default table
	dd 0 ;null node

align 4
proc but_new_file uses ecx edi esi
	mov ecx,vox_offs_data+4
	mov [vox_obj_size],ecx
	mov esi,vox_new_data
	mov edi,[open_file_vox]
	cld
	rep movsb
	ret
endp

align 4
open_file_vox dd 0 ;㪠��⥫� �� ������ ��� ������ 䠩���
open_file_img dd 0 ;㪠��⥫� �� ������ ��� ������ ⥪����

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
	mov [run_file_70.Count], dword max_open_file_size
	m2m [run_file_70.Buffer],dword[open_file_vox]
	mov byte[run_file_70+20], 0
	mov dword[run_file_70.FileName], openfile_path
	mov ebx,run_file_70
	int 0x40 ;����㦠�� 䠩� ����ࠦ����
	cmp ebx,0xffffffff
	je .end_open_file
		; �஢�ઠ �� �ࠢ��쭮��� ���ᥫ쭮�� �ଠ�
		mov edi,[open_file_vox]
		add edi,vox_offs_tree_table
		xor bx,bx
		mov ecx,8
		cld
		@@:
			movzx ax,byte[edi]
			add bx,ax
			inc edi
			loop @b
		cmp bx,28 ;28=0+1+2+...+7
		jne .err_open

		mcall 71,1,openfile_path
		stdcall buf2d_vox_obj_get_size,[open_file_vox]
		mov [vox_obj_size],eax
		call draw_object
		
		jmp .end_open_file
	.err_open:
		call but_new_file
		stdcall [mb_create],msgbox_4,thread
	.end_open_file:
	popad
	ret

align 4
but_save_file:
	pushad
		copy_path open_dialog_name,communication_area_default_path,file_name,0
		mov [OpenDialog_data.type],1
		stdcall [OpenDialog_Start],OpenDialog_data
		cmp [OpenDialog_data.status],2
		je .end_save_file
		;��� �� 㤠筮� ����⨨ �������

		mov eax,6 ;dword[v_zoom] ;������ ����⠡ �� 㬮�砭��
		mov ebx,[open_file_vox]
		mov byte[ebx],al

		stdcall buf2d_vox_obj_get_size, ebx
		mov dword[run_file_70.Count], eax ;ࠧ��� 䠩��
		mov eax,70 ;70-� �㭪�� ࠡ�� � 䠩����
		mov [run_file_70.Function], 2
		mov [run_file_70.Position], 0
		mov [run_file_70.Flags], 0
		mov ebx, dword[open_file_vox]
		mov [run_file_70.Buffer], ebx
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
but_1:
	stdcall open_image_in_buf, buf_i0
	ret

align 4
but_2:
	stdcall open_image_in_buf, buf_i1
	call set_buf_tabs
	ret

align 4
but_3:
	stdcall open_image_in_buf, buf_i2
	call set_buf_tabs
	ret

;description:
; � ����ᨬ��� �� �ਭ� 1-�� ���� ��⠭�������� ������ ��� 2-�� � 3-�� ���஢
align 4
set_buf_tabs:
push eax ebx edi
	mov edi,buf_i0
	cmp buf2d_data,0
	je @f
		movzx eax,buf2d_l
		mov ebx,buf2d_w
		cmp ebx,128
		jle .end_0
			mov ebx,128
		.end_0:
		mov edi,buf_i1
		mov buf2d_l,ax
		add buf2d_l,bx
		add buf2d_l,2
		shl ebx,1
		mov edi,buf_i2
		mov buf2d_l,ax
		add buf2d_l,bx
		add buf2d_l,4
	@@:	
pop edi ebx eax
	ret

align 4
get_scale:
push eax edi
	mov edi,buf_i0
	mov eax,buf2d_w

	mov dword[k_scale],-1
	.cycle_s:
	cmp eax,0
	je @f
		shr eax,1
		inc dword[k_scale]
		jmp .cycle_s
	@@:
pop edi eax
	ret

align 4
proc set_borders uses eax ebx ecx edx edi
	mov ecx,dword[k_scale]
	mov edx,1
	shl edx,cl

	;��।��塞 ���������� ���न���� y �� ��楢�� �࠭�
	mov edi,buf_i2
	mov dword[bby_min],0
	xor ecx,ecx
	.cycle_0:
	xor ebx,ebx
	.cycle_1:
		stdcall [buf2d_get_pixel],edi,ebx,ecx
		cmp eax,buf2d_color
		jne @f
		inc ebx
		cmp ebx,edx
		jl .cycle_1
		inc dword[bby_min]
		inc ecx
		cmp ecx,edx
		jl .cycle_0
	@@:

	;��।��塞 ���������� ���न���� x �� ��楢�� �࠭�
	mov dword[btx_min],0
	xor ebx,ebx
	.cycle_2:
	mov ecx,[bby_min]
	.cycle_3:
		stdcall [buf2d_get_pixel],edi,ebx,ecx
		cmp eax,buf2d_color
		jne @f
		inc ecx
		cmp ecx,edx
		jl .cycle_3
		inc dword[btx_min]
		inc ebx
		cmp ebx,edx
		jl .cycle_2
	@@:

	;��।��塞 ���ᨬ����� ���न���� x �� ��楢�� �࠭�
	mov [btx_max],edx
	dec dword[btx_max]
	mov ebx,[btx_max]
	.cycle_4:
	mov ecx,[bby_min]
	.cycle_5:
		stdcall [buf2d_get_pixel],edi,ebx,ecx
		cmp eax,buf2d_color
		jne @f
		inc ecx
		cmp ecx,edx
		jl .cycle_5
		dec dword[btx_max]
		dec ebx
		cmp ebx,[btx_min]
		jg .cycle_4
	@@:
	
	;stdcall [buf2d_line], edi, 0,[bby_min],50,[bby_min],255
	ret
endp

align 4
btx_min dd 0 ;���� ���孨� ���. x
btx_max dd 0
bty_min dd 0 ;���� ���孨� ���. y
bty_max dd 0
;bbx_min dd 0
;bbx_max dd 0
bby_min dd 0 ;���� ������� ���. y
bby_max dd 0
k_scale dd 0
n_plane dd 0

calc db 0

; ᮧ����� ���ᥫ� � 3 �⠯�:
; 1) �饬 ���� � ������� ��ॢ�, �㤠 ���� ��⠢��� (�᫨ ���� �������, 2-� �⠯ �ய�᪠��)
; 2) ��⠢�塞 ����� ���� � ���ᥫ�� (3-� �⠯ 㦥 �� ������)
; 3) ���塞 梥� ���ᥫ�
align 4
proc buf2d_vox_obj_create_node, v_obj:dword,coord_x:dword,coord_y:dword,\
coord_z:dword,k_scale:dword,color:dword
pushad
locals
	p_node dd 0 ;த�⥫�᪨� 㧥�
endl

	mov edi,[v_obj]
	add edi,vox_offs_data
	mov esi,[k_scale]
	cmp esi,1
	jl .change
	; *** (1) ***
	.found:
	stdcall vox_obj_get_node_position, [v_obj],[coord_x],[coord_y],[coord_z],esi
	movzx bx,byte[edi+3]
	mov [p_node],edi
	add edi,4

	cmp eax,0
	je .end_1
	mov ecx,eax
	cld
	@@: ;横� ��� �ய�᪠ �।���� �����ॢ쥢 � 㧫�
		bt bx,0 ;�஢��塞 ���� �� ���୨� 㧫�
		jnc .end_0
			xor eax,eax
			stdcall vox_obj_rec0 ;� eax �������� �᫮ ���୨� 㧫��, � ������ ��⢨
		.end_0:
		shr bx,1
		loop @b
	.end_1:
	bt bx,0
	jnc .creat ;�᫨ �����ॢ� �� �������, ���室�� � ᮧ�����
	dec esi
	cmp esi,0
	jg .found
	jmp .change

	; *** (2) ***
	.creat:
	mov edx,[color] ;���塞 梥�
	and edx,0xffffff ;��� ��䨫��⨪�
	mov ecx,esi
	stdcall vox_obj_add_nodes_mem, [v_obj],edi,ecx ;����塞 ���� ���������� ��ꥪ⮬, ��� ���������� ����� 㧫��
	mov ebx,[p_node]
	cld
	@@:
		mov dword[edi],edx
		stdcall vox_obj_get_node_bit_mask, [v_obj],[coord_x],[coord_y],[coord_z],esi
		or byte[ebx+3],al

		mov ebx,edi
		add edi,4
		dec esi
		loop @b
	jmp .end_2

	; *** (3) ***
	.change:
	mov eax,[color] ;���塞 梥�
	mov word[edi],ax
	shr eax,16
	mov byte[edi+2],al

	.end_2:
popad
	ret
endp

; ᤢ����� 㧫� ��� ���������� ����� 㧫��
;input:
; p_insert - ������ ��� ��⠢��
; count - �������⢮ ��⠢�塞�� 㧫��
align 4
proc vox_obj_add_nodes_mem uses eax ecx edi esi, v_obj:dword,p_insert:dword,count:dword
	mov esi,[v_obj]
	;stdcall buf2d_vox_obj_get_size,esi
	add esi,[vox_obj_size] ;esi - 㪠��⥫� �� ����� 䠩��
	mov edi,[count]
	shl edi,2
	add [vox_obj_size],edi
	add edi,esi ;edi - 㪠��⥫� �� ���騩 ����� 䠩��
	mov ecx,esi
	sub ecx,[p_insert]
	shr ecx,2 ;ecx - �᫮ 横��� ��� ����஢����
	sub esi,4 ;esi - 㪠��⥫� �� ��᫥���� 㧥�
	sub edi,4 ;edi - 㪠��⥫� �� ���騩 ��᫥���� 㧥�
	std
	rep movsd ;ᤢ����� ������
	ret
endp

;???
; ��।������ ����樨 㧫� � ��ॢ� (�� 0 �� 7)
align 4
proc vox_obj_get_node_position uses ebx ecx edi, v_obj:dword,\
coord_x:dword,coord_y:dword,coord_z:dword,k_scale:dword
	mov ecx,[k_scale]
	dec ecx
	mov eax,[coord_x]
	mov ebx,[coord_y]
	mov edi,[coord_z]
	cmp ecx,1
	jl .end_0
		shr eax,cl
		shr ebx,cl
		shr edi,cl
	.end_0:
	and eax,1
	bt ebx,0
	jnc @f
		bts eax,1
	@@:
	bt edi,0
	jnc @f
		bts eax,2
	@@:

	mov edi,[v_obj]
	add edi,vox_offs_tree_table
	@@:
		cmp al,byte[edi]
		je @f
		inc edi
		jmp @b
	@@:
	sub edi,[v_obj]
	sub edi,vox_offs_tree_table
	mov eax,edi
	
	ret
endp

;???
;input:
; edi - 㪠��⥫� �� ����� ���ᥫ쭮�� ��ꥪ�
;output:
; eax - eax + �᫮ 㧫�� � ������ ����. ��ꥪ�
; edi - 㪠��⥫� �� ᬥ饭�� ����� ����. ��ꥪ�
align 4
proc vox_obj_rec0
	inc eax
	cmp byte[edi+3],0 ;ᬮ�ਬ ���� �� �����ॢ��
	je .sub_trees

		;४��ᨢ�� ��ॡ�� �����ॢ쥢
		push ebx ecx
		mov bh,byte[edi+3]
		add edi,4
		mov bl,8
		.cycle:
			bt bx,8 ;����㥬 ⮫쪮 bh
			jnc .c_next
				stdcall vox_obj_rec0
			.c_next:
			shr bh,1
			dec bl
			jnz .cycle
		pop ecx ebx

		jmp .end_f
	.sub_trees:
		add edi,4
	.end_f:
	ret
endp

;output:
; eax - 1,2,4,8,16, ... ,128
align 4
proc vox_obj_get_node_bit_mask uses ebx ecx edi, v_obj:dword,\
coord_x:dword,coord_y:dword,coord_z:dword,k_scale:dword
	mov ecx,[k_scale]
	dec ecx
	mov eax,[coord_x]
	mov ebx,[coord_y]
	mov edi,[coord_z]
	cmp ecx,1
	jl .end_0
		shr eax,cl
		shr ebx,cl
		shr edi,cl
	.end_0:
	and eax,1
	bt ebx,0
	jnc @f
		bts eax,1
	@@:
	bt edi,0
	jnc @f
		bts eax,2
	@@:

	mov ecx,[v_obj]
	add ecx,vox_offs_tree_table
	@@:
		cmp al,byte[ecx]
		je @f
		inc ecx
		jmp @b
	@@:
	mov eax,1 ;��⠭�������� ��ࢮ��砫쭮� ���祭�� ���
	sub ecx,[v_obj]
	sub ecx,vox_offs_tree_table
	jz @f
		shl eax,cl ;ᤢ����� ���
	@@:
	
	ret
endp

;
;output:
; eax - ࠧ��� � ����� ��������� ��ꥪ⮬ v_obj
align 4
proc buf2d_vox_obj_get_size uses edi, v_obj:dword
	mov edi,[v_obj]
	add edi,vox_offs_data
	xor eax,eax
	stdcall vox_obj_rec0 ;eax - �᫮ 㧫�� � ��ꥪ� v_obj
	shl eax,2
	add eax,vox_offs_data
	ret
endp

;input:
; mode_add - �᫨ �� ࠢ�� 0 ⮣�� ᮧ����� � ०��� ���������� ���ᥫ��
align 4
proc but_run uses eax ebx edi, mode_add:dword
	; �஢�ઠ ࠧ��஢ ����ࠦ���� (⥪����)
	mov edi,buf_i0
	mov ebx,buf2d_h
	cmp ebx,2
	jle .err_size_t
	cmp buf2d_w,ebx
	jne .err_size_t
	;
	mov edi,buf_i1
	cmp ebx,buf2d_h
	jne .err_size_tb
	mov ebx,buf2d_h
	cmp ebx,2
	jle .err_size_b
	cmp buf2d_w,ebx
	jne .err_size_b

		cmp dword[vox_obj_size],vox_offs_data
		jl .n_file ;�᫨ ࠭�� �� �뫮 ������� 䠩���
		cmp dword[mode_add],0
		jne @f
		.n_file:
			call but_new_file
		@@:

		mov edi,buf_i0
		mov eax,buf2d_w
		mov edi,buf_npl
		cmp buf2d_data,0
		jne @f
			; *** ᮧ���� ����
			m2m buf2d_w,eax
			m2m buf2d_h,eax
			stdcall [buf2d_create],edi
			mov edi,buf_npl_p
			m2m buf2d_w,eax
			m2m buf2d_h,eax
			stdcall [buf2d_create],edi
			mov edi,buf_npl_n
			m2m buf2d_w,eax
			m2m buf2d_h,eax
			stdcall [buf2d_create],edi
			jmp .end_0
		@@:
			; *** �����塞 ࠧ���� ���஢
			stdcall [buf2d_resize], edi, eax,eax
			mov edi,buf_npl_p
			stdcall [buf2d_resize], edi, eax,eax
			mov edi,buf_npl_n
			stdcall [buf2d_resize], edi, eax,eax
		.end_0:
		mov dword[n_plane],1
		mov byte[calc],1
		call get_scale
		call set_borders

		stdcall create_plane,buf_npl,0
		stdcall create_plane,buf_npl_n,1
		stdcall create_obj_from_plane,buf_npl,0 ;ᮧ���� ��砫쭮� �祭��

		jmp @f
	.err_size_t:
		stdcall [mb_create],msgbox_0,thread
		jmp @f
	.err_size_b:
		stdcall [mb_create],msgbox_1,thread
		jmp @f
	.err_size_tb:
		stdcall [mb_create],msgbox_3,thread
		;jmp @f
	@@:
	ret
endp

align 4
but_5:
	cmp byte[calc],0
	je @f
		call draw_object
		mov byte[calc],0
	@@:
	ret

align 4
but_rot_z:
	stdcall vox_obj_rot_z, [open_file_vox]
	call draw_object
	ret

align 4
draw_object:
	;�뢮� १���� �� �࠭
	stdcall [buf2d_clear], buf_0, [buf_0.color] ;��⨬ ����
	stdcall [buf2d_clear], buf_0z, 0 ;��⨬ ����
	stdcall [buf2d_vox_obj_draw_3g], buf_0, buf_0z, buf_vox,\
		[open_file_vox], 0,0, 0, 6 ;[k_scale]
	stdcall [buf2d_draw], buf_0 ;������塞 ���� �� �࠭�
	ret

align 4
proc open_image_in_buf, buf:dword
	pushad
	copy_path open_dialog_name,communication_area_default_path,file_name,0
	mov [OpenDialog_data.type],0
	stdcall [OpenDialog_Start],OpenDialog_data
	cmp [OpenDialog_data.status],2
	je .end_open_file

	;stdcall mem.Alloc, dword size ;�뤥�塞 ������ ��� ����ࠦ����
	;mov [buf],eax

	mov eax,70 ;70-� �㭪�� ࠡ�� � 䠩����
	mov [run_file_70.Function], 0
	mov [run_file_70.Position], 0
	mov [run_file_70.Flags], 0
	mov [run_file_70.Count], dword max_open_file_size
	m2m [run_file_70.Buffer],dword[open_file_img]
	mov byte[run_file_70+20], 0
	mov [run_file_70.FileName], openfile_path
	mov ebx,run_file_70
	int 0x40 ;����㦠�� 䠩� ����ࠦ����
	cmp ebx,0xffffffff
	je .end_0
		;��।��塞 ��� ����ࠦ���� � ��ॢ���� ��� �� �६���� ���� image_data
		stdcall dword[img_decode], dword[open_file_img],ebx,0
		cmp eax,0
		je .end_0 ;�᫨ ����襭 �ଠ� 䠩��
		mov dword[image_data],eax
		;�८�ࠧ㥬 ����ࠦ���� � �ଠ�� rgb
		stdcall dword[img_to_rgb2], dword[image_data],dword[open_file_img]

		mov eax,dword[image_data]
		mov edi,[buf]
		cmp buf2d_data,0
		jne @f
			m2m buf2d_w,dword[eax+4] ;+4 = image width
			m2m buf2d_h,dword[eax+8] ;+8 = image heihht
			stdcall [buf2d_create_f_img], edi,[open_file_img]
			jmp .end_1
		@@:
			mov ebx,dword[eax+4]
			mov ecx,dword[eax+8]
			stdcall [buf2d_resize], edi, ebx,ecx ;�����塞 ࠧ���� ����
			imul ecx,ebx
			lea ecx,[ecx+ecx*2]
			mov edi,buf2d_data
			mov esi,[open_file_img]
			cld
			rep movsb ;copy image
		.end_1:

		;㤠�塞 �६���� ���� image_data
		stdcall dword[img_destroy], dword[image_data]
	.end_0:

	call draw_buffers
	.end_open_file:
	popad
	ret
endp

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

align 4
proc str_cat uses eax ecx edi esi, str1:dword, str2:dword
	mov esi,dword[str2]
	stdcall str_len,esi
	mov ecx,eax
	inc ecx
	mov edi,dword[str1]
	stdcall str_len,edi
	add edi,eax
	cld
	repne movsb
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

msgbox_0:
	db 1,0
	db '��������',0
	db '������ ���孥�� ����ࠦ���� �� ��४��',0
	db '�������',0
	db 0

msgbox_1:
	db 1,0
	db '��������',0
	db '������ �������� ����ࠦ���� �� ��४��',0
	db '�������',0
	db 0

msgbox_2:
	db 1,0
	db '��������',0
	db '������ ��।���� ����ࠦ���� �� ��४��',0
	db '�������',0
	db 0

msgbox_3:
	db 1,0
	db '��������',0
	db '������� ���孥�� � �������� ����ࠦ���� �� ᮢ������',13,\
		'��ன� ����ࠦ���� ���������� ࠧ��஢',0
	db '�������',0
	db 0

msgbox_4:
	db 1,0
	db '��������',0
	db '���뢠��� 䠩� ᮤ�ন� �� ���ᥫ�� �ଠ�',0
	db '�������',0
	db 0

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
dd Filter.end - Filter ;.1
.1:
db 'PNG',0
db 'JPG',0
db 'JPEG',0
db 'BMP',0
db 'GIF',0
db 'VOX',0
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
lib_name_2 db 'buf2d.obj',0
err_msg_found_lib_2 db '�� ������� ������⥪� ',39,'buf2d.obj',39,0
err_msg_import_2 db '�訡�� �� ������ ������⥪� ',39,'buf2d',39,0

system_dir_3 db '/sys/lib/'
lib_name_3 db 'msgbox.obj',0
err_msg_found_lib_3 db '�� ������� ������⥪� ',39,'msgbox.obj',39,0
err_msg_import_3 db '�訡�� �� ������ ������⥪� ',39,'msgbox',39,0

l_libs_start:
	lib_0 l_libs lib_name_0, sys_path, file_name, system_dir_0,\
		err_message_found_lib_0, head_f_l, proclib_import,err_message_import_0, head_f_i
	lib_1 l_libs lib_name_1, sys_path, file_name, system_dir_1,\
		err_message_found_lib_1, head_f_l, import_libimg, err_message_import_1, head_f_i
	lib_2 l_libs lib_name_2, sys_path, library_path, system_dir_2,\
		err_msg_found_lib_2,head_f_l,import_buf2d,err_msg_import_2,head_f_i
	lib_3 l_libs lib_name_3, sys_path, library_path, system_dir_3,\
		err_msg_found_lib_3,head_f_l,import_msgbox_lib,err_msg_import_3,head_f_i
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
	init dd sz_init
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
	buf2d_get_pixel dd sz_buf2d_get_pixel
	buf2d_vox_brush_create dd sz_buf2d_vox_brush_create
	buf2d_vox_brush_delete dd sz_buf2d_vox_brush_delete
	buf2d_vox_obj_get_img_w_3g dd sz_buf2d_vox_obj_get_img_w_3g
	buf2d_vox_obj_get_img_h_3g dd sz_buf2d_vox_obj_get_img_h_3g
	buf2d_vox_obj_draw_1g dd sz_buf2d_vox_obj_draw_1g
	buf2d_vox_obj_draw_3g dd sz_buf2d_vox_obj_draw_3g
	buf2d_vox_obj_draw_3g_scaled dd sz_buf2d_vox_obj_draw_3g_scaled
	buf2d_vox_obj_draw_3g_shadows dd sz_buf2d_vox_obj_draw_3g_shadows
	buf2d_vox_obj_draw_pl dd sz_buf2d_vox_obj_draw_pl
	buf2d_vox_obj_draw_pl_scaled dd sz_buf2d_vox_obj_draw_pl_scaled
	dd 0,0
	sz_init db 'lib_init',0
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
	sz_buf2d_get_pixel db 'buf2d_get_pixel',0
	sz_buf2d_vox_brush_create db 'buf2d_vox_brush_create',0
	sz_buf2d_vox_brush_delete db 'buf2d_vox_brush_delete',0
	sz_buf2d_vox_obj_get_img_w_3g db 'buf2d_vox_obj_get_img_w_3g',0
	sz_buf2d_vox_obj_get_img_h_3g db 'buf2d_vox_obj_get_img_h_3g',0
	sz_buf2d_vox_obj_draw_1g db 'buf2d_vox_obj_draw_1g',0
	sz_buf2d_vox_obj_draw_3g db 'buf2d_vox_obj_draw_3g',0
	sz_buf2d_vox_obj_draw_3g_scaled db 'buf2d_vox_obj_draw_3g_scaled',0
	sz_buf2d_vox_obj_draw_3g_shadows db 'buf2d_vox_obj_draw_3g_shadows',0
	sz_buf2d_vox_obj_draw_pl db 'buf2d_vox_obj_draw_pl',0
	sz_buf2d_vox_obj_draw_pl_scaled db 'buf2d_vox_obj_draw_pl_scaled',0

align 4
import_msgbox_lib:
	mb_create dd amb_create
;	mb_reinit dd amb_reinit
;	mb_setfunctions dd amb_setfunctions
dd 0,0
	amb_create db 'mb_create',0
;	amb_reinit db 'mb_reinit',0
;	amb_setfunctions db 'mb_setfunctions',0

mouse_dd dd 0x0
sc system_colors 
last_time dd 0

align 16
procinfo process_information 

align 4
buf_0: dd 0
	dw 5 ;+4 left
	dw 35 ;+6 top
.w: dd 6*64 ;+8 w
.h: dd 7*64 ;+12 h
.color: dd 0xffffff ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_0z: dd 0
	dw 5 ;+4 left
	dw 35 ;+6 top
.w: dd 6*64 ;+8 w
.h: dd 7*64 ;+12 h
.color: dd 0 ;+16 color
	db 32 ;+20 bit in pixel

align 4
buf_i0: dd 0
	dw 5 ;+4 left
	dw 35 ;+6 top
.w: dd 0 ;+8 w
.h: dd 0 ;+12 h
.color: dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_i1: dd 0
	dw 105 ;+4 left
	dw 35 ;+6 top
.w: dd 0 ;+8 w
.h: dd 0 ;+12 h
.color: dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_i2: dd 0
	dw 205 ;+4 left
	dw 35 ;+6 top
.w: dd 0 ;+8 w
.h: dd 0 ;+12 h
.color: dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_npl_p: dd 0
	dw 0 ;+4 left
	dw 0 ;+6 top
.w: dd 0 ;+8 w
.h: dd 0 ;+12 h
.color: dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_npl: dd 0
	dw 5 ;+4 left
	dw 35 ;+6 top
.w: dd 0 ;+8 w
.h: dd 0 ;+12 h
.color: dd 0 ;+16 color
	db 24 ;+20 bit in pixel

align 4
buf_npl_n: dd 0
	dw 0 ;+4 left
	dw 0 ;+6 top
.w: dd 0 ;+8 w
.h: dd 0 ;+12 h
.color: dd 0 ;+16 color
	db 24 ;+20 bit in pixel

;����� ��� ᮧ����� �������쭮�� �����筮�� ���ᥫ�
align 4
vox_6_7_z:
dd 0,0,1,1,0,0,\
   0,2,2,2,2,0,\
   2,2,2,2,2,2,\
   2,3,2,2,3,2,\
   2,3,3,3,3,2,\
   0,3,3,3,3,0,\
   0,0,3,3,0,0

align 4
buf_vox:
	db 6,7,4,3 ;w,h,h_osn,n
	rb BUF_STRUCT_SIZE*(2+1)



;input:
; eax = value
; edi = string buffer
;output:
align 4
convert_int_to_str:
	pushad
		cld
		mov dword[edi+1],0
		mov word[edi+5],0
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
		rb 2048
	thread:
		rb 2048
stacktop:
	sys_path rb 1024
	file_name:
		rb 1024 ;4096 
	library_path rb 1024
	plugin_path rb 4096
	openfile_path rb 4096
	filename_area rb 256
mem:
