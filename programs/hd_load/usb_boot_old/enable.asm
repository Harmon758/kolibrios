format MZ 
heap 0 
stack 800h 
entry main:start 

segment main use16 

use16 
start: 

 mov ax,_data 
 mov ds, ax 
 mov es, ax 

 xor eax, eax 
 mov ax,ds 

 shl eax, 4 

 mov bx, gdt 
 add ebx, eax ; ebx - �������� ����� gdt 

 mov word [gdtPtr], 2 * 8 -1 ; ������ gdt 2 ����������� = 0x000f 
 mov dword [gdtPtr + 2], ebx 

 lgdt pword [gdtPtr] 

 in al, 0x70 ;��������� NMI 
 mov bl, al 
 or al, 0x80 
 out 70h , al 
 in al, 0x71 ;��������� RTC ����� ������ ����� � ���� 0�70 
             ;������� ��������� � ����� 0x71 

 cli 

 mov eax, cr0 
 or al, 01b 
 mov cr0, eax 

 mov dx, 0x08 
 mov gs, dx ;gs - ���������� ������� � ����� 0 � �������� 0xFFFFFFFF 
 mov fs, dx

 and al, 0xFE 
 mov cr0, eax 

 sti 

 mov al, bl 
 out 70h , al 
 in al, 71h 

 mov dx,92h 
 in al,dx 
 or al,2 
 out dx,al 

;������ ����� �������� ������ �� ���� ������ 
;�������� 
;mov eax, [gs:0xFFFFFFF0] ; 

 mov ah,4ch ;���������� ��� ��������� 
 int 21h 

segment _data use16 

 align 16 

 gdt dw 0, 0, 0, 0 ;0 
 _flat dw 0ffffh,0,0F200h,008fh ;08 ������� ������ DPL = 3 
 ;���� 0, ������ 0xFFFFFFFF 

 gdtPtr dq ? 