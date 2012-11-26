//BOX_LIB
dword boxlib = #aEdit_box_lib; //Asper
char aEdit_box_lib[22]="/sys/lib/box_lib.obj\0"; //Asper

dword edit_box_draw = #aEdit_box_draw;
dword edit_box_key  = #aEdit_box_key;
dword edit_box_mouse = #aEdit_box_mouse;
dword version_ed     = #aVersion_ed;

dword  am__ = 0x0;
dword  bm__ = 0x0;

char aEdit_box_draw[9]  = "edit_box\0";
char aEdit_box_key[13] = "edit_box_key\0";
char aEdit_box_mouse[15] = "edit_box_mouse\0";
char aVersion_ed[11] = "version_ed\0";

char aCheck_box_draw  [15] = "check_box_draw\0";
char aCheck_box_mouse [16] = "check_box_mouse\0";
char aVersion_ch      [11] = "version_ch\0";

char aOption_box_draw [16] = "option_box_draw\0";
char aOption_box_mouse[17] = "option_box_mouse\0";
char aVersion_op      [11] = "version_op\0" ;


struct edit_box{
dword width, left, top, color, shift_color, focus_border_color, blur_border_color,
text_color, max, text, mouse_variable, flags, size, pos, offset, cl_curs_x, cl_curs_y, shift, shift_old;
};

//ed_width        equ [edi]               ;�ਭ� ���������
//ed_left         equ [edi+4]             ;��������� �� �� �
//ed_top          equ [edi+8]             ;��������� �� �� �
//ed_color        equ [edi+12]            ;梥� 䮭� ���������
//shift_color     equ [edi+16]            ;=0x6a9480
//ed_focus_border_color   equ [edi+20]    ;梥� ࠬ�� ���������
//ed_blur_border_color    equ [edi+24]    ;梥� �� ��⨢���� ���������
//ed_text_color   equ [edi+28]            ;梥� ⥪��
//ed_max          equ [edi+32]            ;���-�� ᨬ����� ����� ����� ���ᨬ��쭮 �����
//ed_text         equ [edi+36]            ;㪠��⥫� �� ����
//ed_flags        equ [edi+40]            ;䫠��
//ed_size equ [edi+42]                    ;���-�� ᨬ�����
//ed_pos  equ [edi+46]                    ;������ �����
//ed_offset       equ [edi+50]            ;ᬥ饭��
//cl_curs_x       equ [edi+54]            ;�।���� ���न��� ����� �� �
//cl_curs_y       equ [edi+58]            ;�।���� ���न��� ����� �� �
//ed_shift_pos    equ [edi+62]            ;��������� �����
//ed_shift_pos_old equ [edi+66]           ;��஥ ��������� �����
