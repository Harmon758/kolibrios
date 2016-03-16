;
; Ghost Monitor - утилита (стресс) тестирования и мониторинга системы
; Copyright (C) 2005, 2006, 2007 Mihailov Ilia (ghost.nsk@mail.ru)
; All Right Reserved
;
; ВОЗМОЖНА ПОРЧА ОБОРУДОВАНИЯ!
; ИСПОЛЬЗУЙТЕ НА СВОЙ СТРАХ И РИСК!
;
; Заметки :
;	Дописать запуск тестов (rtst) (нет реакции на ошибку запуска)
;	Переделать обновление графика (сделать отдельным процессом)
;	Добавить реакцию на параметры датчиков
;

; Внешние файлы :
;	hwm*.inc	- процедуры работы с датчиками мониторинга
;	system.inc	- вычисление загрузки процессора, завершение теста, etc...
;	tests.inc	- тесты ;-)
;	diag.inc	- поддержка диаграм
;	tab_*.inc	- процедуры отрисовки соответствуюших вкладок


; Identifier buttons :
;	01 - Exit
;	02 - Info tab
;	03 - Tests tab
;	04 - Config tab
;	05 - About tab
;	06 - P5 test (tab_test.inc)
;	07 - P6 test (tab_test.inc)
;	08 - K6 test (tab_test.inc)
;	09 - K7 test (tab_test.inc)
;	10 - MMX test	- not used (tab_test.inc)
;	11 - Update time increment (tab_conf.inc)
;	12 - Update time decriment (tab_conf.inc)
;	13..17 - Color Changing buttons CPU_load, Mem_usage, Temp1, Temp2, Temp3

include 'lang.inc'
include 'config.inc'
include 'macros.inc'

tcol	equ	0x5080d0
atcol	equ	0x4070c0
val_mod equ	0x3090

MEOS_APP_START
include "tests.inc"	; <-- Много выравниваний, лучше здесь
CODE
;--- Init ------------------------------------------------------------
	; Очищаем неинициализированные данные
	mov	ecx, __memory - __udata
	mov	edi, __udata
	xor	eax, eax
	rep	stosb

	xor	eax, eax
	cpuid					; Узнаём производителя проца
	mov	[Vendor + 0], ebx
	mov	[Vendor + 4], edx
	mov	[Vendor + 8], ecx
	mov	eax, 1
	cpuid					; Есть ли MMX ?
	mov	ecx, eax
	shr	al, 4
	and	ax, 0x0F0F
	mov	[CPU_fam], ah
	mov	[CPU_mod], al
	shr	edx, 23
	and	dl, 1
	mov	[CPU_mmx], dl
	shr	ecx, 12
	and	cl, 11b
	mov	[CPU_type], cl

	; ! Цвета из массива colors
	mov	dword[d_cpu_load], 0xFF00	; инициализируем график загрузки АЛУ
	mov	dword[d_mem_usage], 0x8BA169	; -- mem usega
	mov	dword[d_temp1], 0xFF0000	; -- график температуры ЦПУ
	mov	dword[d_temp2], 0xFF		; -- график температуры МП
	mov	dword[d_temp3], 0xFF7E23

	call	hwm_init			; Initialize monitoring chip

	jmp	draw_window
;--- Wait & translate event ------------------------------------------
wait_for_event:
	mov	byte[diag_add_v], 0	; добавляем данные к графику только по таймеру
	mov	ebx, [update_time]
	mov	eax, 23
	int	0x40
	test	eax, eax
	jnz	@f
	inc	byte [diag_add_v]
	jmp	redraw
@@:	dec	eax			; redraw request ?
	jz	draw_window
	dec	eax			; key in buffer ?
	jz	key
	dec	eax			; button in buffer ?
	jz	button
	jmp	wait_for_event
;--- Key pressed -----------------------------------------------------
key:
	mov	eax, 2
	int	0x40
	cmp	ah, 27			; Esc - shutdown
	je	exit
	jmp	wait_for_event
;--- Button down -----------------------------------------------------
button:
	mov	eax, 17 		; get id
	int	0x40
	cmp	ah, 1			; button id == 1 ?
	jne	noexit
exit:	cmp	byte[test_id], 0
	je	@f
	call	stop_test
@@:	mov	eax, -1 	; close this program
	int	0x40
noexit:
	; button tabs (2 .. 5)
	cmp	ah, 5
	jg	no_tabs
	mov	byte[act_tab], ah
	jmp	redraw_tabs
