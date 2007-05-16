;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;   Periodic Table for KolibriOS
;;
;;   ��ਮ���᪠� ��⥬� 娬��᪨� ����⮢
;;         ������ �������� ����������
;;     �����:
;;       navanax [violetlab+rambler.ru]
;;      Johnny_B [john+kolibrios.org]
;;
;; ����� ��� ᮧ����� ���ᨢ� 㪠��⥫�� �� ASCIIZ-��ப�

include '..\..\..\macros.inc'
macro strtbl name,[string]
{
 common
  label name dword
 forward
  local lbl
  dd lbl
 forward
  lbl db string,0
}

use32

 org	0x0
 db	'MENUET01'    ; �⠭����� 8-���⮢� ��������� ��� �ᯮ��塞�� 䠩��� MeOS
 dd	0x01	      ; ����� ���������
 dd	START	      ; 㪠��⥫� �� ��砫� �ᯮ��塞��� ����
 dd	I_END	      ; ࠧ��� ��ࠧ�
 dd	I_END+0x1000   ; �ॡ㥬�� ������ ��� �ਫ������
 dd	I_END+0x1000   ; 㪠��⥫� �� ��砫� �⥪�
 dd	0x0 , 0x0     ; I_Param , I_Icon

;���������� ���������� � ���������
re_ID  dd  0x00000001	;��� �࠭���� ����� ⥪�饣� �뢮������ �����

START:

      mov  eax,48
      mov  ebx,3
      mov  ecx,sc
      mov  edx,sizeof.system_colors
      mcall

red:
      call draw_window

still:		      ;�᭮���� 横� �ணࠬ��
      push 10   ;�������� ᮡ���
      pop  eax
      mcall

      dec  eax
      je   red
      dec  eax
      jne  button

key:
	mov  al,2
	mcall
	jmp  still

button:
	mov  al,17   ;����砥� ID ����⮩ ������
	mcall
	shr  eax,8    ; eax=24-���� ButtonID

	cmp  eax,1    ;����� �� "�������" [x]
	jne  noclose
	or   eax,-1
	mcall

noclose:
      cmp eax,200
      jne no_H
      mov eax,1
no_H: 
	mov [re_ID],eax
;; <mistifi(ator>
	call draw_text
;; </mistifi(ator>
	jmp still

draw_window:
	
        mov  eax,12   ;begin drawing
	xor  ebx,ebx
	inc  ebx
	mcall
				       ; DRAW WINDOW
	xor  eax,eax
	mov  ebx,50*65536+555  ;x
	mov  ecx,30*65536+455  ;y
      mov  edx,[sc.work]
	or   edx,0x33000000    ;style
	mov  edi,caption
	mcall

text_in_window:
	mov  eax,4	     ;�뢮� ⥪��
	mov  ecx,0x80000000  ;asciiz, ���� !!!

	mov  ebx,40*65536+200	      ;���⠭����
	mov  edx,Lantanoid_text
	mcall

	add  ebx,20		       ;��⨭����
	mov  edx,Actinoid_text
	mcall

	mov  ebx,150*65536+255
	mov  edx,number
	mcall

	add  ebx,15
	mov  edx,name
	mcall

	add ebx,15
	mov edx,eng_name
	mcall

	add ebx,15
	mov edx,Atomic_massa
	mcall

	add ebx,15
	mov edx,density
	mcall

	add ebx,15
	mov edx,Step_okisl
	mcall

	add ebx,15
	mov edx,Electrootr
	mcall

	add ebx,15
	mov edx,T_pl
	mcall

	add ebx,15
	mov edx,T_kip
	mcall

	add ebx,15
	mov edx,Raspr
	mcall

	add ebx,15
	mov edx,Otkrytie
	mcall

	; ����� ������� ��������
	mov ebx,30*65536+43
	mov edx,Period_text  ;1
	mcall

	call @f 	     ;2
	call @f 	     ;3
	call @f 	     ;4
	call @f 	     ;5
	call @f 	     ;6
	call @f 	     ;7
	jmp  Table

      @@:
	add ebx,20
	add edx,4   ;�.�. ��ப� = 4 ���⠬(� ���)
	mcall
       ret

Table:		       ;���ᮢ�� ⠡���� � ���� ������祪
	table_row_1:
			shl eax,1
			mov  ebx,50*65536+25
			mov  ecx,35*65536+20
			mov  edx,200
			mov  esi,0xE19123
			mcall
		He_:
			mov  ebx,491*65536+25
			mov  esi,0x0f0c314
			mov  edx,2
			mcall
	table_row_2:
		Li:
		      mov  ebx,50*65536+25
			mov  ecx,56*65536+20
			mov  esi,0xE19123
			inc edx
			mcall
		Be:
			mov  ebx,76*65536+25
			inc edx
			mcall
		B_:
			mov  ebx,361*65536+25
			inc edx
			mov esi,0x0f0c314
			mcall
			mov edi,4
		table_row_2_loop:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns table_row_2_loop
	table_row_3:
		Na:
		       mov  ebx,50*65536+25
			mov  ecx,77*65536+20
			mov  esi,0xE19123
			inc edx
			mcall

			mov  ebx,76*65536+25
			inc edx
			mcall

			mov  ebx,361*65536+25
			inc edx
			mov esi,0x0f0c314
			mcall
			mov edi,4
		table_row_3_loop:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns table_row_3_loop

	Table_row_4:
		K_:
		       mov  ebx,50*65536+25
			mov  ecx,98*65536+20
			mov  esi,0xE19123
			inc edx
			mcall
		Ca:
		       mov  ebx,76*65536+25
			inc edx
			mcall
		Sc:
			mov  ebx,102*65536+25
			inc edx
			mov  esi,0x559beb
			mcall
			mov edi,8
		Table_row_4_loop1:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns Table_row_4_loop1

			mov  ebx,361*65536+25
			inc edx
			mov esi,0x0f0c314
			mcall
			mov edi,4
		table_row_4_loop2:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns table_row_4_loop2

	 Table_row_5:
		Rb_:
		       mov  ebx,50*65536+25
			mov  ecx,119*65536+20
			mov  esi,0xE19123
			inc edx
			mcall
		Sr_:
		       mov  ebx,76*65536+25
			inc edx
			mcall
		Y_:
			mov  ebx,102*65536+25
			inc edx
			mov  esi,0x559beb
			mcall
			mov edi,8
		Table_row_5_loop1:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns Table_row_5_loop1

			mov  ebx,361*65536+25
			inc edx
			mov esi,0x0f0c314
			mcall
			mov edi,4
		table_row_5_loop2:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns table_row_5_loop2

	 Table_row_6:
		Cs_:
		       mov  ebx,50*65536+25
			mov  ecx,140*65536+20
			mov  esi,0xE19123
			inc edx
			mcall
		Ba_:
		       mov  ebx,76*65536+25
			inc edx
			mcall
		La_:
			mov  ebx,102*65536+25
			inc edx
			mov  esi,0x559beb
			mcall
			mov edx,71
			mov edi,8
		Table_row_6_loop1:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns Table_row_6_loop1

			mov  ebx,361*65536+25
			inc edx
			mov esi,0x0f0c314
			mcall
			mov edi,4
		table_row_6_loop2:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns table_row_6_loop2

	 Table_row_7:
		Fr_:
		       mov  ebx,50*65536+25
			mov  ecx,161*65536+20
			mov  esi,0xE19123
			inc edx
			mcall
		Ra_:
		       mov  ebx,76*65536+25
			inc edx
			mcall
		Ac_:
			mov  ebx,102*65536+25
			inc edx
			mov  esi,0x559beb
			mcall
			mov edx,103
			mov edi,8
		Table_row_7_loop1:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns Table_row_7_loop1

			mov  ebx,361*65536+25
			inc edx
			mov esi,0x0f0c314
			mcall
			mov edi,2
		table_row_7_loop2:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns table_row_7_loop2

	row_1:
		Cerium_:
			mov  ebx,102*65536+25
			mov  ecx,190*65536+20
			mov  edx,58
			mov  esi,0x73beeb
			mcall
			mov edi,12

		row_1_loop:
			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns row_1_loop
	row_2:
		dgfsfHe_:
			mov  ebx,102*65536+25
			mov  ecx,211*65536+20
			mov  edx,90
			mcall
			mov edi,12
		row_2_loop:

			add ebx,26*65536
			inc edx
			mcall
			dec edi
			jns row_2_loop

text_on_buttons:
	text_row1:
		;H
			shr eax,1
			mov ebx,62*65536+43
			mov ecx,0x00000000
			mov edx,Symbols
			mov esi,3
			mcall
		;Ne
			mov ebx,500*65536+43
			add edx,3
			mcall
	text_row2_1:
			mov ebx,60*65536+63
			add edx,3
			mcall

			mov ebx,85*65536+63
			add edx,3
			mcall
	text_row2_2:
			mov ebx,374*65536+63
			add edx,3
			mcall
			mov edi,4

	text_row2_2_loop:
			add ebx,25*65536
			add edx,3
			mcall

			dec edi
			jns text_row2_2_loop
	text_row3_1:
			mov ebx,60*65536+83
			add edx,3
			mcall

			mov ebx,85*65536+83
			add edx,3
			mcall
	text_row3_2:
			mov ebx,374*65536+83
			add edx,3
			mcall
			mov edi,4
	text_row3_2_loop:
			add ebx,25*65536
			add edx,3
			mcall

			dec edi
			jns text_row3_2_loop
	text_row4:
			mov ebx,60*65536+104
			add edx,3
			mcall
			mov edi,16
	text_row4_loop:
			add ebx,26*65536
			add edx,3
			mcall

			dec edi
			jns text_row4_loop
	text_row5:
			mov ebx,60*65536+125
			add edx,3
			mcall
			mov edi,16
	text_row5_loop:
			add ebx,26*65536
			add edx,3
			mcall

			dec edi
			jns text_row5_loop
	text_row6:
			mov ebx,60*65536+146
			add edx,3
			mcall
			mov edi,16
	text_row6_loop:
			add ebx,26*65536
			add edx,3
			mcall
			dec edi
			jns text_row6_loop
	text_row7:
			mov ebx,60*65536+167
			add edx,3
			mcall
			mov edi,9
	text_row7_loop:
			add ebx,26*65536
			add edx,3
			mcall

			dec edi
			jns text_row7_loop
			mov edi,4
	text_row7_loop2:
			add ebx,24*65536
			add edx,3
			mcall
			dec edi
			jns text_row7_loop2

	text_row8:
			mov ebx,111*65536+197
			add edx,3
			mcall
			mov edi,12

	text_row8_loop:
			add ebx,26*65536
			add edx,3
			mcall

			dec edi
			jns text_row8_loop

	text_row9:
			mov ebx,111*65536+217
			add edx,3
			mcall
			mov edi,12

	text_row9_loop:
			add ebx,26*65536
			add edx,3
			mcall

			dec edi
			jns text_row9_loop

;; <mistifi(ator> - ���� �뭮ᨬ � �⤥���� ��楤���
	call draw_text
;; </mistifi(ator>

  finish:
	mov eax,12  ;end drawing
	mov ebx,2
	mcall
ret

draw_text:

   ; 1 �뢮� ���浪����� ����� [optmzd-mem]
   ; 2 �뢮� ���᪮�� �������� [optmzd-mem]
   ; 3 �뢮� ������᪮�� �������� [optmzd-spd]
   ; 4 �뢮� �⮬��� ����� [optmzd-spd]
   ; 5 �뢮� ���⭮��� [optmzd-mem]
   ; 6 �뢮� �⥯��� ���᫥��� [optmzd-mem]
   ; 7 �뢮� ���ம���⥫쭮�� [optmzd-spd]
   ; 8 �뢮� ⥬������ ��������� [optmzd-mem]
   ; 9 �뢮� ⥬������ ������� [optmzd-mem]
   ;10 �뢮� ��।������ � ������ ��� [optmzd-mem]
   ;11 �뢮� ��� ��ࢮ���뢠⥫�� [optmzd-mem]

;; <mistifi(ator> - ����᪠, ��-�� �� �뫮 ��������� �� ���ᮢ�� ⥪��
	mov eax,13
	mov ebx,320*65536+210
	mov ecx,255*65536+(15*11)
	mov edx,[sc.work]
	mcall
;; </mistifi(ator>
  

  push edi  ; �⮡� ��⨬���஢��� ��������� ebx

	; 1 �뢮� ���浪����� ����� [optmzd-mem]
	mov ebx,0x30000         ;�뢮���� 3 ����
	mov ecx,[re_ID]
	mov edx,320*65536+255
	xor esi,esi
	mov eax,47
	mcall

;��������! ����� ����𒑟 ���� ����� ���������� ������! [ᤥ���� ��� ᮪�饭�� ���� �� 20 ���� :)))]
	mov ecx,[sc.work_text]  ; ����
      or  ecx,0x80000000      ; asciiz
	mov eax,4  ; �� �㭪�� ��뢠���� 10 ࠧ! ⠪ �� ����襬 1 ࠧ

	; 2 �뢮� ������� �������� [optmzd-mem]
	mov edi,Rus_nazv	 ;㪠��⥫� �� ��砫� ���ᨢ� 㪠��⥫�� ;)
	mov ebx,320*65536+255    ;����� �������� ���न����, ����� �㤥� ���� add ebx,15
	call out_asciiz_item

	xor ecx,ecx   ;color
	; 3 �뢮� ������᪮�� �������� [optmzd-spd]
	mov edx,Elements
	mov edi,[re_ID]
	dec edi
	mov esi,12	 ;����� ��ப�
	imul edi,esi	 ;��⠥� 㪠��⥫� �� �㦭�� ��ப�
	add edx,edi
	add ebx,15	 ; 㢥��稢��� y-���न���� ⥪�� �� 15 ���ᥫ��
	mcall

	; 4 �뢮� �⮬��� ����� [optmzd-spd]
	mov edx,MassNo
	mov edi,[re_ID]
	dec edi
	mov esi,7	 ;����� ��ப�
	imul edi,esi	 ;��⠥� 㪠��⥫� �� �㦭�� ��ப�
	add edx,edi
	add ebx,15
	mcall

	mov  ecx,0x80000000  ;asciiz, ���� !!!
	; 5 �뢮� ��������� [optmzd-mem]
	mov edi,Plotnost
	call out_asciiz_item

	; 6 �뢮� ������� ��������� [optmzd-mem]
	mov edi,Step_ok
	call out_asciiz_item

	xor ecx,ecx  ;color
	; 7 �뢮� ���������������������� [optmzd-spd]
	mov edx,Electr
	mov edi,[re_ID]
	dec edi
	mov esi,4	 ;����� ��ப�
	shl edi,2
	add edx,edi
	add ebx,15
	mcall

	mov  ecx,0x80000000  ;asciiz, ���� !!!
	; 8 �뢮� ����������� ��������� [optmzd-mem]
	mov edi,T_plavl
	call out_asciiz_item

	; 9 �뢮� ����������� ������� [optmzd-mem]
	mov edi,T_kipen
	call out_asciiz_item

	;10 �뢮� ������������� � ������ ���� [optmzd-mem]
	mov edi,Rasprostr
	call out_asciiz_item

	;11 �뢮� ��� ��ࢮ���뢠⥫�� [optmzd-mem]
	mov edi,First_Explore
	call out_asciiz_item

  pop edi   ; �� �� ��࠭� � ��砫� :)
  ret

;�㭪�� �믮���� ����� ����⢨� ��� �뢮�� ���ଠ樨
out_asciiz_item:
	mov edx,[re_ID]
	dec edx
	shl edx,2		 ;㬭����� �� 4 (ࠧ��� 㪠��⥫�)
	mov edx,[edx+edi]	 ;㪠��⥫� �� ��ப� � edx
	add ebx,15
	mcall
ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                      ;;
;;          ������� ������              ;;
;;                                      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;---- begin ---- ����� ��� �������� ���� --------------------------
caption 	  db  '��ਮ���᪠� ��⥬� �.�.���������� by [navanax] & [Johnny_B]',0
density           db  '���⭮���',0
Atomic_massa	  db  '�⮬��� ����',0
name    	  db  '��������',0
eng_name	  db  '����. ��������',0
T_pl		  db  '��������� ��.',0
T_kip		  db  '��������� ���.',0
Electrootr	  db  '�����ம���⥫쭮���',0
Raspr		  db  '���।������ � ����. ���',0
Step_okisl	  db  '�⥯��� ���᫥���',0
Otkrytie	  db  '����⨥',0
number  	  db  '���浪��� �����',0
Period_text:
 db '  I',0
 db ' II',0
 db 'III',0
 db ' IV',0
 db '  V',0
 db ' VI',0
 db 'VII',0
Lantanoid_text db '���⠭����',0
Actinoid_text  db ' ��⨭����',0
;---- end ------ ����� ��� �������� ���� --------------------------


;*********************************************
;    ���� ������ �� ���������� ���������
;*********************************************
;��।������, �� ���-�� ����⮢ = 116. �� �����, �� �����!
;����� ���ᨢ ������ ᮤ�ঠ�� 116 ����⮢,
;�⮡� �� �뫮 ��室� �� ��� �।���

;������᪮� ��������
Elements:	     ;116 ����⮢
db 'Hydrogen    '    ; 1
db 'Helium      '
db 'Lithium     '
db 'Beryllium   '
db 'Boron       '
db 'Carbon      '
db 'Nitrogen    '
db 'Oxygen      '
db 'Fluorine    '
db 'Neon        '
db 'Sodium      '
db 'Magnesium   '
db 'Aluminum    '
db 'Silicon     '
db 'Phosphorus  '
db 'Sulfur      '
db 'Chlorine    '
db 'Argon       '
db 'Potassium   '
db 'Calcium     '  ;20
db 'Scandium    '
db 'Titanium    '
db 'Vandium     '
db 'Chromium    '
db 'Manganese   '
db 'Iron        '
db 'Cobalt      '
db 'Nickel      '
db 'Copper      '
db 'Zinc        ' ;30
db 'Gallium     '
db 'Germanium   '
db 'Arsenic     '
db 'Selenium    '
db 'Bromine     '
db 'Krypton     '
db 'Rubidium    '
db 'Strontium   '
db 'Yttrium     '
db 'Zirconium   '
db 'Niobium     '
db 'Molybdenum  '
db 'Technetium  '
db 'Ruthenium   '
db 'Rhodium     '
db 'Palladium   '
db 'Silver      '
db 'Cadmium     '
db 'Indium      '
db 'Tin         '  ;50
db 'Antimony    '
db 'Tellurium   '
db 'Iodine      '
db 'Xenon       '
db 'Cesium      '
db 'Barium      '
db 'Lanthanum   '
db 'Cerium      '
db 'Praseodymium'
db 'Neodymium   '
db 'Promethium  '
db 'Samarium    '
db 'Europium    '
db 'Gadolinium  '
db 'Terbium     '
db 'Dysprosium  '
db 'Homium      '
db 'Erbium      '
db 'Thulium     '
db 'Ytterbium   '  ;70
db 'Lutetium    '
db 'Hafnium     '
db 'Tantalum    '
db 'Tungsten    '
db 'Rhenium     '
db 'Osmium      '
db 'Iridum      '
db 'Platinum    '
db 'Gold        '
db 'Mercury     '  ;80
db 'Thallium    '
db 'Lead        '
db 'Bismuth     '
db 'Polonium    '
db 'Astatine    '
db 'Radon       '
db 'Francium    '
db 'Radium      '
db 'Actinum     '
db 'Thorium     '  ;90
db 'Protactinium'
db 'Uranium     '
db 'Neptunium   '
db 'Plutonium   '
db 'Americium   '
db 'Curium      '
db 'Berkelium   '
db 'Californium '
db 'Einsteinium '
db 'Fermium     '  ;100
db 'Mendelevium '
db 'Nobelium    '
db 'Lawrencium  '
db 'Reserfordium'
db 'Dubnium     '
db 'Siborgovium '
db 'Boronium    '
db 'Hassium     '
db 'Meithnerium '
db 'Darmstadium '  ;110
db 'Roentgenium '
db 'Ununbium    '
db 'Ununtrium   '
db 'Ununquadium '
db 'Ununpentium '
db 'Ununhexium  '  ;116


;---------------------------------

Symbols:   ;116 ����⮢
db 'H  '
db 'He '
db 'Li '
db 'Be '
db 'B  '
db 'C  '
db 'N  '
db 'O  '
db 'F  '
db 'Ne '
db 'Na '
db 'Mg '
db 'Al '
db 'Si '
db 'P  '
db 'S  '
db 'Cl '
db 'Ar '
db 'K  '
db 'Ca '
db 'Sc '
db 'Ti '
db 'V  '
db 'Cr '
db 'Mn '
db 'Fe '
db 'Co '
db 'Ni '
db 'Cu '
db 'Zn '
db 'Ga '
db 'Ge '
db 'As '
db 'Se '
db 'Br '
db 'Kr '
db 'Rb '
db 'Sr '
db 'Y  '
db 'Zr '
db 'Nb '
db 'Mo '
db 'Tc '
db 'Ru '
db 'Rh '
db 'Pd '
db 'Ag '
db 'Cd '
db 'In '
db 'Sn '
db 'Sb '
db 'Te '
db 'I  '
db 'Xe '
db 'Cs '
db 'Ba '
db 'La '
db 'Hf '
db 'Ta '
db 'W  '
db 'Re '
db 'Os '
db 'Ir '
db 'Pt '
db 'Au '
db 'Hg '
db 'Tl '
db 'Pb '
db 'Bi '
db 'Po '
db 'At '
db 'Rn '
db 'Fr '
db 'Ra '
db 'Ac '
db 'Rf '
db 'Db '
db 'Sg '
db 'Bh '
db 'Hs '
db 'Mt '
db 'Ds '
db 'Rg '
db 'Uub'
db 'Uut'
db 'Uuq'
db 'Uup'
db 'Uuh'
db 'Ce '
db 'Pr '
db 'Nd '
db 'Pm '
db 'Sm '
db 'Eu '
db 'Gd '
db 'Tb '
db 'Dy '
db 'Ho '
db 'Er '
db 'Tm '
db 'Yb '
db 'Lu '
db 'Th '
db 'Pa '
db 'U  '
db 'Np '
db 'Pu '
db 'Am '
db 'Cm '
db 'Bk '
db 'Cf '
db 'Es '
db 'Fm '
db 'Md '
db 'No '
db 'Lr '

;������� �����
MassNo: 	 ;116 ����⮢
db '1.00794'	 ; 1
db '4.0026 '
db '6.941  '
db '9.01218'
db '10.811 '
db '12.0107'
db '14.0067'
db '15.9994'
db '18.9984'
db '20.179 '	 ; 10
db '22.9898'
db '24.305 '
db '26.9815'
db '28.0855'
db '30.9738'
db '32.066 '
db '35.453 '
db '39.948 '
db '39.0983'
db '40.078 '	 ; 20
db '44.9559'
db '47.88  '
db '50.9415'
db '51.996 '
db '54.938 '
db '55.847 '
db '58.9332'
db '58.6934'
db '63.546 '
db '65.39  '	 ; 30
db '69.723 '
db '72.61  '
db '74.922 '
db '78.96  '
db '79.904 '
db '83.80  '
db '85.47  '
db '87.62  '
db '88.906 '
db '91.224 '	 ; 40
db '92.906 '
db '95.94  '
db '(98)   '
db '101.07 '
db '102.905'
db '106.42 '
db '107.868'
db '112.41 '
db '114.82 '
db '118.71 '	 ; 50
db '121.75 '
db '127.60 '
db '126.90 '
db '131.29 '
db '132.90 '
db '137.33 '
db '138.91 '
db '140.12 '
db '140.91 '
db '144.24 '	 ; 60
db '(147)  '
db '150.36 '
db '151.96 '
db '157.25 '
db '158.92 '
db '162.50 '
db '164.93 '
db '167.26 '
db '168.93 '
db '173.04 '	 ; 70
db '174.97 '
db '178.49 '
db '180.95 '
db '183.85 '
db '186.21 '
db '190.2  '
db '192.22 '
db '195.08 '
db '196.97 '
db '200.59 '	 ; 80
db '204.38 '
db '207.19 '
db '208.98 '
db '(209)  '
db '(210)  '
db '(222)  '
db '(223)  '
db '(226)  '
db '(227)  '
db '232.04 '	 ; 90
db '(231)  '
db '238.029'
db '(237)  '
db '(244)  '
db '(243)  '
db '(247)  '
db '(247)  '
db '(251)  '
db '(254)  '
db '(257)  '	 ; 100
db '(258)  '
db '(259)  '
db '(262)  ' ;103
db '(261)  '
db '(262)  '
db '(266)  '
db '(264)  '
db '(269)  '
db '(268)  '
db '(271)  '	 ; 110
db '(272)  '
db '(285)  '
db '(284)  '
db '(289)  '
db '(288)  '
db '(292)  '	 ;116


;-----------------------

;����������������������
Electr:        ; 116 ����⮢
db '2,1 '     ; 1
db '5,5 '
db '0,97'
db '1,47'
db '2,01'
db '2,50'
db '3,07'
db '3,50'
db '4,10'
db '4,84'     ; 10
db '0,93'
db '1,23'
db '1,47'
db '2,25'
db '2,32'
db '2,60'
db '2,83'
db '3,20'
db '0,91'
db '1,04'	  ; 20
db '1,20'
db '1,32'
db '1,45'
db '1,56'
db '1,60'
db '1,64'
db '1,70'
db '1,75'
db '1,75'
db '1,66'     ; 30
db '1,82'
db '2,02'
db '2,11'
db '2,48'
db '2,74'
db '2,94'
db '0,89'
db '0,99'
db '1,11'
db '1,22'	;40
db '1,6 '
db '2,16'
db '1,9 '
db '2.2 '
db '2.28'
db '2,2 '
db '1,93'
db '1,69'
db '1,78'
db '1,96'
db '2,05'
db '2,1 '
db '2,66'
db '??? '
db '0,79'
db '0,89'
db '1,1 '
db '1,12'
db '1,13'
db '1,14'    ;60
db '1,13'
db '1,17'
db '1,2 '
db '1,2 '
db '1,2 '
db '1,22'
db '1,23'
db '1,24'
db '1,25'
db '1,1 '     ; 70
db '1,27'
db '1,3 '
db '1,5 '
db '1,36'
db '1,46'
db '1,52'
db '1,55'
db '1,44'
db '1,42'
db '1,42'     ; 80
db '1,42'
db '1,55'
db '1,67'
db '1,76'
db '1,90'
db '2,06'
db '0,86'
db '0,97'
db '1,00'
db '1,11'  ;90
db '1,14'
db '1,22'
db '1,22'
db '1,2 '
db '1,1 '
db '1,2 '
db '1,1 '
db '1,2 '
db '??? '
db '??? '     ; 100
db '1,2 '
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '     ; 110
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '
db '??? '     ; 116

;-----------------------

;����������� ���������    116 ����⮢
strtbl T_plavl, \
"- 259,19", \
"- 271,15", \
"180,5", \
"1287", \
"2075", \
"4000(����)", \
"-210,0", \
"-218,7", \
"-219,699", \
"-248,6", \
"97,83", \
"648", \
"660,37", \
"1415", \
"44,14(����)  593(����)", \
"119,3", \
"-101,03", \
"-189,34", \
"63,51", \
"842", \	   ;20
"1541", \
"1658", \
"1920", \
"1890", \
"1245", \
"1539", \
"1494", \
"1455", \
"1084,5", \
"419,5", \
"29,78", \
"937", \
"817 (��� ���������)", \
"217", \
"-7,25", \
"-157,37", \
"39,3", \
"768", \
"1528", \
"1855", \	   ;40
"2470", \
"2620", \
"2250", \
"2607", \
"1963", \
"1554", \
"961,93", \
"321,108", \
"156,634", \
"231,9681", \
"630,74", \
"449,8", \
"113,5", \
"-111,85", \
"28,7", \
"727", \
"920", \
"804", \
"931", \
"1024", \   ;60
"1170", \
"1072", \
"826", \
"1312", \
"1356", \
"1409", \
"1470", \
"1522", \
"1545", \
"824", \
"1663", \
"2230", \
"3270", \
"3660", \
"3190", \
"3027", \
"2443", \
"1772", \
"1064,43", \
"-38,862", \	;80
"303,6", \
"327,502", \
"271,44", \
"254", \
"575", \
"-71,0", \
"21", \
"969", \
"1050", \
"1750", \
"1580", \
"1134", \
"637", \
"640", \
"1292", \
"1345", \
"1050", \
"900", \
"860", \
"???", \   ;100
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \    ;110
"???", \
"???", \
"???", \
"???", \
"???", \
"???"	     ; 116



;-----------------------

;����������� �������     116 ����⮢
strtbl T_kipen, \
"- 252,87", \
"- 268,935", \
"1336,6", \
"2471", \
"3700", \
"4200(����)", \
"-195,802", \
"-182,962", \
"-188,2", \
"-246,048", \
"886", \
"1095", \
"2500", \
"3250", \
"287,3 - ���� ", \
"444,674", \
"-34,1", \
"-185,86", \
"760", \
"1495", \	  ;20
"2850", \
"3260", \
"3450", \
"2680", \
"2080", \
"3200", \
"2960", \
"2900", \
"2540", \
"906,2", \
"2403", \
"2850", \
"-", \
"685,3", \
"59,82", \
"153,35", \
"696", \
"1390", \
"3320", \
"4340", \	   ;40
"4927", \
"4630", \
"4630", \
"4900", \
"3700", \
"2940", \
"2170", \
"766,5", \
"2024", \
"2620", \
"1634", \
"990", \
"184,35", \
"-108,12", \
"667,6", \
"1860", \
"3450", \
"3450", \
"3510", \
"3080", \    ;60
"3000", \
"1800", \
"1440", \
"3272", \
"3073", \
"2587", \
"2707", \
"2857", \
"1947", \
"1211", \
"3412", \
"4620", \
"5425", \
"6000", \
"5900", \
"5000", \
"4380", \
"3800", \
"2947", \
"356,66", \  ;80
"1457", \
"1745", \
"1564", \
"962", \
"309", \
"-61,9", \
"669", \
"1536", \
"3300", \
"4200", \
"4500", \
"4200", \
"4100", \
"3350", \
"2880", \
"3200", \
"2630", \
"1227", \
"???", \
"???", \  ;100
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \
"???", \	; 110
"???", \
"???", \
"???", \
"???", \
"???", \
"???"		  ; 116


;���᪮� ��������           ; 116 ����⮢
strtbl Rus_nazv, \
"����த", \		    ;1
"�����", \
"��⨩", \
"��ਫ���", \
"���", \
"����த", \
"����", \
"��᫮த", \
"���", \
"����", \		   ;10
"���਩", \
"������", \
"�����", \
"�६���", \
"�����", \
"���", \
"����", \
"�࣮�", \
"�����", \
"����権", \
"�������", \
"��⠭", \
"�������", \
"�஬", \
"��࣠���", \
"������", \
"�������", \
"������", \
"����", \
"����", \		   ;30
"������", \
"��ଠ���", \
"�����", \
"�����", \
"�஬", \
"�ਯ⮭", \
"�㡨���", \
"��஭権", \
"���਩", \
"��મ���", \
"������", \
"��������", \
"��孥権", \
"��⥭��", \
"�����", \
"��������", \
"��ॡ�", \
"������", \
"�����", \
"�����", \		    ;50
"���쬠", \
"������", \
"���", \
"�ᥭ��", \
"�����", \
"��਩", \
"���⠭", \
"��਩", \
"�ࠧ�����", \
"������", \
"�஬�⨩", \
"����਩", \
"��ய��", \
"���������", \
"��ࡨ�", \
"���஧��", \
"���쬨�", \
"�ࡨ�", \
"�㫨�", \
"���ࡨ�", \		       ;70
"���権", \
"��䭨�", \
"���⠫", \
"�����ࠬ", \
"�����", \
"�ᬨ�", \
"�ਤ��", \
"���⨭�", \
"�����", \
"�����", \
"������", \
"������", \
"�����", \
"�������", \
"����", \
"�����", \
"�࠭権", \
"�����", \
"��⨭��", \
"��਩",\
"��⠪⨭��",\
"�࠭",\
"����㭨�", \		       ;90
"���⮭��", \
"����権", \
"��਩", \
"��ફ��", \
"�����୨�", \
"����⥩���", \
"��ନ�", \
"����������", \
"�������", \
"���७ᨩ", \
"������न�", \
"�㡭��", \
"����࣮���", \
"��਩", \
"���ᨩ", \
"���⭥਩", \
"?����⠤��?", \
"���⣥���", \
"��� ��������", \
"��� ��������", \	    ;110
"��� ��������", \
"��� ��������", \
"��� ��������"		    ;116


;��ࢮ���뢠⥫�              ; 116 ����⮢
strtbl First_Explore, \
"�.��������, 1766", \
"�.���ᥭ, ��.�.�����, 1868", \
"�.��䥤ᮭ, 1817", \
"�.������, 1798", \
"�.�.���-���ᠪ 1808", \
"�����⥭ �� �.�.", \
"�.�������, 1772", \
"�.�.�����, 1772", \
"�.���ᠭ, 1886", \
"�.������,�.�ࠢ���     1898", \
"�.��, 1807",\
"�.��, 1808",\
"�.�.���⥤, 1825",\
"�.�.���-���ᠪ, �.�.�����, 1811",\
"�.�࠭��, 1669",\
"�����⭠ �� �.�.",\
"�.�.�����, 1774",\
"�.������, ��.���, 1894",\
"�.��, 1807",\
"�.��, 1808",\
"�.�.����ᮭ, 1879",\
"�.�ॣ��, 1790 ",\
"�.��᪮, 1869",\
"�.�.������, 1797",\
"�.���, 1774",\
"�����⭮ �� �.�.",\
"�.�࠭��, 1735",\
"�.�஭�⥤�, 1751",\
"�����⭠ �� �.�.",\
"�����⥭ � �।�. �����",\
"�.�.����� �� �㠡��࠭, 1875",\
"�.�.�������, 1886",\
"�����⥭ � �।�. �����",\
"�.���楫���, �.���, 1817",\
"�.�����, 1826",\
"�.������, �.�ࠢ���, 1898",\
"�.�㭧��, �.���壮�, 1861",\
"�.����, 1808",\
"�.�������, 1794",\
"�.������, 1789",\
"�.�����, 1801",\
"�.�.�����, 1778",\
"�.����, �.����, 1937",\
"�.�.�����, 1844",\
"�.������⮭, 1804",\
"�.������⮭, 1803",\
"�����⭮ �� �.�.",\
"�.��஬����, 1817",\
"�.����, �.�����, 1863",\
"�����⭮ �� �.�.",\
"�����⭠ �� �.�.",\
"�.���, 1782",\
"�.�����, 1811",\
"�.������, �.�ࠢ���, 1898",\
"�.�㭧��, �.���壮�, 1860",\
"�.��, 1774",\
"�.��ᠭ���, 1839",\
"�.��ᠭ���, 1839",\
"�.���� 䮭 ����ᡠ�, 1885",\
"�.���� 䮭 ����ᡠ�, 1842",\
"�.���������, �.���� � ��., 1942",\
"�.�.����� �� �㠡��࠭, 1879",\
"�.������, 1901",\
"�.��ਭ��, 1880",\
"�.��ᠭ���, 1843",\
"�.�.����� �� �㠡��࠭, 1886",\
"�.�����, 1879",\
"�.��ᠭ���, 1843",\
"�.�����, 1879",\
"�.��ਭ��, 1878",\
"�.���, �.������, 1907",\
"�.����� � ��., 1922",\
"�.�.������, 1802",\
"�.�.�����, 1781",\
"�. � �.������, �.���, 1925",\
"�.�������, 1804",\
"�.�������, 1804",\
"���. � �ॢ����",\
"�����⭮ �� �.�.",\
"�����⭠ �� �.�.",\
"�.����, 1861",\
"�����⥭ �� �.�.",\
"�.����, 1739",\
"�. � �.���, 1898",\
"�.���ᮭ ��., 1940",\
"�.���, 1940",\
"�.��३, 1939",\
"�. � �.���, �.����, 1898",\
"�.�����, 1899",\
"�.���楫���, 1828",\
"�.���, �.���⭥�, �.�����, 1918",\
"�.������, 1789",\
"�.�.���������, �.�.����ᮭ, 1940",\
"�.����� � ��., 1940",\
"�.����� � ��., 1944",\
"�.����� � ��., 1944",\
"�.����� � ��., 1949",\
"�.����� � ��., 1950",\
"�.�����, �.����ᮭ, 1952",\
"�.�����, �.����ᮭ, 1952",\
"�.����� � ��., 1955",\
"���� ����, 1966",\
"�.����� � ��., 1961",\
"�.�.���஢ � ��., 1964",\
"�.�.���஢ � ��., 1967",\
"�.�.���஢ � ��., 1974",\
"P.Armbruster, G.Munzenber, 1981",\
"P.Armbruster, G.Munzenber, 1984",\
"P.Armbruster, G.Munzenber, 1982",\
"S.Hofmann, V. Ninov, 1994",\
"S.Hofmann, V. Ninov, 1994",\
"S.Hofmann, V. Ninov, 1996",\
"�� �����",\
"���� ���ᨨ, 1998",\
"�� �����",\
"�.�.��������, �.�.�⥭���, 2000"


; ���⭮���        116 ����⮢
strtbl Plotnost, \
"-", \		     ; 1
"-", \
"535", \
"1848", \
"2460", \
"2267", \
"-", \
"-", \
"-", \
"0,90035 �/�", \
"968", \	       ; 10
"1738", \
"2700", \
"2330", \
"1823", \
"1960", \
"-", \
"1,7837 �/�", \
"856", \
"1550", \
"2985", \		; 20
"4507", \
"6110", \
"7140", \
"7470", \
"7874", \
"8900", \
"8908", \
"8920", \
"7140", \
"5904", \		; 30
"5323", \
"5727", \
"4819", \
"-", \
"3,745 �/�", \
"1532", \
"2630", \
"4472", \
"6511", \
"8570", \		; 40
"10280", \
"11500", \
"12370", \
"12450", \
"12023", \
"10490", \
"8650", \
"7310", \
"7310", \
"6697", \		; 50
"6240", \
"4940", \
"5,851 �/�", \
"1879", \
"3510", \
"6146", \
"6689", \
"6640", \
"6800", \
"7264", \		; 60
"7353", \
"5244", \
"7901", \
"8219", \
"8551", \
"8795", \
"9066", \
"9321", \
"6570", \
"9841", \		; 70
"13310", \
"16650", \
"19250", \
"21020", \
"22610", \
"22650", \
"21090", \
"19300", \
"-", \
"11850", \		 ; 80
"11340", \
"9780", \
"9196", \
"-", \
"9,73 �/�", \
"2400", \
"5000", \
"10070", \
"11724", \
"15370", \		 ; 90
"19050", \
"20450", \
"19816", \
"-", \
"13510", \
"14780", \
"15100", \
"-", \
"-", \
"-", \		     ; 100
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \		     ; 110
"-", \
"-", \
"-", \
"-", \
"-", \
"-"		     ; 116


;��������������� � ������ ����            116 ����⮢
strtbl Rasprostr, \
"0,14 %", \		  ; 1
"����� 0,0015 %", \
"0,0030 %", \
"0,0006 %", \
"0,005 %", \
"0,032 %", \
"0,0046 %", \
"46,60 %", \
"0,070 %", \
"0,00007 �/�", \	       ; 10
"2,83 %", \
"2,09 %", \
"8,13 %", \
"27,72 %", \
"0,118 %", \
"0,052 %", \
"0,020 %", \
"0,000004 %", \
"2,59 %", \
"3,63 %", \		  ; 20
"0,001 %", \
"0,57 %", \
"0,0110 %", \
"0,0200 %", \
"0,100 %", \
"5,00 %", \
"0,0023 %", \
"0,0080 %", \
"0,0045 %", \
"0,0065 %", \		    ; 30
"0,0015 %", \
"0,00015 %", \
"0,00017 %", \
"0,000014 %", \
"0,00016 %", \
"0,000114 % (� �⬮���)", \
"0,0120 %", \
"???", \
"0,0040 %", \
"0,0160 %", \		    ; 40
"0,0024 %", \
"0,0003 %", \
"-", \
"0,0000005 %", \
"0.0000001 %", \
"0,000001 %", \
"0,000007 %", \
"0,0000135 %", \
"0,00001 %", \
"0,008 %", \
"0,00005 %", \		     ; 50
"0,000001 %", \
"0,00004 %", \
"0,000039 % (� �⬮���)", \
"0,00037 %", \
"0,040 %", \
"0,0029 %", \
"0,007 %", \
"0,0007 %", \
"0,0025 %", \
"-", \		     ; 60
"0,0007 %", \
"0,00013 %", \
"0,00054 %", \
"0,00043 %", \
"0,0005 %", \
"0,00013 %", \
"0,00033 %", \
"0,000027 %", \
"0,000033 %", \
"0,00008 %", \		     ; 70
"0,0003 - 0,0004 %", \
"0,00025 %", \
"0,0001 %", \
"0,00000007 %", \
"0,000005 %", \
"0,0000001 %", \
"0,0000005 %", \
"0,00000043 %", \
"0,000007 %", \
"0,0003 %", \		    ; 80
"0,0015 %", \
"0,00002 %", \
"0,00000000000002 %", \
"17 �� � ᫮� ���� 1,6 ��", \
"-", \
"-", \
"0,0000000001 %", \
"0,0000000006 %", \
"0,0008 %", \
"-", \		     ; 90
"0,0003 %", \
"-", \
"-", \
"0,0003 %", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \		     ; 100
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \		     ; 110
"-", \
"-", \
"-", \
"-", \
"-"		  ; 116


; �⥯��� ���᫥���   116 ����⮢
strtbl Step_ok, \
"+1 -1", \		     ; 1
"-", \
"+1", \
"+2 (+1)", \
"+3 (+2)", \
"+4 -4", \
"�� +5 �� -3 ", \
"+4 -4", \
"-1", \
"-", \			 ; 10
"+1", \
"+2 (+1)", \
"+3 (+1 +2)", \
"+1 +2 +3 +4", \
"-3 +3 +5", \
"-2 +4 +6", \
"-1 +1 +3 +5 +7", \
"-", \
"+1", \
"+2 (+1)", \		       ; 20
"+3 (+1 +2)", \
"+4 +2 +3", \
"�� +2 �� +5", \
"+2 +3 +6 (+4 +5 +1)", \
"�� +2 �� +7", \
"+2 +3 (+1 +2 +4 +6 +8)", \
"+2 +3 (+1 +4 +5)", \
"+2 (+1 +3 +4)", \
"+1 +2 (+3 +4)", \
"+2", \ 		  ; 30
"+1 +3", \
"+4 +3 +2 +1", \
"+5 +3 -3", \
"+6 +4 -2 (+2)", \
"-1 +1 +3 +5 +7", \
"+2", \
"+1", \
"+2 (+1)", \
"+3", \
"+4 (+1 +2 +3)", \		     ; 40
"+5 (+1 +2 +3 +4)", \
"�� +2 �� +6", \
"�� -1 �� +7", \
"+3 +4 +6 +8", \
"+1 +2 +3 +4 +5 +6", \
"+1 +2 +3 +4 (+5 +6)", \
"+1 +2 +3", \
"+2 (+1)", \
"+3 (+1)", \
"+2 +4", \		     ; 50
"+3 +5 (-3)", \
"-2 +6 +4 (+2)", \
"-1 +1 +3 +5 +7", \
"+2 +4 +6 +8", \
"+1", \
"+2 (+1)", \
"+3", \
"+3 (+4)", \
"+4 +3 (+2)", \
"+4 +3 (+2)", \ 		  ; 60
"+3", \
"+2 +3 (+2)", \
"+3 +2", \
"+3 (+2 +1)", \
"+1 +3 +4", \
"+3 (+2 +4)", \
"+3 (+2)", \
"+3 (+1)", \
"+3 +2 (+4)", \
"+2 +3", \		     ; 70
"+3", \
"+4 (+3 +2)", \
"+5 (+2 +3 +4)", \
"�� +2 �� +6", \
"�� +7 �� -1", \
"+4 +6 +8 +1 +3 +5", \
"+4 +3 +1 +2 +6", \
"+4 +3 +2 (+5 +6)", \
"+1 +3 (+5)", \
"+2 +1", \		     ; 80
"+1 +3", \
"+1 +4 +2", \
"+3 +5 -3 (+1 +2)", \
"+6 +4 +2 -2", \
"-1 +1 +5 (+7)", \
"+2 +4 +6", \
"+1", \
"+2", \
"+3", \
"+4 (+2 +3)", \ 		  ; 90
"+4 +5 (+3)", \
"�� +2 �� +6", \
"+3 +4 +5 (+6 +7)", \
"�� +3 �� +7", \
"�� +2 �� +7", \
"+3 (+4 +6)", \
"+3 (+4)", \
"+2 +3 (+4)", \
"+2 +3", \
"+2 +3", \		     ; 100
"+1 +2 +3", \
"+2 +3", \
"+3", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \
"-", \			 ; 110
"-", \
"-", \
"-", \
"-", \
"-", \
"-"		      ; 116
I_END:
sc   system_colors