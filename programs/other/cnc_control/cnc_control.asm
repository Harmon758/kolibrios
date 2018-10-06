;CNC CONTROL
;Igor Afanasyev (aka IgorA) and Sergey Efremenkov (aka theonlymirage), 2018

;02.10.18 - Only prototype UI

format binary as ""
use32
        org 0
        db 'MENUET01'
        dd 1,start,i_end,mem,stacktop,0,sys_path

include '../../macros.inc'
include '../../proc32.inc'
include '../../KOSfuncs.inc'
include '../../load_img.inc'
include '../../develop/libraries/libs-dev/libimg/libimg.inc'
include '../../develop/libraries/box_lib/trunk/box_lib.mac'
include 'lang.inc'
include 'cnc_control.inc'
include '../../develop/info3ds/info_fun_float.inc'

@use_library_mem mem.Alloc,mem.Free,mem.ReAlloc,dll.Load
caption db 'CNC Control 02.10.18',0 ;������� ����

run_file_70 FileInfoBlock

offs_last_timer dd 0 ;��᫥���� ᤢ�� �������� � �㭪樨 ⠩���

IMAGE_TOOLBAR_ICON_SIZE equ 16*16*3
image_data_toolbar dd 0 ;㪠��⥫� �� �६����� ������. ��� �㦥� �८�ࠧ������ ����ࠦ����
icon_tl_sys dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ��⥬��� ������
icon_toolbar dd 0 ;㪠��⥫� �� ������ ��� �࠭���� ������ ��ꥪ⮢

align 4
start:
        load_libraries l_libs_start,l_libs_end
        ;�஢�ઠ �� ᪮�쪮 㤠筮 ���㧨���� ������⥪�
        mov     ebp,lib_0
        cmp     dword [ebp+ll_struc_size-4],0
        jz      @f
                mcall SF_TERMINATE_PROCESS
        @@:
        mcall SF_STYLE_SETTINGS,SSF_GET_COLORS,sc,sizeof.system_colors
        mcall SF_SET_EVENTS_MASK,0xC0000027
        stdcall [OpenDialog_Init],OpenDialog_data ;�����⮢�� �������

        stdcall [buf2d_create], buf_0 ;ᮧ����� ����

        ;���� ������ �� ᮧ����� ������ (��� ������ 㪠��⥫� image_data_toolbar)
        include_image_file '..\..\fs\kfar\trunk\font6x9.bmp', image_data_toolbar, buf_1.w,buf_1.h
        stdcall [buf2d_create_f_img], buf_1,[image_data_toolbar] ;ᮧ���� ����
        stdcall mem.Free,[image_data_toolbar] ;�᢮������� ������
        stdcall [buf2d_conv_24_to_8], buf_1,1 ;������ ���� �஧�筮�� 8 ���
        stdcall [buf2d_convert_text_matrix], buf_1
        mov eax,[buf_1.h]
        shr eax,8
        mov [font_h],eax

        include_image_file 'toolbar.png', image_data_toolbar

        ;*** ��⠭���� �६��� ��� ⠩���
        mcall SF_SYSTEM_GET,SSF_TIME_COUNT
        mov [last_time],eax

        ;call but_new_file
        option_boxes_set_sys_color sc,opt_grlist1

        ;progress bar trash
        mov    [pb.left],           dword  50
        mov    [pb.top],            dword  30
        mov    [pb.width],          dword  350
        mov    [pb.height],         dword  17
        mov    [pb.max],            dword  100;599
        mov    [pb.min],            dword  0 ;-397
        mov    [pb.value],          dword  50;-397
        mov    [pb.back_color],     dword 00C8D0D4h
        mov    [pb.progress_color], dword 8072B7EBh
        mov    [pb.frame_color],    dword 00406175h

align 4
red_win:
        call draw_window

align 4
still:
        mcall SF_SYSTEM_GET,SSF_TIME_COUNT
        mov ebx,[last_time]
        add ebx,10 ;����প�
        cmp ebx,eax
        jge @f
                mov ebx,eax
        @@:
        sub ebx,eax
        mcall SF_WAIT_EVENT_TIMEOUT
        cmp eax,0
        jne @f
                call timer_funct
                jmp still
        @@:

        cmp al,1
        jz red_win
        cmp al,2
        jz key
        cmp al,3
        jz button
        cmp al,6
        jne @f
                mcall SF_THREAD_INFO,procinfo,-1
                cmp ax,word[procinfo.window_stack_position]
                jne @f ;���� �� ��⨢��
                call mouse
        @@:
        jmp still

align 4
mouse:
        stdcall [option_box_mouse], opt_grlist1
        stdcall [edit_box_mouse], editFileName
        stdcall [edit_box_mouse], editCommand

        push eax ebx ecx
        mcall SF_MOUSE_GET,SSF_BUTTON_EXT
        bt eax,8
        jnc @f
                ;mouse l. but. press
                call mouse_left_d
                jmp .end_l
        @@:
        ;bt eax,0
        ;jnc @f
                ;mouse l. but. move
                ;call mouse_left_m
                ;jmp .end_l
        ;@@:
        bt eax,16
        jnc .end_l
                ;mouse l. but. up
                call mouse_left_u
                ;jmp .end_l
        .end_l:
        ;bt eax,9
        ;jnc @f
                ;mouse r. but. press
                ;jmp .end_r
        ;@@:
        ;bt eax,1
        ;jnc @f
                ;mouse r. but. move
                ;call mouse_right_m
                ;jmp .end_r
        ;@@:
        ;.end_r:

        call buf_get_mouse_coord
        cmp eax,-1
        je .end0
                shl eax,1
                sub eax,[buf_0.w]
                sar eax,1
                mov [mouse_prop_x],eax
                mov ecx,ObjData
                shl ebx,1
                sub ebx,[buf_0.h]
                sar ebx,1
                mov [mouse_prop_y],ebx

                mcall SF_MOUSE_GET,SSF_SCROLL_DATA
                test ax,ax
                jz .end0
                finit
                fld qword[zoom_plus]
                fld1
                fsubp
                fld st0 ;for Y coord

                ;for X coord
                fild dword[mouse_prop_x]
                fmulp st1,st0

                mov ebx,eax
                test ax,0x8000
                jnz .decr
                        ;㢥��祭�� ����⠡�
                        fchs
                        fild dword[ecx+Figure.MCentrX] ;add old value
                        fmul qword[zoom_plus]
                        faddp

                        fld qword[ecx+Figure.MScale]
                        fmul qword[zoom_plus]
                        ;if (Figure.MScale>16.0)
                        ;...
                        jmp @f
                .decr:
                        ;㬥��襭�� ����⠡�
                        fild dword[ecx+Figure.MCentrX] ;add old value
                        fdiv qword[zoom_plus]
                        faddp

                        fld qword[ecx+Figure.MScale]
                        fdiv qword[zoom_plus]
                        fld1
                        fcomp
                        fstsw ax
                        sahf
                        jbe @f
                                ;if (Figure.MScale<1.0)
                                ffree st0
                                fincstp
                                ffree st0
                                fincstp
                                fldz ;default Figure.MCentrX
                                fld1 ;default Figure.MScale
                                mov dword[ecx+Figure.MCentrY],0
                @@:
                fstp qword[ecx+Figure.MScale]
                fistp dword[ecx+Figure.MCentrX]

                ;for Y coord
                fild dword[mouse_prop_y]
                fmulp st1,st0
                test bx,0x8000
                jnz .decr_y
                        ;㢥��祭�� ����⠡�
                        fild dword[ecx+Figure.MCentrY] ;add old value
                        fmul qword[zoom_plus]
                        faddp
                        jmp @f
                .decr_y:
                        ;㬥��襭�� ����⠡�
                        fchs
                        fild dword[ecx+Figure.MCentrY] ;add old value
                        fdiv qword[zoom_plus]
                        faddp
                @@:
                fistp dword[ecx+Figure.MCentrY]

                mov dword[offs_last_timer],0
        .end0:

        pop ecx ebx eax
        ret

;output:
; eax - buffer coord X (�᫨ ����� �� ���஬ -1)
; ebx - buffer coord Y (�᫨ ����� �� ���஬ -1)
align 4
proc buf_get_mouse_coord
        mcall SF_MOUSE_GET,SSF_WINDOW_POSITION
        cmp ax,word[buf_0.t]
        jl .no_buf ;�� ������ � ���� ���� �� �� y
        mov ebx,eax
        shr ebx,16
        cmp bx,word[buf_0.l]
        jl .no_buf ;�� ������ � ���� ���� �� �� x

        and eax,0xffff ;��⠢�塞 ���न���� y
        sub ax,word[buf_0.t]
        cmp eax,[buf_0.h]
        jg .no_buf
        sub bx,word[buf_0.l]
        cmp ebx,[buf_0.w]
        jg .no_buf
        xchg eax,ebx
        jmp .end_f
        .no_buf:
                xor eax,eax
                not eax
                xor ebx,ebx
                not ebx
        .end_f:
        ret
