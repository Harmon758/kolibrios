;
;   RDsave ��� Kolibri (0.6.5.0 � ����)
;   
;   Mario79 2005
;   Heavyiron 12.02.2007
;   <Lrz>     11.05.2009 - ��� ࠡ��� �㦭� ��⥬��� ������⥪� box_lib.obj
;   �������஢��� FASM'��
;
;---------------------------------------------------------------------
include 'lang.inc'
include '..\..\..\macros.inc'

appname equ 'RDsave '
version equ '1.2'
  
  use32 	     ; ������� 32-���� ०�� ��ᥬ����
  org	 0x0	     ; ������ � ���

  db	 'MENUET01'  ; 8-����� �����䨪��� MenuetOS
  dd	 0x01	     ; ����� ��������� (�ᥣ�� 1)
  dd	 START	     ; ���� ��ࢮ� �������
  dd	 I_END	     ; ࠧ��� �ணࠬ��
  dd	 I_END	     ; ������⢮ �����
  dd	 I_END	     ; ���� ���設� ���
  dd	 0x0	     ; ���� ���� ��� ��ࠬ��஢ (�� �ᯮ������)
  dd cur_dir_path

;include '..\..\..\develop\examples\editbox\trunk\editbox.inc'
include '../../../develop/libraries/box_lib/load_lib.mac'
include '../../../develop/libraries/box_lib/trunk/box_lib.mac'
	@use_library

;use_edit_box
;al equ eax      ; \ decrease kpack'ed size
;purge mov       ; /

;---------------------------------------------------------------------
;---  ������ ���������  ----------------------------------------------
;---------------------------------------------------------------------
align 4
START:
sys_load_library  library_name, cur_dir_path, library_path, system_path, \
err_message_found_lib, head_f_l, myimport, err_message_import, head_f_i
	cmp	eax,-1
	jz	close


   mov eax, 40
   mov ebx, 100111b
   mcall
red:			; ����ᮢ��� ����
   mov	eax,48
   mov	ebx,3
   mov	ecx,sc
   mov	edx,sizeof.system_colors
   mcall
	edit_boxes_set_sys_color editbox,editbox_end,sc
    call draw_window	; ��뢠�� ��楤��� ���ᮢ�� ����

;---------------------------------------------------------------------
;---  ���� ��������� �������  ----------------------------------------
;---------------------------------------------------------------------

still:
    mcall 10

    dec  eax		 ; ����ᮢ��� ����?
    jz	 red		 ; �᫨ �� - �� ���� red
    dec  eax 
    jz	 key
    dec  eax
    jz	 button

mouse:
;        mouse_edit_box editbox
	push	dword editbox
	call	[edit_box_mouse]

	jmp	still
    
button:
    mov  al,17		 ; ������� �����䨪��� ����⮩ ������
    mcall

    cmp  ah,1		 ; ������ � id=1("�������")?
    jne  noclose
close:
    or	 eax,-1 	 ; �㭪�� -1: �������� �ணࠬ��
    mcall

noclose:
	push	eax
	call	clear_err
	pop	eax
	push	16
	xor	ebx, ebx
	inc	ebx	; 16.1 = save to /FD/1
	cmp	ah, 2
	je	doit
	inc	ebx	; 16.2 = save to /FD/2
	cmp	ah, 3
	je	doit
	pop	ebx
	push	18
	mov	bl, 6	; 18.6 = save to specified folder
	mov	ecx, path3
	cmp	ah, 4
	je	doit
	mov	ecx, path4
doit:
	pop	eax
	mcall
	call	check_for_error
	jmp	still

key:	     
    mov  al,2
    mcall
;    key_edit_box editbox
	push	dword editbox
	call	[edit_box_key]

    jmp  still


check_for_error:		      ;��ࠡ��稪 �訡��
	mov	ecx, [sc.work_text]
	mov	edx, ok
	test	eax, eax
	jz	print
	mov	ecx, 0xdd2222
	add	edx, error3 - ok
	dec	eax
	dec	eax
	jz	print
	add	edx, error5 - error3
	dec	eax
	dec	eax
	jz	print
	add	edx, error8 - error5
	dec	eax
	dec	eax
	dec	eax
	jz	print
	add	edx, error9 - error8
	dec	eax
	jz	print
	add	edx, error10 - error9
	dec	eax
	jz	print
	add	edx, error11 - error10
	dec	eax
	jz	print
	add	edx, aUnknownError - error11

 print:
    mov eax,4				   ;������
    mov ebx,20 shl 16 + 148
    or	ecx,0x80000000
    mcall
    ret

clear_err:
    mov eax,13
    mov ebx,15 shl 16 + 240
    mov ecx,145 shl 16 +15
    mov edx,[sc.work]
    mcall
    ret

;---------------------------------------------------------------------
;---  ����������� � ��������� ����  ----------------------------------
;---------------------------------------------------------------------

draw_window:

   mov eax,12				 ; �㭪�� 12: ᮮ���� �� �� ���ᮢ�� ����
   mov bl,1				 ; 1 - ��稭��� �ᮢ���
   mcall

					 ; ������� ����
   xor eax,eax				 ; �㭪�� 0 : ��।����� � ���ᮢ��� ����
   mov ebx,200 shl 16 + 270		 ; [x ����] *65536 + [x ࠧ���]
   mov ecx,200 shl 16 + 190		 ; [y ����] *65536 + [y ࠧ���]
   mov edx,[sc.work]			 ; 梥� ࠡ�祩 ������  RRGGBB,8->color gl
   or  edx,0x34000000
   mov edi,title			; ��������� ����
   mcall

	push	dword editbox
	call	[edit_box_draw]

;draw_edit_box editbox                   ;�ᮢ���� edit box

   mov al,13				;���ᮢ�� ⥭�� ������
   mov ebx,194 shl 16 + 60
   mov ecx,34 shl 16 +15
   mov edx,0x444444
   mcall

   add ecx,20 shl 16
   mcall

   add ecx,20 shl 16
   mcall

   add ecx,40 shl 16
   mcall

   mov eax,8				 ;���ᮢ�� ������
   sub ebx,4 shl 16
   sub ecx,4 shl 16
   mov edx,5
   mov esi,[sc.work_button]
   mcall

   sub ecx,40 shl 16
   dec edx
   mcall

   sub ecx,20 shl 16
   dec edx
   mcall

   sub ecx,20 shl 16
   dec edx
   mcall

   mov al,4				 ;������
   mov ebx,45 shl 16 + 12
   mov ecx,[sc.work_text]
   or  ecx,0x80000000
   mov edx,label1
   mcall

   mov ebx,150 shl 16 + 35
   mov edx,path1
   mcall

   add ebx,20
   mov edx,path2
   mcall

   mov ebx,75 shl 16 + 75
   mov edx,path3
   mcall

   mov ebx,30 shl 16 + 97
   mov edx,label2
   mcall

   mov ebx,40 shl 16 + 135
   mov edx,label3
   mcall

   mov ecx,[sc.work_button_text]
   or  ecx,0x80000000
   mov ebx,195 shl 16 + 35
   mov edx,save
   mcall

   add ebx,20
   mcall

   add ebx,20
   mcall

   add ebx,40
   mcall

   mov al,12				; �㭪�� 12: ᮮ���� �� �� ���ᮢ�� ����
   mov ebx,2				; 2, �����稫� �ᮢ���
   mcall

   ret					; ��室�� �� ��楤���


;---------------------------------------------------------------------
;---  ������ ���������  ----------------------------------------------
;---------------------------------------------------------------------

title db appname,version,0

editbox:
edit1 edit_box 170,10,113,0xffffff,0xaabbcc,0,0,0,512,path4,mouse_dd,ed_focus,26,26
editbox_end:

if lang eq ru
save	db '���࠭���',0
label1	db '�롥�� ���� �� ��ਠ�⮢:',0
label2	db '��� ������ ����� ���� � 䠩��:',0
label3	db '�� ����� ������ ����⢮����',0
ok	db 'RAM-��� ��࠭�� �ᯥ譮',0
error3	db '�������⭠� 䠩����� ��⥬�',0
error5	db '���������騩 ����',0
error8	db '��� ���� �� ��᪥',0
error9	db '������ FAT ࠧ��襭�',0
error10 db '����� ����饭',0
error11 db '�訡�� ���ன�⢠',0
aUnknownError db '�������⭠� �訡��',0
else if lang eq et
save	db 'Salvesta',0
label1	db 'Vali �ks variantidest:',0
label2	db 'V�i sisesta teekond failinimeni:',0
label3	db 'K�ik kataloogid peavad eksisteerima',0
ok	db 'RAM-ketas salvestatud edukalt',0
error3	db 'Tundmatu failis�steem',0
error5	db 'Vigane teekond',0
error8	db 'Ketas t�is',0
error9	db 'FAT tabel vigane',0
error10 db 'Juurdep��s keelatud',0
error11 db 'Seadme viga',0
aUnknownError db 'Tundmatu viga',0

else
save	db '  Save',0
label1	db 'Select one of the variants:',0
label2	db '   Or enter full path to file:',0
label3	db '    All folders must exist',0
ok	db 'RAM-drive was saved successfully',0
error3	db 'Unknown file system',0
error5	db 'Incorrect path',0
error8	db 'Disk is full',0
error9	db 'FAT table corrupted',0
error10 db 'Access denied',0
error11 db 'Device error',0
aUnknownError db 'Unknown error',0

end if
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;DATA �����
;�ᥣ�� ᮡ��� ��᫥����⥫쭮��� � �����.
system_path	 db '/sys/lib/'
library_name	 db 'box_lib.obj',0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

err_message_found_lib	db 'Sorry I cannot load library box_lib.obj',0
head_f_i:
head_f_l	db 'System error',0
err_message_import	db 'Error on load import library box_lib.obj',0
align 4
myimport:   

edit_box_draw	dd	aEdit_box_draw
edit_box_key	dd	aEdit_box_key
edit_box_mouse	dd	aEdit_box_mouse
;version_ed      dd      aVersion_ed

		dd	0
		dd	0

aEdit_box_draw	db 'edit_box',0
aEdit_box_key	db 'edit_box_key',0
aEdit_box_mouse db 'edit_box_mouse',0
;aVersion_ed     db 'version_ed',0


;mouse_flag: dd 0x0

path1	db '/fd/1/',0
path2	db '/fd/2/',0
path3	db '/hd0/1/kolibri.img',0
path4	db '/hd0/1/kolibri/kolibri.img',0  ;��� १�ࢭ��� ��࠭����

;---------------------------------------------------------------------

rb 514

sc     system_colors
mouse_dd	rd 1
cur_dir_path	rb 1096
library_path	rb 1096
align 4
rb 0x100	; for stack
I_END:				   ; ��⪠ ���� �ணࠬ��
