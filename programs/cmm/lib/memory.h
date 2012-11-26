#code32 TRUE

inline fastcall dword malloc(dword ECX){
	dword	size, heap;
	#speed
	size = ECX;
	EAX = 68;
	EBX = 11;
	$int 0x40;
	heap = EAX;
	IF (size<=heap)
	{
		EAX = 68;
		EBX = 12;
		ECX = size;
		$int 0x40;
	}
	ELSE EAX=-1;
	#codesize
}

inline fastcall dword free(dword ECX){
	#speed
	EAX = 68;
	EBX = 13;
	$int 0x40;
	#codesize
}

inline fastcall dword realloc(dword ECX, EDX){
	#speed
	EAX = 68;
	EBX = 20;
	$int 0x40;
	#codesize
}

inline fastcall memmov( EDI, ESI, ECX)
{
  asm {
    MOV EAX, ECX
    CMP EDI, ESI
    JG L1
    JE L2
    SAR ECX, 2
    JS L2
    REP MOVSD
    MOV ECX, EAX
    AND ECX, 3
    REP MOVSB
    JMP SHORT L2
L1: LEA ESI, DSDWORD[ ESI+ECX-4]
    LEA EDI, DSDWORD[ EDI+ECX-4]
    SAR ECX, 2
    JS L2
    STD
    REP MOVSD
    MOV ECX, EAX
    AND ECX, 3
    ADD ESI, 3
    ADD EDI, 3
    REP MOVSB
    CLD
L2:
  }
}