endp

align 4
proc timer_funct
        pushad
        mcall SF_SYSTEM_GET,SSF_TIME_COUNT
        mov [last_time],eax

        cmp dword[offs_last_timer],ObjData
        je @f
                mov dword[offs_last_timer],ObjData
                stdcall draw_obj2d,ObjData
                stdcall [buf2d_draw], buf_0
        @@:
        popad
        ret
endp

align 4
draw_window:
pushad
        mcall SF_REDRAW,SSF_BEGIN_DRAW

        ; *** �ᮢ���� �������� ���� (�믮������ 1 ࠧ �� ����᪥) ***
        mov edx,[sc.work]
        or  edx,0x33000000
        mov edi,caption
        mcall SF_CREATE_WINDOW,(20 shl 16)+775,(20 shl 16)+445

        ;;mcall SF_THREAD_INFO,procinfo,-1
        ;;mov eax,dword[procinfo.box.height]
        ;;cmp eax,120
        ;;jge @f
                ;;mov eax,120 ;min size
        ;;@@:
        ;;sub eax,65
        ;;mov ebx,dword[procinfo.box.width]
        ;;cmp ebx,270
        ;;jge @f
                ;;mov ebx,270
        ;;@@:
        ;;sub ebx,215
        ;;cmp eax,dword[buf_0.h] ;ᬮ�ਬ ࠧ��� ����
        ;;jne @f
        ;;cmp ebx,dword[buf_0.w]
        ;;jne @f
                ;;jmp .end0
        ;;@@:
                ;;stdcall [buf2d_resize],buf_0,ebx,eax,1
                mov ecx,[ObjData.FigCount]
                or ecx,ecx
                jz .end0
                mov eax,[ObjData.FigData]
                xor edx,edx
                .cycle0:
                        stdcall FigCalculateSizes,[eax+4*edx],0
                        inc edx
                        loop .cycle0
                stdcall ObjCalculateScale,ObjData
                mov dword[offs_last_timer],0
                call timer_funct
        .end0:

        stdcall [edit_box_draw], editFileName
        stdcall [edit_box_draw], editCommand

        stdcall [option_box_draw], opt_grlist1
        push   pb
        call   [progressbar_draw]

        ; *** ᮧ����� ������ �� ������ ***
        mcall SF_DEFINE_BUTTON,(731 shl 16)+20,(75 shl 16)+20,3, [sc.work_button]
        mcall , (15 shl 16)+20,(27 shl 16)+20,4 ;restore
        mcall ,(538 shl 16)+20,,5 ;connect
        mcall ,(563 shl 16)+20,,6 ;close connect
        mcall ,(437 shl 16)+86,(378 shl 16)+20,7 ;cancel
        mcall ,(616 shl 16)+136,(378 shl 16)+20,8 ;run

        ; ***
        mov ecx,[sc.work_text]
        or ecx,0x81000000
        mcall SF_DRAW_TEXT,(15 shl 16)+5,,txt_preview
        mcall ,(424 shl 16)+5,,txt_port
        mov ecx,[sc.work_button_text]
        or ecx,0x81000000
        mcall ,(440 shl 16)+381,,txt_but_cancel
        mcall ,(619 shl 16)+381,,txt_but_run

        ; *** �ᮢ���� ������ �� ������� ***
        mcall SF_PUT_IMAGE,[image_data_toolbar],(16 shl 16)+16,(733 shl 16)+77 ;icon open

        add ebx,IMAGE_TOOLBAR_ICON_SIZE
        mcall ,,,(17 shl 16)+29 ;restore scale
        add ebx,IMAGE_TOOLBAR_ICON_SIZE
        mcall ,,,(540 shl 16)+29 ;connect
        add ebx,IMAGE_TOOLBAR_ICON_SIZE
        mcall ,,,(565 shl 16)+29 ;close connect
        ;;add ebx,IMAGE_TOOLBAR_ICON_SIZE
        ;;mcall ,,,(439 shl 16)+380 ;cancel
        add ebx,IMAGE_TOOLBAR_ICON_SIZE
        mcall ,,,((638+97) shl 16)+380 ;run

        stdcall [buf2d_draw], buf_0

        mcall SF_REDRAW,SSF_END_DRAW
popad
        ret

align 4
key:
        mcall SF_GET_KEY

        stdcall [edit_box_key], editFileName
        stdcall [edit_box_key], editCommand
        
        mov ecx,eax
        mcall SF_KEYBOARD,SSF_GET_CONTROL_KEYS
        bt eax,2 ;left Ctrl
        jc .key_Ctrl
        bt eax,3 ;right Ctrl
        jc .key_Ctrl
        jmp .end0
        .key_Ctrl:
                cmp ch,15 ;111 ;Ctrl+O
                jne @f
                        call but_open_file
                @@:
        .end0:
        jmp still

align 4
button:
        mcall SF_GET_BUTTON
        cmp ah,3
        jne @f
                call but_open_file
                jmp still
        @@:
        cmp ah,4
        jne @f
                call but_restore_zoom
                jmp still
        @@:

        ;cmp ah,5
        ;jne @f
                ;call but_...
                ;jmp still
        ;@@:
        cmp ah,1
        jne still
.exit:
        stdcall [buf2d_delete],buf_0
        stdcall mem.Free,[image_data_toolbar]
        stdcall mem.Free,[open_file_data]
        mcall SF_TERMINATE_PROCESS



align 4
open_file_data dd 0 ;㪠��⥫� �� ������ ��� ������ 䠩���
open_file_size dd 0 ;ࠧ��� ����⮣� 䠩��

align 4
but_open_file:
        pushad
        copy_path open_dialog_name,communication_area_default_path,file_name,0
        mov [OpenDialog_data.type],0
        stdcall [OpenDialog_Start],OpenDialog_data
        cmp [OpenDialog_data.status],2
        je .end_open_file
        ;��� �� 㤠筮� ����⨨ �������

        push eax ebx ecx edx    ;copy file name path from OpenDialog
        mov eax, openfile_path
        mov ebx, fileNameBuffer ;.data
        mov ecx, 0
      @@:
        mov dl, byte[eax]
        cmp dl, 0 ;byte[eax], 0
        je @f
        mov byte[ebx], dl
        inc eax
        inc ebx
        inc ecx
        jmp @b
      @@:
        mov byte[ebx], 0
        mov dword[fileNameBuffer.size], ecx
        mov dword[editFileName.size], ecx
        mov dword[editFileName.pos], ecx
        pop edx ecx ebx eax

        mov [run_file_70.Function], SSF_GET_INFO
        mov [run_file_70.Position], 0
        mov [run_file_70.Flags], 0
        mov dword[run_file_70.Count], 0
        mov dword[run_file_70.Buffer], open_b
        mov byte[run_file_70+20], 0
        mov dword[run_file_70.FileName], openfile_path
        mcall SF_FILE,run_file_70

        mov ecx,dword[open_b+32] ;+32 qword: ࠧ��� 䠩�� � �����
        inc ecx ;for text files
        stdcall mem.ReAlloc,[open_file_data],ecx
        mov [open_file_data],eax
        dec ecx ;for text files
        mov byte[eax+ecx],0 ;for text files

        mov [run_file_70.Function], SSF_READ_FILE
        mov [run_file_70.Position], 0
        mov [run_file_70.Flags], 0
        mov dword[run_file_70.Count], ecx
        m2m dword[run_file_70.Buffer], dword[open_file_data]
        mov byte[run_file_70+20], 0
        mov dword[run_file_70.FileName], openfile_path
        mcall SF_FILE,run_file_70 ;����㦠�� 䠩� ����ࠦ����
        test eax,eax
        jnz .end_open_file
        cmp ebx,0xffffffff
        je .end_open_file

                mov [open_file_size],ebx
                mcall SF_SET_CAPTION,1,openfile_path

                ;---
                stdcall FileInit,[open_file_data],[open_file_size]
                stdcall [buf2d_clear], buf_0, [buf_0.color] ;��⨬ ����
                stdcall [buf2d_draw], buf_0 ;������塞 ���� �� �࠭�
        .end_open_file:
        popad
        ret

;output:
; ecx - memory size for save file
align 4
proc get_file_save_size uses eax ebx edx esi
        mov ecx,100 ;title
        mov ebx,ObjData
        add ecx,50 ;object
        mov edx,[ebx+Object.FigCount]
        or edx,edx
        jz .cycle1end
        mov esi,[ebx+Object.FigData]
align 4
        .cycle1: ; 横� �� 䨣�ࠬ
                add ecx,80 ;figure
                mov eax,[esi]
                or eax,eax
                jz @f
                        mov eax,[eax+Figure.PoiCount]
                        imul eax,70
                        add ecx,eax ;points
                @@:
                add esi,4
                dec edx
                jnz .cycle1
        .cycle1end:
        ret
endp

align 4
proc but_restore_zoom
        finit
        fld1
        fstp qword[ObjData.MScale]
        mov dword[ObjData.MCentrX],0
        mov dword[ObjData.MCentrY],0
        mov dword[offs_last_timer],0
        ret
endp



;input:
; buf - 㪠��⥫� �� ��ப�, �᫮ ������ ���� � 10 ��� 16 �筮� ����
;output:
; eax - �᫮
align 4
proc conv_str_to_int uses ebx ecx esi, buf:dword
        xor eax,eax
        xor ebx,ebx
        mov esi,[buf]

        ;�� ��砩 �᫨ ��। �᫮� ��室���� �஡���
        @@:
        cmp byte[esi],' '
        jne @f
                inc esi
                jmp @b
        @@:

        ;��।������ ����⥫��� �ᥫ
        xor ecx,ecx
        inc ecx
        cmp byte[esi],'-'
        jne @f
                dec ecx
                inc esi
        @@:

        cmp word[esi],'0x'
        je .load_digit_16

        .load_digit_10: ;���뢠��� 10-���� ���
                mov bl,byte[esi]
                cmp bl,'0'
                jl @f
                cmp bl,'9'
                jg @f
                        sub bl,'0'
                        imul eax,10
                        add eax,ebx
                        inc esi
                        jmp .load_digit_10
        jmp @f

        .load_digit_16: ;���뢠��� 16-���� ���
                add esi,2
        .cycle_16:
                mov bl,byte[esi]
                cmp bl,'0'
                jl @f
                cmp bl,'f'
                jg @f
                cmp bl,'9'
                jle .us1
                        cmp bl,'A'
                        jl @f ;��ᥨ���� ᨬ���� >'9' � <'A'
                .us1: ;��⠢��� �᫮���
                cmp bl,'F'
                jle .us2
                        cmp bl,'a'
                        jl @f ;��ᥨ���� ᨬ���� >'F' � <'a'
                        sub bl,32 ;��ॢ���� ᨬ���� � ���孨� ॣ����, ��� ��饭�� �� ��᫥��饩 ��ࠡ�⪨
                .us2: ;��⠢��� �᫮���
                        sub bl,'0'
                        cmp bl,9
                        jle .cor1
                                sub bl,7 ;convert 'A' to '10'
                        .cor1:
                        shl eax,4
                        add eax,ebx
                        inc esi
                        jmp .cycle_16
        @@:
        cmp ecx,0 ;�᫨ �᫮ ����⥫쭮�
        jne @f
                sub ecx,eax
                mov eax,ecx
        @@:
        ret
endp


;����� ��� ������� ������ 䠩���
align 4
OpenDialog_data:
.type                   dd 0 ;0 - ������, 1 - ��࠭���, 2 - ����� ��४���
.procinfo               dd procinfo     ;+4
.com_area_name          dd communication_area_name      ;+8
.com_area               dd 0    ;+12
.opendir_path           dd plugin_path  ;+16
.dir_default_path       dd default_dir ;+20
.start_path             dd file_name ;+24 ���� � ������� ������ 䠩���
.draw_window            dd draw_window  ;+28
.status                 dd 0    ;+32
.openfile_path          dd openfile_path        ;+36 ���� � ���뢠����� 䠩��
.filename_area          dd filename_area        ;+40
.filter_area            dd Filter
.x:
.x_size                 dw 420 ;+48 ; Window X size
.x_start                dw 10 ;+50 ; Window X position
.y:
.y_size                 dw 320 ;+52 ; Window y size
.y_start                dw 10 ;+54 ; Window Y position

default_dir db '/rd/1',0

communication_area_name:
        db 'FFFFFFFF_open_dialog',0
open_dialog_name:
        db 'opendial',0
communication_area_default_path:
        db '/rd/1/File managers/',0

Filter:
dd Filter.end - Filter ;.1
.1:
db 'NC',0
db 'PNG',0
.end:
db 0



head_f_i:
head_f_l db '���⥬��� �訡��',0

system_dir_0 db '/sys/lib/'
lib_name_0 db 'proc_lib.obj',0
err_message_found_lib_0 db '�� ������� ������⥪� ',39,'proc_lib.obj',39,0
err_message_import_0 db '�訡�� �� ������ ������⥪� ',39,'proc_lib.obj',39,0

system_dir_1 db '/sys/lib/'
lib_name_1 db 'libimg.obj',0
err_message_found_lib_1 db '�� ������� ������⥪� ',39,'libimg.obj',39,0
err_message_import_1 db '�訡�� �� ������ ������⥪� ',39,'libimg.obj',39,0

system_dir_2 db '/sys/lib/'
lib_name_2 db 'buf2d.obj',0
err_msg_found_lib_2 db '�� ������� ������⥪� ',39,'buf2d.obj',39,0
err_msg_import_2 db '�訡�� �� ������ ������⥪� ',39,'buf2d',39,0

system_dir_3 db '/sys/lib/'
lib_name_3 db 'box_lib.obj',0
err_msg_found_lib_3 db '�� ������� ������⥪� ',39,'box_lib.obj',39,0
err_msg_import_3 db '�訡�� �� ������ ������⥪� ',39,'box_lib',39,0

l_libs_start:
        lib_0 l_libs lib_name_0, sys_path, file_name, system_dir_0,\
                err_message_found_lib_0, head_f_l, proclib_import,err_message_import_0, head_f_i
        lib_1 l_libs lib_name_1, sys_path, file_name, system_dir_1,\
                err_message_found_lib_1, head_f_l, import_libimg, err_message_import_1, head_f_i
        lib_2 l_libs lib_name_2, sys_path, library_path, system_dir_2,\
                err_msg_found_lib_2,head_f_l,import_buf2d,err_msg_import_2,head_f_i
        lib_3 l_libs lib_name_3, sys_path, file_name,  system_dir_3,\
                err_msg_found_lib_3, head_f_l, import_box_lib,err_msg_import_3,head_f_i
l_libs_end:

align 4
import_libimg:
        dd alib_init1
        img_is_img  dd aimg_is_img
        img_info    dd aimg_info
        img_from_file dd aimg_from_file
        img_to_file dd aimg_to_file
        img_from_rgb dd aimg_from_rgb
        img_to_rgb  dd aimg_to_rgb
        img_to_rgb2 dd aimg_to_rgb2
        img_decode  dd aimg_decode
        img_encode  dd aimg_encode
        img_create  dd aimg_create
        img_destroy dd aimg_destroy
        img_destroy_layer dd aimg_destroy_layer
        img_count   dd aimg_count
        img_lock_bits dd aimg_lock_bits
        img_unlock_bits dd aimg_unlock_bits
        img_flip    dd aimg_flip
        img_flip_layer dd aimg_flip_layer
        img_rotate  dd aimg_rotate
        img_rotate_layer dd aimg_rotate_layer
        img_draw    dd aimg_draw

        dd 0,0
        alib_init1   db 'lib_init',0
        aimg_is_img  db 'img_is_img',0 ;��।���� �� �����, ����� �� ������⥪� ᤥ���� �� ��� ����ࠦ����
        aimg_info    db 'img_info',0
        aimg_from_file db 'img_from_file',0
        aimg_to_file db 'img_to_file',0
        aimg_from_rgb db 'img_from_rgb',0
        aimg_to_rgb  db 'img_to_rgb',0 ;�८�ࠧ������ ����ࠦ���� � ����� RGB
        aimg_to_rgb2 db 'img_to_rgb2',0
        aimg_decode  db 'img_decode',0 ;��⮬���᪨ ��।���� �ଠ� ����᪨� ������
        aimg_encode  db 'img_encode',0
        aimg_create  db 'img_create',0
        aimg_destroy db 'img_destroy',0
        aimg_destroy_layer db 'img_destroy_layer',0
        aimg_count   db 'img_count',0
        aimg_lock_bits db 'img_lock_bits',0
        aimg_unlock_bits db 'img_unlock_bits',0
        aimg_flip    db 'img_flip',0
        aimg_flip_layer db 'img_flip_layer',0
        aimg_rotate  db 'img_rotate',0
        aimg_rotate_layer db 'img_rotate_layer',0
        aimg_draw    db 'img_draw',0

align 4
proclib_import: ;���ᠭ�� �ᯮ���㥬�� �㭪権
        OpenDialog_Init dd aOpenDialog_Init
        OpenDialog_Start dd aOpenDialog_Start
dd 0,0
        aOpenDialog_Init db 'OpenDialog_init',0
        aOpenDialog_Start db 'OpenDialog_start',0

