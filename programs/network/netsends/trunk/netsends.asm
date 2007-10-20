;
;    NetSend(Server)
;
;    ����: Hex
;    ����: www.mestack.narod.ru
;
;    ���ᠭ��:
;    �ணࠬ�� ��� ������ ᮮ�饭�ﬨ � ��.��ࢥୠ� ����.
;
;    Compile with FASM for Menuet
;    ������������ FASM'�� ��� ������ ��
;

use32
 org    0x0
 db     'MENUET01'    ; header
 dd     0x01          ; header version
 dd     START         ; entry point
 dd     I_END         ; image size
 dd     I_END+0x10000 ; required memory
 dd     I_END+0x10000 ; esp
 dd     0x0 , 0x0     ; I_Param , I_Path

include 'lang.inc'
include 'macros.inc'
remote_ip  db  192,168,0,1


START:                      ; start of execution

    mov  eax, 53                  ; open receiver socket
    mov  ebx, 0
    mov  ecx, 0x5000              ; local port
    mov  edx, 0xffff              ; remote port
    mov  esi, dword [remote_ip]   ; remote IP
    mcall
    mov  [socketNum],eax
    mov  [0],eax                  ; save for remote code

red:
    call draw_window            ; at first, draw the window

still:

    mov  eax,23                 ; wait here for event
    mov  ebx,1
    mcall

    cmp  eax,1                  ; redraw request ?
    jz   red
    cmp  eax,2                  ; key in buffer ?
    jz   key
    cmp  eax,3                  ; button in buffer ?
    jz   button

    mov  eax,53                 ; data from cluster terminal ?
    mov  ebx,2
    mov  ecx,[socketNum]
    mcall

    cmp  eax,0
    jne  data_arrived

    jmp  still

key:
    mov  eax,2
    mcall
    jmp  still

button:

    mov  eax,53
    mov  ebx,1
    mov  ecx,[socketNum]
    mcall
    or  eax,-1
    mcall


data_arrived:

    mov  eax,5                 ; wait a second for everything to arrive
    mov  ebx,10
    mcall

    mov  edi,I_END

  get_data:

    mov  eax,53
    mov  ebx,3
    mov  ecx,[socketNum]
    mcall

    mov  [edi],bl
    inc  edi

    mov  eax,53
    mov  ebx,2
    mov  ecx,[socketNum]
    mcall

    cmp  eax,0
    jne  get_data

    mov  eax,4
    mov  ebx,10*65536+60
    add  ebx,[y]
    mov  ecx,0x000000
    mov  edx,I_END
    mov  esi,100
    mcall

    add  [y],10

    jmp  still

y   dd   0x10



;   *********************************************
;   *******  WINDOW DEFINITIONS AND DRAW ********
;   *********************************************


draw_window:

    mov  eax,12                    ; function 12:tell os about windowdraw
    mov  ebx,1                     ; 1, start of draw
    mcall

                                   ; DRAW WINDOW
    mov  eax,0                     ; function 0 : define and draw window
    mov  ebx,100*65536+300         ; [x start] *65536 + [x size]
    mov  ecx,100*65536+330         ; [y start] *65536 + [y size]
    mov  edx,0x14ffffff            ; color of work area RRGGBB
    mov  edi,title                 ; WINDOW LABEL
    mcall


    ; Re-draw the screen text
    cld
    mov  eax,4
    mov  ebx,10*65536+30           ; draw info text with function 4
    mov  ecx,0x000000
    mov  edx,text
    mov  esi,40
  newline:
    mcall
    add  ebx,16
    add  edx,40
    cmp  [edx],byte 'x'
    jnz  newline


    mov  eax,12                    ; function 12:tell os about windowdraw
    mov  ebx,2                     ; 2, end of draw
    mcall

    ret


; DATA AREA

if lang eq ru
text:
    db '����� ����        : 192.168.0.2       '
    db '����訢���� ���� : 0x5000            '
    db '��᫠��� ᮮ�饭��:                   '
    db 'x' ; <- END MARKER, DONT DELETE
else
text:
    db 'This address     : 192.168.0.2          '
    db 'Used port        : 0x5000               '
    db 'Received messages:                      '
    db 'x' ; <- END MARKER, DONT DELETE
end if

title  db  'NetSend(Server)',0

socketNum   dd  0x0


I_END:
