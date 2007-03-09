; <Lrz> 09.08.2006 �ந������� ��⨬����� ࠡ��� ������ DEL � Backspace? 㬥��襭 ࠧ��� �믮��塞��� ����.
;��䥪⨢��� �ணࠬ��஢���� � KOLIBRI
;��⨬���஢���� ��������� EditBox (��室�� ��ਠ�� �� Maxxxx32)
;��⨬����� ������.
;<Lrz>  - ������ ����ᥩ  www.lrz.land.ru

;��������� �ਫ������
use32		; �࠭����, �ᯮ����騩 32 ࠧ�來�� �������
    org 0x0		; ������ ���� ����, �ᥣ�� 0x0
    db 'MENUET01'	; �����䨪��� �ᯮ��塞��� 䠩�� (8 ����)
    dd 0x1		; ����� �ଠ� ��������� �ᯮ��塞��� 䠩��
    dd start		; ����, �� ����� ��⥬� ��।��� �ࠢ�����
	                ; ��᫥ ����㧪� �ਫ������ � ������
    dd i_end		; ࠧ��� �ਫ������
    dd i_end and not 3	; ��ꥬ �ᯮ��㥬�� �����, ��� �⥪� �⢥��� 0�100 ���� � ��஢��� �� �୨�� 4 ����
    dd i_end and not 3	; �ᯮ����� ������ �⥪� � ������ �����, �ࠧ� �� ⥫�� �ணࠬ��. ���設� �⥪� � ��������� �����, 㪠������ ���
    dd 0x0,0x0		; 㪠��⥫� �� ��ப� � ��ࠬ��ࠬ�.
                 ;    �᫨ ��᫥ ����᪠ ��ࠢ�� ���, �ਫ������ �뫮
                 ;    ����饭� � ��ࠬ��ࠬ� �� ��������� ��ப�

		 ;    㪠��⥫� �� ��ப�, � ������ ����ᠭ ����,
                 ;    ��㤠 ����饭� �ਫ������

	include 'editbox.inc'
	use_edit_box
;������� ����
start:				;��窠 �室� � �ணࠬ��
	mov	eax,40		;��⠭����� ���� ��� ��������� ᮡ�⨩
	mov 	ebx,0x27        ;��⥬� �㤥� ॠ��஢��� ⮫쪮 �� ᮮ�饭�� � ����ᮢ��,����� ������, ��।��񭭠� ࠭��, ᮡ�⨥ �� ��� (��-� ��稫��� - ����⨥ �� ������ ��� ��� ��६�饭��; ���뢠���� �� ���⥭��) � ����⨥ ������
        int	0x40
red_win:
    call draw_window  		;��ࢮ��砫쭮 ����室��� ���ᮢ��� ����
still:				;�᭮���� ��ࠡ��稪 
     mov  eax,10 		;������� ᮡ���
     int  0x40      		;������� ᮡ�⨥ � �祭�� 2 �����ᥪ㭤
  
    cmp al,0x1    ;�᫨ ���������� ��������� ����
    jz red_win
    cmp al,0x2    ;�᫨ ����� ������ � ��३�
    jz key
    cmp al,0x3    ;�᫨ ����� ������ � ��३�
    jz button
	mouse_edit_boxes editboxes,editboxes_end
	jmp still    ;�᫨ ��祣� �� ����᫥����� � ᭮�� � 横�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
button:
mov eax,17		;������� �����䨪��� ����⮩ ������
    int 0x40
    test ah,ah		;�᫨ � ah 0, � ��३� �� ��ࠡ��稪 ᮡ�⨩ still
    jz  still
    or eax,-1		;� eax,-1 - 5 ,���⮢ � ��� �� ⮫쪮 3  ��� 
    int 0x40 		;����� �믮������ ��室 �� �ணࠬ��
    
key:
    mov eax,2		;����㧨� ���祭�� 2 � ॣ���� eax
    int 0x40            ;����稬 ��� ����⮩ ������
    key_edit_boxes editboxes,editboxes_end    
    jmp still

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
draw_window:		;�ᮢ���� ���� �ਫ������
    mov eax,12		;� ॣ���� ����� ���祭�� = 12
    mov ebx,1 		;��᢮��� 1
    int 0x40

    xor eax,eax			;���㫨�� eax
    mov ebx,50*65536+390	;[���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
    mov ecx,30*65536+200	;[���न��� �� �� y]*65536 + [ࠧ��� �� �� y]
    mov edx,0xb3AABBCC		;0xXYRRGGBB  Y =3 - �뢮� ࠬ��, �ᯮ���� �ਭ
    mov esi,0x805080DD		;0xXYRRGGBB - 梥� ���������
    mov edi,hed			;�����⥫� �� ��ப� � ����������
    int 0x40			;���ᮢ��� ���� �ਫ������
  
	draw_edit_boxes editboxes,editboxes_end  ;�ᮢ���� edit box'��

    mov eax,12 			;�㭪�� 12 - �����/�������� ����ᮢ�� ����.
    mov ebx,2			;����㭪�� 2 - �������� ����ᮢ�� ����.
    int 0x40
    ret
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA ����� 
editboxes:
edit1 edit_box 168,5,10,0xffffff,0,0,0,99,ed_buffer.2,ed_figure_only
edit2 edit_box 250,5,30,0xffffff,0,0,0,308,hed,ed_focus,53
edit3 edit_box 35,5,50,0xffffff,0,0,0,9,ed_buffer.3,ed_figure_only
edit4 edit_box 16,5,70,0xffffff,0,0,0,1,ed_buffer.4,ed_figure_only
editboxes_end:
data_of_code dd 0
ed_buffer:
;.1: rb 514;256
.2: rb 101
.3: rb 11
.4: rb 3
;��� ������� ���� ����室��� ��� ⮣� �� �� �� ����९���� ᫥���騥� �����, � ���� ���� 0
;text_b: db '���-�� ᨬ�����'
;buffer: dd 0
buffer_end:
hed db   'EDITBOX optimization and retype <Lrz> date 09.03.2007',0
rb  256
i_end1:
rb 1024
i_end:  