align 4
import_buf2d:
        init dd sz_init
        buf2d_create dd sz_buf2d_create
        buf2d_create_f_img dd sz_buf2d_create_f_img
        buf2d_clear dd sz_buf2d_clear
        buf2d_draw dd sz_buf2d_draw
        buf2d_delete dd sz_buf2d_delete
        buf2d_resize dd sz_buf2d_resize
        buf2d_line dd sz_buf2d_line
        buf2d_line_sm dd sz_buf2d_line_sm
        buf2d_rect_by_size dd sz_buf2d_rect_by_size
        buf2d_filled_rect_by_size dd sz_buf2d_filled_rect_by_size
        buf2d_circle dd sz_buf2d_circle
        buf2d_img_hdiv2 dd sz_buf2d_img_hdiv2
        buf2d_img_wdiv2 dd sz_buf2d_img_wdiv2
        buf2d_conv_24_to_8 dd sz_buf2d_conv_24_to_8
        buf2d_conv_24_to_32 dd sz_buf2d_conv_24_to_32
        buf2d_bit_blt dd sz_buf2d_bit_blt
        buf2d_bit_blt_transp dd sz_buf2d_bit_blt_transp
        buf2d_bit_blt_alpha dd sz_buf2d_bit_blt_alpha
        buf2d_curve_bezier dd sz_buf2d_curve_bezier
        buf2d_convert_text_matrix dd sz_buf2d_convert_text_matrix
        buf2d_draw_text dd sz_buf2d_draw_text
        buf2d_crop_color dd sz_buf2d_crop_color
        buf2d_flip_h dd sz_buf2d_flip_h
        buf2d_flip_v dd sz_buf2d_flip_v
        buf2d_offset_h dd sz_buf2d_offset_h
        buf2d_flood_fill dd sz_buf2d_flood_fill
        buf2d_set_pixel dd sz_buf2d_set_pixel
        dd 0,0
        sz_init db 'lib_init',0
        sz_buf2d_create db 'buf2d_create',0
        sz_buf2d_create_f_img db 'buf2d_create_f_img',0
        sz_buf2d_clear db 'buf2d_clear',0
        sz_buf2d_draw db 'buf2d_draw',0
        sz_buf2d_delete db 'buf2d_delete',0
        sz_buf2d_resize db 'buf2d_resize',0
        sz_buf2d_line db 'buf2d_line',0
        sz_buf2d_line_sm db 'buf2d_line_sm',0
        sz_buf2d_rect_by_size db 'buf2d_rect_by_size',0
        sz_buf2d_filled_rect_by_size db 'buf2d_filled_rect_by_size',0
        sz_buf2d_circle db 'buf2d_circle',0
        sz_buf2d_img_hdiv2 db 'buf2d_img_hdiv2',0
        sz_buf2d_img_wdiv2 db 'buf2d_img_wdiv2',0
        sz_buf2d_conv_24_to_8 db 'buf2d_conv_24_to_8',0
        sz_buf2d_conv_24_to_32 db 'buf2d_conv_24_to_32',0
        sz_buf2d_bit_blt db 'buf2d_bit_blt',0
        sz_buf2d_bit_blt_transp db 'buf2d_bit_blt_transp',0
        sz_buf2d_bit_blt_alpha db 'buf2d_bit_blt_alpha',0
        sz_buf2d_curve_bezier db 'buf2d_curve_bezier',0
        sz_buf2d_convert_text_matrix db 'buf2d_convert_text_matrix',0
        sz_buf2d_draw_text db 'buf2d_draw_text',0
        sz_buf2d_crop_color db 'buf2d_crop_color',0
        sz_buf2d_flip_h db 'buf2d_flip_h',0
        sz_buf2d_flip_v db 'buf2d_flip_v',0
        sz_buf2d_offset_h db 'buf2d_offset_h',0
        sz_buf2d_flood_fill db 'buf2d_flood_fill',0
        sz_buf2d_set_pixel db 'buf2d_set_pixel',0

