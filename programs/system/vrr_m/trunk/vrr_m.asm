;
;   �ਬ�� �ணࠬ�� ��� MenuetOS
;   ����稢��� ��� ����⮩ ������ ;)
;
;   �������஢��� FASM'��
;
;   ��. ⠪��:
;     template.asm  -  �ਬ�� ���⥩襩 �ணࠬ�� (����!)
;     rb.asm        -  ���⥪�⭮� ���� ࠡ�祣� �⮫�
;     example2.asm  -  �ਬ�� ���� � �������⥫��� ����
;     example3.asm  -  �ਬ�� ����, ॠ����������� ��-��㣮��
;---------------------------------------------------------------------

  use32              ; ������� 32-���� ०�� ��ᥬ����
  org    0x0         ; ������ � ���

  db     'MENUET01'  ; 8-����� �����䨪��� MenuetOS
  dd     0x01        ; ����� ��������� (�ᥣ�� 1)
  dd     START       ; ���� ��ࢮ� �������
  dd     I_END       ; ࠧ��� �ணࠬ��
  dd     0x1000      ; ������⢮ �����
  dd     0x1000      ; ���� ���設� ���
  dd     0x0         ; ���� ���� ��� ��ࠬ��஢ (�� �ᯮ������)
  dd     0x0         ; ��१�ࢨ஢���

include 'lang.inc'
include 'macros.inc' ; ������ �������� ����� ��ᥬ����騪��!

;---------------------------------------------------------------------
;---  ������ ���������  ----------------------------------------------
;---------------------------------------------------------------------

START:
;       mcall 5,10
       mcall 21,13,1,drvinfo
;       jmp run_launcher

       mcall 21,13,2
       cmp eax,-1
       je   run_launcher
;       cmp  ecx,280
;       je  change_vrr
;       cmp  ecx,277
;       je  change_vrr
;       cmp  ecx,6
;       je  change_vrr
;       cmp  ecx,7
;       je  change_vrr
;       jmp  run_launcher
change_vrr:
;       mov ax,cx
;       dec cx
;       shl cx,1
;       xor edx,edx
;       mov dx,[vidmode+ecx]
;       mov ebx,ecx
;       shl ebx,2
;       add ebx,ecx   ; ebx=ebx*5
;       shr ax,8
;       dec ax
;       shl ax,1
;       add ebx,eax
;       ror edx,16
;       mov dx,[_m1+ebx]
;       rol edx,16
        ;mov eax,ecx
        xor eax,eax
        sub ecx,3
        mov dx,cx
        cmp cx,274
        je yes_274
        cmp cx,277
        je yes_277
        jmp yes_280
     yes_274:
        add al,10
     yes_277:
        add al,10
     yes_280:
        add al,10
        ror edx,16
        mov dx,[_m1+eax]
        rol edx,16
;       mov dx,bx
;       shl edx,16
;       mov  dx,cx
       mcall 21,13,3
;       mcall 5,300
run_launcher:
       mcall 19,launcher,0
;       mcall 33,text,drvinfo,512,0
       mcall -1
launcher db  'LAUNCHER   '
;text      db  'TEXT       '
drvinfo:   ; 512 bytes driver info area
; +0   - Full driver name
; +32  - Driver version
; +64  - Word List of support video modes (max 32 positions)
; +128 - 5 words list of support vertical rate to each present mode
      org $+32
drvver:
      org $+32
vidmode:
      org $+64
_m1:
      org drvinfo+200h

I_END:                             ; ��⪠ ���� �ணࠬ��
