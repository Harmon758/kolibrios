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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


include mdef.inc
include struct.inc

        modstart        i8fs386

        xref    __I8LS
        xref    __I8RS

; Convert signed 64-bit integer to single precision float
; Input: [EDX, EAX] = 64-bit integer
; Output: [EAX] = 32-bit float

        xdefp   __I8FS
        defp    __I8FS
        or      EDX,EDX         ; check sign
        jns     __U8FS          ; if positive, just convert
        neg     EDX             ;
        neg     EAX             ; take absolute value of number
        sbb     EDX,0           ;
        call    __U8FS          ; convert to FS
        or      EAX,80000000h   ; set sign bit on
        ret                     ; return
        endproc __I8FS

; Convert unsigned 64-bit integer to single precision float
; Input: [EDX, EAX] = 64-bit integer
; Output: [EAX] = 32-bit float

        xdefp   __U8FS
        defp    __U8FS
        or      EAX,EAX         ; if lo is zero
        _if     e               ;
        or      EDX,EDX         ; and if hi is zero
          _if   e               ;
            ret                 ; - return
          _endif
        _endif                  ; endif
        push    ECX             ; save ECX
        push    EBX             ; save EBX
        bsr     ECX,EDX         ; find most significant non-zero bit in hi
        _if     e               ; if all zero bits
          bsr   ECX,EAX         ; - find most significant non-zero bit in lo
        _else                   ; else
          add   CL,32           ; - adjust shift count since hi was not 0
        _endif                  ; endif
        push    ECX             ; save shift count
        neg     CL              ; calculate # of bits to shift by
        add     CL,63-8         ; ...
        mov     EBX,ECX         ; set up for call
        or      CL,CL           ; if mantissa should move left
        _if     ge              ; then
          call  __I8LS          ; - shift left
        _else                   ; else
          neg   BL              ; - make positive
          call  __I8RS          ; - shift right
        _endif                  ; endif
        pop     ECX             ; restore shift count
        and     EDX,007FFFFFh   ; mask out sign and exponent bits
        add     CL,127          ; calculate exponent (excess 127)
;       and     ECX,0FFh        ; isolate exponent (not required)
        shl     ECX,23          ; shift exponent into position
        or      EDX,ECX         ; place into result
        mov     EAX,EDX         ; single precision is only 32 bits
        pop     EBX             ; restore EBX
        pop     ECX             ; restore ECX
        ret                     ; return
        endproc __U8FS

        endmod
        end