align 4
import_box_lib:
        dd sz_init1

        init_checkbox   dd sz_Init_checkbox
        check_box_draw  dd sz_Check_box_draw
        check_box_mouse dd sz_Check_box_mouse
        ;version_ch     dd sz_Version_ch

        option_box_draw  dd sz_Option_box_draw
        option_box_mouse dd sz_Option_box_mouse
        ;version_op      dd sz_Version_op

        edit_box_draw      dd sz_edit_box_draw
        edit_box_key       dd sz_edit_box_key
        edit_box_mouse     dd sz_edit_box_mouse
        edit_box_set_text  dd sz_edit_box_set_text
        scrollbar_ver_draw dd sz_scrollbar_ver_draw
        scrollbar_hor_draw dd sz_scrollbar_hor_draw

        progressbar_draw     dd sz_progressbar_draw
        progressbar_progress dd sz_progressbar_progress

        dd 0,0
        sz_init1 db 'lib_init',0

        sz_Init_checkbox   db 'init_checkbox2',0
        sz_Check_box_draw  db 'check_box_draw2',0
        sz_Check_box_mouse db 'check_box_mouse2',0
        ;sz_Version_ch     db 'version_ch2',0

        sz_Option_box_draw      db 'option_box_draw',0
        sz_Option_box_mouse     db 'option_box_mouse',0
        ;sz_Version_op      db 'version_op',0

        sz_edit_box_draw      db 'edit_box',0
        sz_edit_box_key       db 'edit_box_key',0
        sz_edit_box_mouse     db 'edit_box_mouse',0
        sz_edit_box_set_text  db 'edit_box_set_text',0
        sz_scrollbar_ver_draw db 'scrollbar_v_draw',0
        sz_scrollbar_hor_draw db 'scrollbar_h_draw',0

        sz_progressbar_draw     db 'progressbar_draw', 0
        sz_progressbar_progress db 'progressbar_progress', 0

align 4
mouse_dd dd 0
last_time dd 0

align 16
sc system_colors 

align 16
procinfo process_information 

align 4
buf_0: dd 0 ;㪠��⥫� �� ���� ����ࠦ����
.l: dw 15 ;+4 left
.t: dw 50 ;+6 top
.w: dd 384 ;+8 w
.h: dd 350 ;+12 h
.color: dd 0xffffd0 ;+16 color
        db 24 ;+20 bit in pixel

align 4
buf_1:
        dd 0 ;㪠��⥫� �� ���� ����ࠦ����
        dd 0 ;+4 left,top
.w: dd 0
.h: dd 0,0,24 ;+12 color,bit in pixel

font_h dd 0 ;���� ����

;input:
; eax - �᫮
; edi - ���� ��� ��ப�
; len - ������ ����
;output:
align 4
proc convert_int_to_str uses eax ecx edx edi esi, len:dword
        mov esi,[len]
        add esi,edi
        dec esi
        call .str
        ret
endp

align 4
.str:
        mov ecx,10
        cmp eax,ecx
        jb @f
                xor edx,edx
                div ecx
                push edx
                ;dec edi  ;ᬥ饭�� ����室���� ��� ����� � ���� ��ப�
                call .str
                pop eax
        @@:
        cmp edi,esi
        jge @f
                or al,0x30
                stosb
                mov byte[edi],0 ;� ����� ��ப� �⠢�� 0, ��-�� �� �뫠��� ����
        @@:
        ret

align 4
proc mem_copy uses ecx esi edi, destination:dword, source:dword, len:dword
        cld
        mov esi, dword[source]
        mov edi, dword[destination]
        mov ecx, dword[len]
        rep movsb
        ret
endp

edMaxSize = 511
edMax = 0 ;max, size, pos

align 4
editFileName  edit_box 270,450,78, 0xffffff, 0x6a9480, 0, 0xAABBCC, 0, edMaxSize, fileNameBuffer.data, mouse_dd, 0, edMax, edMax
editCommand   edit_box 150,450,121, 0xffffff, 0x6a9480, 0, 0xAABBCC, 0, edMaxSize, commandBuffer.data, mouse_dd, 0, edMax, edMax

opt1 option_box opt_gr1, 433,64, 6,12,0xd0d0ff, 0xff, 0x80ff,txt_filename,txt_filename.end-txt_filename
opt2 option_box opt_gr1, 433,107,6,12,0xd0d0ff, 0xff, 0x80ff,txt_command,txt_command.end-txt_command
opt_gr1 dd opt1
align 4
opt_grlist1 dd opt1,opt2,0 ;end option group

pb:
.value          dd 0
.left           dd 0
.top            dd 0
.width          dd 0
.height         dd 0
.style          dd 0
.min            dd 0
.max            dd 0
.back_color     dd 0
.progress_color dd 0
.frame_color    dd 0

fileNameBuffer:
.data: rb 512
.size: rd 1
fileNameBufferEnd:

commandBuffer:
.data: rb 512
.size: rd 1
commandBufferEnd:

align 16
i_end:
        rb 2048
thread_coords:
        rb 2048
thread_scale:
        rb 2048
thread_n_file:
        rb 2048
stacktop:
        sys_path rb 1024
        file_name:   rb 1024 ;4096
        library_path rb 1024
        plugin_path  rb 4096
        openfile_path rb 4096
        filename_area rb 256
mem:

