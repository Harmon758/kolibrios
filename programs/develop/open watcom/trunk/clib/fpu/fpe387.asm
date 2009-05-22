;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  Connect/disconnect 80x87 interrupt handler.
;*
;*****************************************************************************


.8087
.386p

include struct.inc
include mdef.inc

        xref            __FPE2Handler_
         
        modstart        fpe387

        datasegment
        
Save87  dd      0
OldMask dd      0
        enddata
 

        xdefp   "C",__Init_FPE_handler
defp    __Init_FPE_handler
        _guess                          ; guess initialization required
          cmp   dword ptr Save87,0      ; - quit if already initialized
          _quif ne                      ; - ...
        _admit                          ; admit: already initialized
           ret                          ; - return
        _endguess                       ; endguess
        push    EAX                     ; save registers
        push    EBX                     ; ...
        push    ECX                     ; ...
        push    EDX                     ; ...

        mov     EAX, 68
        mov     EBX, 15
        mov     EDX, 10000h             ; 1 shl 16 - #MF
        lea     ECX, __FPE2Handler_
        int     40h                     ; set new exception handler
        mov     Save87, EAX             ; save old handler
        mov     OldMask,EBX             ; save old mask
        
        pop     EDX                     ; ...
        pop     ECX                     ; ...
        pop     EBX                     ; ...
        pop     EAX                     ; ...
        ret                             ; return
endproc __Init_FPE_handler


        xdefp   "C",__Fini_FPE_handler
defp    __Fini_FPE_handler
        cmp     dword ptr Save87,0      ; if not initialized
        _if     e                       ; - then
          ret                           ; - return
        _endif                          ; endif
        push    EAX                     ; save registers
        push    EBX                     ; ...
                                        ; ECX ????
        push    EDX                     ; ...
        sub     ESP,4                   ; allocate space for control word
        fstcw   word ptr [ESP]          ; get control word
        fwait                           ; ...
        mov     byte ptr [ESP],7Fh      ; disable exception interrupts
        fldcw   word ptr [ESP]          ; ...
        fwait                           ; ...
        add     ESP,4                   ; remove temporary
        
        mov     EAX, 68
        mov     EBX, 15
        mov     ECX, dword ptr Save87   ; restore handler
        mov     EDX, dword ptr OldMask  ; restore mask
        int     40h                     ; set new ecxeption handler

        pop     EDX                     ; ...
                                        ; ECX ????
        pop     EBX                     ; ...
        pop     EAX                     ; ...
        ret
endproc __Fini_FPE_handler

        endmod
        end
