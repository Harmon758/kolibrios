;
;   CPU -process Manager
;
;------------------------------------------------------------------------------
; version:	1.70
; last update:  04/04/2012
; changed by:   Marat Zakiyanov aka Mario79, aka Mario
; changes:      Code refactoring and optimization.
;               Added russian language support.
;               Fix - processes information showing  not been updated during
;               the processing of mouse events.
;------------------------------------------------------------------------------
; Many fix's and changes created by:
;               Diamond, Heavyiron, SPraid, <Lrz>,
;               Leency, IgorA, kaitz
;---------------------------------------------------------------------
;   integrated with load_lib.obj by <Lrz>
;---------------------------------------------------------------------
;   additions by M.Lisovin lisovin@26.ru
;---------------------------------------------------------------------
;   original author - VTurjanmaa
;------------------------------------------------------------------------------
	use32
	org 0x0
	db 'MENUET01'		; 8 byte id
	dd 0x01			; header version
	dd START		; start of code
	dd IM_END		; size of image
	dd U_END		; memory for app
	dd stack_area		; esp
	dd 0x0			; boot parameters
	dd 0x0			; path
;------------------------------------------------------------------------------
include 'lang.inc'
include '../../../macros.inc'
include '../../../develop/libraries/box_lib/trunk/box_lib.mac'
include '../../../develop/libraries/box_lib/load_lib.mac'
;------------------------------------------------------------------------------
display_processes=32		; number of processes to show
window_x_size=524
window_y_size=430
;------------------------------------------------------------------------------
@use_library	;use load lib macros
;------------------------------------------------------------------------------
START:				; start of execution
	mcall	68,11
sys_load_library  library_name, cur_dir_path, library_path, system_path, \
err_message_found_lib, head_f_l, myimport, err_message_import, head_f_i
	inc	eax
	jz	close
;------------------------------------------------------------------------------
	mcall	40,0x27	;set event
;------------------------------------------------------------------------------
;set window size and position for 0 function
;to [winxpos] and [winypos] variables
;get screen size
	mcall	14
	mov	ebx,eax
;calculate (x_screen-window_x_size)/2    
	shr	ebx,16+1
	sub	ebx,window_x_size/2
	shl	ebx,16
	mov	bx,window_x_size
;winxpos=xcoord*65536+xsize    
	mov	[winxpos],ebx
;calculate (y_screen-window_y_size)/2    
	and	eax,0xffff
	shr	eax,1
	sub	eax,window_y_size/2
	shl	eax,16
	mov	ax,window_y_size
;winypos=ycoord*65536+ysize    
	mov	[winypos],eax
;------------------------------------------------------------------------------	
	mcall	48,3,sc,40
	edit_boxes_set_sys_color edit1,edit1_end,sc		;set color
	check_boxes_set_sys_color check1,check1_end,sc	;set color
;------------------------------------------------------------------------------
align 4	  
;main loop when process name isn't edited.    
red:	
	call	draw_window		; redraw all window
;------------------------------------------------------------------------------
align 4	
still:
	mcall	23,100		; wait here for event 1 sec.

	dec	eax		      ; redraw request ?
	jz	red

	dec	eax		      ; key in buffer ?
	jz	key

	dec	eax		      ; button in buffer ?
	jz	button

;	sub	eax,3		      ; If not use mouse - show 
;	jnz	still_end

	push	dword edit1
	call	[edit_box_mouse]
	
	push	dword[check1+32]
	
	push	dword check1
	call	[check_box_mouse]
	
	pop	eax
	
	cmp	eax, dword[check1+32]
	jz	still_end
	
	push	dword check1
	call	[check_box_draw]
;--------------------------------------
align 4	
show_process_info_1:	
	mcall	26,9
	add	eax,100
	mov	[time_counter],eax

	call	show_process_info	; draw new state of processes
	jmp	still
;------------------------------------------------------------------------------
align 4	
still_end:
	mcall	26,9
	cmp	[time_counter],eax
	ja	still

	add	eax,100
	mov	[time_counter],eax

	call	show_process_info	; draw new state of processes
	jmp	still
;------------------------------------------------------------------------------
align 4	
key:				; key
	mcall	2

	cmp	ah,184 		; PageUp
	jz	pgdn

	cmp	ah,183
	jz	pgup			; PageDown

	cmp	ah,27
	jz	close			; Esc

	push	dword edit1
	call	[edit_box_key]
				; Check ENTER with ed_focus edit_box
	lea	edi,[edit1]
	test	word ed_flags,ed_focus
	jz	still_end 

	sub	ah,13			; ENTER?
	jz	program_start		; RUN a program

	jmp	still
;------------------------------------------------------------------------------
align 4	
button:			
; get button id  
	mcall	17
	shr	eax,8			
;id in [10,50] corresponds to terminate buttons.
	cmp	eax,10
	jb	noterm 		

	cmp	eax,50
	jg	noterm
;calculate button index        
	sub	eax,11
;calculate process slot    
	mov	ecx,[tasklist+4*eax]
;ignore empty buttons
	test	ecx,ecx
	jle	still_end
;terminate application    
	mcall	18,2
	jmp	show_process_info_1	;still_end 
;--------------------------------------
align 4
noterm:
;special buttons
	dec	eax
	jz	close

	sub	eax,50
	jz	pgdn	  ;51

	dec	eax
	jz	pgup	  ;52

	dec	eax
	jz	program_start	;53

	dec	eax
	jz	reboot 	;54

	jmp	still_end
;buttons handlers    
;------------------------------------------------------------------------------
align 4	
pgdn:
	sub	[list_start],display_processes
;	cmp	[list_start],0
	jge	show_process_info_1	;still_end  
	mov	[list_start],0
	jmp	show_process_info_1	;still_end  
;------------------------------------------------------------------------------
align 4	
pgup:
	mov	eax,[list_add]  ;maximal displayed process slot
	mov	[list_start],eax
	jmp	show_process_info_1	;still_end  
;------------------------------------------------------------------------------
align 4	    
program_start:    
	mcall	70,file_start
	jmp	show_process_info_1	;still_end
;------------------------------------------------------------------------------
align 4	
reboot:    
	mcall	70,sys_reboot
;close program if we going to reboot
;------------------------------------------------------------------------------
align 4	
close:
	or	eax,-1 		; close this program
	mcall
;------------------------------------------------------------------------------
align 4	
draw_next_process:
;input:
;  edi - current slot
;  [curposy] - y position
;output:
;  edi - next slot (or -1 if no next slot)
;registers corrupted!
;delete old button
	mov	edx,[index]
	add	edx,(1 shl 31)+11
	mcall	8
;create terminate process button
	mov	ecx,[curposy]
	shl	ecx,16
	mov	cx,10
	mov	edx,[index]
	add	edx,11
	mov	esi,0xaabbcc
;contrast    
	test	dword [index],1
	jz	.change_color_button
	mov	esi,0x8899aa
;--------------------------------------
align 4
.change_color_button:
	mcall	,<10,99>
;draw background for proccess information
; ecx was already set
	mov	edx,0x88ff88
;contrast
	test	dword [index],1
	jz	.change_color_info
	mov	edx,0xddffdd
;--------------------------------------
align 4
.change_color_info:
	mcall	13,<110,395>
;nothing else should be done
;if there is no process for this button    
	test	edi,edi
	jl	.ret
;--------------------------------------
align 4
.return_1:
;find process
	inc	edi
;more comfortable register for next loop    
	mov	ecx,edi
;precacluate pointer to process buffer    
	mov	ebx,process_info_buffer
;--------------------------------------
align 4
.find_loop:
	cmp	ecx,256
	jge	.no_processes
;load process information in buffer
	mcall	9
;if current slot greater than maximal slot,
;there is no more proccesses.    
	cmp	ecx,eax
	jg	.no_processes
;if slot state is equal to 9, it is empty.    
	cmp	[process_info_buffer+process_information.slot_state],9
	jnz	.process_found
    
	inc	ecx
	jmp	.find_loop
;--------------------------------------
align 4
.no_processes:
	or	edi,-1
	ret
;--------------------------------------
align 4
.process_found:
;check on/off check box
	push	edi
	lea	edi,[check1]
	test	dword ch_flags,ch_flag_en
	pop	edi
	jnz	@f

	cmp	dword [process_info_buffer+10],'ICON'
	jz	.return_1 

	cmp	dword [process_info_buffer+10],'OS/I'
	jz	.return_1

	cmp	byte [process_info_buffer+10],'@'
	jz	.return_1
;--------------------------------------
align 4
@@:
	mov	edi,ecx
	mov	[list_add],ecx
;get processor cpeed    
;for percent calculating
	mcall	18,5
	xor	edx,edx
	mov	ebx,100
	div	ebx
;eax = number of operation for 1% now
;calculate process cpu usage percent
	mov	ebx,eax
	mov	eax,[process_info_buffer+process_information.cpu_usage]
;	cdq
	xor	edx,edx ; for CPU more 2 GHz - mike.dld 
	div	ebx
	mov	[cpu_percent],eax