no_tabs:
	; кнопки запуска тество (6 .. 10)
	cmp	ah, 10
	jg	no_test_bt
	; если нет запущеных тестов - запускаем выбранный
	cmp	byte[test_id], 0
	je	run_test
	; если кнопка активного теста - останавливаем тест, иначе ничего не делаем
	cmp	[test_id], ah
	jne	wait_for_event
	call	stop_test
	jmp	redraw_tabs

tsts:	dd	p5, p6, k6_, k7_
run_test:
	push	eax
	movzx	ecx, ah
	mov	ecx, [tsts + (ecx * 4) - (6 * 4)]
rtst:	mov	eax, 51
	mov	edx, stp
	mov	ebx, 1
	int	0x40
	cmp	eax, 0xFFFFFFF0 ; -16
	je	wait_for_event	; <-- ошибка запуска теста, ПЕРЕПИСАТЬ
	mov	[test_pid], eax
	pop	eax
	mov	byte [test_id], ah
	jmp	redraw_tabs
	;-------------------------
	; кнопки изменения времени обновления
no_test_bt:
	cmp	ah, 11
	jne	no_ut_inc
	add	dword[update_time], 10
	jmp	redraw_tabs
no_ut_inc:
	cmp	ah, 12
	jne	no_ut_dec
	cmp	dword[update_time], 10
	jng	wait_for_event
	sub	dword[update_time], 10
	jmp	redraw_tabs
no_ut_dec:
	;-------------------------
	; color change button schedules (13..17)
	cmp	ah, 17
	jg	no_change_color
	sub	ah, 13
	movzx	ecx, ah
	mov	esi, [d_colors + ecx * 4]
	mov	eax, [esi]
	mov	ecx, (colors_end - colors) / 4	; number of colors
@@:	dec	ecx
	cmp	eax, [colors + ecx * 4]
	jne	@b
	mov	ebx, [colors + ecx * 4 - 4]
	mov	[esi], ebx
	jmp	redraw_tabs
no_change_color:
	jmp	wait_for_event
;---------------------------------------------------------------------
draw_window:
	mov	eax,12
	mov	ebx,1
	int	0x40
	; Создаём окно
	xor	eax, eax
	mov	ebx, 300 * 65536 + 309
	mov	ecx, 150 * 65536 + 450
	mov	edx, tcol + 0x14000000	; color of work area RRGGBB,8->color
	mov	edi, title		; color of frames    RRGGBB
	int	0x40
	; Create a graphic frame (not for graph)
	mov	eax, 13
	mov	ebx, 10 * 65536 + 290
	mov	ecx, 138 * 65536 + 105
	mov	edx, atcol
	int	0x40

	; write parameter names (Cpu load, temper, etc ...)
	mov	edx, msgs_mon         ; text for indicators
	mov	ebx, 17 * 65536 + 30
	call    show_text

	mov	edx, msgs_mon2
	mov	ebx, 166 * 65536 + 30
	call	show_text

	jmp draw_tabs ; draw tabs for the first time

redraw_tabs: ; delete previously created tabs

	mov	ecx, 4
	mov	eax, 8
	mov	edx, 0x80000000 + 2
del_tabs:
	int	0x40
	inc	edx
	loop	del_tabs

	; Delete color change buttons
	mov	eax, 8
	mov	edx, 0x80000000 + 13
@@: 
	int	0x40
	inc	edx
	cmp	edx, 0x80000000 + 13 + 5 	; 5 (buttons)
	jne	@b

draw_tabs:

	; Drawing color change buttons
	mov	eax, 8
	mov	ebx, 112 * 65536 + 5
	mov	ecx, 30 * 65536 + 6
	mov	edx, 13

@@: 
	mov	esi, [d_colors - 13 * 4 + edx * 4]
	mov	esi, [esi]
	int	0x40
	add	ecx, 10 * 65536
	inc	edx
	cmp	edx, 13 + 5             ; 5 (buttons)
	jne	@b

	; info, test, config, about buttons
	; Create 4 buttons
	mov	edi, 250 * 65536 + 25
	mov	ecx, 4
	mov	eax, 8
	mov	ebx, 10 * 65536 + 70
	mov	edx, 2
n_bt:   
	push    ecx
	mov	ecx, edi
	mov	esi, tcol
	cmp	byte [act_tab], dl
	jne	na1
	mov	esi, atcol
na1:    
	int	0x40
	inc	edx
	add	ebx, 73 * 65536
	pop	ecx
	loop    n_bt

	; write name tabs
	mov	eax, 4
	mov	ebx, (18 shl 16) + 256
	mov	edx, tab_lab
	mov	ecx, 0x10000000
	mov	esi, 4
	int	0x40
	add	ebx, (72 shl 16)
	mov	edx, tab_lab.2
	mov	esi, 5
	int	0x40
	add	ebx, (72 shl 16)
	mov	edx, tab_lab.3
	mov	esi, 7
	int	0x40
	add	ebx, (72 shl 16)
	mov	edx, tab_lab.4
	mov	esi, 5
	int	0x40

	; create a frame at the bottom of the screen
	mov	eax, 13
	mov	ebx, 10 * 65536 + 290
	mov	ecx, 275 * 65536 + 160
	mov	edx, atcol
	int	0x40
	mov	ebx, 12 * 65536 + 286
	mov	ecx, 277 * 65536 + 156
	mov	edx, tcol
	int	0x40

	; draw the selected tab
	mov	al, [act_tab]
	cmp	al, 2
	je 	info_tab
	cmp	al, 3
	je 	test_tab
	cmp	al, 4
	je 	config_tab
	jmp	about_tab
	
;--- Drow Info tab ---------------
info_tab:
        include "tab_info.inc"
;--- Drow test tab ---------------
test_tab:
        include "tab_test.inc"
;--- Drow Config tab -------------
config_tab:
        include "tab_conf.inc"
;--- Drow About tab --------------
about_tab:
        include "tab_about.inc"

redraw:

	; frame for graph

	mov	eax, 13
	mov	ebx, 12 * 65536 + 286
	mov	ecx, 140 * 65536 + 101
	mov	edx, tcol
	int	0x40
	mov	eax, 38
	mov	edx, atcol
	mov	ebx, 12 * 65536 + 286 + 12
	mov	ecx, 140 * 65536 + 140
@@:	add	ecx, 10 * 65536 + 10
	int	0x40
	cmp	cx, 230 		; привязано к координатам окна
	jl	@b

	; write CPU usage
	call    cpu_usage
	;--- добавляем на график ---
	mov esi, d_cpu_load
	call    d_add
	call    d_show
	;---------------------------
	movzx	ecx, al
	mov	eax, 47
	mov	edx, 121 * 65536 + 30
	xor	esi, esi
	call	digit_len
	clear_prev_no edx
	int	0x40

	; write used memory
	call	mem_usage
	;--- добавляем на график ---
	mov	esi, d_mem_usage
	call	d_add
	call	d_show
	;---------------------------
	movzx	ecx, al
	mov	eax, 47
	mov	edx, 121 * 65536 + 40
	xor	esi, esi
	call	digit_len
	clear_prev_no edx
	int	0x40

	cmp	byte[hwm_enable], 0
	jne	show_mon
	; Нет датчиков - пишем NO
	mov	ecx, 8	;<- number of parameters
	mov	eax, 4
	mov	ebx, 121 * 65536 + 50
	mov	edx, msg_no
	mov	esi, 3
@@:	int	0x40
	add	ebx, 10
	loop	@b

	mov	ecx, 7	;<- number of parameters
	mov	ebx, 260 * 65536 + 30
@@:	int	0x40
	add	ebx, 10
	loop	@b

	jmp	no_monitor
show_mon:
	;Есть датчики пишим инфу
	call	hwm_get_params
	;---------------------------
	; Температуру
	mov	ecx, 3
	mov	esi, hwm_temps
	mov	edx, 121 * 65536 + 50
	xor	eax, eax
sh_temp:push	ecx
	lodsw
	push	esi
	; реакция на изменения
	xor	esi, esi
	cmp	[old_t1 + ecx * 4 - 4], eax
	je	@f
	mov	[old_t1 + ecx * 4 - 4], eax
	mov	esi, val_mod
@@:	movzx	ecx, al
	push	eax
	mov	eax, 47
	mov	ebx, 0x20000
	clear_prev_no edx
	int	0x40
	pop	eax
	mov	cl, ah
	mov	eax, 47
	mov	ebx, 0x10000
	add	edx, 15 * 65536
	int	0x40
	sub	edx, 15 * 65536 - 10
	pop	esi
	pop	ecx
	loop	sh_temp
	;;--- Add to the chart temp1 ---
	movzx	eax, word[hwm_temps]
	mov	esi, d_temp1
	call	d_add
	call	d_show
	;;--- Add to the chart temp2 ---
	movzx	eax, word[hwm_temps + 2]
	mov	esi, d_temp2
	call	d_add
	call	d_show
	;;--- Add to the chart temp3 ---
	movzx	eax, word[hwm_temps + 4]
	mov	esi, d_temp3
	call	d_add
	call	d_show

	;---------------------------
	; Скорости врашения вентилей
	mov	ecx, 5
	mov	esi, hwm_rpms
	mov	edx, 121 * 65536 + 80
sh_fan: push	ecx
	lodsd
	push	esi
	;реакция на изменения
	xor	esi, esi
	cmp	[old_f1 + ecx * 4 - 4], eax
	je	@f
	mov	[old_f1 + ecx * 4 - 4], eax
	mov	esi, val_mod
@@:	mov	ecx, eax
	call	digit_len
	mov	eax, 47
	clear_prev_no edx
	int	0x40
	add	edx, 10
	pop	esi
	pop	ecx
	loop	sh_fan

	;---------------------------
	; Напруги
	mov	esi, hwm_voltages
	mov	edi, old_volts
	mov	ecx, 7
	mov	ebx, 260 * 65536 + 30
	mov	edx, Data_String
volt_nxt:
	push	ecx
	push	esi
	call	ftoa
	;реакция на изменения
	xor	ecx, ecx
	fld	dword [esi]
	fsub	dword [edi]
	fabs

	; fcomp dword [c_eps]	; bug in MeOsEmul
	; fstsw ax
	fcom	dword [c_eps]	; 0xBADCODE
	fstsw	ax
	finit

	sahf
	jc	@f
	mov	ecx, val_mod
@@:	mov	esi, Data_String	; считаем кол-во символов
@@:	lodsb
	test	al, al
	jnz	@b
	sub	esi, Data_String

	mov	eax, 4
	int	0x40
	add	ebx, 10
	pop	esi
	pop	ecx
	movsd
	loop	volt_nxt

no_monitor:
	;---------------------------
	mov	al, [act_tab]   ; check if info tab is active
	cmp	al, 2
	jne	end_drow_tab
	jmp	free_mem    ; in tab_info.inc

end_drow_tab:
	mov	eax, 12
	mov	ebx, 2
	int	0x40
	jmp	wait_for_event
;---------------------------------------------------------------------
	include "system.inc"
	include "hwm.inc"
	include "diag.inc"


DATA
act_tab 	db	2			; active tab number
tab_lab:
.1: db 'Info'
.2: db 'Tests'
.3: db 'Configs'
.4: db 'About'
title		db	'Ghost Monitor v0.4 [18/11/15]',0

msgs_mon mls \
	'CPU Load (%)',\
	'Mem Usage(%)',\
	'Temp1',\
	'Temp2',\
	'Temp3',\
	'Fan1',\
	'Fan2',\
	'Fan3',\
	'Fan4',\
	'Fan5'

msgs_mon2 mls \
	'Vcore',\
	'AGP',\
	'+3.3V',\
	'AVcc (+5V)',\
	'+12V',\
	'-12V',\
	'-5V'

msg_yes 	db	'Yes'
msg_no		db	'N/A'

update_time	dd	300			; период обновления в сотых долях секунды
d_colors	dd	d_cpu_load, d_mem_usage, d_temp1, d_temp2, d_temp3
c_eps		dd	0.01

; Цвета графиков
colors: 	dd	0xff0000, 0xff00, 0xff, 0xffffff, tcol, 0xFF7E23, 0x8BA169, 0xff0000	; первый и последний равны
colors_end:
;---------------------------------------------------------------------
UDATA

test_pid	dd	?			; если test_id == 0 - не имеет смысла
test_id 	db	?			; 0 - тест не запушен (соответствует № кнопки 6..10)
diag_add_v	db	?			; 0 - не добавлять данные на график, !0 - добавлять

Vendor: 	times	12 db ?
CPU_fam:	db	?
CPU_mod:	db	?
CPU_type:	db	?
CPU_mmx:	db	?

; Для реакции на изменения
old_t1: 	dd	?	; последнее значение temp1
old_t2: 	dd	?	; ...
old_t3: 	dd	?
old_f1: 	dd	?
old_f2: 	dd	?
old_f3: 	dd	?
old_volts:	times	7 dd ?

; Грфики
diag_beg:
d_cpu_load:	dd	?
		times 286 db ?

d_mem_usage:	dd	?
		times 286 db ?

d_temp1:	dd	?
		times 286 db ?

d_temp2:	dd	?
		times 286 db ?

d_temp3:	dd	?
		times 286 db ?
diag_end:

; --- стек теста, воизбежание глюков не переносить в другое место ----
th_stack:	times	1024 db ?
stp:
;---------------------------------------------------------------------
MEOS_APP_END
