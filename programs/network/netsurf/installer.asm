;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                                                                 ;;
;; Copyright (C) KolibriOS team 2017. All rights reserved.         ;;
;; Distributed under terms of the GNU General Public License       ;;
;;                                                                 ;;
;;  netsurf-installer - Set up Netsurf Browser on KolibriOS        ;;
;;               Author: ashmew2.                                  ;;
;;                                                                 ;;
;;  Inspired from downloader.asm by hidnplayr@kolibrios.org        ;;
;;            GENERAL PUBLIC LICENSE                               ;;
;;             Version 2, June 1991                                ;;
;;                                                                 ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

URLMAXLEN       = 65535
FILENAMEMAXLEN  = 1024
__DEBUG_LEVEL__ = 1
__DEBUG__       = 0

format binary as ""
use32
        org     0x0
        db      'MENUET01'      ; header
        dd      0x01            ; header version
        dd      START           ; entry point
        dd      I_END          ; image size
        dd      I_END+0x1000    ; required memory
        dd      I_END+0x1000    ; esp
        dd      0x0             ; I_Path
        dd      0x0             ; I_Path

include '../../macros.inc'
include '../../proc32.inc'
include '../../dll.inc'
include '../../debug-fdo.inc'
include '../../develop/libraries/http/http.inc'

virtual at 0
        http_msg http_msg
end virtual

;; Parameters
;; HTTP URL to download
;; Target filename
proc get_file_over_http targeturl, targetfilename
    pusha
	xor eax, eax
    mov [write_to_file.current_offset], eax
    mov [write_to_file.bufsize], eax
    mov [write_to_file.bufptr], eax

    DEBUGF 1, "---- HTTP : Getting %s\n", [targeturl]
    invoke  HTTP_get, [targeturl], 0, 0, 0
    cmp     eax, 0
    je .http_error
	mov [httpstruct], eax

    ;; No HTTP errors, create a new file for the download.
	DEBUGF 1, "---- Creating new file : %s\n", [targetfilename]
	mcall 70, create_new_file
    cmp eax, 0
    jne .file_error

    .http_receive_loop:
		DEBUGF 1, "---- Receiving over http.\n"
     	DEBUGF 1, "---- Invoking http receive.\n"
        invoke HTTP_receive, [httpstruct]
        cmp eax, 0
        je .http_transfer_done

	    mov ebp, [httpstruct]
		DEBUGF 1, "---- http flags = 0x%x.\n", 	[ebp + http_msg.flags]
	    test [ebp + http_msg.flags], 0xffff0000
        jnz .http_error

	    mov ebp, [ebp + http_msg.content_received]
	    cmp ebp, [write_to_file.current_offset]
        jle .http_receive_loop
	    ;; Only proceed if we have more data in HTTP buffer than we have written to file.

        ;; Process data we got (write it to the file)
	    mov ebp, [httpstruct]
	    mov ecx, [ebp + http_msg.content_length]
        mov edx, [ebp + http_msg.content_received]

	    DEBUGF 1, "---- Current file write offset : %u (http got : %u / %u)\n", [write_to_file.current_offset], edx, ecx
        sub edx, [write_to_file.current_offset]
        mov [write_to_file.bufsize], edx

        mov ecx, [ebp + http_msg.content_ptr]
        add ecx, [write_to_file.current_offset]
        mov [write_to_file.bufptr], ecx

	    DEBUGF 1, "---- ecx + offset = 0x%x\n", ecx
	    DEBUGF 1, "---- Writing to file %u bytes at 0x%x to %s\n", [write_to_file.bufsize], [write_to_file.bufptr], current_filename
        mcall 70, write_to_file
        cmp eax, 0
        jne .file_error

	    DEBUGF 1, "---- Wrote to file %u bytes.\n", ebx
        add [write_to_file.current_offset], ebx
	    DEBUGF 1, "---- File offset updated to : %u\n", [write_to_file.current_offset]
    
        jmp .http_receive_loop

    .file_error:
	DEBUGF 1, "file_erroR with eax = %u!", eax
        mcall -1

    .http_error:
	DEBUGF 1, "http_erroR!"
        mcall -1

    .http_transfer_done:
	    ;; Write any remaining bytes from the http buffer into the file
     	DEBUGF 1, "---- http flags = 0x%x.\n", 	[httpstruct + http_msg.flags]
        DEBUGF 1, "Got %u bytes in total\n", [httpstruct + http_msg.content_length] 

	    mov ebp, [httpstruct]
        mov edx, [ebp + http_msg.content_length]

        sub edx, [write_to_file.current_offset]
        mov [write_to_file.bufsize], edx

        mov ecx, [ebp + http_msg.content_ptr]
        add ecx, [write_to_file.current_offset]
        mov [write_to_file.bufptr], ecx

	    DEBUGF 1, "---- Final ecx + offset = 0x%x\n", ecx
	    DEBUGF 1, "-- Writing to file %u bytes at 0x%x to %s\n", [write_to_file.bufsize], [write_to_file.bufptr], current_filename

        mcall 70, write_to_file
        cmp eax, 0
        jne .file_error

	    DEBUGF 1, "-- Wrote to file %u bytes.\n", ebx
        add [write_to_file.current_offset], ebx
	    DEBUGF 1, "-- File offset updated to : %u\n", [write_to_file.current_offset]
	    mov ebp, [httpstruct]
        mov edx, [ebp + http_msg.content_length]
        cmp [write_to_file.current_offset], edx
        jne .http_transfer_done

	invoke HTTP_free, [httpstruct]

	popa
    ret
endp

proc make_new_folder newfolder
	pusha

	mov eax, [newfolder]
    mov [create_new_folder.foldername], eax
	mcall 70, create_new_folder
    test eax, eax
    jz .success
    
	DEBUGF 1, "Failed to create folder: %s\n", [newfolder]
    mcall -1

.success:
    popa
    ret
endp

START:
    mcall   68, 11                  ; init heap so we can allocate memory dynamically

; load libraries
    stdcall dll.Load, @IMPORT
    test    eax, eax
    jnz     .all_files_done_error

	stdcall make_new_folder, dirname_res
	stdcall make_new_folder, dirname_res_pointers
	stdcall make_new_folder, dirname_res_throbber
	stdcall make_new_folder, dirname_res_icons

.get_next_file:	
    mov        edi, current_url
    mov        esi, url

    @@:
	    movsb
        cmp byte[esi], 0
        jne @b
	;;  Loaded the base URL into current URL

    ;; Move onto the subsequent file.
    mov esi, [filelistoffset]
    cmp byte[esi], 0
    je  .all_files_done

    @@:
	    movsb
        cmp byte[esi], 0
        jne @b
    movsb

	;; DEBUGF 1, "-- Current URL with filename is : %s\n", current_url

; Create name of file we will download to
	mov esi, download_file_path
    mov edi, current_filename
	
    @@:
        movsb
        cmp byte[esi], 0
        jne @b
	
    mov esi, [filelistoffset]
    @@:
        movsb
        cmp byte[esi], 0
        jne @b
    movsb
    mov [filelistoffset], esi
    
    ;; current_filename is now set to the name of the file
    ;; current_url is now set to the name of the file we will get after download
	stdcall get_file_over_http, current_url, current_filename
	jmp .get_next_file

.all_files_done:
	DEBUGF 1, "All FILES DONE!\n"
    ;; Inform user that all files are done
	;; print 'fuck off'

.all_files_done_error:
	DEBUGF 1, "FUCKED BIG TIME with eax = %u!\n", eax
    mcall -1

;---------------------------------------------------------------------
; Data area
;-----------------------------------------------------------------------------
align   4
@IMPORT:

library lib_http,       'http.obj'
import  lib_http, \
        HTTP_get,       'get', \
        HTTP_receive,   'receive', \
        HTTP_free,      'free'

include_debug_strings

download_file_path db '/tmp0/1/', 0
dirname_res db '/tmp0/1/res', 0
dirname_res_pointers db '/tmp0/1/res/pointers', 0
dirname_res_throbber db '/tmp0/1/res/throbber', 0
dirname_res_icons    db '/tmp0/1/res/icons', 0

url              db 'www.ashmew2.me/',0
filelist db 'res/adblock.css', 0
         db 'res/quirks.css', 0
         db 'res/Messages', 0
         db 'res/licence.html', 0
         db 'res/default.css', 0
         db 'res/netsurf.png', 0
         db 'res/sans.ttf', 0
         db 'res/welcome.html', 0
         db 'res/internal.css', 0
         db 'res/maps.html', 0
         db 'res/favicon.png', 0
         db 'res/credits.html', 0
         db 'res/throbber/throbber8.png', 0
         db 'res/throbber/throbber3.png', 0
         db 'res/throbber/throbber4.png', 0
         db 'res/throbber/throbber0.png', 0
         db 'res/throbber/throbber6.png', 0
         db 'res/throbber/throbber2.png', 0
         db 'res/throbber/throbber1.png', 0
         db 'res/throbber/throbber7.png', 0
         db 'res/throbber/throbber5.png', 0
         db 'res/pointers/point.png', 0
         db 'res/pointers/no_drop.png', 0
         db 'res/pointers/wait.png', 0
         db 'res/pointers/up-down.png', 0
         db 'res/pointers/help.png', 0
         db 'res/pointers/ru-ld.png', 0
         db 'res/pointers/menu.png', 0
         db 'res/pointers/not_allowed.png', 0
         db 'res/pointers/cross.png', 0
         db 'res/pointers/default.png', 0
         db 'res/pointers/caret.png', 0
         db 'res/pointers/left-right.png', 0
         db 'res/pointers/lu-rd.png', 0
         db 'res/pointers/progress.png', 0
         db 'res/pointers/move.png', 0
         db 'res/icons/back.png', 0
         db 'res/icons/back_g.png', 0
         db 'res/icons/scrollr.png', 0
         db 'res/icons/osk.png', 0
         db 'res/icons/forward_g.png', 0
         db 'res/icons/scrolll.png', 0
         db 'res/icons/history.png', 0
         db 'res/icons/forward.png', 0
         db 'res/icons/home_g.png', 0
         db 'res/icons/history_g.png', 0
         db 'res/icons/reload_g.png', 0
         db 'res/icons/scrollu.png', 0
         db 'res/icons/stop.png', 0
         db 'res/icons/scrolld.png', 0
         db 'res/icons/stop_g.png', 0
         db 'res/icons/home.png', 0
         db 'res/icons/reload.png', 0
         db 'nskolibrios', 0
         db 0

current_url      rb URLMAXLEN
filelistoffset   dd filelist
current_filename rb FILENAMEMAXLEN
httpstruct       dd 0

create_new_file	dd 2, 0, 0, 0, 0
    db 0
    dd current_filename

create_new_folder dd 9, 0, 0, 0, 0
                  db 0
 .foldername dd 0

write_to_file    dd 3
 .current_offset dd 0, 0
 .bufsize        dd 0
 .bufptr         dd 0
                 db 0
                 dd current_filename
	
I_END:
