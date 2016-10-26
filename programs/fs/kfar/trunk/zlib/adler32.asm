; adler32.asm -- compute the Adler-32 checksum of a data stream
; Copyright (C) 1995-2011 Mark Adler
; For conditions of distribution and use, see copyright notice in zlib.h


BASE equ 65521 ;largest prime smaller than 65536
NMAX equ 5552
; NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1

macro DO1 buf,i
{
	mov eax,buf
	add eax,i
	movzx eax,byte[eax]
	add [adler],eax
	mov eax,[adler]
	add [sum2],eax
}
macro DO2 buf,i
{
	DO1 buf,i
	DO1 buf,i+1
}
macro DO4 buf,i
{
	DO2 buf,i
	DO2 buf,i+2
}
macro DO8 buf,i
{
	DO4 buf,i
	DO4 buf,i+4
}
macro DO16 buf
{
	DO8 buf,0
	DO8 buf,8
}

; use NO_DIVIDE if your processor does not do division in hardware --
; try it both ways to see which is faster
; note that this assumes BASE is 65521, where 65536 % 65521 == 15
; (thank you to John Reiser for pointing this out)
macro CHOP a
{
if NO_DIVIDE eq 1
	mov eax,a
	shr eax,16
	and a,0xffff
	shl eax,4
	add a,eax
	shr eax,4
	sub a,eax
end if
}
macro MOD28 a
{
if NO_DIVIDE eq 1
local .end0
	CHOP a
	cmp a,BASE
	jl .end0 ;if (..>=..)
		sub a,BASE
	.end0:
else
push eax ecx edx
	mov eax,a
	xor edx,edx
	mov ecx,BASE
	div ecx
	mov a,edx
pop edx ecx eax
end if
}
macro MOD a
{
if NO_DIVIDE eq 1
	CHOP a
	MOD28 a
else
push eax ecx edx
	mov eax,a
	xor edx,edx
	mov ecx,BASE
	div ecx
	mov a,edx
pop edx ecx eax
end if
}
macro MOD63 a
{
if NO_DIVIDE eq 1
;this assumes a is not negative
;        z_off64_t tmp = a >> 32;
;        a &= 0xffffffff;
;        a += (tmp << 8) - (tmp << 5) + tmp;
;        tmp = a >> 16;
;        a &= 0xffff;
;        a += (tmp << 4) - tmp;
;        tmp = a >> 16;
;        a &= 0xffff;
;        a += (tmp << 4) - tmp;
;        if (a >= BASE) a -= BASE;
else
push eax ecx edx
	mov eax,a
	xor edx,edx
	mov ecx,BASE
	div ecx
	mov a,edx
pop edx ecx eax
end if
}

; =========================================================================
;uLong (adler, buf, len)
;    uLong adler
;    const Bytef *buf
;    uInt len
align 4
proc adler32 uses ebx edx, adler:dword, buf:dword, len:dword
locals
	sum2 dd ? ;uLong
endl
;zlib_debug 'adler32 adler = %d',[adler]
	; split Adler-32 into component sums
	mov eax,[adler]
	shr eax,16
	mov [sum2],eax
	and [adler],0xffff
	mov ebx,[buf]

	; in case user likes doing a byte at a time, keep it fast
	cmp dword[len],1
	jne .end0 ;if (..==..)
		movzx eax,byte[ebx]
		add [adler],eax
		cmp dword[adler],BASE
		jl @f ;if (..>=..)
			sub dword[adler],BASE
		@@:
		mov eax,[adler]
		add [sum2],eax
		cmp dword[sum2],BASE
		jl @f ;if (..>=..)
			sub dword[sum2],BASE
		@@:
		jmp .combine
align 4
	.end0:

	; initial Adler-32 value (deferred check for len == 1 speed)
	cmp ebx,Z_NULL
	jne @f ;if (..==0)
		xor eax,eax
		inc eax
		jmp .end_f
align 4
	@@:

	; in case short lengths are provided, keep it somewhat fast
	cmp dword[len],16
	jge .end1 ;if (..<..)
		.cycle0:
			cmp dword[len],0
			jne @f ;while (..)
			movzx eax,byte[ebx]
			inc ebx
			add [adler],eax
			mov eax,[adler]
			add [sum2],eax
			dec dword[len]
			jmp .cycle0
align 4
		@@:
		cmp dword[adler],BASE
		jl @f ;if (..>=..)
			sub dword[adler],BASE
		@@:
		MOD28 dword[sum2] ;only added so many BASE's
		jmp .combine
align 4
	.end1:

	; do length NMAX blocks -- requires just one modulo operation
	.cycle3:
	cmp dword[len],NMAX
	jl .cycle3end ;while (..>=..)
		sub dword[len],NMAX
		mov edx,NMAX/16 ;NMAX is divisible by 16
		.cycle1: ;do
			DO16 ebx ;16 sums unrolled
			add ebx,16
			dec edx
			cmp edx,0
			jg .cycle1 ;while (..)
		MOD [adler]
		MOD [sum2]
		jmp .cycle3
align 4
	.cycle3end:

	; do remaining bytes (less than NMAX, still just one modulo)
	cmp dword[len],0
	jne .end2 ;if (..) ;avoid modulos if none remaining
		@@:
		cmp dword[len],16
		jl .cycle2 ;while (..>=..)
			sub dword[len],16
			DO16 ebx
			add ebx,16
			jmp @b
align 4
		.cycle2:
			cmp dword[len],0
			jne @f ;while (..)
			movzx eax,byte[ebx]
			inc ebx
			add [adler],eax
			mov eax,[adler]
			add [sum2],eax
			dec dword[len]
			jmp .cycle2
align 4
		@@:
		MOD [adler]
		MOD [sum2]
	.end2:

	; return recombined sums
.combine:
	mov eax,[sum2]
	shl eax,16
	or eax,[adler]
.end_f:
;zlib_debug '  adler32.ret = %d',eax
	ret
endp

; =========================================================================
;uLong (adler1, adler2, len2)
;    uLong adler1
;    uLong adler2
;    z_off64_t len2
align 4
proc adler32_combine_, adler1:dword, adler2:dword, len2:dword
locals
	sum1 dd ? ;uLong
	sum2 dd ? ;uLong
;    unsigned rem;
endl
	; for negative len, return invalid adler32 as a clue for debugging
	cmp dword[len2],0
	jge @f ;if (..<0)
		mov eax,0xffffffff
		jmp .end_f
	@@:

	; the derivation of this formula is left as an exercise for the reader
;    MOD63(len2) ;assumes len2 >= 0
;    rem = (unsigned)len2;
;    sum1 = adler1 & 0xffff;
;    sum2 = rem * sum1;
;    MOD(sum2);
;    sum1 += (adler2 & 0xffff) + BASE - 1;
;    sum2 += ((adler1 >> 16) & 0xffff) + ((adler2 >> 16) & 0xffff) + BASE - rem;
	cmp dword[sum1],BASE
	jl @f ;if (..>=..)
		sub dword[sum1],BASE
	@@:
	cmp dword[sum1],BASE
	jl @f ;if (..>=..)
		sub dword[sum1],BASE
	@@:
	cmp dword[sum2],BASE shl 1
	jl @f ;if (..>=..)
		sub dword[sum2],BASE shl 1
	@@:
	cmp dword[sum2],BASE
	jl @f ;if (..>=..)
		sub dword[sum2],BASE
	@@:
	mov eax,[sum2]
	shl eax,16
	or eax,[sum1]
.end_f:
	ret
endp

; =========================================================================
;uLong (adler1, adler2, len2)
;    uLong adler1
;    uLong adler2
;    z_off_t len2
align 4
proc adler32_combine, adler1:dword, adler2:dword, len2:dword
	stdcall adler32_combine_, [adler1], [adler2], [len2]
	ret
endp

;uLong (adler1, adler2, len2)
;    uLong adler1
;    uLong adler2
;    z_off64_t len2
align 4
proc adler32_combine64, adler1:dword, adler2:dword, len2:dword
	stdcall adler32_combine_, [adler1], [adler2], [len2]
	ret
endp
