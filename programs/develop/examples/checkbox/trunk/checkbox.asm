;��⨬���஢���� ��������� CheckBox (��室�� ��ਠ�� �� Maxxxx32)
;��⨬���஢�� �뢮� ��ப�, ������ ��� CheckBox'a + ⥯��� �� �஢�થ �� ;�ந�室�� ������ ���-�� ᨬ����� � ��ப�
;��⨬����� ������.
;21.02.2007 ����୨���� � �����প� ���� ࠧ��� ��ਠ�⮢ � �ᯮ�짮������ �� 梥⮢ � ��ன �奬��
;19.02.2007 ��饥 ���襭�� ����, 㬥��襭�� ࠧ��� � �ᯮ�짮����� ��⥬��� 梥⮢ ��� �⮡ࠦ���� checkbox
;22.08.2006 �ந������� ��⨬����� ������, �ਭ��� �� �������� ����砭�� DIAMOND'�.
;<Lrz>  - ������ ����ᥩ  www.lrz.land.ru

;��������� �ਫ������
use32                ; �࠭����, �ᯮ����騩 32 ࠧ�來�� �������
    org 0x0                ; ������ ���� ����, �ᥣ�� 0x0
    db 'MENUET01'        ; �����䨪��� �ᯮ��塞��� 䠩�� (8 ����)
    dd 0x1                ; ����� �ଠ� ��������� �ᯮ��塞��� 䠩��
    dd start                ; ����, �� ����� ��⥬� ��।��� �ࠢ�����
                        ; ��᫥ ����㧪� �ਫ������ � ������
    dd i_end                ; ࠧ��� �ਫ������
    dd (i_end+0x100) and not 3        ; ��ꥬ �ᯮ��㥬�� �����, ��� �⥪� �⢥��� 0�100 ���� � ��஢��� �� �୨�� 4 ����
    dd (i_end+0x100) and not 3        ; �ᯮ����� ������ �⥪� � ������ �����, �ࠧ� �� ⥫�� �ணࠬ��. ���設� �⥪� � ��������� �����, 㪠������ ���
    dd 0x0,0x0                ; 㪠��⥫� �� ��ப� � ��ࠬ��ࠬ�.
                 ;    �᫨ ��᫥ ����᪠ ��ࠢ�� ���, �ਫ������ �뫮
                 ;    ����饭� � ��ࠬ��ࠬ� �� ��������� ��ப�

                 ;    㪠��⥫� �� ��ப�, � ������ ����ᠭ ����,
                 ;    ��㤠 ����饭� �ਫ������
;------------------
        include '..\..\..\..\macros.inc'
        include 'check.inc'     ;������� 䠩� check.inc
;        version_ch             ;����室��� ����� ��� ����祭�� "��ன" ���ᨨ check_box
        version_ch1             ;����室��� ����� ��� ����祭�� new ���ᨨ check_box1
;�������⨬� ����祭�� ���� ����᢮ version_ch1 � version_ch - ��� ������᪫���� ���- ��㣠.
        use_check_box           ;�ᯮ���� ������ ����� ��楤��� ��� �ᮢ���� 祪 ����
;������� ����
align 4
start:                          ;��窠 �室� � �ணࠬ��
        mov  eax,48             ;������� ��⥬�� 梥�
        mov  ebx,3
        mov  ecx,sc
        mov  edx,sizeof.system_colors
        mcall

        mov  eax,40          ;��⠭����� ���� ��� ��������� ᮡ�⨩
        mov  ebx,0x25        ;��⥬� �㤥� ॠ��஢��� ⮫쪮 �� ᮮ�饭�� � ����ᮢ��,����� ������, ��।��񭭠� ࠭��, ᮡ�⨥ �� ��� (��-� ��稫��� - ����⨥ �� ������ ��� ��� ��६�饭��; ���뢠���� �� ���⥭��)
        mcall
red_win:
        call draw_window     ;��ࢮ��砫쭮 ����室��� ���ᮢ��� ����
still:                       ;�᭮���� ��ࠡ��稪 
        mov  eax,10          ;������� ᮡ���
        mcall            ;������� ᮡ�⨥ � �祭�� 2 �����ᥪ㭤
  
        cmp al,0x1    ;�᫨ ���������� ��������� ����
        jz  red_win
        cmp al,0x3    ;�᫨ ����� ������ � ��३�
        jz  button
     mouse_check_boxes check_boxes,check_boxes_end  ;�஢�ઠ 祪 ����      
        jmp still    ;�᫨ ��祣� �� ����᫥����� � ᭮�� � 横�
button:
         or eax,-1       ;� eax,-1 - 5 ,���⮢ � ��� �� ⮫쪮 3  ��� 
         mcall ;����� �믮������ ��室 �� �ணࠬ��

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
draw_window:                ;�ᮢ���� ���� �ਫ������
    mov eax,12                ;� ॣ���� ����� ���祭�� = 12
    mov ebx,1                 ;��᢮��� 1
    mcall

    xor  eax,eax              ;���㫨�� eax
    mov  ebx,50*65536+180     ;[���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
    mov  ecx,30*65536+100     ;[���न��� �� �� y]*65536 + [ࠧ��� �� �� y]
    mov  edx,[sc.work]        ; color of work area RRGGBB,8->color gl
    or   edx,0xb3000000
    mov  edi,hed
    mcall                 ;���ᮢ��� ���� �ਫ������
       
        draw_check_boxes check_boxes,check_boxes_end ;�ᮢ���� 祪���ᮢ

    mov eax,12                ;�㭪�� 12 - �����/�������� ����ᮢ�� ����.
    mov ebx,2                 ;����㭪�� 2 - �������� ����ᮢ�� ����.
    mcall
    ret
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA ����� 
;��ଠ� ������ 祪 ����:
;10 - ���न��� 祪 ���� �� � 
;30 - ���न��� 祪 ���� �� �
;0 - 梥� ������� ����� 祪 ����
;0xfffffff - 梥� ࠬ�� 祪 ���� �᫨ �㦭� ⠪ �ᯮ������� �⥬�� 梥�
;0 - 梥� ⥪�� ������  �᫨ �㦭� ⠪ �ᯮ������� �⥬�� 梥�
;ch_text.1 - 㪠��⥫� �� ��砫� ��ப�
;ch_text.e1-ch_text.1 - ����� ��ப�
;
align 4
check_boxes:
;ch1 check_box 10,15,0xffffff,0,0,ch_text.1,ch_text.e1-ch_text.1,ch_flag_en
;ch2 check_box 10,30,0xffffff,0,0,ch_text.2,ch_text.e2-ch_text.2
;ch3 check_box 10,45,0xffffff,0,0,ch_text.3,ch_text.e3-ch_text.3

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA �����  ��� check_box1 - �ᯮ���� ��⥬�� 梥�
;��ଠ� ������ 祪 ����:
;10 - ���न��� 祪 ���� �� � 
;30 - ���न��� 祪 ���� �� �
;ch_text.1 - 㪠��⥫� �� ��砫� ��ப�
;ch_text.e1-ch_text.1 - ����� ��ப�

;
ch1 check_box1 10,15,ch_text.1,ch_text.e1-ch_text.1,ch_flag_en
ch2 check_box1 10,30,ch_text.2,ch_text.e2-ch_text.2
ch3 check_box1 10,45,ch_text.3,ch_text.e3-ch_text.3
check_boxes_end:

ch_text:        ;ᮯ஢�����騩 ⥪�� ��� 祪 ���ᮢ
.1 db 'Check_Box #1' 
.e1:
.2 db 'Check_Box #2'
.e2:
.3 db 'Check_Box #3'
.e3:

hed db 'Checkbox [21.02.2007]',0        ;��������� �ਫ������
sc     system_colors
i_end:                ;����� ����