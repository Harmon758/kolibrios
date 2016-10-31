use32   ; ������� 32-���� ०�� ��ᥬ����
org 0x0 ; ������ � ���
	db 'MENUET01'
	dd 1,START,I_END,MEM,STACKTOP,0,cur_dir_path

include '../../../../proc32.inc'
include '../../../../macros.inc'
include '../../../../KOSfuncs.inc'
include '../../../../develop/libraries/box_lib/load_lib.mac'

include 'deflate.inc'
include 'debug.inc'
include 'zlib.inc'

@use_library

align 4
m0size dd 90 ;ࠧ��� ������ ��� 㯠�����
m1size dd 1024 ;ࠧ��� ���� ������ ��� 㯠�����
m2size dd 0 ;ࠧ��� �ᯠ�������� ������

align 4
m0: ;����� ��� 㯠�����
file 'zlib.txt'

align 4
m1 rb 1024 ;���� ��� 㯠�������� ������
m2 dd 0 ;㪠��⥫� �� �ᯠ������� �����

buf rb 1024 ;���� ��� �뢮�� ᦠ��� ������ � ����
strategy dd Z_DEFAULT_STRATEGY ;���⥣�� ᦠ��

align 4
START:
load_libraries l_libs_start,load_lib_end
mov	ebp,lib0
.test_lib_open:
	cmp	dword [ebp+ll_struc_size-4],0
	jz	@f
	mcall SF_TERMINATE_PROCESS ;exit not correct
@@:
	add ebp,ll_struc_size
	cmp ebp,load_lib_end
	jl .test_lib_open

;	mcall SF_SYS_MISC, SSF_HEAP_INIT

	call test_code

align 4
red:                    ; ����ᮢ��� ����
    call draw_window    ; ��뢠�� ��楤��� ���ᮢ�� ����

align 4
still:
    mcall SF_WAIT_EVENT
    cmp  eax,1          ; ����ᮢ��� ���� ?
    je   red
    cmp  eax,2          ; ����� ������ ?
    je   key
    cmp  eax,3          ; ����� ������ ?
    je   button
    jmp  still

align 4
key:
    mcall SF_GET_KEY

	cmp ah,178 ;Up
	jne @f
		cmp dword[strategy],0
		jle @f
		dec dword[strategy]
		call test_code
		call draw_window
	@@:
	cmp ah,177 ;Down
	jne @f
		cmp dword[strategy],4
		jge @f
		inc dword[strategy]
		call test_code
		call draw_window
	@@:
	cmp ah,176 ;Left
	jne @f
		cmp dword[m0size],8
		jl @f
		dec dword[m0size]
		call test_code
		call draw_window
	@@:
	cmp ah,179 ;Right
	jne @f
		inc dword[m0size]
		call test_code
		call draw_window
	@@:
    jmp  still          ; �������� � ��砫� 横��

;---------------------------------------------------------------------
align 4
button:
	mcall SF_GET_BUTTON

	cmp ah,1
	jne still

.exit: ; ����� �ணࠬ��
  	mcall SF_SYS_MISC,SSF_MEM_FREE,[m2]
    mcall SF_TERMINATE_PROCESS

align 4
draw_window:
    mcall SF_REDRAW, SSF_BEGIN_DRAW ; �㭪�� 12: ᮮ���� �� � ��砫� ���ᮢ��
    mcall SF_STYLE_SETTINGS, SSF_GET_COLORS, sc,sizeof.system_colors
    mov   edx, [sc.work]         ; 梥� 䮭�
    or    edx, 0x33000000        ; � ⨯ ���� 3
    mcall SF_CREATE_WINDOW, <50,600>, <50,180>, , ,title

	cStr edx,'Strategy:'
	mcall SF_DRAW_TEXT, <10,10>,0x40f0,,9
	cStr edx,'Input size:'
	mcall , <10,20>,,,11
	cStr edx,'Compr. size:'
	mcall , <10,30>,,,12
	cStr edx,'Outp. size:'
	mcall , <10,120>,,,11

	mcall SF_DRAW_NUMBER, (1 shl 16)+1, strategy, <90,10>, 0
	mcall , (5 shl 16)+1, m0size, <90,20>
	mcall , (5 shl 16)+1, m1size, <90,30>
	mcall , (5 shl 16)+1, m2size, <90,120>
	;mov ecx,(1 shl 31)
	mov esi,[m2size]
	cmp esi,95
	jle @f
		mov esi,95
	@@:
	mcall SF_DRAW_TEXT, <10,130>, 0, [m2]

	mov esi,7
	mov ebx,(10 shl 16)+45 ;(x shl 16)+y
	mov edx,buf
	.cycle1: ;rows
		mcall SF_DRAW_TEXT,, (1 shl 31)
		add ebx,10
		add edx,32*3
	dec esi
	jnz .cycle1

    mcall SF_REDRAW, SSF_END_DRAW ; �㭪�� 12.2, �����稫� �ᮢ���
    ret

align 4
test_code:
	stdcall [deflateInit2], my_strm,\
		-1, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, [strategy]
;���⥣��:
; Z_DEFAULT_STRATEGY, Z_FILTERED, Z_HUFFMAN_ONLY, Z_RLE, Z_FIXED

	mov eax,my_strm
	mov [eax+z_stream.next_in],m0 ;��⠭�������� ������ ��� ᦠ��
	mov ecx,[m0size]
	mov word[eax+z_stream.avail_in],cx ;ࠧ��� ᦨ����� ������
	mov [eax+z_stream.next_out],m1 ;��⠭�������� ���� ��� ᦠ��
	mov word[eax+z_stream.avail_out],1024 ;ࠧ��� ���� ��� ᦠ�� (���ᨬ� 16 ��)

	;����塞 crc ��� ᦨ����� ������
	stdcall [calc_crc32], 0,m0,ecx
	mov edx,eax

	;call print_z_struct

	stdcall [deflate], my_strm, Z_FINISH ;Z_NO_FLUSH

	;call print_z_struct

	;ࠧ��� ᦠ��� ������: 1024-word[my_strm.avail_out]
	mov ecx,1024
	sub cx,word[my_strm.avail_out]
	mov [m1size],ecx

	;assert(ret != Z_STREAM_ERROR)
	;while (strm.avail_out == 0)

	;�⠢�� crc �� ᦠ�� �����
	mov ecx,[m1size]
	sub ecx,4
	add ecx,m1
	mov [ecx],edx

	;�ନ஢���� ⥪�� ��� �⮡ࠦ���� ᦠ��� ������
	;� 16-�筮� ����, �㦭� ⮫쪮 ��� �ਬ��
	mov ebx,[m1size]
	mov esi,m1
	mov edi,buf
	mov edx,7
align 4
	.cycle1: ;rows
	mov ecx,32
align 4
	.cycle0: ;cols
		stdcall hex_in_str, edi,[esi],2
		add edi,2
		inc esi
		mov byte[edi],' ' ;format space
		dec ebx
		jz .cycle1end ;if end file
		inc edi
		loop .cycle0
	mov byte[edi-1],0
	dec edx
	jnz .cycle1
	.cycle1end:
	mov byte[edi],0

	;㤠����� ���� � ࠭�� �ᯠ������묨 ����묨
	mcall SF_SYS_MISC,SSF_MEM_FREE,[m2]
	
	mov eax,[m1size]
	sub eax,2 ;;; 2? or 6?
	mov [m2size],eax
	mov eax,m1
	add eax,2
	stdcall [deflate_unpack],eax,m2size
	mov [m2],eax ;������ ����� �ᯠ�������� ������
	mov ecx,[m0size] ;;; ???
	mov [m2size],ecx
	ret

align 4
proc print_z_struct uses eax ebx
	mov eax,my_strm
	mov ebx,[eax+z_stream.state]
	stdcall debug_fields,eax,sv_2
	stdcall debug_fields,ebx,sv_3
	ret
endp

sc system_colors

title db 'Zlib test, press on [Up], [Down], [Left], [Right]',0

align 4
import_archiver:
	deflate_unpack dd sz_deflate_unpack
	dd 0,0
	sz_deflate_unpack db 'deflate_unpack',0

align 4
import_zlib:
;	dd sz_lib_init
	deflateInit		dd sz_deflateInit
	deflateInit2	dd sz_deflateInit2
	deflateReset	dd sz_deflateReset
	deflate			dd sz_deflate
	deflateEnd		dd sz_deflateEnd
	calc_crc32		dd sz_calc_crc32

	dd 0,0

;	sz_lib_init db 'lib_init',0
	sz_deflateInit db 'deflateInit',0
	sz_deflateInit2 db 'deflateInit2',0
	sz_deflateReset db 'deflateReset',0
	sz_deflate db 'deflate',0
	sz_deflateEnd db 'deflateEnd',0
	sz_calc_crc32 db 'calc_crc32',0

;--------------------------------------------------
system_dir_0 db '/sys/lib/'
lib_name_0 db 'archiver.obj',0

system_dir_1 db '/sys/lib/'
lib_name_1 db 'zlib.obj',0

err_message_found_lib0 db 'Sorry I cannot load library archiver.obj',0
err_message_found_lib1 db 'Sorry I cannot load library zlib.obj',0
head_f_i:
head_f_l db 'System error',0
err_message_import0 db 'Error on load import library archiver.obj',0
err_message_import1 db 'Error on load import library zlib.obj',0

l_libs_start:
	lib0 l_libs lib_name_0, cur_dir_path, library_path, system_dir_0,\
		err_message_found_lib0, head_f_l, import_archiver,err_message_import0, head_f_i
	lib1 l_libs lib_name_1, cur_dir_path, library_path, system_dir_1,\
		err_message_found_lib1, head_f_l, import_zlib, err_message_import1, head_f_i
load_lib_end:
;---------------------------------------------------------------------

align 16
I_END:
my_strm z_stream
	rd 4096
align 16
STACKTOP:
cur_dir_path:
	rb 4096
library_path:
	rb 4096
MEM:
