//------------------------------------------------------------------------------
// strcmp( ESI, EDI)
// strlen( EDI)
// strcpy( EDI, ESI) --- 0 if ==
// strcat( EDI, ESI)
// strchr( ESI,BL)
// strrchr( ESI,BL)
// strstr( EBX, EDX)
// itoa( ESI)
// atoi( EAX)
// strupr( ESI)
// strlwr( ESI) --- kyrillic symbols may not work
// strttl( EDX)
// strtok( ESI)
// strcpyb(dword searchin, copyin, startstr, endstr) --- copy string between strings
//chrnum(dword searchin, char symbol) --- count of symbol in string
//------------------------------------------------------------------------------

inline fastcall signed int strcmp( ESI, EDI)
{
	loop()
	{
		IF (DSBYTE[ESI]<DSBYTE[EDI]) RETURN -1;
		IF (DSBYTE[ESI]>DSBYTE[EDI]) RETURN 1;
		IF (DSBYTE[ESI]=='\0') RETURN 0;
		ESI++;
		EDI++;
	}
}


inline fastcall signed int strncmp( ESI, EDI, ECX)
{
  asm {
    MOV EBX, EDI
    XOR EAX, EAX
    MOV EDX, ECX
    OR ECX, ECX
    JE L1
    REPNE SCASB
    SUB EDX, ECX
    MOV ECX, EDX
    MOV EDI, EBX
    XOR EBX, EBX
    REPE CMPSB
    MOV AL, DSBYTE[ ESI-1]
    MOV BL, DSBYTE[ EDI-1]
    SUB EAX, EBX
L1:
  }
}


inline fastcall unsigned int strlen( EDI)
{
	$xor eax, eax
	$mov ecx, -1
	$REPNE $SCASB
	EAX-=2+ECX;
}


inline fastcall strcpy( EDI, ESI)
{
	$cld
L2:
	$lodsb
	$stosb
	$test al,al
	$jnz L2
}


inline fastcall strcat( EDI, ESI)
{
  asm {
    mov ebx, edi
    xor ecx, ecx
    xor eax, eax
    dec ecx
    repne scasb
    dec edi
    mov edx, edi
    mov edi, esi
    xor ecx, ecx
    xor eax, eax
    dec ecx
    repne scasb
    xor ecx, 0ffffffffh
    mov edi, edx
    mov edx, ecx
    mov eax, edi
    shr ecx, 2
    rep movsd
    mov ecx, edx
    and ecx, 3
    rep movsb
    mov eax, ebx
	}
}

/*void strcat(char *to, char *from) //���� ��������
{
	while(*to) to++;
	while(*from)
	{
		*to = *from;
		to++;
		from++;
	}
	*to = '\0';
}*/

dword itoa( ESI)
{
	unsigned char buffer[11];
	$pusha

	EDI = #buffer;
	ECX = 10;
	if (ESI < 0)
	{
		 $mov     al, '-'
		 $stosb
		 $neg     esi
	}

	$mov     eax, esi
	$push    -'0'
F2:
	$xor     edx, edx
	$div     ecx
	$push    edx
	$test    eax, eax
	$jnz     F2
F3:
	$pop     eax
	$add     al, '0'
	$stosb
	$jnz     F3
	
	$mov     al, '\0'
	$stosb
	
	$popa 
    return #buffer;
} 



inline fastcall dword atoi( EDI)
{
	ESI=EDI;
	IF(DSBYTE[ESI]=='-')ESI++;
	EAX=0;
	BH=AL;
	do{
		BL=DSBYTE[ESI]-'0';
		EAX=EAX*10+EBX;
		ESI++;
	}while(DSBYTE[ESI]>='0');
	IF(DSBYTE[EDI]=='-') -EAX;
}


inline fastcall signed int strchr( ESI,BL)
{
	int jj=0;
	do{
		jj++;
		$lodsb
		IF(AL==BL) return jj;
	} while(AL!=0);
	return 0;
}


inline fastcall signed int strrchr( ESI,BL)
{
	int jj=0, last=0;
	do{
		jj++;
		$lodsb
		IF(AL==BL) last=jj;
	} while(AL!=0);
	return last;
}


inline fastcall strupr( ESI)
{
	do{
		AL=DSBYTE[ESI];
		IF(AL>='a')IF(AL<='z')DSBYTE[ESI]=AL&0x5f;
		IF (AL>=160) && (AL<=175) DSBYTE[ESI] = AL - 32;	//�-�
		IF (AL>=224) && (AL<=239) DSBYTE[ESI] = AL - 80;	//�-�
 		ESI++;
	}while(AL!=0);
}

inline fastcall strlwr( ESI)
{
	do{
		$LODSB
		IF(AL>='A')&&(AL<='Z'){
			AL+=0x20;
			DSBYTE[ESI-1]=AL;
			CONTINUE;
		}
	}while(AL!=0);
}

inline fastcall strttl( EDX)
{
	AL=DSBYTE[EDX];
	IF(AL>='a')&&(AL<='z')DSBYTE[EDX]=AL&0x5f;
	IF (AL>=160) && (AL<=175) DSBYTE[EDX] = AL - 32;	//�-�
	IF (AL>=224) && (AL<=239) DSBYTE[EDX] = AL - 80;	//�-�
	do{
		EDX++;
		AL=DSBYTE[EDX];
		IF(AL>='A')&&(AL<='Z'){DSBYTE[EDX]=AL|0x20; CONTINUE;}
		IF(AL>='�')&&(AL<='�')DSBYTE[EDX]=AL|0x20; //�-�
		IF (AL>=144) && (AL<=159) DSBYTE[EDX] = AL + 80;	//�-�
	}while(AL!=0);
}



inline fastcall dword strstr( EBX, EDX)
{
  asm {
    MOV EDI, EDX
    XOR ECX, ECX
    XOR EAX, EAX
    DEC ECX
    REPNE SCASB
    NOT ECX
    DEC ECX
    JE LS2
    MOV ESI, ECX
    XOR ECX, ECX
    MOV EDI, EBX
    DEC ECX
    REPNE SCASB
    NOT ECX
    SUB ECX, ESI
    JBE LS2
    MOV EDI, EBX
    LEA EBX, DSDWORD[ ESI-1]
LS1: MOV ESI, EDX
    LODSB
    REPNE SCASB
    JNE LS2
    MOV EAX, ECX
    PUSH EDI
    MOV ECX, EBX
    REPE CMPSB
    POP EDI
    MOV ECX, EAX
    JNE LS1
    LEA EAX, DSDWORD[ EDI-1]
    JMP SHORT LS3
LS2: XOR EAX, EAX
LS3:
  }
}

//
void strcpyb(dword searchin, copyin, startstr, endstr)
{
	dword startp, endp;
	startp = strstr(searchin, startstr) + strlen(startstr);
	endp = strstr(startp, endstr);
	if (startp==endp) return;
	do
	{ 
		DSBYTE[copyin] = DSBYTE[startp];
		copyin++;
		startp++;
	}
	while (startp<endp);
	DSBYTE[copyin] = '\0';
}

int chrnum(dword searchin, char symbol)
{
	int num = 0;
	while(DSBYTE[searchin])
	{ 
		if (DSBYTE[searchin] == symbol)	num++;
		searchin++;
	}
	return num;
}




/* strtok( LPSTR dest, src, divs);
src - ��������� �� �������� ������ ��� ��������� ����������� ������
dest - ��������� �� �����, ���� ����� ����������� �����
divs - ��������� �� ������, ���������� �������-�����������
����������: 0, ���� ���� ������ ���
         �� 0, ���� ����� ����������� � dest (��������� ��� ��������
               � �������� src ��� ������������ ������) */

inline fastcall dword strtok( EDX, ESI, EBX)
{
  asm {
    XOR ECX, ECX
    MOV EDI, EBX
    XOR EAX, EAX
    DEC ECX
    REPNE SCASB
    XOR ECX, 0FFFFFFFFH
    DEC ECX
    PUSH ECX
L1: LODSB
    OR AL, AL
    JZ L4
    MOV EDI, EBX
    MOV ECX, SSDWORD[ ESP]
    REPNE SCASB
    JZ L1
    DEC ESI
L2: LODSB
    MOV EDI, EBX
    MOV ECX, SSDWORD[ ESP]
    REPNE SCASB
    JZ L3
    MOV DSBYTE[ EDX], AL
    INC EDX
    JMP SHORT L2
L3: MOV EAX, ESI
L4: POP ECX
  } DSBYTE[ EDX] = 0;
}

#define strncpy strcpyn
#define strnmov strmovn
#define stricmp strcmpi
#define strcmpn strncmp
#define strncmpi strcmpni
#define stricmpn strcmpni
#define strnicmp strcmpni
#define strincmp strcmpni
#define strcmpin strcmpni
