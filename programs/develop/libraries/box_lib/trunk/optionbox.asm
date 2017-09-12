; ����� Optionbox ��� ������⥪� box_lib.obj
; �� ��� �ਬ����� GPL2 ��業���
; 䠩� ᮧ��� 13.02.2009 <Lrz>
; ��᫥���� ����䨪��� 12.09.2017 IgorA

align 16
option_box_draw:
	pusha

	mov   eax,dword[esp+36]
@@:
	mov   edi,dword[eax]
	test  edi,edi
	je    option_check
	call  option_box_draw_box
	add   eax,4
	jmp   @b 

option_check:
	mov   eax,dword[esp+36]
@@:
	mov   edi,dword[eax]
	test  edi,edi
	je    @f

	mov   ebx,dword[edi]
	mov   ebx,dword[ebx]
	cmp   edi,ebx
	jne   .clear_op

	pusha
	call  option_box_draw_op
	popa
	add   eax,4
	jmp   @b

.clear_op:
	pusha
	call  option_box_clear_op
	popa
	add   eax,4
	jmp   @b

@@:
	popa
	ret 4 


align 16
option_box_mouse:
	pusha
	mcall SF_MOUSE_GET,SSF_BUTTON
	test  eax,eax
	jnz @f
	popa
	ret 4

@@:
	mov   eax,dword[esp+36]
@@:
	mov   edi,dword[eax]
	test  edi,edi
	je    option_check
	call  option_box_mouse_op
	add   eax,4
	jmp   @b 

align 16
option_box_draw_box:
	pusha   ;��࠭�� �� ॣ�����

	movzx ebx,word op_left 
	shl   ebx,16 
	add   ebx,dword op_size 
	movzx ecx,word op_top 
	shl   ecx,16 
	add   ecx,dword op_size 
	mcall SF_DRAW_RECT,,,op_border_color ;��㥬 ࠬ�� 

	add   ebx,1 shl 16 - 2 
	add   ecx,1 shl 16 - 2 
	mcall ,,,op_color ;����訢��� ����७���� 祪���� 

;	mov   eax,dword option_group
;	mov   eax,[eax]
;	cmp   eax,edi
;	jne   @f
;	call  option_box_draw_op  ;���ᮢ��� ����祭�� 祪 ����


;----------------------------
;���� �㤠 �㤥� �ந������ �뢮� ⥪��
;----------------------------
@@:
	movzx ebx,word op_left ;����㧨�� ���祭�� � ��� 祪 ����
	add   ebx,dword op_size
	add   ebx,dword op_text_margin ;�������� ࠧ��� ��஭� � ����ﭨ� �� ���஬ ��筥��� �뢮� ⥪��
	shl   ebx,16       ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
	mov   bx,word op_top ;����㧨� ���祭�� �� y
	mov   eax,op_size
	shr   eax,1
	add   eax,4
	add   ebx,eax
	sub   ebx,7        ;������� ���祭�� ����� ��஭� -9+2
	mov   esi,dword op_text_length
	mcall SF_DRAW_TEXT,,op_text_color,op_text_ptr ;�뢮� ������ �� �����
	popa               ;����⠭����� ���祭�� ॣ���஢ �� �⥪�
	ret                ;�멤�� �� ��楤���

option_box_clear_op:   ;���⪠ 祪 ����
	mov   edx,dword op_color ;梥� ����� 祪 ����
	jmp   @f         ;����᫮��� ��릮� �� ������ ���� @@


option_box_draw_op:    ;���ᮢ��� ����祭�� 祪 ����
	mov   edx,dword op_border_color ;����㧨�� 梥�
@@:
	movzx ebx,word op_left ;����㧨�� ���न���� �� �
	mov   eax,dword op_size
	mov   bp,ax
	shr   eax,2
	push  ax

	push  ax
	add   ebx,eax           
	shl   ebx,16       ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
	pop   ax
	lea   eax,[eax*2]
	sub   bp,ax        ;��࠭�� ॣ���� bx � ॣ���� 㪠��⥫� ����
	mov   bx,bp

	movzx ecx,word op_top ;����㧨�� ���न���� �� �
	pop   ax
	add   cx,ax         
	shl   ecx,16       ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
	mov   cx,bp        ;����㧨� ���祭�� ॣ���� 㪠��⥫� ���� � cx
	mcall SF_DRAW_RECT ;�뢮� ������ �.�. �� ��� ��אַ㣮�쭨��, ����� �⮡ࠦ��� ����祭�� ��������� 祪 ����
	ret                ;��� �� ��楤���


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;��ࠡ��稪 mouse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
align 16
option_box_mouse_op:                 ;��ࠡ�⪠ ��� 
pusha
	mov   esi,dword op_text_length ;����㧨�� ���-�� ᨬ����� � ⥪�⮢�� ��ப�
	imul  esi,6                ;��� ����� � ⠪ 㬭����� �� 6
	add   esi,dword op_text_margin ;�������� 3 - ����ﭨ� �� 祪 ���� �� ������

	mcall SF_MOUSE_GET,SSF_WINDOW_POSITION ;������� ���न���� ����� �⭮�⥫쭮 ���� 

	movzx ebx,word op_top      ;����㧨�� � bx ���祭�� ���न���� �
	cmp   ax,bx                ;�ࠢ���� � � ���न��⮩ �����
	jl    option_box_mouse_end ;SF <> OF �᫨ ����� 
	add   ebx,dword op_size    ;�������� ࠧ��� 
	cmp   ax,bx                ;�ࠢ����
	jg    option_box_mouse_end ;ZF = 0 � SF = OF �᫨ ����� 

	shr   eax,16               ;ࠧ����� �� 65536 ��� ���� ᤢ���� ���� �� 16 ���祭��
	movzx ebx,word op_left     ;�ந������ �������筮� �ࠢ�����
	cmp   ax,bx                ;�ࠢ���� ॣ�����
	jl    option_box_mouse_end ;�᫨ �����
	add   ebx,dword op_size    ;�������� ����� ��஭� ��אַ㣮�쭨��
	add   ebx,esi              ;����� � ���祭�� �� � �� � ����� ������ � 祪�����
	cmp   ax,bx                ;��ࠢ���� ॣ�����
	jg    option_box_mouse_end ;�᫨ ����� 
	mov   eax,dword option_group
	mov   [eax],edi

option_box_mouse_end:
	popa                         ;����⠭����� ॣ����� �� �⥪�
	ret                          ;���
