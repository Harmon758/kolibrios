
format MS COFF

public Start
public _PATH
public _PARAM

extrn Memory
extrn hEnd

extrn _kol_main

section ".text" code
	db "MENUET01"
	dd 1, Start, hEnd, Memory, hStack, _PARAM, _PATH

Start:

; ������������� ����
mov	eax, 68
mov	ebx, 11
int	0x40

; ����� ������� ���������
mov	eax, _kol_main
call	eax

; ���������� ������ ���������
mov	eax, -1
int	0x40

section ".bss"

_PARAM:
rb	256

_PATH:
rb	256

rb	8*1024
hStack:
