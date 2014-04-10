;=============================================================================;
;============================[       HEADER       ]===========================;
;=============================================================================;
  use32
  org	 0x0

  db	 'MENUET01'
  dd	 0x01
  dd	 START
  dd	 I_END
  dd	 0x100000
  dd	 0x100000
  dd	 0x0
  dd	 0x0

  include 'macros.inc'
;=============================================================================;
;============================[       EVENTS       ]===========================;
;=============================================================================;
START:
    mcall   40,0xC0000027                 ; ������������� ����� �������
    call    button_init                   ; ������������� ��������� ������

    mov     eax,48                        ; ������� 48 - ����� ����������� ����
    mov     ebx,3                         ; ���������� 3 - �������� ����������� ����� ����.
    mov     ecx,sc                        ; ��������� �� ����� �������� edx ����, ��� ���������
    mov     edx,200                       ; ������ ������� ������ (������ ���� 40 ����)
    int     0x40                          ; ����������

    mov     eax,48                        ; ������� 48 - ����� ����������� ����
    mov     ebx,2                         ; ���������� 3 - �������� ����������� ����� ����.
    mov     ecx,sc                        ; ��������� �� ����� �������� edx ����, ��� ���������
    mov     edx,200                       ; ������ ������� ������ (������ ���� 40 ����)
    int     0x40                          ; ����������

    mov     eax,48                        ; ������� 48 - ����� ����������� ����
    mov     ebx,3                         ; ���������� 3 - �������� ����������� ����� ����.
    mov     ecx,sc                        ; ��������� �� ����� �������� edx ����, ��� ���������
    mov     edx,200                       ; ������ ������� ������ (������ ���� 40 ����)
    int     0x40                          ; ����������

red:
    call    draw_window
still:
    mov     eax,10                        ; ������� 10 - ����� �������
    int     0x40
    cmp     eax,1                         ; ������������ ���� ?
    je      red                           ; ���� �� - �� ����� red
    cmp     eax,2                         ; ������ ������� ?
    je      key                           ; ���� �� - �� key
    cmp     eax,3                         ; ������ ������ ?
    je      button                        ; ���� �� - �� button
    cmp     eax,6
    je      mouse
    jmp     still                         ; ���� ������ ������� - � ������ �����
;end_still

key:                                      ; ������ ������� �� ����������
    mov     eax,2                         ; ������� 2 - ������� ��� ������� (� ah) (��� � �������� �� �����)
    int     0x40
    jmp     still                         ; ��������� � ������ �����
;end_key

button:
    mov     eax,17                        ; 17 - �������� ������������� ������� ������
    int     0x40
    cmp     ah, 1                         ; ���� ������ ������ � ������� 1,
    jz      bexit                         ; �������
    jmp     still
  bexit:
    mov     eax,-1                        ; ����� ����� ���������
    int     0x40
;end_button

mouse:
    ;       ; ������ �� ���
    call    draw_edit
    call    mouse_local      ; �������� ���������� ���� ������������ ����
    call    button_calc      ; ����������� ��������� �������
    call    process_slot     ; �������� ������� � ������� �����
    call    button_draw      ; ������������ ������
    jmp     still            ; ������ �� �������� �������


;============================[        CODE        ]===========================;

    ret
;end_draw_result


;#___________________________________________________________________________________________________
;****************************************************************************************************|
; ������� ������ ��������� ���� � ��������� ����������                                               |
;----------------------------------------------------------------------------------------------------/
draw_window:
    mov     eax,12                        ; ������� 12: ��������, ��� ����� ���������� ����
    mov     ebx,1                         ; 1,������ ���������
    int     0x40                          ; ����������



    mov     eax,48                        ; ������� 48 - ����� ����������� ����.
    mov     ebx,4                         ; ���������� 4 - ���������� eax = ������ �����.
    int     0x40                          ; ����������
    mov     ecx,eax                       ; ���������� ������ �����

    xor     eax,eax                       ; ������� eax (mov eax,0) (������� 0)
    mov     ebx,200 shl 16+240            ; [���������� �� ��� x]*65536 + [������ �� ��� x]
    add     ecx,200 shl 16+280            ; ������ ����� + [���������� �� y]*65536 + [������ �� y]
    mov     edx,[sc.win_body]                 ; ������ ����� ���� �� �������
    or      edx,0x34000000                ; ��� ���� �� ������ ������������� ��������
    mov     edi,title                     ; ��������� ����
    int     0x40                          ; ����������

    call    process_slot                  ; �������� ������� � ������� �����
    call    button_draw
    call    draw_edit
    call    list_draw

    mov     eax,12                        ; ������� 12: ��������, ��� ����� ���������� ����
    mov     ebx,2                         ; 1,������ ���������
    int     0x40                          ; ����������

    ret



;===================================[ DATA ]==================================;
include   'inc/mouse.inc'                 ; ����
include   'inc/process.inc'               ; ���� � ��������
include   'inc/dtp.inc'                   ; ��������� ����� �������
include   'inc/button.inc'                ; ���������� ������
include   'inc/edit.inc'                  ; �������� ����������� �����
include   'inc/list.inc'                  ; ��������� ������

;; window -------------------------------------------------
    sc          new_colors                ; ����� ������� ������
    title       db 'Color Table',0        ; ���������
    mouse_x     dd 0                      ; ������ ���������� � ���������� ����
    mouse_y     dd 0                      ; ������ ���������� � ���������� ����
    mouse_l     dd 0                      ; ����� ������ 1 - ������ 0 - ���
    win_slot    dd 0                      ; 0- ���� �� �� �����, 1- �� �����
    buffer      rb  80                    ; ��� 9 �������

;; button -------------------------------------------------
    bnext       new_button
    bback       new_button
arrowa:
    file        'l.raw'
arrowb:
    file        'r.raw'
;; edit ---------------------------------------------------
    edit_cnt    dd 1                      ; counter
    edit_win    db 'WINDOW',0             ; 1
    edit_btn    db 'BUTTON',0             ; 2
    edit_gui    db 'ELEMENT',0            ; 3
    edit_cld    db 'SUPPORT',0            ; 4

;; list win -----------------------------------------------
    text_frame    db 'Frame (Activate):',0
    text_inframe  db 'Frame (Inactivate):',0
    text_fcframe  db 'Frame (Focus):',0
    text_face     db 'Face (Activate):',0
    text_inface   db 'Face (Inactivate):',0
    text_fcface   db 'Face (Focus):',0
    text_border   db 'Border (Activate):',0
    text_inborder db 'Border (Inactivate):',0
    text_wtext    db 'Text:',0
    text_graytext db 'Graytext:',0
    text_title    db 'Title:',0
    text_body     db 'Body:',0
    text_reserved db 'Reserved:',0
    text_text     db 'Text (Activate):',0
    text_intext   db 'Text (Inactivate):',0
    text_fctext   db 'Text (Focus):',0
    text_3dlight  db '3D Light:',0
    text_3ddark   db '3D Dark:',0
    text_3dface   db '3D Face:',0
    text_shadow   db 'Shadow:',0
    text_select   db 'Select:',0
    text_p_face   db 'Panel Body:',0
    text_p_frame  db 'Panel Frame:',0
    text_p_text   db 'Panel Text:',0
    text_m_face   db 'Menu Body:',0
    text_m_frame  db 'Menu Frame:',0
    text_m_text   db 'Menu Text:',0
    text_h_face   db 'Hint Body:',0
    text_h_frame  db 'Hint Frame:',0
    text_h_text   db 'Hint Text:',0
    text_hex      db '#',0
;----------------------------------------------------------
I_END:
