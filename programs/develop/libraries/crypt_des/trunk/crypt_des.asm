;
; ���������� ��� ���������� �� ��������� DES
;

format MS COFF
public EXPORTS
section '.flat' code readable align 16

include '../../../../macros.inc'
include '../../../../proc32.inc'

;description:
; ������� ���������� ������
;input:
; key - ������� ���� 64 ���
; mem_keys - ������ ��� ������������ ������ (������ 120 ����)
; block - ��������� �� ������ ��������� 8 ������� ����
; b_count - ����� ������ ��� ����������
align 4
proc des_encryption, key:dword, mem_keys:dword, block:dword, b_count:dword
locals
    n dd ? ;���������� �����
endl
pushad
    stdcall encryption_key, [key],[mem_keys]

    ;��������� ������������ ip
    mov ecx,[b_count]
	cmp ecx,1
	jl .end_funct
    mov edi,[block]
    cld
    @@:
	mov esi,[mem_keys]
	add esi,112
	stdcall fun_convert_bits, edi, esi, oper_ip, 64
	movsd
	movsd
	loop @b

    mov ecx,[b_count]
    mov esi,[block]
    ;cld
	.cycle_3:
	mov dword[n],0
	.funct_f:
	mov edi,[mem_keys]
	add edi,112
	;������������ ���� R0(32) �� ������� e
	stdcall fun_convert_bits, esi, edi, oper_e, 48
    
	;edi - R0~e (48 ���)
	;esi - L0R0 (64 ����)
	stdcall fun_r0xorki_si, edi,[mem_keys],[n]

	;edi - (R0~e xor ki)~Si (32 ����)
	;������������ p
	mov eax,edi
	add eax,4
	stdcall fun_convert_bits, edi, eax, oper_p, 32

	;����� R0 � L0, � (L0 xor (R0~e xor ki)~Si) � R0
	mov edx,dword[esi+4]
	mov ebx,dword[esi]
	xor edx,dword[eax]
	mov dword[esi],edx
	mov dword[esi+4],ebx
    
	inc dword[n]
	cmp dword[n],16
	jl .funct_f

	add esi,8 ;������� �� ��������� 64 ������ ��������� ����
	loop .cycle_3

    ;�������� ������������ ip_inv
    mov ecx,[b_count]
    mov edi,[block]
    ;cld
    @@:
	mov esi,[mem_keys]
	add esi,112
	stdcall fun_convert_bits, edi, esi, oper_ip_inv, 64
	movsd
	movsd
	loop @b

	.end_funct:
popad
    ret
endp

;description:
; ������� ������������� ������
;input:
; key - ������� ���� 64 ���
; mem_keys - ������ ��� ������������ ������ (������ 120 ����)
; block - ��������� �� ������ ���������������� 8 ������� ����
; b_count - ����� ������ ��� �������������
align 4
proc des_decryption, key:dword, mem_keys:dword, block:dword, b_count:dword
locals
    n dd ? ;���������� �����
endl
pushad
    stdcall encryption_key, [key],[mem_keys]

    ;��������� ������������ ip
    mov ecx,[b_count]
	cmp ecx,1
	jl .end_funct
    mov edi,[block]
    cld
    @@:
	mov esi,[mem_keys]
	add esi,112
	stdcall fun_convert_bits, edi, esi, oper_ip, 64
	movsd
	movsd
	loop @b

    mov ecx,[b_count]
    mov esi,[block]
	;cld
    .cycle_3:
	mov dword[n],16
	.funct_f:
	dec dword[n]
	mov edi,[mem_keys]
	add edi,112
	;������������ ���� L0(32) �� ������� e
	add esi,4
	stdcall fun_convert_bits, esi, edi, oper_e, 48
	sub esi,4
    
	;edi - L0~e (48 ���)
	;esi - L0R0 (64 ����)
	stdcall fun_r0xorki_si, edi,[mem_keys],[n]

	;edi - (L0~e xor ki)~Si (32 ����)
	;������������ p
	mov eax,edi
	add eax,4
	stdcall fun_convert_bits, edi, eax, oper_p, 32

	;����� L0 � R0, � (R0 xor (L0~e xor ki)~Si) � L0
	mov edx,dword[esi]
	mov ebx,dword[esi+4]
	xor edx,dword[eax]
	mov dword[esi+4],edx
	mov dword[esi],ebx
	
	cmp dword[n],0
	jg .funct_f

	add esi,8 ;������� �� ��������� 64 ������ ��������� ����
	loop .cycle_3

    ;�������� ������������ ip_inv
    mov ecx,[b_count]
    mov edi,[block]
    ;cld
    @@:
	mov esi,[mem_keys]
	add esi,112
	stdcall fun_convert_bits, edi, esi, oper_ip_inv, 64
	movsd
	movsd
	loop @b

	.end_funct:
popad
    ret
endp

;description:
; ������������ ����� �� ��������� �������
;input:
; m_inp - ������ � ��������� �������
; m_out - ������ ��� ��������������� ������
; c_tbl - ������� ��� ������������
; tbl_s - ������ ������� ��� ������������
align 4
proc fun_convert_bits, m_inp:dword, m_out:dword, c_tbl:dword, tbl_s:dword
pushad
    mov esi,[m_inp]
    mov edi,[m_out]
    mov eax,[c_tbl]
    mov ecx,[tbl_s]
    xor edx,edx
    mov dword[edi],edx
    cmp ecx,32
    jle @f
	mov dword[edi+4],edx
    @@:
    inc edx

    cld
    .cycle_0:
	movzx ebx,byte[eax]
	btr ebx,5
	jc .tbl_h

	bt dword[esi],ebx
	jnc @f
	    or dword[edi],edx
	@@:
	jmp @f
	.tbl_h:
	bt dword[esi+4],ebx
	jnc @f
	    or dword[edi],edx
	@@:

	rol edx,1
	jnc @f
	    add edi,4
	@@:
	inc eax
    loop .cycle_0
popad
    ret
endp

;description:
; �������� R0(48) xor Ki(48) � 8 �������������� �� Si
;intput:
; data_r0 - R0 (48 ���)
; data_key - ��������� �� ������ ������ (�� 48 ���)
; n - ����� ����� � �������
;output:
; data_r0 - (R0 xor Ki)~Si (32 ����)
align 4
proc fun_r0xorki_si, data_r0:dword, data_key:dword, n:dword
pushad
    mov edi,[data_r0]
    mov eax,[n]
    imul eax,7 ;56 ��� - ���������� ����� ������� (���� ����� 48 ���)
    add eax,[data_key]

    mov ebx,dword[eax]
    xor dword[edi],ebx
    mov ebx,dword[eax+4] ;mov bx,word[eax+4]
    xor dword[edi+4],ebx ;xor word[edi+4],bx

    ;edi - ��������� �� �������� R0 xor Ki
    mov eax,dword[edi]
    mov ebx,dword[edi+4]
    ;bx:eax - 48 ��� �������� R0 xor Ki
    lea esi,[oper_s] ;�������� ������� S0
    mov ecx,8
    .cycle_0:
	mov dl,al
	and edx,31
	shr dl,1
	jnc @f
	    bts edx,4
	@@:
	bt eax,5
	jnc @f
	    bts edx,5
	@@:
	add edx,esi
	mov dl,byte[edx] ;dl - 4 ������ �������� ���������� �� Si

	bt ecx,0
	jc @f
	    ;1) ecx - ������
	    mov byte[edi],dl
	    jmp .next
	@@:
	    ;2) ecx - �� ������
	    shl dl,4
	    or byte[edi],dl
	    inc edi
	.next:

	shr eax,6
	ror ebx,6
	mov edx,ebx
	and edx,0xfc000000
	or eax,edx
	;��� 6 ���: shr ebx,1 rcr eax,1
	add esi,64 ;��������� �� ��������� ������� Si
	loop .cycle_0
popad
    ret
endp

;description:
; ��������� ������������ ������ ��� ����������
;intput:
; key - ������� ���� 64 ���
; key_b - 16 ��������������� ������ �� 48 ���
;         ������ ������ key_b ������ ���� �� ������ 120 ����
align 4
proc encryption_key, key:dword, key_b:dword
pushad
    mov esi,[key]
    mov edi,[key_b]

    ;������������ ������� ���� �� �������
    stdcall fun_convert_bits, esi, edi, oper_b, 56

    ;� ebx ������� C0
    mov ebx,dword[edi]
    and ebx,0xfffffff
    ;� edx ������� D0
    mov edx,dword[edi+3]
    shr edx,4

    ;������� 16 ���������, ������� �� 56 ���
    ;;;mov edi,[key_b]
    lea esi,[oper_r]
    .cycle_1:
    mov cl,byte[esi]
    cmp cl,0
    je .cycle_end
    shl ebx,cl
    btr ebx,28
    jnc @f
	or ebx,1
    @@:
    btr ebx,29
    jnc @f
	or ebx,2
    @@:
    shl edx,cl
    btr edx,28
    jnc @f
	or edx,1
    @@:
    btr edx,29
    jnc @f
	or edx,2
    @@:
    
    mov dword[edi+3],0
    mov dword[edi],ebx
    shl edx,4
    or dword[edi+3],edx
    add edi,7
    shr edx,4
    inc esi

    jmp .cycle_1
    .cycle_end:

    ;������ ������ � 56 �� 48 ���
    mov edi,[key_b]
    mov esi,edi
    add esi,112 ;112 - ����� �� ��������� 16*7
    
    mov ecx,16
    cld
    @@:
	stdcall fun_convert_bits, edi, esi, oper_c, 48
	movsd
	movsw
	mov byte[edi],0 ;�������� 7-� ����
	inc edi
	sub esi,6
	loop @b
	
popad
    ret
endp

align 4
oper_ip db 57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,\
61,53,45,37,29,21,13, 5,63,55,47,39,31,23,15, 7,\
56,48,40,32,24,16, 8, 0,58,50,42,34,26,18,10, 2,\
60,52,44,36,28,20,12, 4,62,54,46,38,30,22,14, 6

align 4
oper_ip_inv db 39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,\
37, 5,45,13,53,21,61,29,36, 4,44,12,52,20,60,28,\
35, 3,43,11,51,19,59,27,34, 2,42,10,50,18,58,26,\
33, 1,41, 9,49,17,57,25,32, 0,40, 8,48,16,56,24

align 4
oper_e db 31,0,1,2,3,4,\
 3, 4, 5, 6, 7, 8,\
 7, 8, 9,10,11,12,\
11,12,13,14,15,16,\
15,16,17,18,19,20,\
19,20,21,22,23,24,\
23,24,25,26,27,28,\
27,28,29,30,31, 0

align 4
oper_b db 56,48,40,32,24,16,8,\
 0,57,49,41,33,25,17,\
 9, 1,58,50,42,34,26,\
18,10, 2,59,51,43,35,\
62,54,46,38,30,22,14,\
 6,61,53,45,37,29,21,\
13, 5,60,52,44,36,28,\
20,12, 4,27,19,11, 3

oper_r db 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1,0

align 4
oper_c db 13,16,10,23,0,4,\
 2,27,14, 5,20, 9,\
22,18,11, 3,25, 7,\
15, 6,26,19,12, 1,\
40,51,30,36,46,54,\
29,39,50,44,32,47,\
43,48,38,55,33,52,\
45,41,49,35,28,31

align 4
oper_s db 14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,\
 0,15, 7, 4,14, 2,13,1,10,6,12,11,9,5,3,8,\
 4, 1,14, 8,13, 6,2,11,15,12,9,7,3,10,5,0,\
15,12, 8, 2, 4, 9,1,7,5,11,3,14,10,0,6,13,\
15, 1, 8,14, 6,11,3,4,9,7,2,13,12,0,5,10,\
 3,13, 4, 7,15, 2,8,14,12,0,1,10,6,9,11,5,\
 0,14, 7,11,10, 4,13,1,5,8,12,6,9,3,2,15,\
13, 8,10, 1, 3,15,4,2,11,6,7,12,0,5,14,9,\
10, 0, 9,14, 6, 3,15,5,1,13,12,7,11,4,2,8,\
13, 7, 0, 9, 3, 4,6,10,2,8,5,14,12,11,15,1,\
13, 6, 4, 9, 8,15,3,0,11,1,2,12,5,10,14,7,\
 1,10,13, 0, 6, 9,8,7,4,15,14,3,11,5,2,12,\
 7,13,14, 3, 0, 6,9,10,1,2,8,5,11,12,4,15,\
13, 8,11, 5, 6,15,0,3,4,7,2,12,1,10,14,9,\
10, 6, 9, 0,12,11,7,13,15,1,3,14,5,2,8,4,\
 3,15, 0, 6,10, 1,13,8,9,4,5,11,12,7,2,14,\
 2,12, 4, 1, 7,10,11,6,8,5,3,15,13,0,14,9,\
14,11, 2,12, 4, 7,13,1,5,0,15,10,3,9,8,6,\
 4, 2, 1,11,10,13,7,8,15,9,12,5,6,3,0,14,\
11, 8,12, 7, 1,14,2,13,6,15,0,9,10,4,5,3,\
12, 1,10,15, 9, 2,6,8,0,13,3,4,14,7,5,11,\
10,15, 4, 2, 7,12,9,5,6,1,13,14,0,11,3,8,\
 9,14,15, 5, 2, 8,12,3,7,0,4,10,1,13,11,6,\
 4, 3, 2,12, 9, 5,15,10,11,14,1,7,6,0,8,13,\
 4,11, 2,14,15, 0,8,13,3,12,9,7,5,10,6,1,\
13, 0,11, 7, 4, 9,1,10,14,3,5,12,2,15,8,6,\
 1, 4,11,13,12, 3,7,14,10,15,6,8,0,5,9,2,\
 6,11,13, 8, 1, 4,10,7,9,5,0,15,14,2,3,12,\
13, 2, 8, 4, 6,15,11,1,10,9,3,14,5,0,12,7,\
 1,15,13, 8,10, 3,7,4,12,5,6,11,0,14,9,2,\
 7,11, 4, 1, 9,12,14,2,0,6,10,13,15,3,5,8,\
 2, 1,14, 7, 4,10,8,13,15,12,9,0,3,5,6,11

align 4
oper_p db 15,6,19,20,28,11,27,16,\
 0,14,22,25, 4,17,30, 9,\
 1, 7,23,13,31,26, 2, 8,\
18,12,29, 5,21,10, 3,24


align 16
EXPORTS:
	dd sz_des_encryption, des_encryption
	dd sz_des_decryption, des_decryption
	dd 0,0
	sz_des_encryption db 'des_encryption',0
	sz_des_decryption db 'des_decryption',0