;set text color to display process information
;([tcolor] variable)
;0%      : black    
;1-80%   : green
;81-100% : red
	test	eax,eax
	jnz	.no_black

	mov	[tcolor],eax
	jmp	.color_set
;--------------------------------------
align 4
.no_black:   
	cmp	eax,80
	ja	.no_green

	mov	dword [tcolor],0x107a30
	jmp	.color_set
;--------------------------------------
align 4
.no_green:
	mov	dword [tcolor],0xac0000
;--------------------------------------
align 4
.color_set:
;show slot number
;ecx haven't changed since .process_found    
;	mov	ecx,edi
	mov	edx,[curposy]
	add	edx,15*65536+1
	mcall	47,<2,256>,,,[tcolor]
;show process name
	mov	ebx,[curposy]
	add	ebx,45*65536+1
	mcall	4,,[tcolor],process_info_buffer.process_name,11
;show pid
	mov	edx,[curposy]
	add	edx,125*65536+1
	mcall	47,<8,256>,[process_info_buffer.PID],,[tcolor]
;show cpu usage
	add	edx,60*65536
	mcall	,,[process_info_buffer.cpu_usage]
;show cpu percent
	add	edx,60*65536
	mcall	,<3,0>,[cpu_percent]
;show memory start - obsolete
	add	edx,30*65536
	mcall	,<8,256>,[process_info_buffer.memory_start]
;show memory usage
	mov	ecx,[process_info_buffer.used_memory]
	inc	ecx
	add	edx,60*65536
	mcall
;show window stack and value
	add	edx,60*65536
	mcall	,,dword [process_info_buffer.window_stack_position]
;show window xy size
	mov	ecx,[process_info_buffer.box.left]
	shl	ecx,16
	add	ecx,[process_info_buffer.box.top]
	add	edx,60*65536
	mcall 
;--------------------------------------
align 4
.ret:
;build index->slot map for terminating processes.
	mov	eax,[index]
	mov	[tasklist+4*eax],edi	     
	ret
;------------------------------------------------------------------------------
align 4	
f11:
;full update  
	push	edi
	call	draw_window
	pop	edi
;------------------------------------------------------------------------------
;   *********************************************
;   *******  WINDOW DEFINITIONS AND DRAW ********
;   *********************************************
align 4	
draw_window:
	mcall	12, 1		       
; DRAW WINDOW
	xor	eax,eax				; function 0 : define and draw window
	xor	esi,esi
	mcall	,[winxpos],[winypos],0x34ddffdd,,title
; function 4 : write text to window
	xor	ecx,ecx
	mcall	4,<17,13>,,text,text_len

	push	dword edit1
	call	[edit_box_draw]

	push	dword check1
	call	[check_box_draw]

	mcall	26,9
	add	eax,100
	mov	[time_counter],eax

	call	show_process_info
; previous page button
	mcall	8,<25,96>,<358,10>,51,0xaabbcc
; next page button  52
	inc	edx
	mcall	,<125,96>
; ">" (text enter) button
	add	ecx,20 shl 16
; run button 53
	inc	edx
	mcall	,<451,50>
; reboot button    
	sub	ebx,120*65536		    
	add	ebx,60
	sub	ecx,20 shl 16
	inc	edx
	mcall
;"PREV PAGE", "NEXT PAGE" and "REBOOT" labels
	xor	ecx,ecx
	mcall	4,<45,360>,,tbts,tbte-tbts
;"RUN" labels
	mcall	,<464,380>,,tbts_3,tbte_2-tbts_3
;print application name in text box
	mcall	12, 2
	ret
;------------------------------------------------------------------------------
align 4
show_process_info:
	mov	edi,[list_start]
	mov	[list_add],edi
	mov	dword [index],0
	mov	dword [curposy],32
;--------------------------------------
align 4
.loop_draw:
	call	draw_next_process
	inc	dword [index]
	add	dword [curposy],10
	cmp	[index],display_processes
	jl	.loop_draw
	ret
;------------------------------------------------------------------------------
; DATA AREA
;------------------------------------------------------------------------------
system_path	 db '/sys/lib/'
library_name	 db 'box_lib.obj',0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

err_message_found_lib	db 'Sorry I cannot load library box_lib.obj',0
head_f_i:
head_f_l	db 'System error',0
err_message_import	db 'Error on load import library box_lib.obj',0
;------------------------------------------------------------------------------
align 4	
myimport:   
edit_box_draw		dd aEdit_box_draw
edit_box_key		dd aEdit_box_key
edit_box_mouse		dd aEdit_box_mouse
;version_ed		dd aVersion_ed

check_box_draw		dd aCheck_box_draw
check_box_mouse		dd aCheck_box_mouse
;version_ch		dd aVersion_ch

;option_box_draw	dd aOption_box_draw
;option_box_mouse	dd aOption_box_mouse
;version_op		dd aVersion_op

		dd 0
		dd 0

aEdit_box_draw		db 'edit_box',0
aEdit_box_key		db 'edit_box_key',0
aEdit_box_mouse		db 'edit_box_mouse',0
;aVersion_ed		db 'version_ed',0

aCheck_box_draw		db 'check_box_draw',0
aCheck_box_mouse	db 'check_box_mouse',0
;aVersion_ch		db 'version_ch',0

;aOption_box_draw	db 'option_box_draw',0
;aOption_box_mouse	db 'option_box_mouse',0
;aVersion_op		db 'version_op',0
;------------------------------------------------------------------------------
align 4	
check1 check_box 10,378,6,11,0x80AABBCC,0,0,check_text,check_t_e,0;ch_flag_en
check1_end:
edit1 edit_box 350,95,376,0xffffff,0x6f9480,0,0xAABBCC,0,start_application_c,\
   start_application,mouse_dd,ed_focus,start_application_e,start_application_e
edit1_end:
list_start  dd 0
;------------------------------------------------------------------------------
align 4	
sys_reboot:
	    dd 7
	    dd 0
	    dd 0
	    dd 0
	    dd 0
	    db '/sys/end',0
;------------------------------------------------------------------------------
if lang eq de
text:
	db 'NAME/BEENDEN        PID     CPU-LAST   % '
	db 'SPEICHER START/NUTZUNG  W-STACK   W-SIZE'
text_len = $-text

tbts:	db 'SEITE ZURUECK       SEITE VOR                      REBOOT SYSTEM'
tbte:
tbts_3	db 'START'
tbte_2:
check_text	db '@ on/off'
check_t_e=$-check_text
title	db 'Prozesse  - Ctrl/Alt/Del',0
;--------------------------------------
else if lang eq et
text:
	db 'NIMI/L�PETA         PID    CPU-KASUTUS %   '
	db 'M�LU ALGUS/KASUTUS  W-PUHVER  W-SUURUS'
text_len = $-text

tbts:	db 'EELMINE LEHT   J�RGMINE LEHT                     REBOODI S�STEEM'
tbte:
tbts_3	db 'START'
tbte_2:
check_text	db '@ on/off'
check_t_e=$-check_text
title	db 'Protsessid - Ctrl/Alt/Del'
;--------------------------------------
else if lang eq ru
text:
	db '���/���������      PID     CPU-�������� %  '
	db '������ ������/�����  W-STACK    W-SIZE'
text_len = $-text

tbts:	db '����.���        ����.���                          ������������'
tbte:
tbts_3	db '������'
tbte_2:
check_text	db '@ ���/�몫'
check_t_e=$-check_text
title	db '��ᯥ��� ����ᮢ - Ctrl/Alt/Del',0
;--------------------------------------
else
text:
	db 'NAME/TERMINATE      PID     CPU-USAGE  %   '
	db 'MEMORY START/USAGE  W-STACK    W-SIZE'
text_len = $-text

tbts:	db 'PREV PAGE       NEXT PAGE                         REBOOT SYSTEM'
tbte:
tbts_3	db ' RUN'
tbte_2:
check_text	db '@ on/off'
check_t_e=$-check_text
title	db 'Process manager - Ctrl/Alt/Del',0

end if
;------------------------------------------------------------------------------
align 4	
file_start:
	dd 7
	dd 0
	dd 0
	dd 0
	dd 0
start_application: db '/sys/LAUNCHER',0
start_application_e=$-start_application-1
;                   times 60 db 0
	rb 60
start_application_c=$-start_application-1
;------------------------------------------------------------------------------
IM_END:
;------------------------------------------------------------------------------
align 4	
sc system_colors
winxpos		rd 1
winypos		rd 1
mouse_dd	rd 1
cpu_percent	rd 1
tcolor		rd 1
list_add	rd 1
curposy		rd 1
index		rd 1
tasklist	rd display_processes
time_counter	rd 1
;------------------------------------------------------------------------------
align 4	
library_path:
process_info_buffer process_information
;------------------------------------------------------------------------------
align 4	
cur_dir_path:
	rb 1024
;------------------------------------------------------------------------------
align 4	
	rb 1024
stack_area:
;------------------------------------------------------------------------------
U_END:
;------------------------------------------------------------------------------
