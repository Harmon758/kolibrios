;��������� OptionBox (�᭮��� �� Checkbox)
;��஬��� �������୮��� Maxxxx32, Diamond, � ��㣨� �ணࠬ���⠬, � �� �ணࠬ���, ���
;������ � �� ᬮ� �� ������� ��� ���������. 
;16.02.2007 
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
                 ;    �᫨ ��᫥ ����᪠ ��ࠢ�� ���, �ਫ������ �뫮
                 ;    ����饭� � ��ࠬ��ࠬ� �� ��������� ��ப�

		 ;    㪠��⥫� �� ��ப�, � ������ ����ᠭ ����,
                 ;    ��㤠 ����饭� �ਫ������
;------------------
	include	'macros.inc'
	include 'optionbox.inc'	;������� 䠩� check.inc
	use_option_box		;�ᯮ���� ������ ����� ��楤��� ��� �ᮢ���� 祪 ����
align 16
;������� ����
start:				;��窠 �室� � �ணࠬ��
	mov  eax,48             ;������� ��⥬�� 梥�
	mov  ebx,3
	mov  ecx,sc
	mov  edx,sizeof.system_colors
	int  0x40

	mov	eax,40		;��⠭����� ���� ��� ��������� ᮡ�⨩
	mov 	ebx,0x25        ;��⥬� �㤥� ॠ��஢��� ⮫쪮 �� ᮮ�饭�� � ����ᮢ��,����� ������, ��।��񭭠� ࠭��, ᮡ�⨥ �� ��� (��-� ��稫��� - ����⨥ �� ������ ��� ��� ��६�饭��; ���뢠���� �� ���⥭��)
        int	0x40
red_win:
    call draw_window 		;��ࢮ��砫쭮 ����室��� ���ᮢ��� ����
still:				;�᭮���� ��ࠡ��稪 
     mov  eax,10 		;������� ᮡ���
     int  0x40      		;������� ᮡ�⨥ � �祭�� 2 �����ᥪ㭤
  
    cmp al,0x1    ;�᫨ ���������� ��������� ����
    jz red_win
    cmp al,0x3    ;�᫨ ����� ������ � ��३�
    jz button
     mouse_option_boxes option_boxes,option_boxes_end  ;�஢�ઠ 祪 ����      
	jmp still    ;�᫨ ��祣� �� ����᫥����� � ᭮�� � 横�
button:
    mov eax,17		;������� �����䨪��� ����⮩ ������
    int 0x40
    test ah,ah		;�᫨ � ah 0, � ��३� �� ��ࠡ��稪 ᮡ�⨩ still
    jz  still
    or eax,-1       ;� eax,-1 - 5 ,���⮢ � ��� �� ⮫쪮 3  ��� 
    int 0x40 ;����� �믮������ ��室 �� �ணࠬ��

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
draw_window:		;�ᮢ���� ���� �ਫ������
    mov eax,12		;� ॣ���� ����� ���祭�� = 12
    mov ebx,1 		;��᢮��� 1
    int 0x40

    xor  eax,eax		;���㫨�� eax
    mov  ebx,50*65536+180	;[���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
    mov  ecx,30*65536+200	;[���न��� �� �� y]*65536 + [ࠧ��� �� �� y]
    mov  edx,[sc.work] 	         ; color of work area RRGGBB,8->color gl
    or   edx,0xb3000000
    mov	 esi,[sc.work_text]
    mov  edi,hed;0x005080DD		;0x00RRGGBB - 梥� ࠬ��
    int  0x40			;���ᮢ��� ���� �ਫ������
	draw_option_boxes option_boxes,option_boxes_end ;�ᮢ���� 祪���ᮢ

    mov eax,12 			;�㭪�� 12 - �����/�������� ����ᮢ�� ����.
    mov ebx,2			;����㭪�� 2 - �������� ����ᮢ�� ����.
    int 0x40
    ret

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA ����� 
;��ଠ� ������ 祪 ����:
;10 - ���न��� 祪 ���� �� � 
;30 - ���न��� 祪 ���� �� �
;0xffffff - 梥� ����� 祪 ����
;0 - 梥� ࠬ�� 祪 ����
;0 - 梥� ⥪�� ������
;op_text.1 - 㪠��⥫� �� ��砫� ��ப�
;option_group1 - �� �ਧ��� ��㯯�, �.�. ��� ��� ����� ��ࠡ��뢠�� ����� ��㯯 �� optibox
;op_text.e1-ch_text.1 - ����� ��ப�
;
option_boxes:
op1 option_box 10,15,0xffffff,0,0,ch_text.1,ch_text.e1-ch_text.1,option_group1
op2 option_box 10,30,0xffffff,0,0,ch_text.2,ch_text.e2-ch_text.2,option_group1
op3 option_box 10,45,0xffffff,0,0,ch_text.3,ch_text.e3-ch_text.3,option_group1
op11 option_box 10,80,0xffffff,0,0,ch_text.1,ch_text.e1-ch_text.1,option_group2
op12 option_box 10,95,0xffffff,0,0,ch_text.2,ch_text.e2-ch_text.2,option_group2
op13 option_box 10,110,0xffffff,0,0,ch_text.3,ch_text.e3-ch_text.3,option_group2
option_boxes_end:

ch_text:		; ���஢�����騩 ⥪�� ��� 祪 ���ᮢ
.1 db 'Option_Box #1' 
.e1:
.2 db 'Option_Box #2'
.e2:
.3 db 'Option_Box #3'
.e3:

option_group1	dd op1 	;㪠��⥫�, ��� �⮡ࠦ����� �� 㬮�砭��, ����� �뢮����� 
option_group2	dd op11 ;�ਫ������

hed db 'Optionbox [16.02.2007]',0	;��������� �ਫ������
sc     system_colors
i_end:			;����� ����