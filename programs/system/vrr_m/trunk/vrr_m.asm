;
;   ���⥬��� �ணࠬ�� ��� ��⠭���� ����襭��� (>60 Hz) ���� ���������� 
;   �࠭� �� ��� ᭨����� ࠧ�襭�� 
;   (��� ���室��:  1024�768*60-->800�600*98
;                     800�600*60-->640�480*94)
;
;   �������஢��� FASM'��
;
;   !!!!!_�।�०�����_!!!!!:
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;! �� ⥪�騩 ������ �ணࠬ�� ���� ����������������� (!) �ࠪ��, ���⮬� �ᥣ��   !
;! ��⠥��� ����⭮��� ���� ����㤮����� (�.�. ������). �ᮡ���� �� ��ᠥ��� ���!
;! ��, 祩 ������ �� ����� ����� �� ��ॣ�㧮� �� ����.                         !
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;     
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

include '..\..\..\macros.inc' ; ������ �������� ����� ��ᥬ����騪��!

;---------------------------------------------------------------------
;---  ������ ���������  ----------------------------------------------
;---------------------------------------------------------------------

START:
;       mcall 5,10
        mov     ecx, 1
        mov     edx, drvinfo
        push    @f
        jmp     call_driver
@@:
;       jmp run_launcher

        mov     ecx, 2
        push    @f
call_driver:
        mcall 21,13
        ret
@@:
;       cmp eax,-1
        inc     eax
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
        mov     eax, 10
        cmp cx,277+3
        je  yes_277
        cmp cx,274+3
        jne yes_280
     yes_274:
        add al,10
     yes_277:
        add al,10
     yes_280:
        mov     edx, [_m1+eax-2]
        lea     dx, [ecx-3]
        push    run_launcher
        mov     ecx, 3
        jmp     call_driver
run_launcher:
       mcall 70,launcher
       mcall -1
launcher:
        dd      7
        dd      0
        dd      0
        dd      0
        dd      0
        db      '/sys/LAUNCHER'
I_END:                             ; ��⪠ ���� �ணࠬ��
        db      ?       ; system loader will zero all memory after program end
                        ; this byte will be terminating zero for launcher string
; \begin{Serge}
                        ; A you really believe it?
                        ; ������, ��� ������, ����� ��� �� �����!
; \end{Serge}
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
