;#___________________________________________________________________________________________________
;****************************************************************************************************|
; Program Palitra (c) Sergei Steshin (Akyltist)                                                      |
;----------------------------------------------------------------------------------------------------|
;; Charset:DOS-866 Font:Courier New Size:9pt                                                         |
;.....................................................................................................
;; compiler:     FASM 1.69.31                                                                        |
;; version:      0.3.0                                                                               |
;; last update:  07/11/2013                                                                          |
;; e-mail:       dr.steshin@gmail.com                                                                |
;.....................................................................................................
;; History:                                                                                          |
;; 0.1.0 - ��ࢠ� ����� �ணࠬ��.                                                                  |
;; 0.2.0 - ��ࠢ���� ��������� � ���, ������ ������訩 �� ��������� ���ᥫ�.                     |
;;       - ��������� ����㭪�, ��� ॣ㫨஢���� rgb ��⠢����� 梥� � �뢮� ��� ��⠢�����.   |
;;       - ��࠭ �뢮� 梥� � ����୮� ���� (���� �� �� ����������� � �� ���㠫쭮����).            |
;;       - ������ ��ᬥ⨪�.                                                                         |
;; 0.3.0 - ��������� ��४��祭�� ����� 梥⮢�� �奬 (������ NEXT)                                  |
;;       - ����襭� ����㭪�, �ந�������� ��ࠡ�⪠ ������ �冷� � ����㭪��.                      |
;;       - ��᫮ �⮪ � 梥⮢�� �奬� 㬥��襭� � 6 �� 4 (��⭮��� 256, ���� �㡫�஢���� 梥�).|
;;       - ������ ��ᬥ⨪�.                                                                         |
;; 0.4.0 - ��������� ��४��祭�� ����� ���� 梥⠬�                                                |
;;       - �������� ����㭮� ॣ㫨஢���� �஧�筮�� (��� ���㠫���樨).                          |
;;       - ��������� ��ࠢ������� ���祭�� rgba �� 業���, � ����ᨬ��� �� ������.                  |
;;       - ��ᬥ��᪨� �ࠢ��.                                                                     |
;;       - �������� ��⨬�����.                                                                    |
;; 0.5.0 - ��������� ������ ᬥ�� 䮭� ࠡ�祣� �⮫� (�� Leency).                                   |
;;       - ��������� ��������� 䮭� ࠡ�祣� �⮫� �ࠤ���⭮� ��������.                             |
;;       - ��ᬥ��᪨� �ࠢ��.                                                                     |
;;       - �������� ����⨬�����.                                                                  |
;; 0.6.0 - ��������� ����������� ����᪠ � ��ࠬ��ࠬ�                                               |
;;       - �������� ०�� H (hidden) �ந������ ������ 䮭� ࠡ�祣� �⮫� �ࠤ���⭮� ��������.     |
;;       - ������ ����⨬�����.                                                                    |
;; 0.7.0 - ��������� ����⪠ - �롮� �� �।��� ������ ���                                          |
;; 0.7.5 - ����⨥ �ࠢ�� �����襩 ��� �� �祩�� � 梥⮬ ��⠭�������� �� �᭮���, � ������. 梥�  |
;;       - �����襭� ���栭�� �� ࠡ�� ����⪨                                                     |
;; 0.7.6 - ���ࠡ�⠭ ०�� H (hidden): ⥯��� 梥� ����㦠���� �� wallpaper.dat       (eAndrew)   |
;;       - �� ��⠭���� 䮭� �������, 梥� 䮭� ��࠭���� � wallpaper.dat             (eAndrew)   |
;.....................................................................................................
;; All rights reserved.                                                                              |
;;                                                                                                   |
;; Redistribution and use in source and binary forms, with or without modification, are permitted    |
;; provided that the following conditions are met:                                                   |
;;       * Redistributions of source code must retain the above copyright notice, this list of       |
;;         conditions and the following disclaimer.                                                  |
;;       * Redistributions in binary form must reproduce the above copyright notice, this list of    |
;;         conditions and the following disclaimer in the documentation and/or other materials       |
;;         provided with the distribution.                                                           |
;;       * Neither the name of the <organization> nor the names of its contributors may be used to   |
;;         endorse or promote products derived from this software without specific prior written     |
;;         permission.                                                                               |
;;                                                                                                   |
;; THIS SOFTWARE IS PROVIDED BY Sergei Steshin ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,      |
;; INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A        |
;; PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY DIRECT, |
;; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED    |
;; TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS       |
;; INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT          |
;; LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS  |
;; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                      |
;....................................................................................................|


;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ��������� ������������ ����� ���������� ��� ������� ��                                             |
;----------------------------------------------------------------------------------------------------/
  use32
  org	 0x0
  db	 'MENUET01'
  dd	 0x01
  dd	 START
  dd	 I_END
  dd	 I_RDATA+4096
  dd	 I_RDATA+4096
  dd	 params
  dd	 0x0

  include '../../../macros.inc'

  WIN_W  equ 295	    ; �ਭ� ����
  WIN_H  equ 195	    ; ���� ����
  WIN_X  equ 250	    ; ���न��� � ����
  WIN_Y  equ 190	    ; ���न��� � ����

panel:
  file "panel.raw"

START:
    mcall   68,11			  ; ���樠������ ���
    mcall   40,0x27			  ; ��⠭�������� ���� ᮡ�⨩
    include 'params_init.inc'		  ; ��ࠡ�⪠ ��ࠬ��஢ ��������� ��ப�

;#___________________________________________________________________________________________________
;****************************************************************************************************|
; �������� ���� ��������� - ��������� �������                                                        |
;----------------------------------------------------------------------------------------------------/
red:
    call draw_main			  ; ��뢠�� ����ᮢ�� ���� �ਫ������
still:
    mcall   10				  ; �㭪�� 10 - ����� ᮡ���
    cmp     eax,1			  ; ����ᮢ��� ���� ?
    je	    red 			  ; �᫨ �� - �� ���� red
    cmp     eax,2			  ; ����� ������ ?
    je	    key 			  ; �᫨ �� - �� key
    cmp     eax,3			  ; ����� ������ ?
    je	    button			  ; �᫨ �� - �� button
    cmp     eax,6			  ; ᮡ�⨥ �� ��� ��� ����
    je	    mouse			  ; �᫨ �� - �� button
    jmp     still			  ; �᫨ ��㣮� ᮡ�⨥ - � ��砫� 横��
;end_still

key:					  ; ����� ������ �� ���������
    mcall   2				  ; �㭪�� 2 - ����� ��� ᨬ���� (� ah) (��� � �ਭ樯� �� �㦭�)
    jmp     still			  ; �������� � ��砫� 横��
;end_key

mouse:
    cmp     [renmode],2
    jne     left
    call    cyrcle_draw
    jmp     center
  left:
    mcall   37,2
    cmp     al,1b
    jne     right
    mov     [mouse_f],1
    jmp     still
  right:
    cmp     al,10b
    jne     still
    mov     [mouse_f],2
    jmp     still
  center:
    mcall   37,2
    cmp     al,100b
    jne     still
    mov     [mouse_f],3
    mov     [color],edx
    call    draw_result
    jmp     still			  ; �������� � ��砫� 横��
;end_mouse

button:
    mcall   17				  ; 17 - ������� �����䨪��� ����⮩ ������
    cmp     ah, 1			  ; �᫨ ����� ������ � ����஬ 1,
    jz	    bexit			  ; ��室��
  ;��ࠡ�⪠ ������ Next
    cmp     ah, 12			  ; �᫨ ����� ������ NEXT
    jne     next_bg			  ; ��室��
    inc     [pnext]			  ; 㢥��稢��� �� ����⨨ ����� �������
    mov     [renmode],0 		  ; ����砥� 梥⮢� �奬�
    mov     eax,[pnext] 		  ; ����ᨬ ���祭�� � ���
    cmp     al,6			  ; �ࠢ������ � ������ ������⢮� ������
    jne     next_redraw 		  ; �᫨ �� ����� ���ᨬ㬠 � �� �맮� ���ᮢ��
    xor     eax,eax			  ; ���� ����塞 ������� �� default
    mov     [pnext],eax 		  ; � ���������� �� ��ᨫ� ������� �� default
  next_redraw:
    call    draw_palitra		  ; ������ �������
    jmp     still			  ; �室�� �� �������� ��㣮�� ᮡ���
  next_bg:
    cmp     ah, 14			  ; ������ BACKGROUND
    jne     circle_bg			  ; �᫨ �� ����� � ��室��
    call    set_background		  ; ���� ��⠭�������� 䮭
    jmp     still			  ; � �� �������� ᮡ���
  circle_bg:
    cmp     ah, 15			  ; ������ ��㣮��� ������
    jne     next_end			  ; �᫨ �� ����� � ��室��
    mov     [renmode],2 		  ; ����砥� ���ᮢ�� ��㣮��� �������
    call    draw_palitra		  ; ������ �������
    jmp     still			  ; � �� �������� ᮡ���
  next_end:
    cmp     ah,13			  ; COLOR SWAP
    jne     color_swap_end
    push    [color2]
    push    [color]
    pop     [color2]
    pop     [color]
    call    draw_result
    jmp     still			  ; � �室�� �� �������� ��㣮�� ᮡ���
  color_swap_end:
    cmp     ah, 7			  ; �஢��塞 ����� ������ � ID=7
    jne     color_button		  ; �᫨ �� �����, � ��� �����
    call    mouse_get			  ; ���� ����砥� ��ࠡ��稪 ���, �⮡� ����� ���祭�� 梥� � �������
    jmp     still			  ; � �室�� �� �������� ��㣮�� ᮡ���
  color_button: 			  ; ����� ���न��� ��� ����㭪�� RGBA
    push    eax 			  ; ���������� ���
    call    mouse_local 		  ; ����砥� ������� ���न����
    mov     ebx,137			  ; ������ �࠭�� ����㭪� �� �
    mov     ecx,[mouse_y]		  ; ����ᨬ � ��� ���祭�� ����� �� �
    sub     ebx,ecx			  ; ��室�� ࠧ����� (�.�. �㤠 ᬥ頥��� ����㭮�)
    mov     ecx,3			  ; ����ᨬ � ��� ���� 3 (256/3=85, ��� 85-���� ����㭪��)
    imul    ecx,ebx			  ; ��室�� ��⨭�� ��ࠬ��� 梥� � ���⮬ ����⠡� ����㭪�---+
    pop     eax 			  ; ����⠭�������� ���                                          :
  ;red_button:                            ; ���� �४���                                              :
    cmp     ah, 8			  ; ID=8                                                         :
    jne     green_button		  ; �᫨ ���, � �஢��塞 ����� �४���                       :
    mov     [cred],cl			  ; ���� ��ᢠ����� ���祭��, ��᭮�� 梥�� ᯥ���    <------+
    call    set_spectr			  ; ��⠭�������� ᯥ���
    jmp     still			  ; �室�� �� �������� ��㣮�� ᮡ���
  green_button:
    cmp     ah, 9
    jne     blue_button
    mov     [cgreen],cl
    call    set_spectr
    jmp     still			  ; �室�� �� �������� ��㣮�� ᮡ���
  blue_button:
    cmp     ah, 10
    jne     alpha_button
    mov     [cblue],cl
    call    set_spectr
    jmp     still			  ; �室�� �� �������� ��㣮�� ᮡ���
  alpha_button:
    cmp     ah, 11
    jne     still
    mov     [calpha],cl
    call    set_spectr
    jmp     still			  ; �室�� �� �������� ��㣮�� ᮡ���
  bexit:
    mcall -1				  ; ���� ����� �ணࠬ��
;end_button

;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ������� ������ ��������� ���� � ��������� ����������                                               |
;----------------------------------------------------------------------------------------------------/
draw_main:
    mov     eax,12			  ; �㭪�� 12: ����砥�, �� �㤥� �ᮢ����� ����
    mov     ebx,1			  ; 1,��砫� �ᮢ����
    int     0x40			  ; ���뢠���

    mov     eax,48			  ; �㭪�� 48 - �⨫� �⮡ࠦ���� ����
    mov     ebx,3			  ; ����㭪�� 3 - ������� �⠭����� 梥� ����.
    mov     ecx,sc			  ; �����⥫� �� ���� ࠧ��஬ edx ����, ��� ��������
    mov     edx,sizeof.system_colors	  ; ������ ⠡���� 梥⮢ (������ ���� 40 ����)
    int     0x40			  ; ���뢠���

    mov     eax,48			  ; �㭪�� 48 - �⨫� �⮡ࠦ���� ����.
    mov     ebx,4			  ; ����㭪�� 4 - �����頥� eax = ���� ᪨��.
    int     0x40			  ; ���뢠���
    mov     ecx,eax			  ; ���������� ����� ᪨��

    mov     edi,[runmode]
    cmp     edi,2
    jne     no_hiddenmode
    mov     edi,hidden
    jmp     set_title
  no_hiddenmode:
    cmp     edi,3
    jne     no_dialogmode
    mov     edi,hidden
    jmp     set_title
  no_dialogmode:
    mov     edi,title			  ; ��������� ����
  set_title:

    xor     eax,eax			  ; ��頥� eax (mov eax,0) (�㭪�� 0)
    mov     ebx,WIN_X shl 16+WIN_W	  ; [���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
    add     ecx,WIN_Y shl 16+WIN_H	  ; ���� ᪨�� + [���न��� �� y]*65536 + [ࠧ��� �� y] (168 ��� ���ᨨ 0.2)
    mov     edx,[sc.work]		  ; ������ �⨫� ���� �� ��䮫��
    or	    edx,0x34000000		  ; ��� ���� � ᪨��� 䨪�஢����� ࠧ��஢

    int     0x40			  ; ���뢠���


    call    draw_palitra		  ; ������ �������
    call    draw_result 		  ; ������ ���������

    mov     eax,8			  ; �㭪�� 8 - ��।�����/㤠���� ������
    mov     ebx,110 shl 16+147		  ; ��砫�� ���न���� �� � [�-� x]*65536 + [ࠧ���]
    mov     ecx,9 shl 16+147		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    mov     edx,0x60000007		  ; ��砫쭮� ID ������ - 0xXYnnnnnn
    int     0x40

    mov     ebx,13 shl 16+12		  ; ��砫�� ���न���� �� � [�-� x]*65536 + [ࠧ���]
    mov     edx,0x60000008		  ; ID = 8
    mov     ecx,51 shl 16+85		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���㥬 ��������� ������ ��� ᫠���஬ red
    add     ebx,23 shl 16		  ; ������塞
    inc     edx 			  ; ID = 9
    int     0x40			  ; ���㥬 ��������� ������ ��� ᫠���஬ green
    add     ebx,23 shl 16		  ; ������塞
    inc     edx 			  ; ID = 10
    int     0x40			  ; ���㥬 ��������� ������ ��� ᫠���஬ blue
    add     ebx,23 shl 16		  ; ������塞
    inc     edx 			  ; ID = 11
    int     0x40			  ; ���㥬 ��������� ������ ��� ᫠���஬ alpha

    mov     eax,8			  ; �㭪�� 8 - ��।�����/㤠���� ������ (����� �����)
    mov     ebx,11 shl 16+20		  ; ��砫�� ���न���� �� � [�-� x]*65536 + [ࠧ���]
    mov     ecx,12 shl 16+20		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    mov     edx,0x6000000D		  ; ID = 13
    int     0x40

    call    draw_bottom_panel
    call    draw_left_panel

    mov     eax,12			  ; �㭪�� 12: ����砥�, �� �㤥� �ᮢ����� ����
    mov     ebx,2			  ; 2 - ����� �ᮢ����
    int     0x40			  ; ���뢠���
    ret

;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ���� ��������������� �������� � ������� ����������                                                 |
;----------------------------------------------------------------------------------------------------/


    ;------------------------------------------------------------------------------------------------+
    draw_left_panel:			  ; ���ᮢ�� ������� ������
    ;.................................................................................................
    mov     eax,8			  ; button_next_colorsheme
    mov     ebx,266 shl 16+16		  ; [x] + [size]
    mov     ecx,9 shl 16+16		  ; [y] + [size]
    mov     edx,0x6000000C		  ; ID = 12
    mov     esi,[sc.work_button]	  ; RGB
    int     0x40			  ; call
    ; circle diagram
    add     ecx,19 shl 16		  ; move rect
    mov     edx,0x6000000F		  ; ID = 15
    int     0x40			  ; call

    mov     eax,7			  ; 7 - ����� �㭪樨
    mov     ebx,panel			  ; 㪠��⥫� �� ����ࠦ���� � �ଠ� BBGGRRBBGGRR...
    mov     ecx, 16 shl 16 + 149	  ; [ࠧ��� �� �� x]*65536 + [ࠧ��� �� �� y]
    mov     edx,266 shl 16 +   9	    ; [���न��� �� �� x]*65536 + [���न��� �� �� y]
    int     0x40

    ;mov     eax,13                        ; draw rect
    ;mov     ebx,266 shl 16+16             ; [x] + [size]
    ;mov     ecx,9 shl 16+16               ; [y] + [size]
    ;mov     edx,0x666666                  ; RGB
    ;push    esi                           ; backup esi
    ;mov     esi,8                         ; counter=8
    ;draw_lpanel:                          ; loop label
    ;  int     0x40                        ; call draw black rect
    ;  add     ecx,19 shl 16               ; move rect
    ;  dec     esi                         ; decrement counter
    ;  cmp     esi,0                       ; if counter!=zero
    ;  jne     draw_lpanel                 ; then goto label
    ;  mov     esi,8                       ; else counter=8
    ;  mov     ebx,267 shl 16+14           ; [x] + [size]
    ;  mov     ecx,10 shl 16+14            ; [y] + [size]
    ;  mov     edx,0xF3F3F3                ; RGB
    ;draw_lpanel2:                         ; 2 loop label
    ;  int     0x40                        ; call draw white rect
    ;  add     ecx,19 shl 16               ; move rect
    ;  dec     esi                         ; decrement counter
    ;  cmp     esi,0                       ; if counter!=0
    ;  jne     draw_lpanel2                ; then goto label2
    ;pop     esi                           ; restore esi
    ; draw_left_arrow for button_next_colorsheme
    ;mov     eax,4                         ; Write string
    ;mov     ebx,272 shl 16+13             ; [x] + [y]
    ;mov     ecx,0x0                       ; RGB
    ;mov     edx,larrow                    ; string pointer
    ;mov     esi,1                         ; count symbol
    ;int     0x40                          ; call
    ;mov     eax,38                        ; draw line
    ;mov     ebx,270 shl 16+272            ; [start x] + [end x]
    ;mov     ecx,16 shl 16+16              ; [start y] + [end y]
    ;mov     edx,0x0                       ; RGB
    ;int     0x40                          ; call
    ret 				  ; return
    ;.................................................................................................


    ;------------------------------------------------------------------------------------------------+
    draw_bottom_panel:			  ; ���ᮢ�� ������ ������
    ;.................................................................................................
    mov     eax,8			  ; draw button
    mov     ebx,92 shl 16+100		  ; [x] + [size]
    mov     ecx,169 shl 16+16		  ; [y] + [size]
    mov     edx,14			  ; ID = 14
    mov     esi,[sc.work_button]	  ; RGB
    int     0x40			  ; call
    mov     eax,4			  ; Write string
    mov     ebx,113 shl 16+174		  ; [x] + [y]
    mov     ecx,[sc.work_button_text]	  ; RGB
    mov     edx,bground 		  ; string pointer
    mov     esi,10			  ; count symbol
    int     0x40			  ; call
    mov     eax,38			  ; draw line
    mov     ebx,4 shl 16+282		  ; [start x] + [end x]
    mov     ecx,163 shl 16+163		  ; [start y] + [end y]
    mov     edx,0x00666666		  ; RGB
    int     0x40			  ; call
    ret 				  ; return
    ;.................................................................................................

mouse_global:
    ;.................................................................................................
    ; ����砥� ���न���� ���
    ;.................................................................................................
    mov     eax,37			  ; �㭪�� 37 - ࠡ�� � �����
    mov     ebx,0			  ; ��� �㦭� �������� ���न����
    int     0x40			  ; eax = x*65536 + y, (x,y)=���न���� ����� ���
    mov     ecx,eax			  ;
    shr     ecx,16			  ; ecx = x+1
    movzx   edx,ax			  ; edx = y+1
    dec     ecx 			  ; ecx = x
    dec     edx 			  ; edx = y
    mov     [mouse_x],ecx		  ; mouse_x = x
    mov     [mouse_y],edx		  ; mouse_y = y
    ret 				  ; �����頥� �ࠢ�����
;end_mouse_global

mouse_local:
    ;.................................................................................................
    ; ����砥� ���न���� ��� �⭮�⥫쭮 ����
    ;.................................................................................................
    mov     eax,37			  ; �㭪�� 37 - ࠡ�� � �����
    mov     ebx,1			  ; ��� �㦭� �������� ���न����
    int     0x40			  ; eax = x*65536 + y, (x,y)=���न���� ����� ���
    mov     ecx,eax			  ;
    shr     ecx,16			  ; ecx = x+1
    movzx   edx,ax			  ; edx = y+1
    dec     ecx 			  ; ecx = x
    dec     edx 			  ; edx = y
    mov     [mouse_x],ecx		  ; mouse_x = x
    mov     [mouse_y],edx		  ; mouse_y = y
    ret 				  ; �����頥� �ࠢ�����
;end_mouse_local

set_background:
    ;.................................................................................................
    ; ��⠭�������� 䮭 ࠡ�祣� �⮫�
    ;.................................................................................................
    mov     eax,15			  ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    mov     ebx,4			  ; ����㭪�� 4 - ��⠭����� ०�� ���ᮢ�� 䮭�.
    mov     ecx,2			  ; ����� ���ᮢ�� - �������� (1)
    int     0x40			  ; ��뢠��

    mov     eax,15			  ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    mov     ebx,1			  ; ����㭪�� 1 - ��⠭����� ࠧ��� 䮭����� ����ࠦ����.
    mov     ecx,2			  ; ��ਭ� ����ࠦ����
    mov     edx,2			  ; ���� ����ࠦ����
    int     0x40			  ; ��뢠��

    mov     eax,[color]
    mov     [cm+0],al
    mov     [cm+9],al
    shr     eax,8
    mov     [cm+1],al
    mov     [cm+10],al
    shr     eax,8
    mov     [cm+2],al
    mov     [cm+11],al

    mov     eax,[color2]
    mov     [cm+3],al
    mov     [cm+6],al
    shr     eax,8
    mov     [cm+4],al
    mov     [cm+7],al
    shr     eax,8
    mov     [cm+5],al
    mov     [cm+8],al

    mov     eax,15			  ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    mov     ebx,5			  ; ����㭪�� 5 - �������� ���� ���ᥫ�� �� 䮭.
    mov     ecx,cm			; �����⥫� �� ����� � �ଠ� BBGGRRBBGGRR
    mov     edx,0			  ; C��饭�� � ������ 䮭����� ����ࠦ����
    mov     esi,3*4			 ; ������ ������ � ����� = 3 * �᫮ ���ᥫ��
    int     0x40			  ; ��뢠��

    mov     eax,15			  ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    mov     ebx,3			  ; ����㭪�� 3 - ����ᮢ��� 䮭.
    int     0x40			  ; ��뢠��

    mov     eax, [color]
    mov     [fi.file+0], al
    shr     eax, 8
    mov     [fi.file+1], al
    shr     eax, 8
    mov     [fi.file+2], al

    mov     eax, [color2]
    mov     [fi.file+3], al
    shr     eax, 8
    mov     [fi.file+4], al
    shr     eax, 8
    mov     [fi.file+5], al

    mov     [fi.p00], dword 2
    mov     [fi.p04], dword 0
    mov     [fi.p08], dword 0
    mov     [fi.p12], dword 6
    mov     [fi.p16], dword fi.file
    mov     [fi.p20], dword 0
    mov     [fi.p21], dword dat_file
    mcall   70, fi

    ret
;end_set_background

desktop_get:
    ;.................................................................................................
    ; ��।��塞 �ਭ� �࠭�
    ;.................................................................................................
    mov     eax,14			  ; ��।��塞 �ਭ� �࠭� (eax = [xsize]*65536 + [ysize])
    int     0x40			  ; xsize = ࠧ��� �� ��ਧ��⠫� - 1
    mov     ebx,eax			  ;
    shr     ebx,16			  ; ebx = xsize-1
    movzx   edx,ax			  ; edx = ysize-1 (��譨� ���)
    inc     ebx 			  ; ebx = xsize
    inc     edx 			  ; edx = ysize (��譨� ���)
    mov     [desctop_w],ebx
    mov     [desctop_h],edx
    ret
;end_desktop_get

mouse_get:
    mov     esi,2			  ; �������: 䫠� ��� ��������� ��横�������
    call    mouse_global
    call    desktop_get
    re_mouse_loop:			  ; �������: ��⪠ ��� ������ �᫨ ������ � ���
      mov     ebx,[desctop_w]
      imul    ebx,[mouse_y]		  ; ebx = y*xsize
      add     ebx,[mouse_x]		  ; ebx = y*xsize+x

      ;.................................................................................................
      ; ��६ 梥� � ������� � ��६�����
      ;.................................................................................................
      mov     eax,35			  ; �㭪�� ����� 梥�
      ;mov     ebx,ecx                    ;; ebx = y*xsize+x (��譨� ���)
      int     0x40			  ; ����砥� 梥� � eax
      cmp     eax,[sc.work]		  ; �ࠢ������ � 䮭�� �ਫ������
      je      mouse_err 		  ; �᫨ �� �� - � ��祣� �� ������
      cmp     eax,0x222222		  ; �ࠢ������ � 梥⮬ �⪨
      je      mouse_err 		  ; �᫨ �� �� - � ��祣� �� ������
      jmp     mouse_set 		  ; �������: ��룠�� �⮡� �� ���� 梥� �⪨
    mouse_err:				  ; �������: �᫨ ������ � ��� ��� 䮭
      inc     [mouse_y] 		  ; �������: ᬥ頥� �� ��������� ᭠砫� �� �
      inc     [mouse_x] 		  ; �������: ᬥ頥� �� ��������� ��⮬ �� �
      dec     esi			  ; �������: �����蠥� 䫠�
      cmp     esi,0			  ; �������: �ࠢ������ � �㫥�
    jz	      mouse_exit		  ; �������: �᫨ ���� � ᤥ���� ��� �� �����
    jmp    re_mouse_loop		  ; �������: �᫨ �� ���� � ���஡㥬 ����� �ᥫ��� ���ᥫ�
    mouse_set:				  ; ���� ���������� ���� 梥�
      cmp     [mouse_f],1
      jne     was_right
      mov     [color],eax
      call    draw_result
      jmp     mouse_exit
    was_right:
      cmp     [mouse_f],2
      jne     mouse_exit
      mov     [color2],eax
      call    draw_result		    ; �뢮��� १����
    mouse_exit:
    ret 				  ; �����頥� �ࠢ�����
;end_mouse_get----------------------------------------------------------------------------------------

draw_palitra:
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    mov     ebx,110 shl 16+148		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,9 shl 16+148		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    mov     edx,[sc.work]		  ; 梥�
    int     0x40

    cmp     [renmode],2
    je	    cyrcle_draw
    ;cmp     [renmode],1
    ;je      picker_draw
    cmp     [renmode],0
    je	    sheme_draw
    ret

    ;.................................................................................................
    ; ���ᮢ�� ��㣮��� ����ࠬ��
    ;.................................................................................................
    cyrcle_draw:
    ;mov     eax,13
    ;mov     edx,0x666666
    ;mov     ebx,108*65536+152
    ;mov     ecx,125*65536+33
    ;mcall
    ;mov     edx,0xF3F3F3
    ;mov     ebx,109*65536+150
    ;mov     ecx,126*65536+31
    ;mcall

    ;mov     eax,4                         ; 4 - �뢥�� ��ப� ⥪�� � ����
    ;mov     ebx,115 shl 16+131            ; [���न��� �� �� x]*65536 + [���न��� �� �� y]
    ;mov     ecx,0x666666                  ; 0xX0RRGGBB (RR, GG, BB ������ 梥� ⥪��)
    ;mov     edx,hint                      ; 㪠��⥫� �� ��砫� ��ப�
    ;mov     esi,24                        ; �뢮���� esi ᨬ�����
    ;mcall
    ;mov     ebx,125 shl 16+144            ; [���न��� �� �� x]*65536 + [���न��� �� �� y]
    ;mov     edx,hint2                     ; 㪠��⥫� �� ��砫� ��ப�
    ;mov     esi,21                        ; �뢮���� esi ᨬ�����
    ;mcall

    ;mov     edx,[color]
    ;mov     ebx,109*65536+150
    ;mov     ecx, 10*65536+150
    ;mcall
    call    mouse_local 		  ; ����砥� ���न���� ��� �⭮�⥫쭮 ����
    mov     ecx, [mouse_x]		  ; ����ᨬ � ॣ����
    mov     edx, [mouse_y]		  ; ����ᨬ � ॣ����
    cmp     ecx, WIN_W-10
    jg	    cyrcle_draw_2
    cmp     edx, WIN_H
    jle     end_cyrcle_draw

    cyrcle_draw_2:
	mcall	5,10
    call    desktop_get
    call    mouse_global

    mov     ebx,112*65536+11
    mov     esi,0			  ; counter=8
    circle_loop:
      mov     ecx, 10*65536+11
      mov     edi,0
      circle_loop2:
	mov	eax,13
	call	circle_pixel_read
	mcall
	add	ecx,11 shl 16
	inc	edi
	cmp	edi,13
	jne	circle_loop2

      add     ebx,11 shl 16
      inc     esi
      cmp     esi,13
      jne     circle_loop


    mov     eax,13
    mov     edx,0x0
    mov     ebx,177*65536+13
    mov     ecx, 76*65536+13
    mcall
    mov     ecx, [mouse_x]
    mov     edx, [mouse_y]
    inc     ecx
    inc     edx
    mov     ebx, edx
    imul    ebx, [desctop_w]
    add     ebx, ecx
    mov     eax, 35
    mcall
    mov     edx,eax
    mov     eax,13
    mov     ebx,178*65536+11
    mov     ecx, 77*65536+11
    mcall
    ret
    end_cyrcle_draw:
    mov     eax,13
    mov     edx,0x666666
    mov     ebx,111*65536+145
    mov     ecx,  9*65536+145
    mcall
    ret

    circle_pixel_read:
    push    ecx
    push    ebx
    push    eax
    mov     ecx, [mouse_x]
    mov     edx, [mouse_y]
    inc     ecx
    add     ecx, esi
    cmp     ecx, 6
    jl	    _cpr_exit
    sub     ecx, 6
    inc     edx
    add     edx, edi
    cmp     edx, 6
    jl	    _cpr_exit
    sub     edx, 6

    mov     ebx, edx
    imul    ebx, [desctop_w]
    add     ebx, ecx
    mov     eax, 35
    mcall
    mov     edx,eax
    pop     eax
    pop     ebx
    pop     ecx
    ret

    _cpr_exit:
    mov     edx,0x00000000
    pop     eax
    pop     ebx
    pop     ecx
    ret

    ;picker_draw:
    ;ret

    ;.................................................................................................
    ; ���ᮢ�� 䮭� ��� ������
    ;.................................................................................................
    sheme_draw:
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    mov     edx,0x222222		  ; 梥�
    mov     ecx,9 shl 16+73		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    mov     esi,2			  ; ���稪 ����� ��������
    re_draw:
    mov     ebx,110 shl 16+73		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     edi,2			  ; ���稪 ������⢠ ��������
    for_fon_loop:
      int     0x40			  ; ���뢠���
      add     ebx,75 shl 16		  ; ���頥� ��������� ����� �� �
      dec     edi			  ; �����蠥� ���稪 ������
      cmp     edi,0			  ; �ࠢ������ � �㫥�
    jnz     for_fon_loop		  ; �᫨ �� ���� � � ��砫� 横��
    dec     esi 			  ; �����蠥� ���
    cmp     esi,0			  ; �ࠢ������ � �㫥�
    mov     ecx,84 shl 16+73		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    jnz     re_draw			  ; �᫨ �� ���� � � ��砫� 横��

    ;.................................................................................................
    ; ���ᮢ�� ������ �� 横��
    ;.................................................................................................
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    mov     edx,0x0FFFFFFF		  ; 梥�
    mov     esi,4			  ; ���稪 ������⢠ ����楢 (#4,8)
    mov     ebx,99 shl 16+8		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    for_abz:
      ;;push    esi                       ; ���࠭塞 ���祭�� ���稪� ����� � �⥪
      cmp     esi,2
      jne     x2_line
      mov     ebx,99 shl 16+8
      x2_line:
      add     ebx,3 shl 16		  ; ���頥� ��������� ����� �� x
      mov     edi,8			  ; ���稪 ������⢠ ������ � ��ப�
      for_stolbik:
	push	edi			  ; ���࠭塞 ���祭�� ���稪� ����� � �⥪
	mov	edi,8			  ; ���稪 ������⢠ ������ � ��ப�
	mov	ecx,  1 shl 16+8	  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
	cmp	esi,2
	jg	y2_line 		  ; �᫨ ����� 4 � ����ਬ
	mov	ecx,76 shl 16+8
	y2_line:
	add	ebx,9 shl 16		  ; ���頥� ��������� ����� �� x
	for_loop:
	  add	  ecx,9 shl 16		  ; ���頥� ��������� ����� �� y
	  int	  0x40			  ; ���뢠���
	  call	  sub_red
	  dec	  edi			  ; �����蠥� ���稪 ������
	  cmp	  edi,0 		  ; �ࠢ������ � �㫥�
	  jnz	  for_loop		  ; �᫨ �� ���� � � ��砫� 横��
      call    sub_green
      pop     edi			  ; ���� ����⠭�������� ���稪 �����
      dec     edi			  ; �����蠥� ���
      cmp     edi,0			  ; �ࠢ������ � �㫥�
      jnz     for_stolbik		  ; �᫨ �� ���� � � ��砫� 横��
    call    sub_blue
    ;;pop     esi                         ; ���� ����⠭�������� ���稪 �����
    dec     esi 			  ; �����蠥� ���
    cmp     esi,0			  ; �ࠢ������ � �㫥�
    jnz     for_abz			  ; �᫨ �� ���� � � ��砫� 横��
    ret 				  ; �����頥� �ࠢ�����
;end_draw_palitra-------------------------------------------------------------------------------------


sub_red:
    ;.................................................................................................
    ; ������ ᬥ饭�� ��᭮�� 梥� � �����樨 梥� �������
    ;.................................................................................................
    push    eax
    mov     eax,[pnext]
  ;sub_red_0                              ; palitra
    cmp     al,0
    jne     sub_red_1
    sub     edx,0x40
    jmp     sub_red_e
  sub_red_1:				  ; random
    cmp     al,1
    jne     sub_red_2
    sub     edx,0x20
    jmp     sub_red_e
  sub_red_2:				  ; gree
    cmp     al,2
    jne     sub_red_3
    sub     edx,0x010101
    jmp     sub_red_e
  sub_red_3:				  ; red
    cmp     al,3
    jne     sub_red_4
    sub     edx,0x0101
    jmp     sub_red_e
  sub_red_4:				  ; green
    cmp     al,4
    jne     sub_red_5
    sub     edx,0x010001
    jmp     sub_red_e
  sub_red_5:				  ; blue
    cmp     al,5
    jne     sub_red_e
    sub     edx,0x010100
    jmp     sub_red_e
  sub_red_e:
    pop     eax
    ret 				  ; �����頥� �ࠢ�����
;end_sub_red------------------------------------------------------------------------------------------

sub_green:
    ;.................................................................................................
    ; ������ ᬥ饭�� ��᭮�� 梥� � �����樨 梥� �������
    ;.................................................................................................
    push    eax
    mov     eax,[pnext]
  ;sub_green_0
    cmp     al,0
    jne     sub_green_1
    sub     edx,0x4000
    jmp     sub_green_e
  sub_green_1:
    cmp     al,1
    jne     sub_green_e
    sub     edx,0x2000
    ;jmp     sub_green_e
  sub_green_e:
    pop     eax
    ret 				  ; �����頥� �ࠢ�����
;end_sub_green----------------------------------------------------------------------------------------


sub_blue:
    ;.................................................................................................
    ; ������ ᬥ饭�� ��᭮�� 梥� � �����樨 梥� �������
    ;.................................................................................................
    push    eax
    mov     eax,[pnext]
  ;sub_blue_0
    cmp     al,0
    jne     sub_blue_1
    sub     edx,0x600000
    jmp     sub_blue_e
  sub_blue_1:
    cmp     al,1
    jne     sub_blue_e
    sub     edx,0x400000
    ;jmp     sub_blue_e
  sub_blue_e:
    pop     eax
    ret 				  ; �����頥� �ࠢ�����
;end_sub_blue-----------------------------------------------------------------------------------------

draw_result:
    ;.................................................................................................
    ; ������ ࠬ�� �뢮�� १����
    ;.................................................................................................
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    mov     edx,0x00666666		  ; 梥�-�������
    mov     ebx,4 shl 16+98		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,9 shl 16+148		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40
    mov     edx,0x00F3F3F3		  ; 梥�-䮭�
    mov     ebx,5 shl 16+96		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,10 shl 16+146		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40

    ;.................................................................................................
    ; ���ᮢ�� १���� 梥� � hex color2
    ;.................................................................................................
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    ; color 2 - draw fringle
    mov     edx,0x222222		  ; 梥�-�������
    mov     ebx,16 shl 16+15		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,16 shl 16+15		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���뢠��� (�� ���� ��אַ㣮�쭨�)
    ; color 2 - draw color
    mov     edx,[color2]		  ; 梥�
    mov     ebx,17 shl 16+13		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,17 shl 16+13		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���뢠��� (�� ���� ��אַ㣮�쭨�)
    ; color 1 - draw fringle
    mov     edx,0x222222		  ; 梥�-�������
    mov     ebx,11 shl 16+15		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,12 shl 16+15		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���뢠��� (�� ���� ��אַ㣮�쭨�)
    ; value - draw fringle
    mov     ebx,34 shl 16+62		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,16 shl 16+15		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���뢠��� (�� ���� ��אַ㣮�쭨�)

    ; color 2 - draw color
    mov     edx,[color] 		  ; 梥�
    mov     ebx,12 shl 16+13		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,13 shl 16+13		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���뢠��� (�� ���� ��אַ㣮�쭨�)
    ; value - draw background
    mov     edx,0xFFFFFF		  ; 梥�-䮭�
    mov     ebx,35 shl 16+60		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,17 shl 16+13		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40			  ; ���뢠��� (�� ���� ��אַ㣮�쭨�)

    mov     eax,47			  ; �㭪�� 47 - �뢮� �᫠ � ����
    mov     ecx,[color] 		  ; �᫮ (�� bl=0) ��� 㪠��⥫� (�� bl=1)
    mov     esi,0x0			  ; 0xX0RRGGBB
    mov     ebx,256+8 shl 16		  ; ��ࠬ���� �८�ࠧ������ �᫠ � ⥪�� (HEX)
    mov     edx,45 shl 16+20		  ; [���न��� �� �� x]*65536 + [���न��� �� �� y]
    int     0x40			  ; ���뢠��� - �뢮��� १���� � ���� (HEX)

    mov     eax,4			  ; �㭪�� 4: ������� ⥪�� � ����
    mov     ebx,38*65536+20		  ; [x ��砫��] *65536 + [y ��砫��]
    mov     ecx,0x0			  ; 梥� ⥪�� RRGGBB
    mov     edx,hex			  ; ��㥬 '#'
    mov     esi,1			  ; ����� ⥪�� � �����
    int     0x40

    ;.................................................................................................
    ; ���ᮢ�� r g b ���祭��
    ;.................................................................................................
    call    get_spectr

    movzx   eax,[cred]			  ; ����
    mov     ebx,9*65536+142		  ; [x ��砫��] *65536 + [y ��砫��]
    call    draw_value			  ; �뢮��� १����

    movzx   eax,[cgreen]		  ; ������
    mov     ebx,32*65536+142		  ; [x ��砫��] *65536 + [y ��砫��]
    call    draw_value			  ; �뢮��� १����

    movzx   eax,[cblue] 		  ; ᨭ��
    mov     ebx,55*65536+142		  ; [x ��砫��] *65536 + [y ��砫��]
    call    draw_value			  ; �뢮��� १����

    movzx   eax,[calpha]		  ; ����
    mov     ebx,78*65536+142		  ; [x ��砫��] *65536 + [y ��砫��]
    call    draw_value			  ; �뢮��� १����

    ;.................................................................................................
    ; �뢮��� �㪢� r g b a
    ;.................................................................................................
    mov     eax,4			  ; 4 - �뢥�� ��ப� ⥪�� � ����
    mov     ebx,16 shl 16+40		  ; [���न��� �� �� x]*65536 + [���न��� �� �� y]
    mov     ecx,0x0			  ; 0xX0RRGGBB (RR, GG, BB ������ 梥� ⥪��)
    mov     edx,cname			  ; 㪠��⥫� �� ��砫� ��ப�
    mov     esi,1			  ; �뢮���� esi ᨬ�����
    newline_s:				  ; 横�
      int     0x40			  ; ���뢠���
      add     ebx,23 shl 16		  ; ������塞
      add     edx,1			  ; ������塞
      cmp     [edx],byte 'x'		  ; �ࠢ����� � ���⮬ �
    jne    newline_s			  ; �᫨ �� ��� ��� �� ࠢ��

    ;.................................................................................................
    ; ���ᮢ�� ᫠���஢
    ;.................................................................................................
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    mov     edx,0x222222		  ; 梥�-�������
    mov     ebx,16 shl 16+4		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,51 shl 16+86		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40
    add     ebx,23 shl 16		  ; ��砫�� ���न���� �� x [�-� y]*65536 + [ࠧ���]
    int     0x40
    add     ebx,23 shl 16		  ; ��砫�� ���न���� �� x [�-� y]*65536 + [ࠧ���]
    int     0x40
    add     ebx,23 shl 16		  ; ��砫�� ���न���� �� x [�-� y]*65536 + [ࠧ���]
    int     0x40

    mov     edx,0xFA0919		  ; 梥�-�������
    mov     ebx,17 shl 16+2		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    mov     ecx,52 shl 16+84		  ; ��砫�� ���न���� �� y [�-� y]*65536 + [ࠧ���]
    int     0x40
    mov     edx,0x08CE19		  ; 梥�-�������
    add     ebx,23 shl 16		  ; ��砫�� ���न���� �� x [�-� y]*65536 + [ࠧ���]
    int     0x40
    mov     edx,0x0909FF		  ; 梥�-�������
    add     ebx,23 shl 16		  ; ��砫�� ���न���� �� x [�-� y]*65536 + [ࠧ���]
    int     0x40
    mov     edx,0xE4E4E4		  ; 梥�-�������
    add     ebx,23 shl 16		  ; ��砫�� ���न���� �� x [�-� y]*65536 + [ࠧ���]
    int     0x40

    ;.................................................................................................
    ; ���ᮢ�� ����㭪��
    ;.................................................................................................
    mov     edx,0x0			  ; 梥�-����㭪��
    mov     ebx,17 shl 16+11		  ; ��砫�� ���न���� �� x [�-� x]*65536 + [ࠧ���]
    movzx   eax,[cred]			  ; ��६ ���祭�� 梥�
    call    draw_slider
    add     ebx,23 shl 16
    movzx   eax,[cgreen]		  ; ��६ ���祭�� 梥�
    call    draw_slider
    add     ebx,23 shl 16
    movzx   eax,[cblue] 		  ; ��६ ���祭�� 梥�
    call    draw_slider
    add     ebx,23 shl 16
    movzx   eax,[calpha]		  ; ��६ ���祭�� 梥�
    call    draw_slider

    ret 				  ; �����頥� �ࠢ�����
;end_draw_result

draw_slider:
    xor     ecx,ecx
    mov     cl,0x3			  ; �㤥� ������ ��� �� 3 ⠪ ��� ����㭪� ������� 85 ���ᥫ��
    div     cl				  ; ����� - 楫�� � al ���⮪ � ah
    mov     cl,136			  ; ������ �窠 ����㭪�
    sub     cl,al			  ; cl=cl-al
    shl     ecx,16
    add     ecx,1			  ; ������ ��砫쭮� ���������
    mov     eax,13			  ; �㭪�� 13 - ���ᮢ��� ��אַ㣮�쭨�
    int     0x40
    push    ebx 			  ; ��࠭塞 ��� ᫥���饣� ����㭪� x+w
    sub     ebx,8			  ; ⠪ ��� �ਭ� ����� 11 � �⭨���� 8 �⮡� �㡨� �⠫ �ਭ�� 3
    add     ebx,8 shl 16		  ; ���न���� �� x ��� ������ �� ����㭪� ᬥ頥� �� 8 � �ࠢ�
    inc     ecx 			  ; �뤫������ ����� ����㭪� �� 3-� (1+1=2)
    inc     ecx 			  ; (2+1=3) �� ��� � ��� �㤥� �㡨� 3�3 �� 8 � �ࠢ� �� �����
    int     0x40			  ; ��㥬
    pop     ebx 			  ; ����⠭�������� ��ଠ��� ���न����
    ret 				  ; �����頥� �ࠢ�����

;end_slider

draw_value:
    ;.................................................................................................
    ; �뢮� �᫠ �� ��ப� � 㪠������ ������
    ;.................................................................................................
    push    ebx 			  ; ��࠭塞 ��᫠��� ���न����
    mov     ebx,10			  ; ��⠭�������� �᭮����� ��⥬� ��᫥���
    mov     edi,buff			  ; 㪠��⥫� �� ��ப� ����
    call    int2ascii			  ; ��������㥬 �᫮ � ����� ��� ��ப� � ���� + esi �����
    mov     eax,4			  ; �㭪�� 4: ������� ⥪�� � ����
    pop     ebx 			  ; ���⠥� �� �⥪� ��᫠��� ���न����
    cmp     esi,2			  ; ������������� ������
    jne     draw_value_1
    add     ebx,4 shl 16
    jmp     draw_value_e
  draw_value_1:
    cmp     esi,1
    jne     draw_value_e
    add     ebx,7 shl 16
  draw_value_e:
    mov     ecx,0x0			  ; 梥� ⥪�� RRGGBB
    mov     edx,buff			  ; 㪠��⥫� �� ��砫� ⥪��
    int     0x40
    ret 				  ; �����頥� �ࠢ�����
;end_draw_value

  _read_params:
      mov   [fi.p00], dword 0
      mov   [fi.p04], dword 0
      mov   [fi.p08], dword 0
      mov   [fi.p12], dword 6
      mov   [fi.p16], dword fi.file
      mov   [fi.p20], dword 0
      mov   [fi.p21], dword dat_file
      mcall 70, fi

      xor   eax, eax
      mov   al, [fi.file+2]
      shl   eax, 8
      mov   al, [fi.file+1]
      shl   eax, 8
      mov   al, [fi.file+0]
      mov   [color], eax

      xor   eax, eax
      mov   al, [fi.file+5]
      shl   eax, 8
      mov   al, [fi.file+4]
      shl   eax, 8
      mov   al, [fi.file+3]
      mov   [color2], eax

   ;   mov al,[params+2]
   ;   mov [params_c+0],al
   ;
   ;   mov al,[params+3]
   ;   mov [params_c+1],al
   ;
   ;   mov al,[params+4]
   ;   mov [params_c+2],al
   ;
   ;   mov al,[params+5]
   ;   mov [params_c+3],al
   ;
   ;   mov al,[params+6]
   ;   mov [params_c+4],al
   ;
   ;   mov al,[params+7]
   ;   mov [params_c+5],al
   ;
   ;   mov al,[params+8]
   ;   mov [params_c+6],al
   ;
   ;   mov al,[params+9]
   ;   mov [params_c+7],al
   ;
   ;   mov   esi,params_c
   ;   mov   ecx,16
   ;   call  ascii2int
   ;   mov   [color],eax
   ;
   ;   mov al,[params+11]
   ;   mov [params_c+0],al
   ;
   ;   mov al,[params+12]
   ;   mov [params_c+1],al
   ;
   ;   mov al,[params+13]
   ;   mov [params_c+2],al
   ;
   ;   mov al,[params+14]
   ;   mov [params_c+3],al
   ;
   ;   mov al,[params+15]
   ;   mov [params_c+4],al
   ;
   ;   mov al,[params+16]
   ;   mov [params_c+5],al
   ;
   ;   mov al,[params+17]
   ;   mov [params_c+6],al
   ;
   ;   mov al,[params+18]
   ;   mov [params_c+7],al
   ;
   ;   mov   esi,params_c
   ;   mov   ecx,16
   ;   call  ascii2int
   ;   mov   [color2],eax

      ret

hex_digit:
    ;.................................................................................................
    ; �८�ࠧ������ � ASCII (��� ����ᨬ��� �� ��⥬� ��᫥���)
    ;.................................................................................................
    cmp    dl,10			  ; � dl ��������� �᫮ �� 0 �� 15
    jb	   .less			  ; �᫨ dl<10 � ���室��
    add    dl,'A'-10			  ; 10->A 11->B 12->C ...
    ret 				  ; �����頥� �ࠢ�����
    .less:
    or	   dl,'0'			  ; �᫨ ��⥬� ��᫥��� 10-� � �����
    ret 				  ; �����頥� �ࠢ�����
;end_hex_digit

int2ascii:
    ;.................................................................................................
    ; �८�ࠧ������ �᫠ � ��ப�
    ;.................................................................................................
    ; eax - 32-� ���筮� �᫮
    ; ebx - �᭮����� ��⥬� ��᫥���
    ; edi - 㪠��⥫� �� ��ப� ����
    ; �����頥� ���������� ���� � esi - ����� ��ப�
    ;pushad
    xor     esi,esi			  ; ����塞 ���稪 ᨬ�����
    convert_loop:
    xor     edx,edx			  ; ����塞 ॣ���� ��� ���⮪
    div     ebx 			  ; eax/ebx - ���⮪ � edx
    call    hex_digit			  ; �८�ࠧ㥬 ᨬ���
    push    edx 			  ; ����� � �⥪
    inc     esi 			  ; 㢥��稢��� ���稪
    test    eax,eax			  ; �᫨ �� ����� ������
    jnz     convert_loop		  ; � ������ ��
    cld 				  ; ����������� ���뢠�� 䫠� ���ࠢ����� DF (������ �����)
    write_loop: 			  ; ����
    pop     eax 			  ; ���⠥� �� �⥪� � ���
    stosb				  ; �����뢠�� � ���� �� ����� ES:(E)DI
    dec     esi 			  ; 㬥��蠥� ���稪
    test    esi,esi			  ; �᫨ ���� �� ���⠢��� �� �⥪�
    jnz     write_loop			  ; � �����
    mov     byte [edi],0		  ; ���� ������뢠�� �㫥��� ����
    ;popad                                 ; ����⠭�������� ���祭�� ॣ���஢
    ; ��� ���� �� ����� ��祣� ��饣� � �㭪樨, ���� �����頥� �� ������ ����祭��� ��ப�
    mov     edi,buff			  ; 㪠��⥫� �� ��砫� ⥪��
    call    str_len
    mov     esi,eax
    ret 				  ; � �����頥� �ࠢ�����
;end_int2ascii

char2byte:
    sub al,'0'
    cmp al,10
    jb	done
    add al,'0'
    and al,0x5f
    sub al,'A'-10
    and al,0x0f
    done:
    ret
;end_char2byte

ascii2int:
    ;.................................................................................................
    ; �८�ࠧ������ ��ப� � �᫮
    ;.................................................................................................
    ; esi - 㪠��⥫� �� ����ନ��஢����� ��ப�
    ; ecx - �᭮����� ��⥬� ��᫥���
    ; �����頥� eax - �᫮
    push esi
    xor eax,eax
    xor ebx,ebx
    cmp byte [esi],'-'
    jnz .next
    inc esi
    .next:
    lodsb
    or al,al
    jz .done
    call char2byte
    imul ebx,ecx
    add ebx,eax
    jmp .next
    .done:
    xchg ebx,eax
    pop esi
    cmp byte [esi],'-'
    jz .negate
    ret
    .negate:
    neg eax
    ret
;end_ascii2int

get_spectr:
    ;.................................................................................................
    ; �����頥� r,g,b ��⮢���騥 梥�
    ;.................................................................................................
    mov     ecx,[color]
    mov     [cblue],cl
    shr     ecx,8
    mov     [cgreen],cl
    shr     ecx,8
    mov     [cred],cl
    shr     ecx,8
    mov     [calpha],cl
    ret 				  ; � �����頥� �ࠢ�����
;end_get_spectr

set_spectr:
    ;.................................................................................................
    ; ��⠭�������� �� r,g,b 梥�
    ;.................................................................................................
    movzx   eax,[calpha]
    shl     eax,8
    mov     al,[cred]
    shl     eax,8
    mov     al,[cgreen]
    shl     eax,8
    mov     al,[cblue]
    mov     [color],eax
    call    draw_result 		  ; �뢮��� १����
    ret 				  ; � �����頥� �ࠢ�����
;end_get_spectr

str_len:
    ;.................................................................................................
    ; ��।���� ����� ��ப� (�室->EDI ZS offset ; ��室->EAX ZS length)
    ;.................................................................................................
	push ecx
	push esi
	push edi

	cld
	xor   al, al
	mov ecx, 0FFFFFFFFh
	mov esi, edi
	repne scasb
	sub edi, esi
	mov eax, edi
	dec eax

	pop edi
	pop esi
	pop ecx

	ret
;end_str_len



;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ���� ���������� � ��������                                                                         |
;----------------------------------------------------------------------------------------------------/
circle:
    dat_file	db '/sys/settings/wallpaper.dat', 0

    cm		db 12 dup(0)
    color	dd 00000000h		  ; �࠭�� ���祭�� ��࠭���� 梥�
    color2	dd 00FFFFFFh		  ; �࠭�� ���祭�� ��ண� ��࠭���� 梥�
    mouse_x	dd 0			  ; �࠭�� ��������� � ���न���� ���
    mouse_y	dd 0			  ; �࠭�� ��������� � ���न���� ���
    mouse_f	dd 0			  ; �࠭�� ����� � ⮬ ����� ������ ��� �뫠 �����
    desctop_w	dd 0			  ; �࠭�� �ਭ� �࠭�
    desctop_h	dd 0			  ; �࠭�� ����� �࠭�
    sc		system_colors		  ; �࠭�� �������� ��⥬��� 梥⮢ ᪨��
    title	db 'Palitra v0.75',0	  ; �࠭�� ��� �ணࠬ��
    hidden	db 'Hidden',0
    hex 	db '#',0		  ; ��� �뢮�� ���⪨ ��� ⥪��
    cname	db 'RGBAx'		  ; �࠭�� ࠧ��� 梥⮢ (red,green,blue) x-��⪠ ����
    cred	db 0			  ; �࠭�� ���� ᯥ���
    cgreen	db 0			  ; �࠭�� ������ ᯥ���
    cblue	db 0			  ; �࠭�� ᨭ�� ᯥ���
    calpha	db 0			  ; �࠭�� �஧�筮���
    larrow	db 0x1A,0
    buff	db '000',0
    bground	db 'BACKGROUND',0	  ; ��� ������ - 14
    pnext	dd 0			  ; ���稪 ��४��祭�� �������
    renmode	dd 0			  ; ०�� ���ᮢ�� (1-梥⮢�� �奬�,2-����⪠,3-��㣮���)
    runmode	dd 1			  ; ०�� ����᪠ (1-normal, 2-hidden, 3-colordialog)
    params	db 20 dup(0)		  ; ��� ��ࠬ��஢
 ;   params_c    db 9  dup(0)              ; ��񬭨� ��� 梥�
I_END:


fi:
 .p00:
    rd	    1
 .p04:
    rd	    1
 .p08:
    rd	    1
 .p12:
    rd	    1
 .p16:
    rd	    1
 .p20:
    rb	    1
 .p21:
    rd	    1
 .file:
    rb	    32

I_RDATA: