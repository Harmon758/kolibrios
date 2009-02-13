;Libray from Editbox
; SEE YOU File FAQ.txt and HISTORY. Good Like!
;Last Change 13/02/2009
;;;;;;;;;;;;;;;;;;

format MS COFF

public EXPORTS

section '.flat' code readable align 16
include 'macros.inc'
include 'editbox.mac'   ;����� ����� ������ �������� ����� :) ᯥ樠�쭮 ��� editbox
include 'checkbox.mac'	;����� ᮤ�ন� ॠ������ checkbox
include 'optionbox.mac' ;����� ᮤ�ন� ॠ������ optionbox
;----------------------------------------------------
;EditBox
;----------------------------------------------------

use_editbox_draw	;����� ����뢠�� �㭪�� �⮡ࠦ���� ����.
use_editbox_key  	;����� ����뢠�� �㭪�� ��ࠡ�⪨ �������.
use_editbox_mouse	;����� ����뢠�� �㭪�� ��ࠡ�⪨ ���.

;----------------------------------------------------
;CheckBox
;----------------------------------------------------
use_checkbox_draw	;����� ����뢠�� �㭪�� �⮡ࠦ���� ����.
use_checkbox_mouse	;����� ����뢠�� �㭪�� ��ࠡ�⪨ ���.


;--------------------------------------------------
;radiobutton Group
;--------------------------------------------------
use_optionbox_driver	;������ ����� �ࠢ���� ࠡ�⮩ ���� )
use_optionbox_draw	;����� ����뢠�� �㭪�� �⮡ࠦ���� ����.
use_optionbox_mouse	;����� ����뢠�� �㭪�� ��ࠡ�⪨ ���.

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
