; ���⮩ �ਬ�� �ணࠬ�� ��� KolibriOS
; ����稢��� ��� ����⮩ ������
; - ��।���� ��� �ਬ�� �ᯮ�짮����� tooltip

use32 ; ������� 32-���� ०�� ��ᥬ����
org 0x0 ; ������ � ���

db 'MENUET01' ; 8-����� �����䨪��� MenuetOS
dd 0x01 ; ����� ��������� (�ᥣ�� 1)
dd START ; ���� ��ࢮ� �������
dd CODE_END ; ࠧ��� �ணࠬ��
dd DATA_END ; ������⢮ �����
dd STACK_END ; ���� ���設� ���
dd 0x0 ; ���� ���� ��� ��ࠬ��஢
dd cur_dir_path      ; 㪠��⥫� �� ����, �㤠 ����頥��� ��ப�, ᮤ�ঠ�� ���� �� �ணࠬ�� � ������ ����᪠.

include '../../../../../proc32.inc'
include '../../../../../macros.inc'
include '../../../../../KOSfuncs.inc'
include '../../../../../Dll.inc'	; malloc fn
include '../../trunk/box_lib.mac'
include '../../load_lib.mac'
;include 'proc32.inc'
;include 'macros.inc'
;include 'KOSfuncs.inc'

;---------------------------------------------------------------------
;--- ������ ��������� ----------------------------------------------
;---------------------------------------------------------------------
; ��� ����� ��易⥫�� ��� ��� �������⮢, �ᯮ������ heap
; �஬� ⮣�, ��易⥫쭮 �����஢��� lib_init - �� ������ ��।�������
; �㭪樨 娯� ��� ������⥪�
@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load

START:
;---------------------------------------------------------------------
;--- ������������� ----------------------------------------
;---------------------------------------------------------------------
;mov eax, mem_alloc
;mov [mem.alloc], eax
;mov eax, mem_realloc
;mov [mem.realloc], eax
;mov eax, mem_free
;mov [mem.free], eax
mcall	68, 11

mcall 40, $C0000027 ; ��᪠ ᮡ�⨩ - ���� ⮫쪮 � ��⨢��� ����

sys_load_library  lib_name, cur_dir_path, lib_path, sys_path, \
	e_notfound_lib, head_f_l, myimport, e_import, head_f_i
test eax,eax
jz	@f
	mcall -1 ; alarm exit
@@:


invoke tooltip_init, redbox_tt 	; only begin of list

red: ; ����ᮢ��� ����

call draw_window ; ��뢠�� ��楤��� ���ᮢ�� ����

;---------------------------------------------------------------------
;--- ���� ��������� ������� ----------------------------------------
;---------------------------------------------------------------------

still:
mcall 23, 5 ; �㭪�� 23 - ����� ᮡ��� �� ����� 祬 0.05�
test eax, eax ; ��� ᮡ�⨩ - �஢���� �ᮢ���� ��⨯�� �� ⠩����
je yield
cmp eax,1 ; ����ᮢ��� ���� ?
je red ; �᫨ �� - �� ���� red
cmp eax,2 ; ����� ������ ?
je key ; �᫨ �� - �� key
cmp eax,3 ; ����� ������ ?
je button ; �᫨ �� - �� button
cmp eax,6 ; ᮡ�⨥ ���
je mouse ; �᫨ �� - �� mouse

jmp still ; �᫨ ��㣮� ᮡ�⨥ - � ��砫� 横��


;---------------------------------------------------------------------
yield:
invoke tooltip_test_show, redbox_tt
jmp still ; �������� � ��砫� 横��

mouse:
invoke tooltip_mouse, redbox_tt
jmp still ; �������� � ��砫� 横��

key: ; ����� ������ �� ���������
mcall 2 ; �㭪�� 2 - ����� ��� ᨬ���� (� ah)

jmp still ; �������� � ��砫� 横��

;---------------------------------------------------------------------

button:
mcall 17 ; 17 - ������� �����䨪��� ����⮩ ������

cmp ah, 1 ; �᫨ �� ����� ������ � ����஬ 1,
jne still ; ��������

pexit:
invoke tooltip_delete, redbox_tt	; �᢮������� ������
mcall -1 ; ���� ����� �ணࠬ��


;---------------------------------------------------------------------
;--- ����������� � ��������� ���� ----------------------------------
;---------------------------------------------------------------------

draw_window:

mcall 12, 1 ; �㭪�� 12: ᮮ���� �� � ��砫� ���ᮢ��

mcall 48, 3, sc,sizeof.system_colors

mov edx, [sc.work] ; 梥� 䮭�
or edx, 0x33000000 ; � ⨯ ���� 3
mcall 0, <200,300>, <200,150>, , ,title

; �뢮� �����⨪��
mcall 13, <60,50>, <50,50>, $FF0000
mcall 13, <140,50>, <50,50>, $FF


mcall 12, 2 ; �㭪�� 12.2, �����稫� �ᮢ���

ret ; ��室�� �� ��楤���


CODE_END: ; ��⪠ ���� �ணࠬ��; --------------------------------------------;

; ---------------------------------------------------------------------------- ;
;---------------------------------------------------------------------
;--- ������ ��������� ----------------------------------------------
;---------------------------------------------------------------------

sys_path	db '/sys/lib/'
;sys_path	db '/tmp0/1/'
lib_name    db 'box_lib.obj',0
cur_dir_path    rb 4096
lib_path    rb 4096

e_notfound_lib    db 'Sorry I cannot load library box_lib.obj',0

head_f_i:
head_f_l    db 'System error',0
e_import    db 'Error on load import library box_lib.obj',0

myimport:
				dd sz_lib_init ;�㭪�� ����᪠���� ����ᮬ 1 ࠧ �� ������祭�� 
;������⥪�, ��⮬� � �ணࠬ�� ��⪠ �� ��� �� �㦭�
tooltip_init  	dd sz_tooltip_init
tooltip_delete	dd sz_tooltip_delete
tooltip_test_show	dd sz_tooltip_test_show
tooltip_mouse	dd sz_tooltip_mouse
get_font_size	dd sz_get_font_size
    dd    0
    dd    0

sz_lib_init 			db 'lib_init',0
sz_tooltip_init			db 'tooltip_init', 0
sz_tooltip_delete		db 'tooltip_delete', 0
sz_tooltip_test_show	db 'tooltip_test_show', 0
sz_tooltip_mouse		db 'tooltip_mouse', 0
sz_get_font_size		db 'get_font_size', 0


;tooltip txt, next, zone_x, zone_w, zone_y, zone_h, col_txt, col_bkg, tm_wait
redbox_tt    tooltip redboxtxt, blubox_tt, 60, 50, 50, 50, 0, $FFF473, 100
blubox_tt    tooltip bluboxtxt, 0, 140, 50, 50, 50, $110000FF, $FFF473, 100

redboxtxt	db 'Red Box Tooltip', 13, 'May be multilined', 13, 13, 'Even with empty lines', 0
bluboxtxt	db 'Blue Box Tooltip', 0

sc system_colors

title db 'Toooltip demo',0

; stack----------------------------------------------------------------------- ;
	   rb 4096
STACK_END  dd ?

DATA_END: ; ��⪠ ���� ������ �ணࠬ��; ------------------------------------ ;
