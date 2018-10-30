; ����� TreeList ��� ������⥪� box_lib.obj
; �� ��� �ਬ����� GPL2 ��業���
; ��᫥���� ����䨪��� 12.09.2017 IgorA


struct TreeNode
	type dw ? ;+ 0 ⨯ �����, ��� ������ ������ ��� 㧫�
	level db ? ;+ 2 �஢��� �����
	close db ? ;+ 3 䫠� �������, ��� ������ (����� ��� ��� த�⥫�᪮�� 㧫�)
	perv dd ? ;+ 4 ������ �।��饣� �����
	next dd ? ;+ 8 ������ ��᫥���饣� �����
	t_create dd ? ;+12 �६. ᮧ�����
	t_delete dd ? ;+16 �६. 㤠�����
ends


;�뤥���� ����� ��� ������� ᯨ᪠ � �᭮���� ���ଠ樨 (���������)
align 16
proc tl_data_init uses eax ecx edi, tlist:dword
	mov edi,[tlist]

	xor ecx,ecx
	mov cx,tl_info_size
	imul ecx,tl_info_max_count
	invoke mem.alloc,ecx
	mov tl_data_info,eax ;�����㥬 㪠��⥫� �� ����祭�� ������ � ��������
	mov tl_data_img,0  ;����塞 㪠��⥫� 'data_img'
	mov tl_data_img_sys,0 ;����塞 㪠��⥫� 'data_img_sys'

	mov ecx,sizeof.TreeNode
	imul ecx,tl_info_max_count
	invoke mem.alloc,ecx
	mov tl_data_nodes,eax ;�����㥬 㪠��⥫� �� ����祭�� ������ � ��������

	stdcall tl_info_clear, edi

	;����ன�� ���୥�� �஫�����
	cmp tl_p_scroll,0
	je @f
		mov eax,tl_p_scroll
		;*** 梥⮢� ����ன�� ***
		mov ecx,tl_col_bkg
		mov dword[eax+sb_offs_bckg_col],ecx
		mov ecx,tl_col_zag
		mov dword[eax+sb_offs_frnt_col],ecx
		mov ecx,tl_col_txt
		mov dword[eax+sb_offs_line_col],ecx
		;*** ����ன�� ࠧ��஢ ***
		mov ecx,tl_box_left
		add ecx,tl_box_width
		mov word[eax+2],cx
		mov ecx,tl_box_height
		mov word[eax+4],cx
		mov ecx,tl_box_top
		mov word[eax+6],cx
	@@:
	ret
endp

;���⪠ ����� ����� (��������)
align 4
proc tl_data_clear uses eax edi, tlist:dword
	mov edi,[tlist]
	cmp tl_data_img,0
	je @f
		invoke mem.free,tl_data_img ;��⪠ ��⥬��� ������
	@@:
	cmp tl_data_img_sys,0
	je @f
		invoke mem.free,tl_data_img_sys ;��⪠ ��⥬��� ������
	@@:
	invoke mem.free,tl_data_info
	invoke mem.free,tl_data_nodes
	ret
endp

;���⪠ ᯨ᪠ (���ଠ樨)
align 4
proc tl_info_clear uses eax ecx edi, tlist:dword
	mov edi,[tlist]
	mov tl_ch_tim,0
	mov tl_tim_undo,0
	mov tl_cur_pos,0
	mov ecx,sizeof.TreeNode
	imul ecx,tl_info_max_count
	mov eax,tl_data_nodes
	@@:
		mov byte[eax],0 ;��⨬ 㧫� 0-��
		inc eax
		loop @b
	mov eax,tl_data_nodes ;㪠��⥫� �� 0-� 㧥�
	mov dword[eax+TreeNode.next],1 ;㪠��⥫� next � 0-� 㧫� ��ࠢ������ � 1

	cmp tl_p_scroll,0 ;��ࠡ�⪠ �஫�����
	je @f
		mov eax,tl_p_scroll
		mov dword[eax+sb_offs_position],0
		call tb_scrol_resize
	@@:
	ret
endp

;ॠ��� �� ����������
align 4
proc tl_key uses ebx ecx edi, tlist:dword
	mov edi,[tlist]

	mov ebx,tl_el_focus
	cmp [ebx],edi
	jne .no_focus ;����� �� � 䮪��

	push eax
	mcall SF_KEYBOARD,SSF_GET_INPUT_MODE ;������� ०�� ����� � ����������

	lea ecx,[tl_key_scan]
	cmp eax,1 ;1 = ᪠�����
	je @f
		lea ecx,[tl_key_ascii]
	@@:
	pop eax

	xor bx,bx
	cmp ah,byte[ecx] ;Enter
	jne @f
	cmp tl_on_press,0
	je @f
		call tl_on_press
	@@:
	cmp ah,byte[ecx+1] ;Space
	jne @f
		stdcall tl_node_close_open, edi
	@@:
	cmp ah,byte[ecx+2] ;Up
	jne @f
		stdcall tl_cur_perv, edi
	@@:
	cmp ah,byte[ecx+3] ;Down
	jne @f
		stdcall tl_cur_next, edi
	@@:
	cmp ah,byte[ecx+7] ;Page Up
	jne @f
		stdcall tl_cur_page_up, edi
	@@:
	cmp ah,byte[ecx+8] ;Page Down
	jne @f
		stdcall tl_cur_page_down, edi
	@@:

	bt tl_style,0 ;tl_key_no_edit
	jc .no_edit
		cmp ah,byte[ecx+4] ;Left
		jne @f
			stdcall tl_node_lev_dec, edi
			mov bx,1
		@@:
		cmp ah,byte[ecx+5] ;Right
		jne @f
			stdcall tl_node_lev_inc, edi
			mov bx,1
		@@:
		cmp ah,byte[ecx+6] ;Delete
		jne @f
			stdcall tl_node_delete, edi
			mov bx,1
		@@:
	.no_edit:

	cmp bx,1
	jne .no_focus
		stdcall tl_draw, edi
	.no_focus:
	ret
endp

;ॠ��� �� ����
align 4
proc tl_mouse, tlist:dword
	pushad
	mov edi,[tlist]

	mcall SF_MOUSE_GET,SSF_WINDOW_POSITION ;���न���� ��� �⭮�⥫쭮 ����

	mov ebx,tl_box_left
	shl ebx,16
	cmp eax,ebx ;����� �࠭�� ����
	jl .no_in_wnd ;.test_scroll �� �ਬ��塞
		shr ebx,16
		add ebx,tl_box_width
		shl ebx,16
		cmp eax,ebx ;�ࠢ�� �࠭�� ����
		jg .test_scroll

		mov ebx,tl_box_top
		add ebx,tl_box_height
		cmp ax,bx ;������ �࠭�� ����
		jg .test_scroll

		sub ebx,tl_box_height
		add bx,tl_capt_cy
		cmp ax,bx ;������ �࠭�� ���� + ���� ������
		jl .test_scroll

push eax ebx
	mcall SF_MOUSE_GET,SSF_SCROLL_DATA
	mov edx,eax
	xor	ecx,ecx
	test eax,eax
	jz .mouse_next
	test ax,0x8000
	jnz .decr
	shr eax,16
	test ax,0x8000
	jnz .decr_1

	mov cx,dx ;dx = mouse scroll data vertical
	test ecx,ecx
	jnz @f
	shr edx,16
	mov cx,dx ;dx = mouse scroll data horizontal
	test ecx,ecx
	jz .mouse_next
@@:
	stdcall tl_cur_next, edi
	loop @r
	jmp .mouse_next
;------------------------------------------------
.decr: ;���⨪��쭠� �ப��⪠ ����⥫쭠�
	mov bx,ax ;ax = mouse scroll data vertical
	jmp @f
.decr_1: ;��ਧ��⠫쭠� �ப��⪠ ����⥫쭠�
	mov bx,ax ;ax = mouse scroll data horizontal
@@:
	mov ecx,0xffff
	sub ecx,ebx
	inc ecx
@@:
	stdcall tl_cur_perv, edi
	loop @r
;------------------------------------------------
.mouse_next:

		mcall SF_MOUSE_GET,SSF_BUTTON
		bt eax,0 ;left mouse button press
pop ebx eax
		jae .no_draw

		mov esi,tl_el_focus
		mov [esi],edi ;set focus

		; if '+' or '-' press
		mov esi,eax
		shr esi,16
		sub esi,tl_box_left ;esi = mouse x coord in element window

		and eax,0xffff
		sub eax,ebx
		xor edx,edx
		movzx ecx,tl_img_cy
		div ecx
		cmp tl_p_scroll,0 ;���뢠�� �஫����
		je @f
			mov edx,tl_p_scroll
			add eax,[edx+sb_offs_position] ;������塞 �஫���� �� �����
		@@:

		mov ecx,eax
		call tl_get_node_count ;eax = node count
		bt tl_style,3 ;tl_cursor_pos_limited
		jnc @f
		or eax,eax
		jz @f
			dec eax ;�᫨ ����� �⠥� �� �������騥 㧫�
		@@:
		cmp eax,ecx
		jl @f
			mov eax,ecx ;�᫨ ����� �� ��襫 �� �।��� 㧫��, ����⠭�������� ��஥ ���祭�� eax
		@@:

		cmp eax,tl_cur_pos ;�᫨ ����� ���祭�� ����� ᮢ���� � �।��騬
		je @f ;� �� ��ࠥ� �����
		push esi
			mov esi,tl_box_top
			add esi,tl_box_height ;esi = coord bottom border
			call tl_draw_null_cursor ;��ࠥ� �����
		pop esi
		@@:

		mov tl_cur_pos,eax

		; if '+' or '-' press
		call tl_get_cur_node_index ;eax = node index
		cmp eax,2
		jl .no_open_close ;����� �⮨� �� ���⮬ ����, ��� 㧫��
		imul eax,sizeof.TreeNode
		add eax,tl_data_nodes
		xor bx,bx
		mov bl,byte[eax+TreeNode.level] ;��࠭塞 �஢��� ⥪�饣� 㧫�
		inc bx ;+ ���� ��� �����

		cmp si,tl_img_cx
		jl .no_open_close ;��襩 ������ �� ����� ���� ��� �����, ��� �筮 ��� '+' � '-'
			mov eax,esi
			xor edx,edx
			xor ecx,ecx
			mov cx,tl_img_cx
			div ecx

			cmp ax,bx
			jne .no_open_close

			stdcall tl_node_close_open, edi
		.no_open_close:

		mov esi,tl_box_top
		add esi,tl_box_height ;esi = coord bottom border
		call tl_draw_cursor ;����ᮢ�� �����
		call tl_draw_caption_cur_pos
		jmp .no_draw
;--- mouse event for children scrollbar ----------------------------------------
.test_scroll:
	mov edx,tl_p_scroll
	or edx,edx
	jz .no_in_wnd ;���짮��⥫� �� ᮧ��� ���୥�� �஫�����
		shr ebx,16
		add bx,word[edx] ;+0 .size_x
		shl ebx,16
		cmp eax,ebx ;�ࠢ�� �࠭�� ����
		jg .no_in_wnd

		mov eax,[edx+sb_offs_max_area]
		cmp eax,[edx+sb_offs_cur_area]
		jbe .no_in_wnd ;�� 㧫� �������� � ���� �஫�����
			stdcall scroll_bar_vertical.mouse, edx ;scrollbar_ver_mouse

			cmp dword[edx+sb_offs_redraw],0
			je @f
				mov dword[edx+sb_offs_redraw],0
				stdcall tl_draw, edi ;�ந��諨 ��������� �஫�����
			@@:
			cmp dword[edx+sb_offs_delta2],0
			jne .no_draw ;������ �� �஫���� - �� ᭨���� 䮪�� � TreeList
;-------------------------------------------------------------------------------
	.no_in_wnd: ;�� ������ � ���� - ����� 䮪�� (�� �᫮��� �� 䮪�� �� �� ������ ������)
	mcall SF_MOUSE_GET,SSF_BUTTON
	or eax,eax ;��祣� �� ������ eax=0
	jz .no_draw
		mov ebx,tl_el_focus
		cmp [ebx],edi
		jne .no_draw ;����� �� � 䮪��
			mov dword[ebx],0 ;reset focus
			mov esi,tl_box_top
			add esi,tl_box_height ;esi = coord bottom border
			call tl_draw_cursor ;��㥬 ����� � ������ 䮪�ᮬ
	.no_draw:

	popad
	ret
endp

;�⬥�� ����⢨�
align 4
proc tl_info_undo uses eax edi, tlist:dword
	mov edi,[tlist]
	mov eax,tl_tim_undo
	cmp tl_ch_tim,eax
	jbe @f
		inc tl_tim_undo
		call tb_scrol_resize ;��ࠡ�⪠ �஫�����
	@@:
	ret
endp

;����� ����⢨�
align 4
proc tl_info_redo uses edi, tlist:dword
	mov edi,[tlist]
	cmp tl_tim_undo,1
	jl @f
		dec tl_tim_undo
		call tb_scrol_resize ;��ࠡ�⪠ �஫�����
	@@:
	ret
endp

;㤠����� �⬥������ ����⢨�
;����७��� �㭪��, �� ��� �ᯮ��
align 4
tl_info_set_undo:
	cmp tl_tim_undo,1
	jl .no_work

	push eax ebx ecx edx
	mov edx,tl_data_nodes
	mov ecx,edx
	add ecx,sizeof.TreeNode
	call tl_move_next ;long i=node[0].next;

	mov eax,tl_tim_undo
	sub tl_ch_tim,eax ;ch_tim-=tim_undo;

	cmp edx,ecx
	jle @f
		;if(node[i].tc>ch_tim){ // �᫨ ᮧ����� ᨬ���� �뫮 �⬥����
		mov eax,tl_ch_tim
		cmp [edx+TreeNode.t_create],eax
		jle .no_u1
			mov dword[edx+TreeNode.t_create],0
			mov dword[edx+TreeNode.t_delete],0

			mov ebx, [edx+TreeNode.perv]
			imul ebx,sizeof.TreeNode
			add ebx, tl_data_nodes ;.next
			push dword[edx+TreeNode.next] ;node[node[i].perv].next=node[i].next;
			pop dword[ebx+TreeNode.next]

			mov ebx, [edx+TreeNode.next]
			imul ebx,sizeof.TreeNode
			add ebx, tl_data_nodes ;.perv
			push dword[edx+TreeNode.perv] ;node[node[i].next].perv=node[i].perv;
			pop dword[ebx+TreeNode.perv]
		.no_u1:

		;else if(node[i].td>ch_tim) node[i].td=0; // �᫨ 㤠����� ᨬ���� �뫮 �⬥����
		cmp [edx+TreeNode.t_delete],eax
		jle .no_u2
			mov dword[edx+TreeNode.t_delete],0
		.no_u2:
		call tl_move_next
		jmp @b
	@@:
	mov tl_tim_undo,0
	pop edx ecx ebx eax
	.no_work:
	ret

;�뢮� ᯨ᪠ �� �࠭
align 4
proc tl_draw, tlist:dword
	pushad
	;draw dir_list main rect
	mov edi,[tlist]
	mov ebx,tl_box_left
	shl ebx,16
	add ebx,tl_box_width
	mov ecx,tl_box_top
	shl ecx,16
	mov cx,tl_capt_cy
	mov edx,tl_col_zag
	mcall SF_DRAW_RECT ;draw window caption

	add ecx,tl_box_top
	shl ecx,16
	add ecx,tl_box_height
	sub cx,tl_capt_cy
	mcall ,,,tl_col_bkg ;draw window client rect

	cmp tl_capt_cy,9 ;9 - minimum caption height
	jl @f
	mov ebx,edi ;calculate cursor position
	mov eax,tl_cur_pos
	inc eax
	lea edi,[txt_capt_cur.v]
	stdcall tl_convert_to_str, 5

	mov edi,ebx
	mov eax,tl_tim_undo
	lea edi,[txt_capt_otm.v]
	stdcall tl_convert_to_str, 5
	mov edi,ebx ;restore edi

	mov eax,SF_DRAW_TEXT ;captions
	mov ebx,tl_box_left
	shl ebx,16
	add ebx,5*65536+3
	add ebx,tl_box_top
	mov ecx,tl_col_txt
	or  ecx,0x80000000
	lea edx,[txt_capt_cur]
	int 0x40

	mov ebx,tl_box_left
	shl ebx,16
	add ebx,100*65536+3
	add ebx,tl_box_top
	lea edx,[txt_capt_otm]
	int 0x40
	@@:

	;cycle to nodes
	xor eax,eax
	mov edx,tl_data_nodes
	mov ecx,edx
	add ecx,sizeof.TreeNode

	;*** �ய�� 㧫��, ����� �஬�⠭� �஫������ ***
	cmp tl_p_scroll,0 ;�᫨ ���� 㪠��⥫� �� �஫����
	je .end_c1
		mov esi,tl_p_scroll
		cmp dword[esi+sb_offs_position],0 ;�᫨ �஫���� �� �����, ��室��
		je .end_c1
		@@:
			call tl_iterat_next
			cmp edx,ecx
			jle .end_draw
			inc eax
			cmp eax,dword[esi+sb_offs_position]
			jl @b
	.end_c1:

	xor eax,eax
	mov esi,tl_box_top
	add esi,tl_box_height ;esi = coord bottom border
	@@:
		call tl_iterat_next
		cmp edx,ecx
		jle @f
		call tl_draw_node
		inc eax
		jmp @b
	@@:

	call tl_draw_cursor

	mov edi,tl_p_scroll ;��㥬 ���୨� �஫����
	cmp edi,0    ;��� ⮣� ��-�� ��� �� ��諮�� �ᮢ��� � ���짮��⥫�᪮� �ணࠬ��
	je .end_draw ;�᫨ ��� �஫����� ��室��
		stdcall scroll_bar_vertical.draw, edi
	.end_draw:
	popad
	ret
endp

;���室 �� ᫥��騩 ������ 㧥� (�ய�᪠� �������)
;input:
; ecx = pointer to 1 node struct
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; edx = pointer to next node struct
align 4
tl_iterat_next:
	push bx
	mov bl,0x7f
	cmp byte[edx+TreeNode.close],1
	jne @f
		mov bl,byte[edx+TreeNode.level]
	@@:

	cmp tl_tim_undo,0
	je .else

	push eax
	.beg0:
		call tl_move_next
		cmp edx,ecx
		jle @f
		call tl_node_not_vis ;�ய�� 㤠������ � �⬥������
		cmp al,1
		je .beg0
		cmp bl,byte[edx+TreeNode.level] ;�ய�� ��������
		jl .beg0
		@@:
	pop eax
	pop bx
	ret

	.else:
		call tl_move_next
		cmp edx,ecx
		jle .endif
		cmp dword[edx+TreeNode.t_delete],0 ;�ய�� 㤠������
		jne .else
		cmp bl,byte[edx+TreeNode.level] ;�ய�� ��������
		jl .else
	.endif:
	pop bx
	ret

;���室 �� ᫥��騩 ������ 㧥� (� �� ������� ⮦�)
;input:
; ecx = pointer to 1 node struct
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; edx = pointer to next visible node struct
align 4
tl_iterat_next_all:
	cmp tl_tim_undo,0
	je .else

	push eax
	@@:
		call tl_move_next
		cmp edx,ecx
		jle @f
		call tl_node_not_vis
		cmp al,1
		je @b
	@@:
	pop eax
	ret
	.else:
		call tl_move_next
		cmp edx,ecx
		jle .endif
		cmp dword[edx+TreeNode.t_delete],0 ;td -> time delete
		jne .else
	.endif:
	ret

;���室 �� �।��騩 ������ 㧥� (�ய�᪠� �������)
;input:
; ecx = pointer to 1 node struct
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
align 4
proc tl_iterat_perv uses eax
	cmp tl_tim_undo,0
	je .beg1

	.beg0:
		call tl_move_perv
		cmp edx,ecx
		jle @f
		call tl_node_not_vis ;�ய�� 㤠������ � �⬥������
		cmp al,1
		je .beg0

	.beg1:
		call tl_move_perv
		cmp edx,ecx
		jle @f
		cmp dword[edx+TreeNode.t_delete],0 ;td = 'time delete' -> �ய�� 㤠������
		jne .beg1

	@@:
	call tl_move_max_clo_par
	ret
endp

;��室�� த�⥫�᪨� ������� 㧥� ���ᨬ��쭮�� �஢��
;input:
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; edx = pointer closed parent node with maximum level
align 4
proc tl_move_max_clo_par uses eax ebx
	mov eax,edx
	xor ebx,ebx
	.beg:
		call tl_move_par
		cmp byte[edx+TreeNode.close],1 ;த�⥫�᪨� 㧥� ������ ?
		jne @f
			mov eax,edx
		@@:
		cmp ebx,edx
		je .end_f
			mov ebx,edx
			jmp .beg
	.end_f:
	mov edx,eax
	ret
endp

;input:
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; edx = pointer to next node struct
align 4
tl_move_next:
	mov edx,[edx+TreeNode.next]
	imul edx,sizeof.TreeNode
	add edx,tl_data_nodes
	ret

;input:
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; edx = pointer to perv node struct
align 4
tl_move_perv:
	mov edx,[edx+TreeNode.perv]
	imul edx,sizeof.TreeNode
	add edx,tl_data_nodes
	ret

;��।�������� �� த�⥫�᪨� 㧥�, �᫨ ⠪��� ���, � ��⠢�塞 ��஥ ���祭�� 㪠��⥫�
;input:
; ecx = pointer to 1 node struct
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; edx = pointer to parent node struct
align 4
tl_move_par:
	cmp byte[edx+TreeNode.level],0
	je .end_f ;㧥� 0-�� �஢�� �� ����� ���� ���୨�
	push eax ebx esi
	mov esi,edx ;copy node pointer (edx)
	mov bl,byte[edx+TreeNode.level]
	@@:
		call tl_move_perv
		cmp edx,ecx
		jle @f ;�� ��� ���騥 㧫� �� த�⥫�᪨�
		call tl_node_not_vis ;�ய�� 㤠������ � �⬥������
		cmp al,1
		je @b
		cmp byte[edx+TreeNode.level],bl
		jl .end_0 ;㤠筮 ��諨 த�⥫�᪨� 㧥�
		jmp @b
	@@:
		mov esi,ebx ;restore node pointer
	.end_0:
	pop esi ebx eax
	.end_f:
	ret

;�஢���� ������ �� 㪠����� 㧥� � ��⮬: ����������, 㤠�����, �⬥� ����⢨�
;input:
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
;output:
; al = 1 if sumbol not visible
; (node[i].td+tim_Undo<=ch_tim && node[i].td) || (node[i].tc>ch_tim-tim_Undo)
align 4
tl_node_not_vis:
	cmp dword[edx+TreeNode.t_delete],0
	je @f
	mov eax,[edx+TreeNode.t_delete] ;eax=node[i].td
	add eax,tl_tim_undo
	cmp eax,tl_ch_tim
	jg @f
		mov al,1
		ret
	@@:

	mov eax,tl_ch_tim
	sub eax,tl_tim_undo
	cmp [edx+TreeNode.t_create],eax ;time create
	jle @f
		mov al,1
		ret
	@@:
	xor al,al
	ret

;��㥬 ����� �� �࠭�
;input:
; edi = pointer to TreeInfo struct
; esi = coord bottom border
align 4
proc tl_draw_cursor uses eax ebx ecx edx esi
	call tl_get_display_cur_pos ;eax = cursor pos in screen
	cmp eax,0
	jl .end_f ;����� ��室���� ��� ����, � ������ �ப��祭��� �஫������

	cmp tl_data_img_sys,0 ;ᬮ�ਬ ���� �� 㪠��⥫� �� ���⨭�� ��⥬��� ������
	jne @f
		mov ebx,tl_box_left
		shl ebx,16
		mov bx,tl_img_cx
		movzx ecx,tl_img_cy
		imul ecx,eax
		add ecx,tl_box_top
		add cx,tl_capt_cy

		;crop image if on the border
		cmp esi,ecx ;�᫨ ����� ����� � ��� ����� �� �����
		jl .end_f

		sub esi,ecx
		shl ecx,16
		mov cx,tl_img_cy
		cmp si,tl_img_cy
		jge .crop0
			mov cx,si ;�᫨ ����� ����� ���筮 (����� �� ������ �࠭���)
		.crop0:

		mov edx,tl_col_txt
		mcall SF_DRAW_RECT ;��㥬 ���⮩ ��אַ㣮�쭨�, �.�. ��� ��⥬��� ������
		jmp .end_f
	@@:
	mov ebx,tl_data_img_sys
	imul ax,tl_img_cy
	mov edx,tl_box_left
	shl edx,16
	mov dx,ax
	add edx,tl_box_top
	add dx,tl_capt_cy

	mov ecx,tl_el_focus ;�஢��塞 � 䮪�� ����� ��� ���
	cmp dword[ecx],edi
	je .focus
		;�᫨ �� � 䮪�� ᤢ����� ���न���� �� ������ �� ��⨢���� �����
		movzx ecx,tl_img_cx
		movzx eax,tl_img_cy
		imul eax,ecx
		imul eax,4*3 ;4=icon index 3=rgb
		add ebx,eax
	.focus:

	mov cx,tl_img_cx
	shl ecx,16
	mov cx,tl_img_cy

	;crop image if on the border
	cmp si,dx ;�᫨ ����� ����� � ��� ����� �� �����
	jl .end_f

		sub si,dx
		cmp si,tl_img_cy
		jge .crop1
			mov cx,si ;�᫨ ����� ����� ���筮 (����� �� ������ �࠭���)
		.crop1:

		mcall SF_PUT_IMAGE ;��㥬 ������ �����
	.end_f:
	ret
endp

;��ࠥ� ����� �� �࠭�
;input:
; edi = pointer to TreeInfo struct
; esi = coord bottom border
align 4
proc tl_draw_null_cursor uses eax ebx ecx edx esi
	call tl_get_display_cur_pos ;eax = cursor pos in screen
	cmp eax,0
	jl .end_f ;����� ��室���� ��� ����, � ������ �ப��祭��� �஫������
		mov ebx,tl_box_left
		shl ebx,16
		mov bx,tl_img_cx
		movzx ecx,tl_img_cy
		imul ecx,eax
		add ecx,tl_box_top
		add cx,tl_capt_cy

		;crop image if on the border
		cmp esi,ecx ;�᫨ ����� ����� � ��� ����� �� �����
		jl .end_f

		sub esi,ecx
		shl ecx,16
		mov cx,tl_img_cy
		cmp si,tl_img_cy
		jge @f
			mov cx,si ;�᫨ ����� ����� ���筮 (����� �� ������ �࠭���)
		@@:
		mcall SF_DRAW_RECT,,,tl_col_bkg ;��㥬 ���⮩ ��אַ㣮�쭨� � 䮭��� 梥⮬
	.end_f:
	ret
endp

;���� ������ �����, �⭮�⥫쭮 �࠭�
;input:
; edi = pointer to TreeInfo struct
;output:
; eax = index
align 4
tl_get_display_cur_pos:
	mov eax,tl_cur_pos
	cmp tl_p_scroll,0
	je @f
		push ebx
		mov ebx,tl_p_scroll
		mov ebx,dword[ebx+sb_offs_position]
		sub eax,ebx ;�⭨���� ������ �஫�����
		pop ebx
	@@:
	ret

;���� 㧥� �: ���⨭���, ��������, ������� ������/������� � ����ﬨ � த��. 㧫�
;input:
; eax = node position (0, ..., max_nodes-scroll_pos) �� �� tl_box_height/tl_img_cy
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
; esi = coord of bottom border
align 4
proc tl_draw_node uses eax ebx ecx edx esi
	mov ebx,1 ;1 - ���� ��� �����
	bt tl_style,2 ;tl_list_box_mode
	jc @f
		inc ebx ;+1 - ���� ��� ���� +,-
		add bl,byte[edx+TreeNode.level] ;������塞 �஢��� ����� ��� ��� ��� � ����� ����㯥 ������
	@@:
	imul bx,tl_img_cx
	add ebx,tl_box_left

	shl ebx,16
	mov bx,tl_img_cx
	movzx ecx,tl_img_cy
	imul ecx,eax
	add cx,tl_capt_cy
	jc .end_draw ;����� ����� 㧫��, � ����� ���� ��९������� ���न���� cx
	add ecx,tl_box_top

	;crop image if on the border
	cmp esi,ecx ;�᫨ 㧥� ����� � ��� ����� �� �����
	jl .end_draw

	sub esi,ecx
	shl ecx,16
	mov cx,tl_img_cy
	cmp si,tl_img_cy
	jge @f
		mov cx,si ;�᫨ 㧥� ����� ���筮 (����� �� ������ �࠭���)
		jmp .crop ;�ய�᪠�� �ᮢ���� ������, ����� ᪮॥ �ᥣ� ⮦� �뫥��� �� ������ �࠭���
	@@:
		call tl_draw_node_caption
	.crop:
	mov esi,ecx ;save ecx

	push edx
	cmp tl_data_img,0
	jne .draw_img_n
		mcall SF_DRAW_RECT,,,tl_col_txt ;draw node rect
		jmp @f
	.draw_img_n:
	push ebx esi
		movzx esi,word[edx+TreeNode.type] ;get icon type
		mov edx,ebx
		ror ecx,16
		mov dx,cx
		mov cx,bx
		ror ecx,16
		mov ebx,3 ;rgb = 3 bytes
		imul bx,tl_img_cx
		imul bx,tl_img_cy
		imul ebx,esi ;esi = icon index
		add ebx,tl_data_img

		mcall SF_PUT_IMAGE ;draw node icon '-'
	pop esi ebx
	@@:
	pop edx

	mov al,byte[edx+TreeNode.level] ;draw minus '-'
	mov ecx,tl_data_nodes
	add ecx,sizeof.TreeNode

	mov ah,10 ;get icon index '+' or '-' ?
	cmp byte[edx+TreeNode.close],1
	jne .close
		dec ah
	.close:

	call tl_draw_node_icon_opn_clo ;�ᮢ���� ������ ����⮣� ��� �����⮣� 㧫�
	bt tl_style,1
	jae .end_draw
		call tl_draw_node_icon_par_lin ;�ᮢ���� ����� � த�⥫�᪮�� ������
		call tl_draw_node_icon_par_lin_up ;�ᮢ���� ���⨪��쭮� ����� � த�⥫�᪮�� ������
	.end_draw:
	ret
endp

;���� ������ ����⮣� ��� �����⮣� 㧫� (���筮 + ��� -)
;input:
; al = �஢��� �����
; ecx = pointer to 1 node struct
; edx = pointer to node struct
;...
align 4
proc tl_draw_node_icon_opn_clo uses eax ebx ecx edx esi
	inc al
	call tl_iterat_next_all ;get next visible item
	cmp edx,ecx
	jle @f
		mov ecx,esi ;load ecx
		cmp al,byte[edx+TreeNode.level]
		jne @f
		ror ebx,16
		sub bx,tl_img_cx
		ror ebx,16
		cmp tl_data_img_sys,0
		jne .draw_img_s
			mcall SF_DRAW_RECT,,,tl_col_txt ;draw minus rect, if not system icons
			jmp @f
		.draw_img_s:
		mov ecx,esi ;load ecx
		mov edx,ebx
		ror ecx,16
		mov dx,cx
		mov cx,bx
		ror ecx,16
		mov ebx,3 ;rgb = 3 bytes
		imul bx,tl_img_cx
		imul bx,tl_img_cy
		shr eax,8
		and eax,0xff
		imul ebx,eax ;eax = icon index
		add ebx,tl_data_img_sys
		mcall SF_PUT_IMAGE ;draw minus icon '-'
	@@:
	ret
endp

;�ᮢ���� ����� � த�⥫�᪮�� ������
;input:
; al = �஢��� �����
; ecx = pointer to 1 node struct
; edx = pointer to node struct
;...
align 4
tl_draw_node_icon_par_lin:
	cmp byte[edx+TreeNode.close],1
	je .close
	or al,al
	jz .close
	push eax ebx ecx edx esi
		call tl_iterat_next_all ;get next visible item
		cmp edx,ecx
		jle .line3 ;if end of list
			cmp al,byte[edx+TreeNode.level]
			jne .line3 ;jg ???
			mov eax,3 ;line in middle element
			jmp .line2
		.line3:
			mov eax,6 ;line in end element
		.line2:

		mov ecx,esi ;load ecx
		ror ebx,16
		sub bx,tl_img_cx
		ror ebx,16
		cmp tl_data_img_sys,0
		jne .draw_img_s
			mcall SF_DRAW_RECT,,,tl_col_txt ;draw minus rect, if not system icons
			jmp @f
		.draw_img_s:
		mov edx,ebx
		ror ecx,16
		mov dx,cx
		mov cx,bx
		ror ecx,16
		mov ebx,3 ;rgb = 3 bytes
		imul bx,tl_img_cx
		imul bx,tl_img_cy

		imul ebx,eax ;eax = icon index
		add ebx,tl_data_img_sys
		mcall SF_PUT_IMAGE ;draw line icon
		@@:
	pop esi edx ecx ebx eax
	.close:
	ret


;input:
; al = �஢��� �����
; ebx = (node.left shl 16) + tl_img_cx
; ecx = pointer to 1 node struct
; edx = pointer to node struct
; edi = pointer to 'TreeList' struct
align 4
proc tl_draw_node_icon_par_lin_up uses eax ebx ecx edx esi
	cmp tl_data_img_sys,0 ;if not image
	je @f
	or al,al
	jz @f
		xor esi,esi ;� si �㤥� �����뢠�� ���-�� ������, �㦭�� ��� ���ᮢ�� �����
;--- 横� ��� ���᫥��� �������⢠ ���⨪����� ����� ---
		.cycle0:
			call tl_iterat_perv ;get perv visible item
			cmp edx,ecx
			jle .cycle1 ;if begin of list

			cmp byte[edx+TreeNode.level],al
			jle .cycle0end ;�஢��� ���孥�� ����� �� �ॡ�� ���ᮢ��
			inc si
			jmp .cycle0
		.cycle0end:
		or si,si ;si = ���-�� ������ ����� ����� �㦭� ���ᮢ��� ᢥ���
		jz @f
		shl esi,16

		pop ecx ;esi->ecx
		push ecx ;save esi

		ror ebx,16
		sub bx,tl_img_cx
		ror ebx,16

		mov edx,ebx
		ror ecx,16
		mov dx,cx
		mov cx,bx
		ror ecx,16
		mov cx,tl_img_cy ;restore size y (if crop)
		mov ebx,3 ;rgb = 3 bytes
		imul bx,tl_img_cx
		imul bx,tl_img_cy
		add ebx,tl_data_img_sys

		add esi,tl_box_top
		add si,tl_capt_cy ;si = ������ �࠭�� ����
		mov eax,SF_PUT_IMAGE
;--- 横� ��� �ᮢ���� ���⨪��쭮� ����� ---
		.cycle1:
		sub dx,tl_img_cy ;��������� ���न���� y �����
		cmp dx,si
		jl @f
			cmp esi,0x10000
			jl @f
			int 0x40 ;draw line icon
			sub esi,0x10000 ;㬥��蠥� ���稪 ������
		jmp .cycle1
	@@:
	ret
endp

;input:
; edi = pointer to TreeInfo struct
;output:
; eax = rows
align 4
tl_get_rows_count:
	push ecx edx
		mov eax,tl_box_height
		sub ax,tl_capt_cy
		movzx ecx,tl_img_cy
		xor edx,edx
		div ecx
	pop edx ecx
	ret

;input:
; eax = node position
; ebx = [���न��� �� �� x]*65536 + [img_cx]
; ecx = [���न��� �� �� y]*65536 + [img_cy]
; edx = pointer to node struct
; edi = pointer to TreeInfo struct
align 4
proc tl_draw_node_caption uses ebx ecx edx esi
	xor esi,esi
	mov si,tl_info_size
	cmp si,tl_info_capt_offs
	jle @f ;if caption size <= 0
		push eax
			call tl_get_node_index ;eax = node index
			imul esi,eax
		pop eax
		add si,tl_info_capt_offs
		add esi,tl_data_info
		mov edx,esi

		shr ebx,16
		add bx,tl_img_cx ;ᤢ����� ������� �� ��ਧ��⠫� --->
		add bx,3 ;�����
		;bx = coord.x
		call tl_strlen ;eax = strlen
		call tl_get_draw_text_len
		mov cx,bx
		ror ecx,16
		mov ebx,ecx
		add bx,tl_img_cy ;��ࠢ������� �� ������ �࠭�� ������
		sub bx,9 ;�⭨���� ����� ⥪��
		mov ecx,tl_col_txt
		and ecx,0xffffff
		mcall SF_DRAW_TEXT
	@@:
	ret
endp

;input:
; eax = strlen
; ebx = text coord x
;output:
; esi = text len
align 4
proc tl_get_draw_text_len uses eax ecx edx
	mov esi,eax ;��६ ������ ��ப�
	mov eax,tl_box_left
	add eax,tl_box_width
	cmp eax,ebx
	jle .text_null ;�᫨ ������� ��������� ��� �� �࠭��
		sub eax,ebx
		xor edx,edx
		mov ecx,6 ;�ਭ� ��⥬���� ����
		div ecx ;ᬮ�ਬ ᪮�쪮 ᨬ����� ����� ���������� �� �࠭�
		cmp esi,eax
		jl @f
			mov esi,eax ;�᫨ ������ ⥪�� �����, 祬 �� ���� ��� ��ப�
			jmp @f
	.text_null:
	xor esi,esi
	@@:
	ret
endp

;input:
; esi = pointer to string
;output:
; eax = strlen
align 4
tl_strlen:
	mov eax,esi
	@@:
		cmp byte[eax],0
		je @f
		inc eax
		jmp @b
	@@:
	sub eax,esi
	ret

;�������� 㧥�
;input:
; tlist - 㪠��⥫� �� �������� ����
; n_opt - ��樨 ����������
; n_info - 㪠��⥫� �� ������塞� �����
align 4
proc tl_node_add uses eax ebx ecx edx edi, tlist:dword, n_opt:dword, n_info:dword
	mov edi,[tlist]

	call tl_info_set_undo

	mov ebx,sizeof.TreeNode
	imul ebx,tl_info_max_count
	add ebx,tl_data_nodes
;--
	call tl_get_cur_node_index ;eax=po_t
	imul eax,sizeof.TreeNode
	add eax,tl_data_nodes
	mov edx,eax
	call tl_move_perv
	call tl_get_node_index ;eax = index of pointer [edx]
;--
	mov edx,sizeof.TreeNode
	shl edx,1
	add edx,tl_data_nodes
	@@: ;for(i=2;i<nodeMax;i++)
		cmp dword[edx+TreeNode.t_create],0
		jne .u0
		cmp dword[edx+TreeNode.t_delete],0
		jne .u0

		inc tl_ch_tim
		mov ecx,dword[n_opt]
		ror ecx,16 ;cx = type
		mov word[edx+TreeNode.type],cx
		rol ecx,8 ;cl = close|open
		mov byte[edx+TreeNode.close],cl ;node[i].clo
		mov byte[edx+TreeNode.level],0 ;node[i].lev=0
		bt tl_style,2 ;tl_list_box_mode
		jc .l_box_m
			mov cl,byte[n_opt]
			mov byte[edx+TreeNode.level],cl ;node[i].lev
		.l_box_m:
		push tl_ch_tim       ;node[i].tc=ch_tim;
		pop dword[edx+TreeNode.t_create]
		mov [edx+TreeNode.perv],eax ;node[i].perv=po_t;
		;*** copy node data ***
		push esi
		xor ecx,ecx
		mov cx,tl_info_size
		mov esi,ecx

		push eax
			call tl_get_node_index ;eax = node index
			imul esi,eax
		pop eax
		add esi,tl_data_info
		mov edi,dword[n_info] ;pointer to node data
		xchg edi,esi
		rep movsb

		mov esi,edi
		mov edi,[tlist] ;restore edi
		mov cx,tl_info_capt_offs
		cmp cx,tl_info_size
		jge .no_text_data
		cmp tl_info_capt_len,0 ;�஢��塞 ���� �� ��࠭�祭�� �� ������ ��ப�
		je .no_len_ogran
			add cx,tl_info_capt_len
			and ecx,0xffff
			add esi,ecx
			mov cx,tl_info_size
			sub esi,ecx
		.no_len_ogran:
		dec esi
		mov byte[esi],0
		.no_text_data:
		pop esi ;restore esi

		mov ecx,eax
		imul ecx,sizeof.TreeNode
		add ecx,tl_data_nodes ; *** ecx = node[po_t] ***
		add ecx,TreeNode.next ; *** ecx = node[po_t].next ***
		push dword[ecx] ;node[i].next=node[po_t].next;
		pop dword[edx+TreeNode.next]

		call tl_get_node_index ;*** eax = i ***
		cmp eax,tl_info_max_count
		jge .u0
			mov [ecx],eax ;node[po_t].next=i; // ��뫪� ��७��ࠢ�塞
			mov ecx,[edx+TreeNode.next] ; *** ecx = node[i].next ***
			imul ecx,sizeof.TreeNode
			add ecx,tl_data_nodes ; *** ecx = node[node[i].next] ***
			mov [ecx+TreeNode.perv],eax ;node[node[i].next].perv=i;

			call tb_scrol_resize ;��ࠡ�⪠ �஫�����
			jmp @f
		.u0:
		add edx,sizeof.TreeNode
		cmp edx,ebx ;enf of node memory ?
		jle @b
	@@:
	ret
endp

;input:
; edi = pointer to TreeInfo struct
align 4
proc tb_scrol_resize uses eax ecx edx
	cmp tl_p_scroll,0 ;��ࠡ�⪠ �஫�����
	je @f
		call tl_get_node_count ;eax = node count
		mov ecx,eax
		call tl_get_rows_count
		cmp ecx,eax
		jg .ye_sb
			xor ecx,ecx
		.ye_sb:
		mov edx,tl_p_scroll
		mov dword[edx+sb_offs_cur_area],eax
		mov dword[edx+sb_offs_max_area],ecx
		stdcall scroll_bar_vertical.draw,edx
	@@:
	ret
endp

;input:
;n_info - pointer to node info
align 4
proc tl_node_set_data uses eax ecx edx edi esi, tlist:dword, n_info:dword
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		xor ecx,ecx
		mov cx,tl_info_size
		imul eax,ecx
		add eax,tl_data_info
		mov edi,eax
		mov esi,dword[n_info] ;pointer to node data
		rep movsb

		mov esi,edi
		mov edi,[tlist] ;restore edi
		mov cx,tl_info_capt_offs
		cmp cx,tl_info_size
		jge .no_text_data
			mov ax,tl_info_capt_len ;�஢��塞 ���� �� ��࠭�祭�� �� ������ ⥪��
			or ax,ax
			jz .no_limit
				add cx,ax ;cx = tl_info_capt_offs + tl_info_capt_len
				and ecx,0xffff
				xor eax,eax
				mov ax,tl_info_size
				cmp eax,ecx
				jl .no_limit ;���짮��⥫� ����� ᫨誮� ������� ������ ⥪��
				add esi,ecx
				sub esi,eax
			.no_limit:
			dec esi
			mov byte[esi],0 ;����塞 ��᫥���� ᨬ��� ������, ��-�� �� ���稫� �᫨ ���짮��⥫� ����� ���ࠢ����� ��������
		.no_text_data:
	@@:
	ret
endp

;����� 㪠��⥫� �� ����� 㧫� ��� ����஬
;input:
; tlist - pointer to 'TreeList' struct
;output:
; eax - pointer to node data
align 4
proc tl_node_get_data uses ecx edi, tlist:dword
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		movzx ecx,tl_info_size
		imul eax,ecx
		add eax,tl_data_info
		jmp .end_f ;return node data pointer
	@@:
	xor eax,eax
	.end_f:
  ret
endp

;����� 㪠��⥫� �� �������� 㧫� � 㪠������ ����樨
;input:
; tlist - pointer to 'TreeList' struct
; node_ind - node index
;output:
; eax - pointer to node info
align 4
proc tl_node_poi_get_info uses ebx ecx edx edi, tlist:dword, node_ind:dword
	mov edi,[tlist]
	mov ebx,dword[node_ind]

	;cycle to nodes
	mov edx,tl_data_nodes
	mov ecx,edx
	add ecx,sizeof.TreeNode
	@@:
		call tl_iterat_next_all
		cmp edx,ecx
		jle @f
		dec ebx
		cmp ebx,0
		jg @b
		jmp .find
	@@:
		xor edx,edx
	.find:
	mov eax,edx
	ret
endp

;����� 㪠��⥫� �� ᫥����� �������� 㧫�
;input:
; tlist - pointer to 'TreeList' struct
; node_p - node param struct
;output:
; eax - pointer to next node struct
align 4
proc tl_node_poi_get_next_info uses ecx edx edi, tlist:dword, node_p:dword
	mov edi,[tlist]
	mov edx,dword[node_p]

	mov ecx,tl_data_nodes
	add ecx,sizeof.TreeNode

	call tl_iterat_next_all
	cmp edx,ecx
	jg @f
		xor edx,edx
	@@:
	mov eax,edx
	ret
endp

;����� 㪠��⥫� �� ����� 㧫�
;input:
; tlist - pointer to 'TreeList' struct
; node_p - node param struct
;output:
; eax - pointer
align 4
proc tl_node_poi_get_data uses ecx edx edi, tlist:dword, node_p:dword
	mov edi,[tlist]
	mov edx,dword[node_p]

	call tl_get_node_index ;eax = node index
	cmp eax,2
	jl @f
		xor ecx,ecx
		mov cx,tl_info_size
		imul eax,ecx
		add eax,tl_data_info
		jmp .end_f ;return node data pointer
	@@:
		xor eax,eax ;�����頥� 0 � ��砥 �� 㤠筮�� ���᪠
	.end_f:
	ret
endp

;���� ������ ��� ����஬
;input:
; edi = pointer 'tl' struct
;output:
; eax = index of current node
align 4
proc tl_get_cur_node_index uses ecx edx
	;cycle to nodes
	xor eax,eax
	mov edx,tl_data_nodes
	mov ecx,edx
	add ecx,sizeof.TreeNode
	@@:
		call tl_iterat_next
		cmp edx,ecx
		jle @f
		cmp eax,tl_cur_pos
		je @f
		inc eax
		jmp @b
	@@:
	mov eax,edx
	sub eax,tl_data_nodes
	xor edx,edx
	mov ecx,sizeof.TreeNode
	div ecx
	ret
endp

;���� ������ 㪠������� ᨬ����
;input:
; edx = pointer node memory
; edi = pointer 'tl' struct
;output:
; eax = struct index of current node
align 4
tl_get_node_index:
	push ecx edx
	mov eax,edx
	sub eax,tl_data_nodes
	xor edx,edx
	mov ecx,sizeof.TreeNode
	div ecx
	pop edx ecx
	ret

;㤠���� 㧥�
align 4
proc tl_node_delete uses eax edx edi, tlist:dword
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		imul eax,sizeof.TreeNode
		add eax,tl_data_nodes
		mov edx,eax
		inc tl_ch_tim
		mov eax,tl_ch_tim
		mov dword[edx+TreeNode.t_delete],eax
		call tb_scrol_resize ;��ࠡ�⪠ �஫�����
	@@:
	ret
endp

;���⠢��� ����� �� ���� 㧥�
align 4
proc tl_cur_beg uses edi, tlist:dword
	mov edi,[tlist]
	mov tl_cur_pos,0
	cmp tl_p_scroll,0
	je @f
		mov edi,tl_p_scroll
		mov dword[edi+sb_offs_position],0
		stdcall scroll_bar_vertical.draw, edi
	@@:
	ret
endp

;��७��� ����� �� 1 ������ ����
align 4
proc tl_cur_next uses eax ebx edi esi, tlist:dword
	mov edi,[tlist]
	call tl_get_node_count ;eax = node count
	bt tl_style,3 ;tl_cursor_pos_limited
	jnc @f
	or eax,eax
	jz @f
		dec eax ;�᫨ ����� �⠥� �� �������騥 㧫�
	@@:
	cmp tl_cur_pos,eax
	jge .no_redraw
		mov esi,tl_box_top
		add esi,tl_box_height ;esi = coord bottom border
		call tl_draw_null_cursor ;��ࠥ� �����
		inc tl_cur_pos

		cmp tl_p_scroll,0 ;if not scrol struct
		je @f
		call tl_get_rows_count ;eax = rows count
		mov ebx,tl_p_scroll
		add eax,dword[ebx+sb_offs_position]
		cmp tl_cur_pos,eax
		jl @f
			inc dword[ebx+sb_offs_position]
			stdcall scroll_bar_vertical.draw,ebx
			stdcall tl_draw,dword[tlist] ;������ ����ᮢ�� ����
			jmp .no_redraw
		@@:
		mov edi,[tlist] ;restore edi
		call tl_draw_cursor ;����ᮢ�� �����
		call tl_draw_caption_cur_pos
	.no_redraw:
	ret
endp

;���� �᫮ ��� ������� 㧫�� (�� ���� �������� ���୨�)
;input:
; edi = pointer 'tl' struct
;output:
; eax = struct index of current node
align 4
tl_get_node_count:
push ecx edx
	;cycle to nodes
	xor eax,eax
	mov edx,tl_data_nodes
	mov ecx,edx
	add ecx,sizeof.TreeNode
	@@:
		call tl_iterat_next
		cmp edx,ecx
		jle @f
		inc eax
		jmp @b
	@@:
pop edx ecx
	ret

;���� �᫮ ��� ������� 㧫�� (���� ������� ���୨�)
;input:
; edi = pointer 'tl' struct
;output:
; eax = struct index of current node
align 4
tl_get_node_count_all:
push ecx edx
	;cycle to nodes
	xor eax,eax
	mov edx,tl_data_nodes
	mov ecx,edx
	add ecx,sizeof.TreeNode
	@@:
		call tl_iterat_next_all
		cmp edx,ecx
		jle @f
		inc eax
		jmp @b
	@@:
pop edx ecx
	ret

;��७��� ����� �� 1 ������ ���
align 4
proc tl_cur_perv uses eax edi esi, tlist:dword
	mov edi,[tlist]
	cmp tl_cur_pos,0
	je .no_redraw
		mov esi,tl_box_top
		add esi,tl_box_height ;esi = coord bottom border
		call tl_draw_null_cursor ;��ࠥ� �����
		dec tl_cur_pos ;������� ����� �����

		cmp tl_p_scroll,0 ;�᫨ ���� 㪠��⥫� �� �஫����
		je @f
		mov eax,tl_p_scroll
		cmp dword[eax+sb_offs_position],0 ;�᫨ �஫���� �� �����, ��室��
		je @f
		mov edi,tl_cur_pos
		cmp edi,dword[eax+sb_offs_position] ;�᫨ ����� �襫 ��� �஫�����, ⮣�� ���᪠�� �஫����
		jge @f
			dec dword[eax+sb_offs_position]
			stdcall scroll_bar_vertical.draw, eax
			stdcall tl_draw, dword[tlist] ;������ ����ᮢ�� ����
			jmp .no_redraw
		@@:
			mov edi,[tlist] ;restore edi
			call tl_draw_cursor ;����ᮢ�� �����
			call tl_draw_caption_cur_pos
	.no_redraw:
	ret
endp

;��७��� ����� �� 1 ��࠭��� ���
align 4
proc tl_cur_page_up uses eax edi esi, tlist:dword
	mov edi,[tlist]

	cmp tl_p_scroll,0 ;�᫨ ���� 㪠��⥫� �� �஫����
	je .no_redraw
		mov esi,tl_p_scroll
		call tl_get_rows_count ;eax = rows count

		cmp tl_cur_pos,0
		jne @f
		cmp dword[esi+sb_offs_position],0 ;�᫨ �஫���� �� �����, ��室��
		jne @f
			jmp .no_redraw
		@@:
		cmp tl_cur_pos,eax ;�஢��塞 ������ ����� � ���-�� �⮪ �� ��࠭��
		jl @f ;�᫨ �����, � ��ࠢ������ � 0, ��-�� �� ����� ����� 祬 ����
			sub tl_cur_pos,eax
			jmp .cursor
		@@:
			mov tl_cur_pos,0
		.cursor:
		cmp dword[esi+sb_offs_position],eax
		jl @f
			sub dword[esi+sb_offs_position],eax
			jmp .scroll
		@@:
		mov dword[esi+sb_offs_position],0
		.scroll:
		;����ᮢ�� ���� � �஫�����
		stdcall tl_draw, edi ;draw window
		stdcall scroll_bar_vertical.draw, esi
	.no_redraw:
	ret
endp

;��७��� ����� �� 1 ��࠭��� ����
align 4
proc tl_cur_page_down uses eax ebx ecx edi esi, tlist:dword
;eax - ���-�� ��ப �� ��࠭��
;ebx - ����. ������ �����
;ecx - ����. ������ �஫�����
	mov edi,[tlist]
	cmp tl_p_scroll,0 ;�᫨ ���� 㪠��⥫� �� �஫����
	je .no_redraw
		mov esi,tl_p_scroll
		call tl_get_node_count ;eax = node count
		bt tl_style,3 ;tl_cursor_pos_limited
		jnc @f
		or eax,eax
		jz @f
			dec eax ;�᫨ ����� �⠥� �� �������騥 㧫�
		@@:
		mov ebx,eax
		call tl_get_rows_count ;eax = rows count

		mov ecx,ebx
		inc ecx ;�᫨ ������ 㧥� ����� �� ��������
		cmp ecx,eax ;if (ecx>eax) { ecx=ecx-eax } else { ecx=0 }
		jl @f
			sub ecx,eax ;㬥��蠥� ���ᨬ����� ������ �஫�����, ⠪ ��-�� �뫨 ����� ��᫥���� 㧫�
			jmp .control
		@@:
			xor ecx,ecx ;ecx=0 - �� 㧫� ������ � �࠭, �஫���� �� �㦥�
		.control:

		cmp tl_cur_pos,ebx ;����� ����� ?
		jl @f
		cmp dword[esi+sb_offs_position],ecx ;�஫���� ����� ?
		jl @f
			jmp .no_redraw
		@@:
		add tl_cur_pos,eax ;��६�頥� �����
		cmp tl_cur_pos,ebx
		jl @f
			mov tl_cur_pos,ebx
		@@:
		add dword[esi+sb_offs_position],eax ;��६�頥� �஫����
		cmp dword[esi+sb_offs_position],ecx
		jl @f
			mov dword[esi+sb_offs_position],ecx
		@@:

		;����ᮢ�� ���� � �஫�����
		stdcall tl_draw, edi ;draw window
		stdcall scroll_bar_vertical.draw, esi
	.no_redraw:
	ret
endp

;������/������� 㧥� (ࠡ�⠥� � 㧫��� ����� ����� ���୨� 㧫�)
align 4
proc tl_node_close_open uses eax edx edi, tlist:dword
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax = ������ 㧫� �� ���஬ �⮨� �����
	cmp eax,2 ;����� �⮨� �� 㧫� ?
	jl @f
		imul eax,sizeof.TreeNode
		add eax,tl_data_nodes
		;eax = 㪠��⥫� �� �������� 㧫� ��࠭���� ����஬
		push eax
			stdcall tl_node_poi_get_next_info, edi,eax
			mov edx,eax ;edx = 㪠��⥫� �� �������� 㧫� ����� ���� ��᫥ 㧫� eax
		pop eax
		or edx,edx ;���� �� 㧫� ���� ��࠭���� ���� ?
		jz @f
			mov dl,byte[edx+TreeNode.level] ;��६ �஢��� ������� 㧫�
			cmp byte[eax+TreeNode.level],dl
			jge @f ;�᫨ ������ 㧫� ����襣� �஢��, ����� ��� �� ���୨�, ����� �㭪樨
				xor byte[eax+TreeNode.close],1 ;*** ����⨥/�����⨥ 㧫� ***
				call tb_scrol_resize ;��ࠡ�⪠ �஫�����

				stdcall tl_draw, edi ;���������� ����
	@@:
	ret
endp

;㢥����� �஢���
align 4
proc tl_node_lev_inc uses eax ecx edx edi, tlist:dword
	mov edi,[tlist]
	bt tl_style,2 ;tl_list_box_mode
	jc @f
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		mov ecx,tl_data_nodes
		imul eax,sizeof.TreeNode
		add eax,ecx ;eax = pointer to node struct
		add ecx,sizeof.TreeNode ;ecx = pointer to 1 node struct

		mov edx,eax
		call tl_iterat_perv ;�஢��塞 ���� �� ���孨� 㧥�
		cmp edx,ecx
		jle @f ;�᫨ ���孥�� 㧫� ��� � ⥪�騩 㧥� �� �������
		mov cl,byte[edx+TreeNode.level] ;��६ �஢��� த�⥫�᪮�� 㧫�
		inc cl ;������塞 1 � ����砥� ���ᨬ��쭮� ���祭��
		cmp byte[eax+TreeNode.level],cl
		jge @f
			inc byte[eax+TreeNode.level] ;㢥��稢��� ���祭�� 㧫�
	@@:
	ret
endp

;㬥����� �஢���
align 4
proc tl_node_lev_dec uses eax edi, tlist:dword
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		imul eax,sizeof.TreeNode
		add eax,tl_data_nodes
		cmp byte[eax+2],0
		je @f
		dec byte[eax+2]
	@@:
	ret
endp

;��६�頥� 㧥� �����
align 4
proc tl_node_move_up tlist:dword
pushad
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		mov ebx,eax ;copy index of node struct
		mov edx,tl_data_nodes
		mov ecx,edx
		add ecx,sizeof.TreeNode
		imul eax,sizeof.TreeNode
		add eax,edx ;eax = pointer to 2 node struct
		mov edx,eax ;edx = pointer to 2 node struct
		mov esi,eax ;esi = pointer to 2 node struct
		call tl_iterat_perv ;edx = pointer to 1 node struct
		call tl_get_node_index ;eax = index of 1 node struct
		cmp edx,ecx
		jle @f
			cmp dword[edx+TreeNode.next],ebx
			jne .po8
				call tl_node_move_po6 ;㧫� ���� ����� ���塞 6 ��뫮�
				jmp .cur_mov
			.po8:
				call tl_node_move_po8 ;㧫� ���� �� ����� ���塞 8 ��뫮�
			.cur_mov:
				push dword edi
				call tl_cur_perv
				push dword edi
				call tl_draw
	@@:
popad
	ret
endp

;��६�頥� 㧥� ����
align 4
proc tl_node_move_down tlist:dword
pushad
	mov edi,[tlist]
	call tl_get_cur_node_index ;eax=po_t
	cmp eax,2
	jl @f
		mov ebx,eax ;copy index of node struct
		mov edx,tl_data_nodes
		mov ecx,edx
		add ecx,sizeof.TreeNode
		imul eax,sizeof.TreeNode
		add eax,edx ;eax = pointer to 1 node struct
		mov edx,eax ;edx = pointer to 1 node struct
		mov esi,eax ;esi = pointer to 1 node struct
		call tl_iterat_next ;edx = pointer to 2 node struct
		call tl_get_node_index ;eax = index of 2 node struct
		cmp edx,ecx
		jle @f
			cmp dword[esi+TreeNode.next],eax
			jne .po8
				xchg eax,ebx ;���� ���冷� ᫥������� �����塞�� 㧫��
				xchg edx,esi
				call tl_node_move_po6 ;㧫� ���� ����� ���塞 6 ��뫮�
				jmp .cur_mov
			.po8: ;� ��� ���冷� ᫥������� 㧫�� �� ���塞
				call tl_node_move_po8 ;㧫� ���� �� ����� ���塞 8 ��뫮�
			.cur_mov:
				stdcall tl_cur_next, edi
				stdcall tl_draw, edi
	@@:
	ret
popad
endp

align 4
tl_node_move_po6:
	mov ecx,edx ;save node pointer
	call tl_move_perv
	mov dword[edx+TreeNode.next],ebx

	mov edx,esi
	call tl_move_next
	mov dword[edx+TreeNode.perv],eax
	mov edx,ecx ;restore node pointer

	mov ecx,dword[edx+TreeNode.perv]
	mov dword[esi+TreeNode.perv],ecx
	mov ecx,dword[esi+TreeNode.next]
	mov dword[edx+TreeNode.next],ecx

	mov dword[edx+TreeNode.perv],ebx
	mov dword[esi+TreeNode.next],eax
	ret

;input
;eax = index 1 node struct
;ebx = index 2 node struct
;edx = pointer 1 node struct
;esi = pointer 2 node struct
;edi = pointer to 'TreeList' struct
;output:
;eax = ?
;ebx = ?
;ecx = ?
align 4
tl_node_move_po8:
	mov ecx,edx ;save node pointer
	call tl_move_perv
	mov dword[edx+TreeNode.next],ebx
	mov edx,ecx
	call tl_move_next
	mov dword[edx+TreeNode.perv],ebx
	mov edx,esi
	call tl_move_perv
	mov dword[edx+TreeNode.next],eax
	mov edx,esi
	call tl_move_next
	mov dword[edx+TreeNode.perv],eax
	mov edx,ecx ;restore node pointer

	mov eax,dword[edx+TreeNode.perv]
	mov ebx,dword[esi+TreeNode.perv]
	xchg eax,ebx
	mov dword[edx+TreeNode.perv],eax
	mov dword[esi+TreeNode.perv],ebx

	mov eax,dword[edx+TreeNode.next]
	mov ebx,dword[esi+TreeNode.next]
	xchg eax,ebx
	mov dword[edx+TreeNode.next],eax
	mov dword[esi+TreeNode.next],ebx
	ret

;input:
; edi = pointer to 'TreeList' struct
align 4
tl_draw_caption_cur_pos:
	cmp tl_capt_cy,9 ;9 - minimum caption height
	jl @f
	pushad
		mov ebx,edi ;calculate cursor position
		mov eax,tl_cur_pos
		inc eax
		lea edi,[txt_capt_cur.v]
		stdcall tl_convert_to_str, 5
		mov edi,ebx

		mov ebx,tl_box_left
		shl ebx,16
		add ebx,5*65536+3
		add ebx,tl_box_top
		mov ecx,tl_col_txt
		or  ecx,0xc0000000 ;0x40000000 ����訢��� 䮭 梥⮬ edi
		lea edx,[txt_capt_cur]
		mov edi,tl_col_zag
		mcall SF_DRAW_TEXT ;captions
	popad
	@@:
	ret

;input:
; tlist - pointer to 'TreeList' struct
; opt - options: 0 - first element, 1 - add next element
; h_mem - pointer to memory
; mem_size - memory size
;output:
; eax - error code
align 4
proc tl_save_mem uses ebx ecx edx edi esi, tlist:dword, opt:dword, h_mem:dword, mem_size:dword
	mov esi,[h_mem]
	mov edi,[tlist]

	cmp dword[opt],0 ;add mode
	je @f
		stdcall tl_get_mem_size, edi,esi ;eax = ࠧ��� ࠭�� ��࠭��� ������
		add esi,eax
	@@:

	xor ebx,ebx
	mov bx,tl_info_size

	call tl_get_node_count_all ;eax = all node count

	mov ecx,eax  ;����塞 ᪮�쪮 ����� ������ ���� ���������
	imul ecx,ebx ;㬭����� �� ࠧ��� �������� 㧫�
	add ecx,tl_save_load_heder_size+1 ;element header +1 end element sumbol
	add ecx,esi  ;������塞 㪠��⥫� �� ��砫� ����� (� ��⮬ ࠭�� ����ᠭ��� �������)
	sub ecx,dword[h_mem] ;�⭨���� 㪠��⥫� �� ��砫� ����� (��� ࠭�� ����ᠭ��� �������)
	cmp ecx,dword[mem_size] ;ecx = element memory size
	jg .err_mem_size

		;save tree params (in header)
		mov dword[esi],'tree'
		mov word[esi+4],bx
		mov dword[esi+6],eax ;element count

		mov eax,tl_style
		mov dword[esi+10],eax

		mov eax,tl_cur_pos
		mov dword[esi+14],eax

		mov ax,tl_info_capt_offs
		mov word[esi+18],ax

		mov ax,tl_info_capt_len
		mov word[esi+20],ax

		;copy scroll position
		mov edx,tl_p_scroll
		mov eax,dword[edx+sb_offs_position]
		mov dword[esi+22],eax

		add esi,tl_save_load_heder_size ;add header size

		;cycle to nodes
		mov edx,tl_data_nodes
		mov ecx,edx
		add ecx,sizeof.TreeNode
		@@:
			call tl_iterat_next_all
			cmp edx,ecx
			jle @f
		;save node params
		call tl_get_node_index ;eax = index of pointer [edx]
		mov dword[esi],eax

		add esi,4
		mov eax,dword[edx+TreeNode.type] ;eax = (type; level; close)
		mov dword[esi],eax
		add esi,4

			stdcall tl_node_poi_get_data, edi,edx ;eax - pointer node data

		;call tl_node_copy_data
		push ecx edi
		mov edi,eax
		mov ecx,ebx
		xchg esi,edi
		rep movsb
		mov esi,edi
		pop edi ecx
		;add esi,ebx
		jmp @b
	@@:
	mov byte[esi],0 ;end of 'treelist'
		xor eax,eax ;return error code
	jmp @f
	.err_mem_size:
		mov eax,tl_err_save_memory_size
	@@:
	ret
endp

;input:
; tlist - pointer to 'TreeList' struct
; opt   - options: element index + (2*(add mode)+(init mode)) shl 16
; h_mem - pointer to memory
; mem_size - memory size
;   ࠧ��� �����, ���� �� �ᯮ������ (�����砫�� ��� ����஫�)
;   ��� ��� �ᯮ�짮����� �㦭� ��ࠡ���� �㭪��
;output:
; eax - error code
;memory header format:
;  +0 - (4) 'tree'
;  +4 - (2) info size
;  +6 - (4) count nodes
; +10 - (4) tlist style
; +14 - (4) cursor pos
; +18 - (2) info capt offs
; +20 - (2) info capt len
; +22 - (4) scroll pos
;memory data format:
; +26 - (info size + 8) * count nodes
align 4
proc tl_load_mem uses ebx ecx edx edi esi, tlist:dword, opt:dword, h_mem:dword, mem_size:dword 
locals
	er_code dd ?
endl
	mov esi,[h_mem]
	mov edi,[tlist]

	mov dword[er_code],0 ;return error code

	mov ecx,[opt]
	or cx,cx ;load in array mode
	jz @f
		;stdcall tl_get_mem_size, esi,edi ;��६ ࠧ��� ࠭�� ��࠭��� ������
		;add esi,eax
		and ecx,0xffff
		cld
		.beg_cycle:
			cmp dword[esi],'tree'
			jne .no_tree
			movzx ebx,word[esi+4]
			add bx,8
			imul ebx,dword[esi+6]
			add ebx,tl_save_load_heder_size
			add esi,ebx
			loop .beg_cycle
	@@:

	cmp dword[esi],'tree'
	jne .no_tree
		bt dword[opt],17 ;load in add mode
		jc @f
			stdcall tl_info_clear, edi
		@@:

		xor ebx,ebx
		mov bx,word[esi+4] ;info_size
		cmp bx,tl_info_size
		je @f
			or dword[er_code],tl_err_load_info_size
		@@:
		mov ecx,[esi+6] ;count nodes
		cmp ecx,1
		jl .end_f
		mov edx,esi ;save header pointer
		add esi,tl_save_load_heder_size

		cld
		@@: ;load node params
			mov eax,dword[esi+4]
			ror eax,16 ;eax - options (type; level; close)
			add esi,8
			stdcall tl_node_add, edi,eax,esi
			stdcall tl_cur_next, edi
;...
			add esi,ebx
			loop @b

		bt dword[opt],17 ;load in add mode
		jc .no_tree
			mov eax,dword[edx+14] ;set cursor pos
			mov tl_cur_pos,eax
			mov ebx,tl_p_scroll
			or ebx,ebx
			jz .end_f
				mov eax,dword[edx+22] ;set scroll pos
				mov dword[ebx+sb_offs_position],eax
				stdcall scroll_bar_vertical.draw, ebx
				jmp .end_f
	.no_tree:
	mov dword[er_code],tl_err_load_caption
	.end_f:
	mov eax,dword[er_code]
	ret
endp

;���� ࠧ��� ����� ����⮩ �㭪樥� tl_save_mem �� ��࠭���� ����⮢
;output:
; eax - error code
align 4
proc tl_get_mem_size uses ebx edi, tlist:dword, h_mem:dword 
	mov edi,[tlist]
	mov eax,[h_mem]
	@@:
		cmp dword[eax],'tree'
		jne @f
		xor ebx,ebx
		mov bx,word[eax+4]
		add bx,8 ;ࠧ��� �������⥫쭮� ���ଠ樨 �� 㧫� (������ �����; ������ ������, �஢���, ...)
		imul ebx,dword[eax+6]
		add ebx,tl_save_load_heder_size
		add eax,ebx
		jmp @b
	@@:
	sub eax,dword[h_mem] ;�⭨���� 㪠��⥫� �� ��砫� �����
		;� ����砥� ࠧ��� ����� �����
	ret
endp


;ascii scan key
;  13    28 Enter
;  32    57 Space
; 178    72 Up
; 177    80 Down
; 176    75 Left
; 179    77 Right
; 182    83 Delete
; 184    73 Pg Up
; 183    81 Pg Dn

tl_key_ascii db 13,32,178,177,176,179,182,184,183
tl_key_scan  db 28,57, 72, 80, 75, 77, 83, 73, 81

txt_capt_cur: db '��ப� '
.v: db '     ',0
txt_capt_otm: db '�⬥�� '
.v: db '     ',0

;description:
; ��ॢ�� �᫠ � ASCII ��ப�
;input:
; eax = value
; edi = string buffer
align 4
proc tl_convert_to_str, len:dword
pushad
	mov esi,[len]
	add esi,edi
	dec esi
	call .str
popad
	ret
endp

align 4
.str:
	mov ecx,10 ;�������� ��⥬� ��᫥���
	cmp eax,ecx  ;�ࠢ���� �᫨ � eax ����� 祬 � ecx � ��३� �� @@-1 �.�. �� pop eax
	jb @f
		xor edx,edx ;������ edx
		div ecx     ;ࠧ������ - ���⮪ � edx
		push edx    ;�������� � �⥪
		call .str   ;�맢��� ᠬ� ᥡ� � ⠪ �� ⮣� ������ ���� � eax �� �⠭�� ����� 祬 � ecx
		pop eax
	@@:
	cmp edi,esi
	jge @f
		or al,0x30  ;������塞 ᨬ��� '0'
		stosb       ;������� al � �祪� ����� [edi]
		mov byte[edi],0
	@@:
	ret
