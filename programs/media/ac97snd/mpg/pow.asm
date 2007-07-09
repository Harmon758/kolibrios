;   ix87 specific implementation of pow function.
;   Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.
;   This file is part of the GNU C Library.
;   Contributed by Ulrich Drepper <drepper@cygnus.com>, 1996.

;   The GNU C Library is free software; you can redistribute it and/or
;   modify it under the terms of the GNU Library General Public License as
;   published by the Free Software Foundation; either version 2 of the
;   License, or (at your option) any later version.

;   The GNU C Library is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;   Library General Public License for more details.

;   You should have received a copy of the GNU Library General Public
;   License along with the GNU C Library; see the file COPYING.LIB.  If not,
;   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
;   Boston, MA 02111-1307, USA.  */

format MS COFF

include 'proc32.inc'

section '.text' code readable executable

public _scalbn

align 4
proc _scalbn
	          fild	dword [esp+12]
	          fld	qword [esp+4]
	          fscale
	          fstp	st1
	          ret
endp

