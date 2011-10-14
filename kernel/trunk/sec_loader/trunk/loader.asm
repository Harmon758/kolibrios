; Copyright (c) 2008-2009, <Lrz>
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

;start of the project 13.02.2008 year.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Secondary Loader copyright Alexey Teplov nickname <Lrz>
;if you need log preproc
;/////////////
;include 'listing.inc'
;enable listing
;////////////
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;start of code:                                                               ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
use16
                  org   0x0
        jmp     start
include 'sl_equ.inc'            ; � ����� ��������� ��� equ ���������������
include 'boot_st.inc'
include 'debug_msg.inc'         ;here is message from debug
include 'parse_dat.inc'
include 'sl_proc.inc'
include 'parse.inc'
include 'parse_loader.inc'
include 'parse_any.inc'
include 'parse_def_sect.inc'
include 'parse_err.inc'

file_data  dw 0x0,ini_data_  ;������: ��������: ������� �.�. ������������ les
size_data  dw 16 ;16 ������ �� 4 �� �.� ������ �� 64 ��
name_ini_f db 'kord/startos.ini',0 

;////////////
loader_callback dd ?
load_drive dw ?
load_ft    dw ?
;Start code

start:
; Save far pointer to callback procedure, ds:si is point
        mov     word [cs:loader_callback], si
        mov     word [cs:loader_callback+2], ds
; Save type of drive
        mov     word [cs:load_drive], ax
; Save type of FT
        mov     word [cs:load_ft], bx
; set up stack
        mov     ax, cs
        mov     ss, ax
        xor     sp, sp
; set up segment registers
        mov     ds, ax
        mov     es, ax
; just to be sure: force DF=0, IF=1
        cld
        sti

; set videomode
        mov     ax, 3
        int     0x10

        mov     si, version
        call    printplain
        mov     al, '#'
        mov     cx, 80
;input cx=size al=char ����� ������ ������ ������� ��� ������� � cx
@@:
        call    putchar
        loop    @b

  if DEBUG
        pushad
        mov     ax, cs
        shl     eax, 4  ; � ���������� �������  ����� ��������
        mov     cx, 0xa
        mov     di, cseg_msg
        call    decode
;***************
        mov     si, cseg_msg
        call    printplain
        popad
  end if


  if DEBUG
        mov     si, stack_msg
        call    printplain
  end if

; Require 586 or higher processor (cpuid and rdtsc,rdmsr/wrmsr commands)
; install int 6 (#UD) handler
        xor     bx, bx
        mov     ds, bx
        push    word [bx+6*4+2]
        push    word [bx+6*4]
        mov     word [bx+6*4], ud16
        mov     word [bx+6*4+2], cs
; issue CPUID command
        xor     eax, eax        ; N.B.: will cause #UD before 386
        cpuid                   ; N.B.: will cause #UD before later 486s
        test    eax, eax
        jz      cpubad
; get processor features
        xor     eax, eax
        inc     ax
        cpuid
        test    dl, 10h         ; CPUID[1].edx[4] - TSC support
        jz      cpubad
        test    dl, 20h         ; CPUID[1].edx[5] - MSR support
        jnz     cpugood

ud16:   ; #UD handler, called if processor did not recognize some commands
cpubad:
; restore int 6 (#UD) handler
        pop     word [6*4]
        pop     word [6*4+2]
; say error
        push    cs
        pop     ds
        mov     si, badprocessor
sayerr:
        call    printplain
        jmp     $

cpugood:
; restore int 6 (#UD) handler
        pop     dword [6*4]
        push    cs
        pop     ds

; set up esp
        movzx   esp, sp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; init memory
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




; Load  startos.ini
        mov     cx, loop_read_startos_file      ;���-�� ������� ������ ����� ������������ startos.ini
align 4
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Load startos.ini                                                           ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
load_startos_file:

        xor     ax, ax
        mov     di, file_data
        inc     ax        ;function 1 - read file
        push    cx
        call    far dword [loader_callback]
        pop     cx
        push    cs
        push    cs
        pop     ds
        pop     es

        test    bx, bx
        jz      check_conf_file
        dec     cx
        jnz     load_startos_file

;SET DEFAULT Not use ini file 
error_ini:
        mov     si, error_ini_f1    ;Error: cannot load ini file, buffer is full
        dec     bx
        jz      err_show_ini
        mov     si, error_ini_f2    ;Error: ini file not found
        dec     bx
        jz      err_show_ini
        mov     si, error_ini_f3    ;Error: cannot read ini file
        dec     bx
        jz      err_show_ini

        mov     si, error_ini_nf    ;Error: unrecognized error when loading ini file
err_show_ini:
        call    printplain
        mov     si, error_ini_common
        call    printplain
; wait for keypress
        xor     ax, ax
        int     16h

ini_loaded:

        jmp     $

align 4
check_conf_file:
;Check config file in current dir
        push    ax      ;save size file
  if DEBUG
        mov     cx, 0x0a
        mov     di, show_decode
        call    decode
;Show size
        mov     si, show_string
        call    printplain
  end if


;Show message
        mov     si, load_ini
        call    printplain

        pop     cx      ;restore size file
use_parse               ;parsing startos.ini
;
        jmp     ini_loaded

;;;;;;;;;;;;;;;;;;;;;;;;;;;
;DATA
;;;;;;;;;;;;;;;;;;;;;;;;;;;
; table for move to extended memory (int 15h, ah=87h)
align 4
table_15_87:
        db      0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0
        db      0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0

        db      0xff,0xff
        db      0x0,0x10 
        db      0x00,0x93,0x0,0x0

        db      0xff,0xff,0x0,0x00,0x10,0x93,0x0,0x0

        db      0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0
        db      0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0
        db      0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0
        db      0x00,0x00,0x0,0x00,0x00,0x00,0x0,0x0

fat12_buffer:
.BS_jmpBoot             db 0x90,0x90,0x90       ;3 �����   NOP ���������� - ������ �� ������ 
.BS_OEMName             db 'K SyS 64'   ;8 ����
.BPB_BytsPerSec         dw      512     ;���-�� ������ � ������� ����� ���� ����� 512 1024 2048 4096 2 �����
.BPB_SecPerClus         db      0x1     ;���-�� �������� � ��������
.BPB_RsvdSecCnt         dw      0x1     ;��� FAt12/16 ������ 1, ��� FAT32 ������ 32     
.BPB_NumFATs            db      0x1     ;���-�� ��� ������, �� ��� ������ ���� ����� ����� �� ������� ������ ��� �����
.BPB_RootEntCnt         dw      512     ;��� ��� ������������� � fat16 
.BPB_TotSec16           dw      0x0     ;��-�� ��������
.BPB_Media              db      0xF0
.BPB_FATSz16            dw      0x0
.BPB_SecPerTrk          dw      0x0     ;�������� ��������� ����� ��� RAMFS �� ��� �� ��� �������, ���� ������ ����, ����� ������ �������� ��������.
.BPB_NumHeads           dw      0x0
.BPB_HiddSec            dd      0x0     ;���-�� ������� ��������
.BPB_TotSec32           dd      0x0
.BS_DrvNum              db      'R'     ;�� ����� RAM
.BS_Reserved1           db      0x0
.BS_BootSig             db      0x29
.BS_VolID               db      'RFKS'  
.BS_VolLab              db      'RAM DISK FS'   ;11 ��������
.BS_FilSysType          db      'FAT12   '      ;8 ��������
;62 ����� ��������� fat12.
db (512-($-fat12_buffer))dup(0x90)



;��������� ��� ����������� fat
struc FAT_32_entry ;Byte Directory Entry Structure
{
.DIR_Name       rb      11
.DIR_Attr       db      ?
.DIR_NTRes      db      ?
.DIR_CrtTimeTenth db    ?
.DIR_CrtTime    dw      ?
.DIR_CrtDate    dw      ?
.DIR_LstAccDate dw      ?
.DIR_FstClusHI  dw      ?
.DIR_WrtTime    dw      ?
.DIR_WrtDate    dw      ?
.DIR_FstClusLO  dw      ?
.DIR_FileSize   dd      ?


}
;��� ����� ������������� ������, ������� �������������� ����������� � �������� �������....
;;;
;timer
shot_name_fat   rb      11      ;��������� ����� ��� fat12, � ��� ��������� ����� ������ ����������� � �������� FAT /* ����������� ��������� � ����

if DEBUG
                rb      1       ;����� ��� ������� � ������ ����� ����� ����� ��������������
dest_name_fat   db      24 dup('_');12
db      0x0
end if

value_timeout   rw      1       ;value to timeout
old_timer       rd      1       ;������ �������� ������� �������
start_timer     rd      1       ;�������� �������
timer_          rd      1       ;����� �������� ������� ������� �.�. SL
start_stack     rw      1       ;save stack 
save_bp_from_timer rw   1       ;save bp from timer

