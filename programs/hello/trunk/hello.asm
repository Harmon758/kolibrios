; Hello, World! - Programm example for CMD shell
; Compile with FASM for Menuet
;
; You must run this program from CMD shell only
;

use32

   org 0x0

   db 'MENUET01'
   dd 0x01
   dd START
   dd I_END
   dd 0x100000
   dd 0x7fff0
   dd 0x0
   dd 0x0

include "lang.inc"
include "cmdipc.inc"       ; ��������� 䠩� CMDIPC.INC

START:
 call initipc              ; ���樠����஢��� ��� ࠡ��� � CMD

 mov eax,26                ; ����� ��ப�
 mov ebx,hello_text        ; �����⥫� �� ��ப�
 call print                ; �뢥�� ��ப�

again:
 call getkey               ; ������� ������ ������ � ����� �� � key

 cmp byte [key],'Y'        ; ��� �ணࠬ�� �஢����, ����� �� �㦭. ������
 je goodday
 cmp byte [key],'y'
 je goodday
 cmp byte [key],'N'
 je goodmorning
 cmp byte [key],'n'
 je goodmorning

 jmp again                 ; �᫨ �㦭. ������ �� �����, � ����� �������
                           ; ������
goodday:                   ; �뢥�� �ਢ���⢨� Good Day, World!
 call eol                  ; �ய����� ��ப�

 mov eax,16
 mov ebx,text4
 call print

 call eol                  ; �ய����� ��ப�

 jmp endipc                ; �������� �ணࠬ��

goodmorning:               ; �뢥�� �ਢ���⢨� Good Morning, World!
 call eol

 mov eax,20
 mov ebx,text5
 call print

 call eol

 jmp endipc                ; �������� �ணࠬ��

hello_text db 'Is it after 12 noon? [Y\N]?'

text4 db 'Good day, World!'
text5 db 'Good morning, World!'

I_END:
