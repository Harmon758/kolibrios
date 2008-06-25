;Libray from Editbox
; SEE YOU File FAQ.txt and HISTORY. Good Like!
;;;;;;;;;;;;;;;;;;

format MS COFF

public EXPORTS

section '.flat' code readable align 16
include 'macros.inc'
include 'editbox.mac'    ;����� ����� ������ �������� ����� :) ᯥ樠�쭮 ��� editbox

edit_box:
ed_width        equ [edi]               ;�ਭ� ���������
ed_left         equ [edi+4]             ;��������� �� �� �
ed_top          equ [edi+8]             ;��������� �� �� �
ed_color        equ [edi+12]            ;梥� 䮭� ���������
shift_color     equ [edi+16]            ;=0x6a9480 ��� �ਬ�� ���쥬 ������ 梥� ����
ed_focus_border_color   equ [edi+20]    ;梥� ࠬ�� ���������
ed_blur_border_color    equ [edi+24]    ;梥� �� ��⨢���� ���������
ed_text_color   equ [edi+28]            ;梥� ⥪��
ed_max          equ [edi+32]                    ;���-�� ᨬ����� ����� ����� ���ᨬ��쭮 �����
ed_text         equ [edi+36]                    ;㪠��⥫� �� ����
ed_flags        equ [edi+40]            ;䫠��
ed_size equ [edi+44]                    ;���-�� ᨬ�����
ed_pos  equ [edi+48]                    ;������ �����
ed_offset       equ [edi+52]            ;ᬥ饭��
cl_curs_x       equ [edi+56]            ;�।��饥 ���न��� ����� �� �
cl_curs_y       equ [edi+60]            ;�।��饥 ���न��� ����� �� �
ed_shift_pos    equ [edi+64]            ;��������� �����
ed_shift_pos_old equ [edi+68]           ;��஥ ��������� �����
.draw:
        pusha
        mov     eax,9
        push    procinfo
        pop     ebx
        or      ecx,-1
        mcall
;--- ��㥬 ࠬ�� ---
        mov     edi,dword [esp+36]
        call    .draw_border            ; �㭪�� �⠡��쭠
.draw_bg_cursor_text:
;--- �����塞 ᬥ饭��, �᫨ ���� ---
        call    .check_offset           ;���᫥��� ����樨 ����� �⠡��쭠
;--- ��㥬 ����७��� ������� ---
        call    .draw_bg                ;���ᮢ��� ��אַ㣮�쭨� ࠡ�祩 ������
;---- ��㥬 �뤥�����, �� shift �᫨ ����
        call    .draw_shift
.draw_cursor_text:
;--- ��㥬 ����� ---
        ;--- ����� ��� �� ���� �ᮢ��� ----
        test    word ed_flags,ed_focus
        je     @f
        call    .draw_cursor
@@:
        call    .draw_text
;        ret
;;;;;;;;;;;;;;;;;;;;;;;;;;
;��騩 ��室 �� editbox ��� ��� �㭪権 � ���� ��ࠡ��稪��
;;;;;;;;;;;;;;;;;;;;;;;;;;
.editbox_exit:
        popa
        ret 4

;==========================================================
;=== ��ࠡ�⪠ ���������� =================================
;==========================================================
edit_box_key:
pusha
        mov     edi,dword [esp+36]
        test    word ed_flags,ed_focus ; �᫨ �� � 䮪��, ��室��
        je      edit_box.editbox_exit
        test    word ed_flags,ed_mouse_on
        jne     edit_box.editbox_exit

;�஢�ઠ ����� shift ?
        call    edit_box_key.check_shift
;----------------------------------------------------------
;--- �஢��塞, �� ����� --------------------------------
;----------------------------------------------------------
use_key_process  backspase,delete,left,right,home,end,insert
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;�����誠 �� ��ࠡ��� ������ ����� � ���� �.�. �� �����㦥��� ��� ����� �ந�室�� ��室 �� ��ࠡ��稪�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_key_no_process   up,down,esc
;--- ����� ��㣠� ������ ---
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;�஢�ઠ ��⠭����� �� 䫠� �� ���஬ �㦭� �뢮���� ⮫쪮 ���� � �㦭�� ���� �᫨ ⠪������室����� ��� �㦭� ��������஢��� �����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_key_figures_only
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;�஢�ઠ �� shift �� �� �����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
are_key_shift_press
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; �஢��塞, ��室���� �� ����� � ���� + ���쭥��� ��ࠡ�⪠
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
are_key_cur_end
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;��ࠡ�⪠ ������ insert,delete.backspase,home,end,left,right
;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_work_key

;==========================================================
;=== ��ࠡ�⪠ ��� =======================================
;==========================================================
;save for stdcall ebx,esi,edi,ebp
edit_box_mouse:
        pop     eax     ;opint to back
        pop     edx     ;scr_w
        pop     ecx     ;ebp     ;scr_h
        push    eax

;        pop     eax
;        pop     edx     ;scr_w
;        pop     ecx     ;scr_h
;        push    eax     ;pointer to back
pusha
        mov     ebp,ecx

        mov     edi,dword [esp+36]
;debug
;----------------------------------------------------------
;--- ����砥� ���ﭨ� ������ ��� -----------------------
;----------------------------------------------------------
        mcall   37,2
;----------------------------------------------------------
;--- �஢��塞 ���ﭨ� ----------------------------------
;----------------------------------------------------------
        test    eax,1
        jnz     edit_box_mouse.mouse_left_button
        and     word ed_flags,ed_mouse_on_off
        xor     ebx,ebx
        mov     dword [mouse_flag],ebx
        jmp     edit_box.editbox_exit
.mouse_left_button:
;----------------------------------------------------------
;--- �����஢�� �� 䮪��஢�� � ��㣨� ����� �� ��������� �� ��� �����
;----------------------------------------------------------
        mov     eax,dword [mouse_flag]
        test    eax,eax
        jz      @f
        cmp     eax,edi
        je      @f
        jmp     edit_box_mouse._blur
;----------------------------------------------------------
;--- ����砥� ���न���� ��� �⭮�⥫쭮 0 �.� �ᥩ ������ �࠭�
;----------------------------------------------------------
@@:     mcall   37,0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;�㭪�� ��ࠡ�⪨  ��誨 ����祭�� ���न��� � �஢�ઠ �� + �뤥�����
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_work_mouse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;��騥 �㭪樨 ��ࠡ�⪨
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_general_func
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;�㭪樨 ��� ࠡ��� � key
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_key_func
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;�㭪樨 ��� ࠡ��� � mouse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use_mouse_func ;scr_w
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Bit mask from editbox
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ed_figure_only= 1000000000000000b   ;���� ᨬ����
ed_always_focus= 100000000000000b
ed_focus=                     10b   ;䮪�� �ਫ������
ed_shift_on=                1000b   ;�᫨ �� ��⠭����� -����� ����� ����� shift,�᫨ �� ��⠭�����, ����� �� 㦥 �� - � ������ 㤥ন��� shift
ed_shift_on_off=1111111111110111b
ed_shift=                    100b   ;����砥��� �� ����⨨ �� shift �.�. �᫨ �������
ed_shift_off=   1111111111111011b
ed_shift_bac=              10000b   ;��� ��� ���⪨ �뤥������ shift �.�. �� ��⠭���� ������ �� ���� �뤥�����
ed_shift_bac_cl=1111111111101111b   ;���⪠ �� 㤠����� �뤥�����
ed_shift_cl=    1111111111100011b
ed_shift_mcl=   1111111111111011b
ed_left_fl=               100000b
ed_right_fl=    1111111111011111b
ed_offset_fl=            1000000b
ed_offset_cl=   1111111110111111b
ed_insert=              10000000b
ed_insert_cl=   1111111101111111b
ed_mouse_on =          100000000b
ed_mous_adn_b=         100011000b
ed_mouse_on_off=1111111011111111b
ed_height=14 ; ����

;----------------------------------------------------
;CheckBox
;----------------------------------------------------
check_box_draw:
;ch_struc_size=24
ch_left equ [edi]    ;���न��� ��砫� �ᮢ���� �� �
ch_top equ [edi+2]   ;���न��� ��砫� �ᮢ���� �� �
ch_text_margin equ [edi+4]      ;=4 ����ﭨ� �� ��אַ㣮�쭨�� 祪 ���� �� ������ 
ch_size equ [edi+8]             ;12 ࠧ��� ������ 祪 ���� 
ch_color equ [edi+12]
ch_border_color equ [edi+16] ; or [edi+4] ;梥� ࠬ�� checkbox �� ����� ������ ᠬ����⥫쭮
ch_text_color equ   [edi+20];[edi+4]  ;梥� ⥪��
ch_text_ptr equ [edi+24]    ;㪠��⥫� �� ��砫� ⥪�⮢�� ��ப� 
ch_text_length equ [edi+28]
ch_flags equ [edi+32]       ;䫠�� 

       pusha   ;��࠭�� �� ॣ�����
       mov     edi,dword [esp+36]
       mov eax,13 
       movzx ebx,word ch_left 
       shl ebx,16 
       add ebx,ch_size 
       mov ecx,ch_top 
       shl ecx,16 
       add ecx,dword ch_size 
       mov edx,dword ch_border_color 
       mcall ;��㥬 ࠬ�� 

       mov edx,dword ch_color 
       add ebx,1 shl 16 - 2 
       add ecx,1 shl 16 - 2 
       mcall ;����訢��� ����७���� 祪���� 

       test dword ch_flags,2  ;������ ���祭�� ��� �� ��६����� � �������� �  䫠� CF 
       jz   @f                ;� �᫨ CF=1, � �믮���� ᫥������ ��楤��� ���� ��३� �� ������ @@
       call check_box_draw_ch  ;���ᮢ��� ����祭�� 祪 ����
@@:
;----------------------------
;���� �㤠 �㤥� �ந������ �뢮� ⥪��
;----------------------------
        movzx ebx,word ch_left        ;����㧨�� ���祭�� � ��� 祪 ����
        add   ebx,dword ch_size
        add   ebx,dword ch_text_margin;�������� ࠧ��� ��஭� � ����ﭨ� �� ���஬ ��筥��� �뢮� ⥪��
        shl   ebx,16                ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
        mov   bx,word ch_top        ;����㧨� ���祭�� �� y
        add   ebx,ch_size
        mov   ecx,dword ch_text_color        ;����㧨� 梥� ������ + flags
        sub   ebx,7        ;������� ���祭�� ����� ��஭� -9+2
        
        mov   edx,dword ch_text_ptr                ;㪠��� ���� �� �㤠 �㦭� �뢮���� ��ப�
        mov   esi,dword ch_text_length
        ;���ᥬ � eax ���祭�� �뢮�� ������ �� �����
        mov   eax,4
        mcall                  ;�뢮� 
popa                              ;����⠭����� ���祭�� ॣ���஢ �� �⥪�
ret 4                             ;�멤�� �� ��楤���

check_box_clear_ch:                        ;���⪠ 祪 ����
        mov   edx,dword ch_color   ;梥� ����� 祪 ����
        jmp   @f             ;����᫮��� ��릮� �� ������ ���� @@

check_box_draw_ch:            ;���ᮢ��� ����祭�� 祪 ����
        mov   edx,dword ch_border_color        ;����㧨�� 梥�
@@:
        movzx ebx,word ch_left  ;����㧨�� ���न���� �� �
        mov   eax,dword ch_size
        push  ax
        shr   eax,2
        add   ebx,eax          ;�������� (��஭� ��אַ㣮�쭨��/3)
        shl   ebx,16            ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
        pop   bx
        shr   bx,1              ;����㧨�� (��஭� ��אַ㣮�쭨��/2)
        mov   bp,bx             ;��࠭�� ॣ���� bx � ॣ���� 㪠��⥫� ����

        movzx ecx,word ch_top ;����㧨�� ���न���� �� �
        mov   eax,dword ch_size
        shr   eax,2
        add   ecx,eax         ;�������� (��஭� ��אַ㣮�쭨��/4)
        shl   ecx,16          ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
        mov   cx,bp           ;����㧨� ���祭�� ॣ���� 㪠��⥫� ���� � cx
        mov   eax,13          ;� eax - ���祭�� �㭪樨 ��� �뢮�� ������ �.�. �� ��� ��אַ㣮�쭨��, ����� �⮡ࠦ��� ����祭�� ��������� 祪 ����
        mcall            ;�뢮�
ret                                ;��� �� ��楤���
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;��ࠡ��稪 mouse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
check_box_mouse:      ;��ࠡ�⪠ ��� 
pusha
        mov     edi,dword [esp+36]
        mov     eax,37           ;�㤥� �� � ������ �᫨ � ��� �� - ���� �����
        mov     ebx,2            ;����� � ॣ���� ���祭�� 2
        mcall             ;�஢�ઠ �� ����� �� ���짮��⥫� ������ ��誨
        test    eax,eax   ;�஢�ઠ �᫨ � ��� � eax=0, � ��⠭���� 䫠� � �멤��
        jnz     @f         ;��३� �� ������ ���� @@
        btr     dword ch_flags,2  ;�����祭�� ���祭�� ��������� ��� � 䫠� cf � ��������� ��� ���祭�� �� �㫥���. 
        jmp     check_box_mouse_end
        
@@:
        bts  dword ch_flags,2   ;�஢�ઠ 䫠�� �.�. ��७�� � cf ���祭�� ��� � ��⠭���� ��� � ���ﭨ� ����祭�
        jc   check_box_mouse_end         ;�᫨ CF=1 � ��३�  � ����� �.�. �� ��室
        mov  esi,dword ch_text_length ;����㧨�� ���-�� ᨬ����� � ⥪�⮢�� ��ப�
        ;��������� �� 6 ����஥ 㬭������ ����� ��ᯮ�짮������ ��� �� ��⮤��, �� �� ����� ������� (386,486,P1)����॥ �㤥� � ������樥� Lea
        ;lea esi,[eax*2+eax]
        ;shl eax,1
        imul esi,6             ; ��� ����� � ⠪ 㬭����� �� 6
        add  esi,dword ch_text_margin ;�������� 3 - ����ﭨ� �� 祪 ���� �� ������

        mov  eax,37             ;����稬 ���न���� ��誨 
        mov  ebx,1              ;�������� 1
        mcall               ;������� ���न���� ����� �⭮�⥫쭮 ���� 
        
        movzx ebx,word ch_top  ;����㧨�� � bx ���祭�� ���न���� �
        cmp   ax,bx              ;�ࠢ���� � � ���न��⮩ �����
        jl   check_box_mouse_end          ;SF <> OF �᫨ ����� 
        add   ebx,dword ch_size        ;�������� ࠧ��� 
        cmp   ax,bx              ;�ࠢ����
        jg   check_box_mouse_end          ;ZF = 0 � SF = OF �᫨ ����� 
             
        shr   eax,16              ;ࠧ����� �� 65536 ��� ���� ᤢ���� ���� �� 16 ���祭��
        movzx ebx,word ch_left  ;�ந������ �������筮� �ࠢ�����
        cmp   ax,bx            ;�ࠢ���� ॣ�����
        jl   check_box_mouse_end        ;�᫨ �����
        add   ebx,dword ch_size      ;�������� ����� ��஭� ��אַ㣮�쭨��
        add   ebx,esi          ;����� � ���祭�� �� � �� � ����� ������ � 祪�����
        cmp   ax,bx            ;��ࠢ���� ॣ�����
        jg   check_box_mouse_end        ;�᫨ ����� 

        bts  dword ch_flags,1  ;�����祭�� ���祭�� ��������� ��� � 䫠� cf � ��������� ��� ���祭�� �� 1. 
        jc   @f                ;CF=1 � ��३� �� ������ @@
        
        call check_box_draw_ch        ;�⮡ࠧ��� ����祭�� 祪 ����
;        mov   dword [esp+24],1 ;���쭥��� �஢�ઠ 祪 ���ᮢ ����᫥��, �� �⮬� � �⥪, ��� �ᯮ�������� ecx ������� 0 
        jmp  check_box_mouse_end       ;��� 
@@:
        btr   word ch_flags,1  ;�����祭�� ���祭�� ��������� ��� � 䫠� cf � ��������� ��� ���祭�� �� �㫥���. 
        call check_box_clear_ch         ;�몫���� 祪 ���� �.�. �� ���� ����襭���� ��אַ㣮�쭨�� �⮡ࠧ��� 梥� 䮭�.
check_box_mouse_end:
popa                                ;����⠭����� ॣ����� �� �⥪�
ret  4                              ;���

;--------------------------------------------------
;radiobutton Group
;--------------------------------------------------
option_box_draw_box:
option_group equ [edi]
op_left equ [edi+4]    ;���न��� ��砫� �ᮢ���� �� �
op_top equ [edi+6]     ;���न��� ��砫� �ᮢ���� �� �
op_text_margin equ [edi+8]      ;=4 ����ﭨ� �� ��אַ㣮�쭨�� 祪 ���� �� ������ 
op_size equ [edi+12]             ;12 ࠧ��� ������ 祪 ���� 
op_color equ [edi+16]
op_border_color equ [edi+20] ; or [edi+4] ;梥� ࠬ�� checkbox �� ����� ������ ᠬ����⥫쭮
op_text_color equ   [edi+24];[edi+4]  ;梥� ⥪��
op_text_ptr equ [edi+28]    ;㪠��⥫� �� ��砫� ⥪�⮢�� ��ப� 
op_text_length equ [edi+32]
op_flags equ [edi+36]       ;䫠��

        pusha   ;��࠭�� �� ॣ�����

        movzx ebx,word op_left 
        shl ebx,16 
        add ebx,dword op_size 
        movzx ecx,word op_top 
        shl ecx,16 
        add ecx,dword op_size 
        mov edx,dword op_border_color 
        mov eax,13 
        mcall ;��㥬 ࠬ�� 
  
        mov edx,dword op_color 
        add ebx,1 shl 16 - 2 
        add ecx,1 shl 16 - 2 
        mcall ;����訢��� ����७���� 祪���� 

;        mov     eax,dword option_group
;        mov     dword eax,[eax]
;        cmp     eax,edi
;        jne     @f
;        call    option_box_draw_op  ;���ᮢ��� ����祭�� 祪 ����


;----------------------------
;���� �㤠 �㤥� �ந������ �뢮� ⥪��
;----------------------------
@@:     movzx ebx,word op_left        ;����㧨�� ���祭�� � ��� 祪 ����
        add   ebx,dword op_size
        add   ebx,dword op_text_margin;�������� ࠧ��� ��஭� � ����ﭨ� �� ���஬ ��筥��� �뢮� ⥪��
        shl   ebx,16                ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
        mov   bx,word op_top        ;����㧨� ���祭�� �� y
        add   ebx,op_size
        mov   ecx,dword op_text_color        ;����㧨� 梥� ������ + flags
        sub   ebx,7        ;������� ���祭�� ����� ��஭� -9+2
        
        mov   edx,dword op_text_ptr                ;㪠��� ���� �� �㤠 �㦭� �뢮���� ��ப�
        mov   esi,dword op_text_length
        ;���ᥬ � eax ���祭�� �뢮�� ������ �� �����
        mov   eax,4
        mcall                  ;�뢮� 
popa                           ;����⠭����� ���祭�� ॣ���஢ �� �⥪�
ret                            ;�멤�� �� ��楤���

option_box_clear_op:                      ;���⪠ 祪 ����
        mov     edx,dword op_color    ;梥� ����� 祪 ����
        jmp     @f              ;����᫮��� ��릮� �� ������ ���� @@


option_box_draw_op:            ;���ᮢ��� ����祭�� 祪 ����
        mov   edx,dword op_border_color        ;����㧨�� 梥�
@@:
        movzx ebx,word op_left  ;����㧨�� ���न���� �� �
        mov   eax,dword op_size
        push  ax
        shr   eax,2
        add   ebx,eax          ;�������� (��஭� ��אַ㣮�쭨��/3)
        shl   ebx,16            ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
        pop   bx
        shr   bx,1              ;����㧨�� (��஭� ��אַ㣮�쭨��/2)
        mov   bp,bx             ;��࠭�� ॣ���� bx � ॣ���� 㪠��⥫� ����

        movzx ecx,word op_top ;����㧨�� ���न���� �� �
        mov   eax,dword op_size
        shr   eax,2
        add   ecx,eax         ;�������� (��஭� ��אַ㣮�쭨��/4)
        shl   ecx,16          ;ᤢ���� �� 16 ࠧ�冷� � ���� (㬭���� �� 65536)
        mov   cx,bp           ;����㧨� ���祭�� ॣ���� 㪠��⥫� ���� � cx
        mov   eax,13          ;� eax - ���祭�� �㭪樨 ��� �뢮�� ������ �.�. �� ��� ��אַ㣮�쭨��, ����� �⮡ࠦ��� ����祭�� ��������� 祪 ����
        mcall            ;�뢮�
ret                                ;��� �� ��楤���
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;��ࠡ��稪 mouse
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
option_box_mouse_op:                 ;��ࠡ�⪠ ��� 
pusha
        mov     eax,37          ;�㤥� �� � ������ �᫨ � ��� �� - ���� �����
        mov     ebx,2           ;����� � ॣ���� ���祭�� 2
        mcall           ;�஢�ઠ �� ����� �� ���짮��⥫� ������ ��誨
        test    eax,eax    ;�஢�ઠ �᫨ � ��� � eax=0, � ��⠭���� 䫠� � �멤��
        jnz     @f         ;��३� �� ������ ���� @@

        jmp     option_box_mouse_end
                           ;�᫨ ��祣� �� �ந��諮, � ����⠭���� ���祭�� ॣ���஢ �� �⥪�
                           ;��室
@@:
        mov     esi,dword op_text_length ;����㧨�� ���-�� ᨬ����� � ⥪�⮢�� ��ப�
        ;��������� �� 6 ����஥ 㬭������ ����� ��ᯮ�짮������ ��� �� ��⮤��, �� �� ����� ������� (386,486,P1)����॥ �㤥� � ������樥� Lea
        ;lea    esi,[eax*2+eax]
        ;shl    eax,1
        imul    esi,6               ; ��� ����� � ⠪ 㬭����� �� 6
        xor     ebx,ebx
        add     esi,dword op_text_margin   ;�������� 3 - ����ﭨ� �� 祪 ���� �� ������
        
        mov     eax,37          ;����稬 ���न���� ��誨 
        inc     ebx             ;�������� 1
        mcall                ;������� ���न���� ����� �⭮�⥫쭮 ���� 

        movzx   ebx,word op_top           ;����㧨�� � bx ���祭�� ���न���� �
        cmp     ax,bx               ;�ࠢ���� � � ���न��⮩ �����
        jl      option_box_mouse_end          ;SF <> OF �᫨ ����� 
        add     ebx,dword op_size          ;�������� ࠧ��� 
        cmp     ax,bx               ;�ࠢ����
        jg      option_box_mouse_end          ;ZF = 0 � SF = OF �᫨ ����� 
        
        shr     eax,16              ;ࠧ����� �� 65536 ��� ���� ᤢ���� ���� �� 16 ���祭��
        movzx   ebx,word op_left          ;�ந������ �������筮� �ࠢ�����
        cmp     ax,bx                ;�ࠢ���� ॣ�����
        jl      option_box_mouse_end           ;�᫨ �����
        add     ebx,dword op_size          ;�������� ����� ��஭� ��אַ㣮�쭨��
        add     ebx,esi              ;����� � ���祭�� �� � �� � ����� ������ � 祪�����
        cmp     ax,bx                ;��ࠢ���� ॣ�����
        jg      option_box_mouse_end           ;�᫨ ����� 
        mov     eax,dword option_group
        mov     [eax],edi

option_box_mouse_end:
popa                              ;����⠭����� ॣ����� �� �⥪�
ret                               ;���

option_box_draw:
        pusha

        mov     eax,dword [esp+36]
@@:     mov     edi,dword [eax]
        test    edi,edi
        je      option_check
        call    option_box_draw_box
        add     eax,4
        jmp    @b 

option_check:
        mov     eax,dword [esp+36]
@@:     mov     edi,dword [eax]
        test    edi,edi
        je      @f
        
        mov     ebx,dword [edi]
        mov     ebx,dword [ebx]
        cmp     edi,ebx
        jne     .clear_op
        
        pusha
        call    option_box_draw_op
        popa
        add     eax,4
        jmp     @b

.clear_op:
        pusha
        call    option_box_clear_op
        popa
        add     eax,4
        jmp     @b


@@:     popa
        ret 4 
        ; exit ����� :)


option_box_mouse:
        pusha

        mov     eax,dword [esp+36]
@@:     mov     edi,dword [eax]
        test    edi,edi
        je      option_check
        call    option_box_mouse_op
        add     eax,4
        jmp    @b 


align 16
EXPORTS:

        dd      sz_edit_box,            edit_box
        dd      sz_edit_box_key,        edit_box_key
        dd      sz_edit_box_mouse,      edit_box_mouse
        dd      szVersion_ed,           0x00000001
        dd      sz_check_box_draw,      check_box_draw
        dd      sz_check_box_mouse,     check_box_mouse
        dd      szVersion_ch,           0x00000001
        dd      sz_option_box_draw,     option_box_draw
        dd      sz_option_box_mouse,    option_box_mouse
        dd      szVersion_op,           0x00000001
        dd      0,0

sz_edit_box            db 'edit_box',0
sz_edit_box_key        db 'edit_box_key',0
sz_edit_box_mouse      db 'edit_box_mouse',0
szVersion_ed           db 'version_ed',0
sz_check_box_draw      db 'check_box_draw',0
sz_check_box_mouse     db 'check_box_mouse',0
szVersion_ch           db 'version_ch',0
sz_option_box_draw     db 'option_box_draw',0
sz_option_box_mouse    db 'option_box_mouse',0
szVersion_op           db 'version_op',0

;;;;;;;;;;;
;;Data
;;;;;;;;;;;
align 16
mouse_flag dd 0x0
procinfo process_information