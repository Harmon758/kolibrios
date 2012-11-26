//lev
//:dword boxlib = #abox_lib;
:char abox_lib[]="/sys/lib/box_lib.obj";

:char aboxlib_init[]  = "lib_init";
:char aScrollbar_v_draw	= "scrollbar_v_draw";
:char aScrollbar_v_mouse	= "scrollbar_v_mouse";
:char aScrollbar_h_draw	= "scrollbar_h_draw";
:char aScrollbar_h_mouse	= "scrollbar_h_mouse";
:char aVersion_scrollbar	= "version_scrollbar";

dword boxlib_init = #aboxlib_init;
dword scrollbar_v_draw	= #aScrollbar_v_draw;
dword scrollbar_v_mouse	= #aScrollbar_v_mouse;
dword scrollbar_h_draw	= #aScrollbar_h_draw;
dword scrollbar_h_mouse	= #aScrollbar_h_mouse;
:dword version_scrollbar	= #aVersion_scrollbar;

dword  sc_am__ = 0x0;
dword  sc_bm__ = 0x0;

struct scroll_bar
{
//scroll_bar:
word size_x,//		equ [edi]
start_x,//		equ [edi+2]
size_y,//		equ [edi+4]
start_y;//		equ [edi+6]
dword btn_height, //		equ [edi+8]
type,//			equ [edi+12]
max_area,//		equ [edi+16]
cur_area,//		equ [edi+20]
position,//		equ [edi+24]
bckg_col,//		equ [edi+28]
frnt_col,//		equ [edi+32]
line_col,//		equ [edi+36]
redraw;//		equ [edi+40]
word delta,//		equ [edi+44]
delta2,//		equ [edi+46]
r_size_x,//		equ [edi+48]
r_start_x,//	equ [edi+50]
r_size_y,//		equ [edi+52]
r_start_y;//	equ [edi+54]
dword m_pos,//		equ [edi+56]
m_pos_2,//		equ [edi+60]
m_keys,//		equ [edi+64]
run_size,//		equ [edi+68]
position2,//	equ [edi+72]
work_size,//	equ [edi+76]
all_redraw,//	equ [edi+80]
ar_offset;//	equ [edi+84]
};

/*scroll_bar scroll1 = {
18,//word size_x,
200, //start_x,
398, //size_y,
44,//start_y;
18,//dword btn_height,
0,//type,
115,//max_area, (100+cur_area)
15,//cur_area,
0,//position,
0xeeeeee,//bckg_col,
0xD2CED0,//frnt_col,
0x555555,//line_col,
0,//redraw;
0,//word delta,
0,//delta2,
0,//r_size_x,
0,//r_start_x,
0,//r_size_y,
0,//r_start_y;
0,//dword m_pos,
0,//m_pos_2,
0,//m_keys,
0,//run_size,
0,//position2,
0,//work_size,
1,//all_redraw,
1//ar_offset;
};*/

//btn_height - ���� ������� ������ (����� � �ࠢ�� ��� ��ਧ. � ������ � ������ ��� ����.)
//type - type - 0-1-2, ��⠫�� �룫��� ��� 0.
//max_area - ������� ���ᨬ��쭠�, ���㠫쭮 ����� �� ������ � �⮡ࠦ����� �������, �.�. ��砩 ��⨢���� ��������� (���� ���ᨬ���� ࠧ��� ���㬥��)
//cur_area - ࠧ��� ���㭪�//������� �⮡ࠦ����� (����� ���� ���㬥�� ������ � �࠭)
//position - ������ ���㭪� �����塞�� �� 0 �� ���祭�� (max_area-cur_area)
//bckg_col - 梥� ����७���, �ਬ������ ��� �����⥩ ����� ���㭪�� � �������
//frnt_col - 梥� ���㦭�, �ਬ������ ��� �����⥩ �⫨��� �� �ਬ��塞�� � bckg_col
//line_col - 梥� ����� � ��५�� �� �������
//redraw - �������� ����室����� ����ᮢ�� �ࠢ�塞�� ������ ���뢠� ��������� position � cur_area
//delta2 - �������� ��墠� 䮪�� �஫���஬ (㤥ঠ��� ���㭪�)
//all_redraw - 䫠� ��⠭��������� � 1 ��� ����ᮢ�� �ᥣ� ���������, �᫨ ���⠢��� 0, � ����ᮢ�� �ந�室�� ⮫쪮 ��� ������ ���㭪�. ������� ��� �᪮७�� ���ᮢ��, ��᪮��� �� �� ��� ����� �ॡ���� �ਭ㤨�쭮 ���ᮢ뢠�� �� ��� ���������. ����� ��뢠���� �� ᮡ��� ��� �ᥣ�� ���ᮢ뢠���� ᠬ�� ��������⮬ � ���祭��� 0.
//ar_offset - ����稭� ᬥ饭�� �� ������⭮� ����⨨ ������� ������.
