;��������� OptionBox (�᭮��� �� Checkbox)
;��஬��� �������୮��� Maxxxx32, Diamond, Heavyiron � ��㣨� �ணࠬ���⠬, � �� �ணࠬ���, ���
;������ � �� ᬮ� �� ������� ��� ���������. 
;21.02.2007 ����୨���� � �����প� ���� ࠧ��� ��ਠ�⮢ � �ᯮ�짮������ �� 梥⮢ � ��ன �奬��
;19.02.2007 ��饥 ���襭�� ����, 㬥��襭�� ࠧ��� � �ᯮ�짮����� ��⥬��� 梥⮢ ��� �⮡ࠦ���� optionkbox
;16.02.2007 ��� ᮧ����� ���������
;<Lrz>  - ������ ����ᥩ  www.lrz.land.ru

;��������� �ਫ������
use32		; �࠭����, �ᯮ����騩 32 ࠧ�來�� �������
    org 0x0		; ������ ���� ����, �ᥣ�� 0x0
    db 'MENUET01'	; �����䨪��� �ᯮ��塞��� 䠩�� (8 ����)
    dd 0x1		; ����� �ଠ� ��������� �ᯮ��塞��� 䠩��
    dd start		; ����, �� ����� ��⥬� ��।��� �ࠢ�����
	                ; ��᫥ ����㧪� �ਫ������ � ������
    dd i_end		; ࠧ��� �ਫ������
    dd (i_end+0x100) and not 3	; ��ꥬ �ᯮ��㥬�� �����, ��� �⥪� �⢥��� 0�100 ���� � ��஢��� �� �୨�� 4 ����
    dd (i_end+0x100) and not 3	; �ᯮ����� ������ �⥪� � ������ �����, �ࠧ� �� ⥫�� �ணࠬ��. ���設� �⥪� � ��������� �����, 㪠������ ���
    dd 0x0,0x0		; 㪠��⥫� �� ��ப� � ��ࠬ��ࠬ�.
                 ;    �᫨ ��᫥ ����᪠ �� ࠢ�� ���, �ਫ������ �뫮
                 ;    ����饭� � ��ࠬ��ࠬ� �� ��������� ��ப�

		     ;    㪠��⥫� �� ��ப�, � ������ ����ᠭ ����,
                 ;    ��㤠 ����饭� �ਫ������
;------------------
	include	'..\..\..\..\macros.inc'
	include 'optionbox.inc'	;������� 䠩� opeck.inc

      version_op              ;��ਠ��, �� ���஬ �ᯮ������� 梥�, ����� ������ ���짮��⥫�
;	version_op1             ;梥� ������� �� ��⥬�
	use_option_box		;�ᯮ���� ������,����� ��楤��� ��� �ᮢ���� optionbox
align 16
;������� ����
start:				;��窠 �室� � �ணࠬ��
	mov  eax,48             ;������� ��⥬�� 梥�
	mov  ebx,3
	mov  ecx,sc
	mov  edx,sizeof.system_colors
	mcall

	mov	eax,40		;��⠭����� ���� ��� ��������� ᮡ�⨩
	mov 	ebx,0x25          ;��⥬� �㤥� ॠ��஢��� ⮫쪮 �� ᮮ�饭�� � ����ᮢ��,����� ������, ��।��񭭠� ࠭��, ᮡ�⨥ �� ��� (��-� ��稫��� - ����⨥ �� ������ ��� ��� ��६�饭��; ���뢠���� �� ���⥭��)
        mcall
red_win:
    call draw_window 		;��ࢮ��砫쭮 ����室��� ���ᮢ��� ����
still:				;�᭮���� ��ࠡ��稪 
     mov  eax,10 		      ;������� ᮡ���
     mcall      		;������� ᮡ�⨥ � �祭�� 2 �����ᥪ㭤
  
    cmp al,0x1    ;�᫨ ���������� ��������� ����
    jz red_win
    cmp al,0x3    ;�᫨ ����� ������ � ��३�
    jz button
     mouse_option_boxes option_boxes,option_boxes_end  ;�஢�ઠ 祪 ����      
    jmp still    ;�᫨ ��祣� �� ����᫥����� � ᭮�� � 横�
button:
;    mov eax,17		;������� �����䨪��� ����⮩ ������
;    mcall
;    test ah,ah		;�᫨ � ah 0, � ��३� �� ��ࠡ��稪 ᮡ�⨩ still
;    jz  still
    or eax,-1       ;� eax,-1 - 5 ,���⮢ � ��� �� ⮫쪮 3  ��� 
    mcall ;����� �믮������ ��室 �� �ணࠬ��

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
draw_window:		;�ᮢ���� ���� �ਫ������
    mov eax,12		;� ॣ���� ����� ���祭�� = 12
    mov ebx,1 		;��᢮��� 1
    mcall

    xor  eax,eax		;���㫨�� eax
    mov  ebx,50*65536+180	;[���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
    mov  ecx,30*65536+200	;[���न��� �� �� y]*65536 + [ࠧ��� �� �� y]
    mov  edx,[sc.work] 	         ; color of work area RRGGBB,8->color gl
    or   edx,0xb3000000
    mov  edi,hed
    mcall			;���ᮢ��� ���� �ਫ������

draw_option_boxes option_boxes,option_boxes_end ;�ᮢ���� 祪���ᮢ

    mov eax,12 			;�㭪�� 12 - �����/�������� ����ᮢ�� ����.
    mov ebx,2			;����㭪�� 2 - �������� ����ᮢ�� ����.
    mcall
    ret

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA ����� 
;��ଠ� ������ 祪 ����:
;10 - ���न��� 祪 ���� �� � 
;30 - ���न��� 祪 ���� �� �
;0 
;0 - 梥� ࠬ�� 祪 ����
;0 - 梥� ⥪�� ������
;op_text.1 - 㪠��⥫� �� ��砫� ��ப�
;option_group1 - �� �ਧ��� ��㯯�, �.�. ��� ��� ����� ��ࠡ��뢠�� ����� ��㯯 �� optibox
;op_text.e1-op_text.1 - ����� ��ப�
;
align 16
option_boxes:
;op1 option_box1 option_group1,10,15,op_text.1,op_text.e1-op_text.1
;op2 option_box1 option_group1,10,30,op_text.2,op_text.e2-op_text.2
;op3 option_box1 option_group1,10,45,op_text.3,op_text.e3-op_text.3
;op11 option_box1 option_group2,10,80,op_text.1,op_text.e1-op_text.1
;op12 option_box1 option_group2,10,95,op_text.2,op_text.e2-op_text.2
;op13 option_box1 option_group2,10,110,op_text.3,op_text.e3-op_text.3
;
;struc option_box point_gr,left,top,color,border_color,text_color,text,text_length
op1 option_box option_group1,10,15,0xffffff,0,0,op_text.1,op_text.e1-op_text.1
op2 option_box option_group1,10,30,0xFFFFFF,0,0,op_text.2,op_text.e2-op_text.2
op3 option_box option_group1,10,45,0xffffff,0,0,op_text.3,op_text.e3-op_text.3
op11 option_box option_group2,10,80,0xffffff,0,0,op_text.1,op_text.e1-op_text.1
op12 option_box option_group2,10,95,0xffffff,0,0,op_text.2,op_text.e2-op_text.2
op13 option_box option_group2,10,110,0xffffff,0,0,op_text.3,op_text.e3-op_text.3

option_boxes_end:

op_text:		; ���஢�����騩 ⥪�� ��� 祪 ���ᮢ
.1 db 'Option_Box #1' 
.e1:
.2 db 'Option_Box #2'
.e2:
.3 db 'Option_Box #3'
.e3:

option_group1	dd op1  ;㪠��⥫�, ��� �⮡ࠦ����� �� 㬮�砭��, ����� �뢮����� 
option_group2	dd op11 ;�ਫ������

hed db 'Optionbox [21.02.2007]',0	;��������� �ਫ������
sc     system_colors
i_end:			;����� ����