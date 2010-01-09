; Copyright (c) 2009, <Lrz>
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;       * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;       * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;       * Neither the name of the <organization> nor the
;       names of its contributors may be used to endorse or promote products
;       derived from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY Alexey Teplov nickname <Lrz> ''AS IS'' AND ANY
; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;*****************************************************************************

;��������� �ਫ������
use32		     ; �࠭����, �ᯮ����騩 32 ࠧ�來�� �������
    org 0x0		   ; ������ ���� ����, �ᥣ�� 0x0
    db 'MENUET01'	 ; �����䨪��� �ᯮ��塞��� 䠩�� (8 ����)
    dd 0x1		  ; ����� �ଠ� ��������� �ᯮ��塞��� 䠩��
    dd start		    ; ����, �� ����� ��⥬� ��।��� �ࠢ�����
			; ��᫥ ����㧪� �ਫ������ � ������
    dd mem		  ; ࠧ��� �ਫ������
    dd mem		    ; ��ꥬ �ᯮ��㥬�� �����, ��� �⥪� �⢥��� 0�100 ���� � ��஢��� �� �୨�� 4 ����
    dd mem		    ; �ᯮ����� ������ �⥪� � ������ �����, �ࠧ� �� ⥫�� �ணࠬ��. ���設� �⥪� � ��������� �����, 㪠������ ���
    dd 0x0		; 㪠��⥫� �� ��ப� � ��ࠬ��ࠬ�.
    dd way_of_ini
include '../../../../macros.inc'
include '../../box_lib/trunk/box_lib.mac'
include '../../box_lib/load_lib.mac'
	@use_library	;use load lib macros
start:
;universal load library/librarys
sys_load_libraries l_libs_start,end_l_libs
;if return code =-1 then exit, else nornary work
	cmp	eax,-1
	jz	exit
	mcall	40,0x27 	;��⠭����� ���� ��� ��������� ᮡ�⨩

	mov  eax,48
	mov  ebx,3
	mov  ecx,sc
	mov  edx,sizeof.system_colors
	mcall
	mov  eax,dword [sc.work]
	mov  dword [con_colors+4],eax

;       mcall   66,1,0
       call [initialization_font]	; ���樠������ ᯨ᪠ ���⮢ 
       push dword (8 shl 16 +16)	; ���� �㦭��� ���� � ����� ���⮢ (���� ����㯥� ⮫쪮 8�16)
       call [get_font]
	test	eax,eax 		;��諨 ? 
	jnz	exit
;;;;;;;;;;;;;;;;;;;;
	mcall	40,0x27 	;��⠭����� ���� ��� ��������� ᮡ�⨩
red_win:
    call draw_window		;��ࢮ��砫쭮 ����室��� ���ᮢ��� ����
align 4
still:				;�᭮���� ��ࠡ��稪
	mcall	10		;������� ᮡ���
	dec  eax
	jz   red_win
	dec  eax
	jz   key
	dec  eax
	jz   button

	push	dword edit1
	call	[edit_box_mouse]

	jmp still    ;�᫨ ��祣� �� ����᫥����� � ᭮�� � 横�
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
button:
	mcall	17	;������� �����䨪��� ����⮩ ������
	test ah,ah	;�᫨ � ah 0, � ��३� �� ��ࠡ��稪 ᮡ�⨩ still
	jz  still
exit:	
	call	[free_fulder_info]
	call	[free_font]
	mcall	-1
key:
	mcall	2	;����㧨� ���祭�� 2 � ॣ���� eax � ����稬 ��� ����⮩ ������

	push	dword edit1
	call	[edit_box_key]
 

	mcall	13,<20,650>,<40,16>, dword[con_colors+4]


	push	dword 20 shl 16 + 40	; esp+12= dd x shl 16 + y x- ���न��� �� �, y - ���न��� �� Y
	push	dword con_colors	; esp+8 = dd point to color of background and font
	push	dword text		; esp+4 = dd point to ASCIIZ
; esp+0 = dd back
	call	[font_draw_on_string]	; �뢥�� �� ���䠬 �����


	jmp still

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
align 4
draw_window:		;�ᮢ���� ���� �ਫ������
	mcall	12,1
	xor  eax,eax		 ;���㫨�� eax
	mov  ebx,50*65536+680	 ;[���न��� �� �� x]*65536 + [ࠧ��� �� �� x]
	mov  ecx,30*65536+200	 ;[���न��� �� �� y]*65536 + [ࠧ��� �� �� y]
	mov  edx,[sc.work]	 ; color of work area RRGGBB,8->color gl
	or   edx,0x34000000
	mov  edi,hed
	mcall			 ;���ᮢ��� ���� �ਫ������
	
	push	dword edit1
	call	[edit_box_draw]
;
	push	dword 20 shl 16 + 40	; esp+12= dd x shl 16 + y x- ���न��� �� �, y - ���न��� �� Y
	push	dword con_colors	; esp+8 = dd point to color of background and font
	push	dword text; esp+4 = dd point to ASCIIZ
; esp+0 = dd back
	call	[font_draw_on_string]	; �뢥�� �� ���䠬 �����
	mov eax,12		 ;�㭪�� 12 - �����/�������� ����ᮢ�� ����.
	mov ebx,2		 ;����㭪�� 2 - �������� ����ᮢ�� ����.
	mcall
	ret
;;;;;;;;;;;;
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA �����
l_libs_start:
library01 l_libs library_name, way_of_ini, library_path, system_path, err_message_found_lib, head_f_l, font_import, err_message_import, head_f_i

library02 l_libs library_name1, way_of_ini, library_path, system_path1, err_message_found_lib1, head_f_l, import_box_lib, err_message_import1, head_f_i

end_l_libs:

;
system_path	 db '/sys/lib/'
library_name	 db 'fonts_lib.obj',0

system_path1	  db '/sys/lib/'
library_name1	  db 'box_lib.obj',0

err_message_found_lib	db 'Sorry I cannot load library fonts_lib.obj',0
err_message_found_lib1	db 'Sorry I cannot load library box_lib.obj',0

head_f_i:
head_f_l	db 'System error',0
err_message_import	db 'Error on load import library fonts_lib.obj',0
err_message_import1	db 'Error on load import library box_lib.obj',0

align 4
import_box_lib:   

edit_box_draw	dd	aEdit_box_draw
edit_box_key	dd	aEdit_box_key
edit_box_mouse	dd	aEdit_box_mouse
version_ed	dd	aVersion_ed

		dd	0
		dd	0

aEdit_box_draw	db 'edit_box',0
aEdit_box_key	db 'edit_box_key',0
aEdit_box_mouse db 'edit_box_mouse',0
aVersion_ed	db 'version_ed',0

font_import:
initialization_font	dd	a_initialization_font
get_font		dd	a_get_font
free_fulder_info	dd	a_free_fulder_info
free_font		dd	a_free_font
font_draw_on_string	dd	a_font_draw_on_string
show_all_glif		dd	a_show_all_glif
Version_fn	      dd      a_Version_fn
		      dd      0,0

a_initialization_font	db 'initialization_font',0
a_get_font		db 'get_font',0
a_free_fulder_info	db 'free_fulder_info',0
a_free_font		db 'free_font',0
a_font_draw_on_string	db 'font_draw_on_string',0
a_show_all_glif 	db 'show_all_glif',0
a_Version_fn	      db 'version_fn',0



edit1 edit_box 350,175,5,0xffffff,0x6f9480,0,0xAABBCC,0,test_leght,text,ed_focus,text_end-text-1,text_end-text-1

text db   '�ਬ�� �ᯮ�짮����� ������⥪� ���⮢ fonts_lib.obj',0
text_end:
rb  256
test_leght = ($-text)-1

hed db	 "Font's demo <Lrz>",0
align 4
con_colors	dd	0x1E1EFF, 0x96FFCF

align 4
sc     system_colors
way_of_ini	rb 4096
library_path	rb 4096

align 4
i_end:
rb 1024
mem:
		;����� ����