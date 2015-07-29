#ifndef INCLUDE_DLL_H
#define INCLUDE_DLL_H

#ifndef INCLUDE_KOLIBRI_H
#include "../lib/file_system.h"
#endif


char a_libdir[43]  = "/sys/lib/\0";

:inline void error_init(dword text)
{
	dword TEXT_ERROR = malloc(1024);
	#ifdef LANG_RUS
		strcpy(TEXT_ERROR,"'�訡�� �� ����㧪� ������⥪� `");
	#elif LANG_EST
		strcpy(TEXT_ERROR,"'Viga teegi laadimisel `");
	#else
		strcpy(TEXT_ERROR,"'Error while loading library `");
	#endif
	strcat(TEXT_ERROR,text);
	strcat(TEXT_ERROR,"`' -E");
	notify(TEXT_ERROR);
	free(TEXT_ERROR);
}

// stdcall with 1 parameter
void dll_Load() {
asm {
        push    ebp
        mov     ebp, esp
        mov     esi, SSDWORD[EBP+8]
		@next_lib:    
        mov     edx, DSDWORD[ESI]
        or      edx, edx
        jz      exit
        push    esi
        mov     esi, DSDWORD[ESI+4]
        mov     edi, #a_libdir+9

@loc01: 
        lodsb
        stosb
        or      al, al
        jnz     loc01

        mov     eax, 68
        mov     ebx, 19
        mov     ecx, #a_libdir
        int     0x40
        or      eax, eax
        jz      fail

        push    edx
        push    eax
        call    dll_Link

        push    eax
        mov     eax, DSDWORD[eax]
        cmp     DSDWORD[EAX], '_bil'    // somehow this needs to be reversed..
        pop     eax
        jnz     loc02

        push    DSDWORD[EAX+4]
        call    dll_Init
@loc02:

        pop     esi
        add     esi, 8
        jmp     next_lib
@exit:
	xor     eax, eax
        leave
        ret     4
        
@fail:        
        add     esp, 4
        xor     eax, eax
        inc     eax
        leave
        ret     4
    }
}

//stdcall with 2 parameters
void dll_Link() {
asm {
        push    ebp
        mov     ebp, esp
        push    eax
        mov     esi, SSDWORD[EBP+12]
        test    esi, esi
        jz      done
@next:        
        lodsd
        test    eax, eax
        jz      done
        push    eax
        push    SSDWORD[EBP+8]
        call    dll_GetProcAddress
        or      eax, eax
        jz      loc03
        mov     DSDWORD[esi-4], eax
        jmp     next
@loc03: 
        mov     SSDWORD[esp], 0
@done:
        pop     eax
        leave
        ret     8
    }
}


//stdcall with 1 parameter
void dll_Init() {
asm {
        push    ebp
        mov     ebp, esp
        pushad
        mov     eax, #mem_Alloc
        mov     ebx, #mem_Free;
        mov     ecx, #mem_ReAlloc;
        mov     edx, #dll_Load;
        call    SSDWORD[EBP+8]
        popad
        leave
        ret     4
    }
}


// stdcall with 2 parameters
void dll_GetProcAddress(){
asm {
        push    ebp
        mov     ebp, esp
        mov     edx, CSDWORD[EBP+8]
        xor     eax, eax

@next:        
        or      edx, edx
        jz      end
        cmp     CSDWORD[edx], 0
        jz      end

        push    CSDWORD[EBP+12]
        push    CSDWORD[EDX]
        call    dll_StrCmp
        test    eax, eax
        jz      ok
        add     edx, 8
        jmp     next
@ok:
        mov     eax, DSDWORD[EDX+4]
@end:
        leave
        ret     8
    }
}


// stdcall with 2 parameters
void dll_StrCmp() {
asm {
        push    ebp
        mov     ebp, esp
        push    esi
        push    edi
        mov     esi, CSDWORD[EBP+8]
        mov     edi, CSDWORD[EBP+12]
        xor     eax, eax
@label1:
        lodsb
        scasb
        jne     fail
        or      al, al
        jnz     label1
        jmp     label_ok
@fail:
        or      eax, -1
@label_ok:
        pop     edi
        pop     esi
        leave
        ret     8
    }
}

int load_dll2(dword dllname, import_table, byte need_init)
{
   //dword dllentry=0;
// load DLL
        $mov     eax, 68
        $mov     ebx, 19
        ECX=dllname;
        $int     0x40
        $test    eax, eax
        $jz      exit01

// initialize import
        $mov     edx,eax
        ESI=import_table;

@import_loop01:
        $lodsd
        $test    eax,eax
        $jz      import_done01
        $push    edx
@import_find01:
        $mov     ebx,DSDWORD[EDX]
        $test    ebx, ebx
        $jz      exit01
        $push    eax
@nex101:
        $mov     cl,DSBYTE[EAX];
        $cmp     cl,DSBYTE[EBX];
        $jnz     import_find_next01
        $test    cl,cl
        $jz      import_found01
        $inc     eax
        $inc     ebx
        $jmp     nex101
@import_find_next01:
        $pop     eax
        $add     edx, 8
        $jmp     import_find01
@import_found01:
        $pop     eax
        $mov     eax,DSDWORD[edx+4]
        $mov     DSDWORD[esi-4],eax
        $pop     edx
       
        $jmp     import_loop01
@import_done01:
        IF (need_init) dll_Init (DSDWORD[EDX+4]);
        return 0;
@exit01:
        return -1;
}

void load_dll(dword dllname, import_table, byte need_init)
{
	if (load_dll2(dllname, import_table, need_init)!=0) error_init(dllname);
}


#endif