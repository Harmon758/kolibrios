;#___________________________________________________________________________________________________
;****************************************************************************************************|
; Program Palitra (c) Sergei Steshin (Akyltist)                                                      |
;----------------------------------------------------------------------------------------------------|
;; Charset:DOS-866 Font:Courier New Size:9pt                                                         |
;.....................................................................................................
;; compiler:     FASM 1.69.31                                                                        |
;; version:      0.3.0                                                                               |
;; last update:  08/11/2012                                                                          |
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
;; 0.7.6 - �������� ०�� B, ����� �ந������ ������ 䮭� ࠡ�祣� �⮫� �㬭�� �������� (e-andrew)|
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
  org    0
  db     'MENUET01'
  dd     1,START,I_END,I_MEM,stacktop,params,sys_path

  include '../../../macros.inc'
  include '../../../proc32.inc'
  include '../../../KOSfuncs.inc'
  include '../../../dll.inc'

  include 'draw_sliders.inc'
  include 'draw_utils.inc'
  include 'draw_palitra.inc'



  WIN_W  = 374            ; �ਭ� ����
  WIN_H  = 251            ; ���� ����
  WIN_X  = 250            ; ���न��� � ����
  WIN_Y  = 190            ; ���न��� � ����

  Left_Border=4
  SliderPanel_W = 110
  DRAWY  = 9

  CELLW  = 11; 11            ; not used yet, but has to be :)

  ICONX  = WIN_W - 39
  ICONS  = 18             ; icon size  
  SLIDEW = 25

  palitra_x = Left_Border+SliderPanel_W+12
  palitra_w = CELLW*(8)+8+1
  palitra_xw = palitra_x shl 16 + palitra_w
  palitra_yw = DRAWY shl 16 + palitra_w

START:
    mcall   SF_SYS_MISC,SSF_HEAP_INIT ; ���樠������ ���
    mcall SF_SYS_MISC, SSF_MEM_OPEN, i18_name
    mov [icons18], eax
    mcall SF_SYS_MISC, SSF_MEM_OPEN, i18bg_name
    mov [icons18bg], eax

    stdcall dll.Load, @IMPORT
    or      eax,eax
    jnz     bexit
    mcall   SF_SET_EVENTS_MASK,EVM_REDRAW+EVM_KEY+EVM_BUTTON+EVM_MOUSE ; ��⠭�������� ���� ᮡ�⨩
    include 'params_init.inc'             ; ��ࠡ�⪠ ��ࠬ��஢ ��������� ��ப�

;#___________________________________________________________________________________________________
;****************************************************************************************************|
; �������� ���� ��������� - ��������� �������                                                        |
;----------------------------------------------------------------------------------------------------/
red:
    call draw_main                        ; ��뢠�� ����ᮢ�� ���� �ਫ������
still:
    mcall   SF_WAIT_EVENT         ; �㭪�� 10 - ����� ᮡ���
    cmp     eax,EV_REDRAW                 ; ����ᮢ��� ���� ?
    je      red                           ; �᫨ �� - �� ���� red
    cmp     eax,EV_KEY                    ; ����� ������ ?
    je      key                           ; �᫨ �� - �� key
    cmp     eax,EV_BUTTON                 ; ����� ������ ?
    je      button                        ; �᫨ �� - �� button
    cmp     eax,EV_MOUSE                  ; ᮡ�⨥ �� ��� ��� ����
    je      mouse                         ; �᫨ �� - �� button
    jmp     still                         ; �᫨ ��㣮� ᮡ�⨥ - � ��砫� 横��
;end_still

key:                                      ; ����� ������ �� ���������
    mcall   SF_GET_KEY    ; �㭪�� 2 - ����� ��� ᨬ���� (� ah) (��� � �ਭ樯� �� �㦭�)
    jmp     still                         ; �������� � ��砫� 横��
;end_key

mouse:
    cmp     [renmode],2
    jne     left
    call    cyrcle_draw
    jmp     center
  left:
    mcall   SF_MOUSE_GET,SSF_BUTTON
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
    mcall   SF_MOUSE_GET,SSF_BUTTON
    cmp     al,100b
    jne     still
    mov     [mouse_f],3
    mov     [color],edx
    call    draw_result
    jmp     still                         ; �������� � ��砫� 横��
;end_mouse

button:
    mcall   SF_GET_BUTTON         ; 17 - ������� �����䨪��� ����⮩ ������
    cmp     ah, 1                         ; �᫨ ����� ������ � ����஬ 1,
    jz      bexit                         ; ��室��
  ;��ࠡ�⪠ ������ Next
    cmp     ah, 12                        ; �᫨ ����� ������ NEXT
    jne     next_bg                       ; ��室��
    inc     [pnext]                       ; 㢥��稢��� �� ����⨨ ����� �������
    mov     [renmode],0                   ; ����砥� 梥⮢� �奬�
    mov     eax,[pnext]                   ; ����ᨬ ���祭�� � ���
    cmp     al,6                          ; �ࠢ������ � ������ ������⢮� ������
    jne     next_redraw                   ; �᫨ �� ����� ���ᨬ㬠 � �� �맮� ���ᮢ��
    xor     eax,eax                       ; ���� ����塞 ������� �� default
    mov     [pnext],eax                   ; � ���������� �� ��ᨫ� ������� �� default
  next_redraw:
    call    draw_palitra                  ; ������ �������
    jmp     still                         ; �室�� �� �������� ��㣮�� ᮡ���
  next_bg:
    cmp     ah, 14                        ; ������ BACKGROUND
    jne     next_bg2                      ; �᫨ �� ����� � ��室��
    call    set_background                ; ���� ��⠭�������� 䮭
    jmp     still                         ; � �� �������� ᮡ���
  next_bg2:
    cmp     ah, 16                        ; ������ BACKGROUND
    jne     circle_bg                     ; �᫨ �� ����� � ��室��
    call    set_background2               ; ���� ��⠭�������� 䮭
    jmp     still                         ; � �� �������� ᮡ���
  circle_bg:
    cmp     ah, 15                        ; ������ ��㣮��� ������
    jne     next_end                      ; �᫨ �� ����� � ��室��
    mov     [renmode],2                   ; ����砥� ���ᮢ�� ��㣮��� �������
    call    draw_palitra                  ; ������ �������
    jmp     still                         ; � �� �������� ᮡ���
  next_end:
    cmp     ah,13                         ; COLOR SWAP
    jne     color_swap_end
    push    [color2]
    push    [color]
    pop     [color2]
    pop     [color]
    call    draw_result
    jmp     still                         ; � �室�� �� �������� ��㣮�� ᮡ���
  color_swap_end:
    cmp     ah, 7                         ; �஢��塞 ����� ������ � ID=7
    jne     color_button                  ; �᫨ �� �����, � ��� �����
    call    mouse_get                     ; ���� ����砥� ��ࠡ��稪 ���, �⮡� ����� ���祭�� 梥� � �������
    jmp     still                         ; � �室�� �� �������� ��㣮�� ᮡ���
  color_button:                           ; ����� ���न��� ��� ����㭪�� RGBA
    push    eax                           ; ���������� ���
    call    mouse_local                   ; ����砥� ������� ���न����
    mov     ebx, 188;137                       ; ������ �࠭�� ����㭪� �� �
    mov     ecx,[mouse_y]                 ; ����ᨬ � ��� ���祭�� ����� �� �
    sub     ebx,ecx                       ; ��室�� ࠧ����� (�.�. �㤠 ᬥ頥��� ����㭮�)
    mov     ecx, 2;3                         ; ����ᨬ � ��� ���� 3 (256/3=85, ��� 85-���� ����㭪��)
    imul    ecx,ebx                       ; ��室�� ��⨭�� ��ࠬ��� 梥� � ���⮬ ����⠡� ����㭪�---+
    pop     eax                           ; ����⠭�������� ���                                          :
  ;red_button:                            ; ���� �४���                                              :
    cmp     ah, 8                         ; ID=8                                                         :
    jne     green_button                  ; �᫨ ���, � �஢��塞 ����� �४���                       :
    mov     [cred],cl                     ; ���� ��ᢠ����� ���祭��, ��᭮�� 梥�� ᯥ���    <------+
    call    set_spectr                    ; ��⠭�������� ᯥ���
    jmp     still                         ; �室�� �� �������� ��㣮�� ᮡ���
  green_button:
    cmp     ah, 9
    jne     blue_button
    mov     [cgreen],cl
    call    set_spectr
    jmp     still                         ; �室�� �� �������� ��㣮�� ᮡ���
  blue_button:
    cmp     ah, 10
    jne     alpha_button
    mov     [cblue],cl
    call    set_spectr
    jmp     still                         ; �室�� �� �������� ��㣮�� ᮡ���
  alpha_button:
    cmp     ah, 11
    jne     still
    mov     [calpha],cl
    call    set_spectr
    jmp     still                         ; �室�� �� �������� ��㣮�� ᮡ���
  bexit:
    mcall SF_TERMINATE_PROCESS ; ���� ����� �ணࠬ��
;end_button

;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ������� ������ ��������� ���� � ��������� ����������                                               |
;----------------------------------------------------------------------------------------------------/
draw_main:
    ; �㭪�� 12: ����砥�, �� �㤥� �ᮢ����� ����
    mcall   SF_REDRAW,SSF_BEGIN_DRAW

    ; �㭪�� 48 - �⨫� �⮡ࠦ���� ����
    mcall   SF_STYLE_SETTINGS,SSF_GET_COLORS,sc,sizeof.system_colors

    ; �㭪�� 48 - �⨫� �⮡ࠦ���� ����
    mcall   SF_STYLE_SETTINGS,SSF_GET_SKIN_HEIGHT
    mov     ecx,eax                       ; ���������� ����� ᪨��

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
    mov     edi,title                     ; ��������� ����
  set_title:

    xor     eax,eax                       ; ��頥� eax (mov eax,0) (�㭪�� 0)
    mov     ebx,WIN_X shl 16+WIN_W        ; [���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
    add     ecx,WIN_Y shl 16+WIN_H        ; ���� ᪨�� + [���न��� �� y]*65536 + [ࠧ��� �� y] (168 ��� ���ᨨ 0.2)
    mov     edx,[sc.work]                 ; ������ �⨫� ���� �� ��䮫��
    or      edx,0x34000000                ; ��� ���� � ᪨��� 䨪�஢����� ࠧ��஢

    int     0x40                          ; ���뢠���


    call    draw_palitra                  ; ������ �������
    call    draw_result                   ; ������ ���������

    mcall   SF_DEFINE_BUTTON, <palitra_x,palitra_w*2+1  > , <DRAWY,palitra_w*2+2>, 7+BT_HIDE ; palitra

    inc     edx
    mcall   , <10,22>, <56,128>           ; ���㥬 ��������� ������ ��� ᫠���஬ red
    add     ebx,25*65536                  ; ������塞
    inc     edx                           ; ID = 9
    int     0x40                          ; ���㥬 ��������� ������ ��� ᫠���஬ green
    add     ebx,25*65536                  ; ������塞
    inc     edx                           ; ID = 10
    int     0x40                          ; ���㥬 ��������� ������ ��� ᫠���஬ blue
    add     ebx,25*65536                  ; ������塞
    inc     edx                           ; ID = 11
    int     0x40                          ; ���㥬 ��������� ������ ��� ᫠���஬ alpha

    ; �㭪�� 8 - ��।�����/㤠���� ������ (����� �����)
    mcall   , <13,19>, <20,18>, 0x0D+BT_HIDE

    call    draw_bottom_panel
    call    draw_left_panel

    ; �㭪�� 12: ����砥�, �� �㤥� �ᮢ����� ����
    mcall SF_REDRAW,SSF_END_DRAW
    ret



;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ���� ��������������� �������� � ������� ����������                                                 |
;----------------------------------------------------------------------------------------------------/


    ;------------------------------------------------------------------------------------------------+
    draw_left_panel:                      ; ���ᮢ�� ������� ������  SL97: �� ᠬ�� ���� �ࠢ��.
    ;.................................................................................................
    ; button_next_colorsheme
    mcall   SF_DEFINE_BUTTON, <ICONX,ICONS+3>, <DRAWY,ICONS+3>, 12+BT_HIDE

    ; palitra button                    ; ID = 14
    mcall , ,(DRAWY+150) shl 16 + ICONS+3, 14+BT_HIDE

    ; pipet button                        ; ID = 15
    mcall , , (DRAWY+150) shl 16 + ICONS+3, 15+BT_HIDE

    mov     ebx,[icons18bg]
    add     ebx,ICONS*ICONS*4*53
    mcall   SF_PUT_IMAGE_EXT, ebx, <ICONS,ICONS>, <ICONX+2,DRAWY+2>, 32, 0, 0

    add     ebx,ICONS*ICONS*4*(39-53)
    mov     edx,(ICONX+2)*65536+WIN_H-90
    mcall

    mov     ebx,[icons18]
    add     ebx,ICONS*ICONS*4*(53-1)
    sub     edx,40
    mcall

    stdcall DrawDeepRectangle, ICONX-1, DRAWY-1,   ICONS+5, ICONS+5, [sc.work_graph], [sc.work_graph]
    stdcall DrawDeepRectangle, ICONX,   DRAWY,     ICONS+3, ICONS+3, [sc.work_light], [sc.work_dark]

    stdcall DrawDeepRectangle, ICONX-1, DRAWY+109, ICONS+5, ICONS+5, [sc.work_graph], [sc.work_graph]
    stdcall DrawDeepRectangle, ICONX,   DRAWY+110, ICONS+3, ICONS+3, [sc.work_dark], [sc.work_light]
    stdcall DrawDeepRectangle, ICONX+1, DRAWY+111, ICONS+1, ICONS+1, 0xFFFfff, 0xFFFfff

    stdcall DrawDeepRectangle, ICONX-1, DRAWY+149, ICONS+5, ICONS+5, [sc.work_graph], [sc.work_graph]
    stdcall DrawDeepRectangle, ICONX,   DRAWY+150, ICONS+3, ICONS+3, [sc.work_light], [sc.work_dark]

    ;stdcall DrawRectangle3D, ICONX, DRAWY, 22, 22, [sc.work_light], [sc.work_dark]   ;Leency: draw rectangle around the button, buggy now

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
    ret                                   ; return
    ;.................................................................................................


    ;------------------------------------------------------------------------------------------------+
    draw_bottom_panel:                    ; ���ᮢ�� ������ ������
    ;.................................................................................................
    mcall   SF_DEFINE_BUTTON, <129,90>, <WIN_H-27,16>, 16, [sc.work_button]

    add     ebx, 100 shl 16
    add     edx, 2
    int     0x40

    ; Write string
    mov     ecx,[sc.work_text]            ; RGB
    add     ecx, 0x90000000
    mcall   SF_DRAW_TEXT, <35, WIN_H-26>, ,bground

    mov     ecx, [sc.work_button_text]
    add     ecx, 0x90000000
    add     ebx, 107 shl 16
    mov     edx, bground1
    mcall

    add     ebx, 113 shl 16
    mov     edx, bground2
    mcall

    mcall SF_DRAW_LINE, <4, WIN_W-14>, <WIN_H-35, WIN_H-35>, [sc.work_graph]
    ret
    ;.................................................................................................

mouse_global:
    ;.................................................................................................
    ; ����砥� ���न���� ���
    ;.................................................................................................
    mcall   SF_MOUSE_GET,SSF_SCREEN_POSITION
    ; eax = x*65536 + y, (x,y)=���न���� ����� ���
    mov     ecx,eax                       ;
    shr     ecx,16                        ; ecx = x+1
    movzx   edx,ax                        ; edx = y+1
    dec     ecx                           ; ecx = x
    dec     edx                           ; edx = y
    mov     [mouse_x],ecx                 ; mouse_x = x
    mov     [mouse_y],edx                 ; mouse_y = y
    ret                                   ; �����頥� �ࠢ�����
;end_mouse_global

mouse_local:
    ;.................................................................................................
    ; ����砥� ���न���� ��� �⭮�⥫쭮 ����
    ;.................................................................................................
    mcall   SF_MOUSE_GET,SSF_WINDOW_POSITION
        ; eax = x*65536 + y, (x,y)=���न���� ����� ���
    mov     ecx,eax                       ;
    shr     ecx,16                        ; ecx = x+1
    movzx   edx,ax                        ; edx = y+1
    dec     ecx                           ; ecx = x
    dec     edx                           ; edx = y
    mov     [mouse_x],ecx                 ; mouse_x = x
    mov     [mouse_y],edx                 ; mouse_y = y
    ret                                   ; �����頥� �ࠢ�����
;end_mouse_local

set_background:
    ;.................................................................................................
    ; ��⠭�������� 䮭 ࠡ�祣� �⮫�
    ;.................................................................................................
    ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    ; ����㭪�� 4 - ��⠭����� ०�� ���ᮢ�� 䮭�.
    ; ����� ���ᮢ�� - �������� (1), ������� (2)
    mcall   SF_BACKGROUND_SET,SSF_MODE_BG,2

    ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    ; ����㭪�� 1 - ��⠭����� ࠧ��� 䮭����� ����ࠦ����.
    mcall   SF_BACKGROUND_SET,SSF_SIZE_BG,2,2

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

    ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    ; ����㭪�� 5 - �������� ���� ���ᥫ�� �� 䮭.
    ; - �����⥫� �� ����� � �ଠ� BBGGRRBBGGRR
    ; - C��饭�� � ������ 䮭����� ����ࠦ����
    ; - ������ ������ � ����� = 3 * �᫮ ���ᥫ��
    mcall   SF_BACKGROUND_SET,SSF_IMAGE_BG,cm,0,3*4

    ; �㭪�� 15 - ࠡ�� � 䮭���� ��䨪��
    ; ����㭪�� 3 - ����ᮢ��� 䮭.
    mcall   SF_BACKGROUND_SET,SSF_REDRAW_BG

    stdcall save_eskin_ini, 'H '

    ret
;end_set_background

desktop_get:
    ;.................................................................................................
    ; ��।��塞 �ਭ� �࠭�
    ;.................................................................................................
    ; ��।��塞 �ਭ� �࠭� (eax = [xsize]*65536 + [ysize])
    mcall   SF_GET_SCREEN_SIZE ; xsize = ࠧ��� �� ��ਧ��⠫� - 1
    mov     ebx,eax                       ;
    shr     ebx,16                        ; ebx = xsize-1
    movzx   edx,ax                        ; edx = ysize-1 (��譨� ���)
    inc     ebx                           ; ebx = xsize
    inc     edx                           ; edx = ysize (��譨� ���)
    mov     [desctop_w],ebx
    mov     [desctop_h],edx
    ret
;end_desktop_get

mouse_get:
    mov     esi,2                         ; �������: 䫠� ��� ��������� ��横�������
    call    mouse_global
    call    desktop_get
    re_mouse_loop:                        ; �������: ��⪠ ��� ������ �᫨ ������ � ���
      mov     ebx,[desctop_w]
      imul    ebx,[mouse_y]               ; ebx = y*xsize
      add     ebx,[mouse_x]               ; ebx = y*xsize+x

      ;.................................................................................................
      ; ��६ 梥� � ������� � ��६�����
      ;.................................................................................................
       ;mov     ebx,ecx                    ;; ebx = y*xsize+x (��譨� ���)
      mcall   SF_GET_PIXEL    ; ����砥� 梥� � eax
      cmp     eax,[sc.work]               ; �ࠢ������ � 䮭�� �ਫ������
      je      mouse_err                   ; �᫨ �� �� - � ��祣� �� ������
      cmp     eax,0x222222                ; �ࠢ������ � 梥⮬ �⪨
      je      mouse_err                   ; �᫨ �� �� - � ��祣� �� ������
      jmp     mouse_set                   ; �������: ��룠�� �⮡� �� ���� 梥� �⪨
    mouse_err:                            ; �������: �᫨ ������ � ��� ��� 䮭
      inc     [mouse_y]                   ; �������: ᬥ頥� �� ��������� ᭠砫� �� �
      inc     [mouse_x]                   ; �������: ᬥ頥� �� ��������� ��⮬ �� �
      dec     esi                         ; �������: �����蠥� 䫠�
      cmp     esi,0                       ; �������: �ࠢ������ � �㫥�
    jz        mouse_exit                  ; �������: �᫨ ���� � ᤥ���� ��� �� �����
    jmp    re_mouse_loop                  ; �������: �᫨ �� ���� � ���஡㥬 ����� �ᥫ��� ���ᥫ�
    mouse_set:                            ; ���� ���������� ���� 梥�
      cmp     [mouse_f],1
      jne     was_right
      mov     [color],eax
      call    draw_result
      jmp     mouse_exit
    was_right:
      cmp     [mouse_f],2
      jne     mouse_exit
      mov     [color2],eax
      call    draw_result                   ; �뢮��� १����
    mouse_exit:
    ret                                   ; �����頥� �ࠢ�����
;end_mouse_get----------------------------------------------------------------------------------------






draw_value:
    ;.................................................................................................
    ; �뢮� �᫠ �� ��ப� � 㪠������ ������
    ;.................................................................................................
    push    ebx                           ; ��࠭塞 ��᫠��� ���न����
    mov     ebx,10                        ; ��⠭�������� �᭮����� ��⥬� ��᫥���
    mov     edi,buff              ; 㪠��⥫� �� ��ப� ����
        call    int2ascii         ; ��������㥬 �᫮ � ����� ��� ��ப� � ���� + esi �����
    mov     eax,SF_DRAW_TEXT  ; �㭪�� 4: ������� ⥪�� � ����
    pop     ebx                           ; ���⠥� �� �⥪� ��᫠��� ���न����
    cmp     esi,2                         ; ������������� ������
    jne     draw_value_1
    add     ebx,4 shl 16
    jmp     draw_value_e
  draw_value_1:
    cmp     esi,1
    jne     draw_value_e
    add     ebx,7 shl 16
  draw_value_e:
    mov     ecx,0x0;0x10000000                 ; 梥� ⥪�� RRGGBB
    add     ecx,[sc.work_text]
    mov     edx,buff                      ; 㪠��⥫� �� ��砫� ⥪��
    int     0x40
    ret                                   ; �����頥� �ࠢ�����
;end_draw_value

  _read_params:

      mov eax,dword[params+2]
      mov dword[params_c+0],eax

      mov eax,dword[params+6]
      mov dword[params_c+4],eax

      mov   esi,params_c
      mov   ecx,16
      call  ascii2int
      mov   [color],eax

      mov eax,dword[params+11]
      mov dword[params_c+0],eax

      mov eax,dword[params+15]
      mov dword[params_c+4],eax

      mov   esi,params_c
      mov   ecx,16
      call  ascii2int
      mov   [color2],eax

      ret

hex_digit:
    ;.................................................................................................
    ; �८�ࠧ������ � ASCII (��� ����ᨬ��� �� ��⥬� ��᫥���)
    ;.................................................................................................
    cmp    dl,10                          ; � dl ��������� �᫮ �� 0 �� 15
    jb     .less                          ; �᫨ dl<10 � ���室��
    add    dl,'A'-10                      ; 10->A 11->B 12->C ...
    ret                                   ; �����頥� �ࠢ�����
    .less:
    or     dl,'0'                         ; �᫨ ��⥬� ��᫥��� 10-� � �����
    ret                                   ; �����頥� �ࠢ�����
;end_hex_digit

int2ascii:
    ;.................................................................................................
    ; �८�ࠧ������ �᫠ � ��ப�
    ;.................................................................................................
    ; eax - 32-� ���筮� �᫮
    ; ebx - �᭮����� ��⥬� ��᫥���
    ; edi - 㪠��⥫� �� ��ப� ����
    ; �����頥� ���������� ���� � esi - ����� ��ப�
    push    edi
    xor     esi,esi                       ; ����塞 ���稪 ᨬ�����
    convert_loop:
    xor     edx,edx                       ; ����塞 ॣ���� ��� ���⮪
    div     ebx                           ; eax/ebx - ���⮪ � edx
    call    hex_digit                     ; �८�ࠧ㥬 ᨬ���
    push    edx                           ; ����� � �⥪
    inc     esi                           ; 㢥��稢��� ���稪
    test    eax,eax                       ; �᫨ �� ����� ������
    jnz     convert_loop                  ; � ������ ��
    cld                                   ; ����������� ���뢠�� 䫠� ���ࠢ����� DF (������ �����)
    write_loop:                           ; ����
    pop     eax                           ; ���⠥� �� �⥪� � ���
    stosb                                 ; �����뢠�� � ���� �� ����� ES:(E)DI
    dec     esi                           ; 㬥��蠥� ���稪
    test    esi,esi                       ; �᫨ ���� �� ���⠢��� �� �⥪�
    jnz     write_loop                    ; � �����
    mov     byte [edi],0                  ; ���� ������뢠�� �㫥��� ����
    pop     edi
    ; ��� ���� �� ����� ��祣� ��饣� � �㭪樨, ���� �����頥� �� ������ ����祭��� ��ப�
    call    str_len
    mov     esi,eax
    ret                                   ; � �����頥� �ࠢ�����
;end_int2ascii

char2byte:
    sub al,'0'
    cmp al,10
    jb  done
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
    ret                                   ; � �����頥� �ࠢ�����
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
    call    draw_result                   ; �뢮��� १����
    ret                                   ; � �����頥� �ࠢ�����
;end_get_spectr

str_len:
    ;.................................................................................................
    ; ��।���� ����� ��ப� (�室->EDI ZS offset ; ��室->EAX ZS length)
    ;.................................................................................................
        push ecx esi edi

        cld
        xor   al, al
        mov ecx, 0FFFFFFFFh
        mov esi, edi
        repne scasb
        sub edi, esi
        mov eax, edi
        dec eax

        pop edi esi ecx

        ret
;end_str_len


 ;-------------------------------

 proc random uses ebx ecx edx, max_value
    mov     ebx, 0
    mov     eax, ebx
    or      eax, eax
    jnz     @f
    rdtsc
    xor     eax, edx
    mov     ebx, eax

 @@:
    xor     edx, edx
    mov     ecx, 127773
    div     ecx
    mov     ecx, eax
    mov     eax, 16807
    mul     edx
    mov     edx, ecx
    mov     ecx, eax
    mov     eax, 2836
    mul     edx
    sub     ecx, eax
    xor     edx, edx
    mov     eax, ecx
    mov     ebx, ecx
    mov     ecx, 100000
    div     ecx
    mov     eax, edx

    xor     edx, edx
    mov     ebx, [max_value]
    div     ebx
    mov     eax, edx

    ret
 endp

set_background2:
    mcall   SF_SYS_MISC, SSF_HEAP_INIT
    mcall   SF_SYS_MISC, SSF_MEM_ALLOC, 256 * 256 * 3
    mov     [bgimg_buf], eax

    mov     edx, eax
    mov     ecx, 256 * 256
  @@:
    stdcall random, 15 + 1
    sub     al, 15 / 2

    mov     bh, byte [color + 0]
    add     bh, al
    mov     [edx + 0], bh
    mov     bh, byte [color + 1]
    add     bh, al
    mov     [edx + 1], bh
    mov     bh, byte [color + 2]
    add     bh, al
    mov     [edx + 2], bh
    add     edx, 3
    loop    @b

    mcall   SF_BACKGROUND_SET, SSF_SIZE_BG, 256, 256
    mcall   SF_BACKGROUND_SET, SSF_MODE_BG, 1
    mcall   SF_BACKGROUND_SET, SSF_IMAGE_BG, [bgimg_buf], 0, 256 * 256 * 3
    mcall   SF_BACKGROUND_SET, SSF_REDRAW_BG

    mcall   SF_SYS_MISC, SSF_MEM_FREE, [bgimg_buf]
    stdcall save_eskin_ini, 'B '
ret

align 4
proc save_eskin_ini, opt_HB:dword
        ;save to file eskin.ini
        xor     al,al
        mov     ecx,1024
        mov     edi,sys_path+2
        repne   scasb
        sub     edi,sys_path+3
        invoke  ini_set_str, inifileeskin, amain, aprogram, sys_path+2, edi
        ;add param 'H '
        mov     eax,[opt_HB]
        mov     word[params],ax
        mov     eax,[color]
        or      eax,0xf ;��� ��������� ��筮�� 横�� �᫨ eax=0
        mov     edi,params+2
        @@:
        rol     eax,8
        or      al,al
        jnz     @f
        mov     word[edi],'00' ;�㫨 ��। �᫮�
        add     edi,2
        jmp     @b
        @@:
        and     al,0xf0
        jnz     @f
        mov     byte[edi],'0'
        inc     edi
        @@:
        mov     eax,[color]
        mov     ebx,16
        call    int2ascii
        mov     byte[params+10],' '
        ;add color2
        mov     eax,[color2]
        or      eax,0xf ;��� ��������� ��筮�� 横�� �᫨ eax=0
        mov     edi,params+11
        @@:
        rol     eax,8
        or      al,al
        jnz     @f
        mov     word[edi],'00' ;�㫨 ��। �᫮�
        add     edi,2
        jmp     @b
        @@:
        and     al,0xf0
        jnz     @f
        mov     byte[edi],'0'
        inc     edi
        @@:
        mov     eax,[color2]
        mov     ebx,16
        call    int2ascii

        invoke  ini_set_str, inifileeskin, amain, aparam, params, 19
        ret
endp

;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ���� ���������� � ��������                                                                         |
;----------------------------------------------------------------------------------------------------/
circle:
    title       db 'Palitra v0.77',0      ; �࠭�� ��� �ணࠬ��
    hidden      db 'Hidden',0
;    hex         db '#',0                 ; ��� �뢮�� ���⪨ ��� ⥪��
    cname       db 'RGBAx'                ; �࠭�� ࠧ��� 梥⮢ (red,green,blue) x-��⪠ ����
    larrow      db 0x1A,0
    buff        db '000',0
    bground     db 'Background',0         ; ��� ������ - 14
    bground1    db 'Gradient',0           ; ��� ������ - 15
    bground2    db 'Noisy',0              ; ��� ������ - 16
    runmode     dd 1                      ; ०�� ����᪠ (1-normal, 2-hidden, 3-colordialog)
    color2      dd 00FFFFFFh              ; �࠭�� ���祭�� ��ண� ��࠭���� 梥�

    inifileeskin db '/sys/settings/system.ini',0
    amain       db 'style',0
    aprogram    db 'bg_program',0
    aparam      db 'bg_param',0

    i18_name    db 'ICONS18',0
    i18bg_name  db 'ICONS18W',0


align 16
@IMPORT:

library \
    libini , 'libini.obj'

import  libini, \
        ini_set_str, 'ini_set_str'

I_END:
    cm          rb 12
    color       rd 1                      ; �࠭�� ���祭�� ��࠭���� 梥�
    mouse_x     rd 1                      ; �࠭�� ��������� � ���न���� ���
    mouse_y     rd 1                      ; �࠭�� ��������� � ���न���� ���
    mouse_f     rd 1                      ; �࠭�� ����� � ⮬ ����� ������ ��� �뫠 �����
    desctop_w   rd 1                      ; �࠭�� �ਭ� �࠭�
    desctop_h   rd 1                      ; �࠭�� ����� �࠭�
    sc          system_colors             ; �࠭�� �������� ��⥬��� 梥⮢ ᪨��
    cred        rb 1                      ; �࠭�� ���� ᯥ���
    cgreen      rb 1                      ; �࠭�� ������ ᯥ���
    cblue       rb 1                      ; �࠭�� ᨭ�� ᯥ���
    calpha      rb 1                      ; �࠭�� �஧�筮���
    pnext       rd 1                      ; ���稪 ��४��祭�� �������
    renmode     rd 1                      ; ०�� ���ᮢ�� (1-梥⮢�� �奬�,2-����⪠,3-��㣮���)
    params      rb 20                     ; ��� ��ࠬ��஢
    params_c    rb 9                      ; ��񬭨� ��� 梥�
    bgimg_buf   rd 1                      ; buffer for a generated image
    icons18     dd ?                      ; pointer to a shared memory of icons18.png
    icons18bg   dd ?                      ; pointer to a shared memory of icons18.png with filled bg

        rd 1024
stacktop:
        sys_path rb 1024
I_MEM: