
//BOX_LIB

typedef Dword dword;
typedef unsigned short word;
typedef dword __stdcall dword_func(dword);
//typedef dword __stdcall dword3_func(dword,dword,dword);

dword  am__ = 0x0;
dword  bm__ = 0x0;

char aEdit_box_draw[]   = "edit_box";
char aEdit_box_key[]    = "edit_box_key";
char aEdit_box_mouse[]  = "edit_box_mouse";
char aVersion_ed[]      = "version_ed";

char aCheck_box_draw[]  = "check_box_draw";
char aCheck_box_mouse[] = "check_box_mouse";
char aVersion_ch[]      = "version_ch";

char aOption_box_draw[] = "option_box_draw";
char aVersion_op[]      = "version_op" ;

char aScrollbar_v_draw [] = "scrollbar_v_draw";
char aScrollbar_v_mouse[] = "scrollbar_v_mouse";
char aScrollbar_h_draw [] = "scrollbar_h_draw";
char aScrollbar_h_mouse[] = "scrollbar_h_mouse";
char aVersion_scrollbar[] = "version_scrollbar";

dword_func *edit_box_draw  =(dword_func*) &aEdit_box_draw;
dword_func *edit_box_key   =(dword_func*) &aEdit_box_key;
dword_func *edit_box_mouse =(dword_func*) &aEdit_box_mouse;

dword_func *scrollbar_v_draw  = (dword_func*) &aScrollbar_v_draw;
dword_func *scrollbar_v_mouse = (dword_func*) &aScrollbar_v_mouse;
dword_func *scrollbar_h_draw  = (dword_func*) &aScrollbar_h_draw;
dword_func *scrollbar_h_mouse = (dword_func*) &aScrollbar_h_mouse;


char lib_path[] = "/sys/lib/box_lib.obj";
dword lib_path_addr = (dword)lib_path;
dword dummy = 0;

struct edit_box{
dword width, 
	left,
	top, 
	color, 
	shift_color, 
	focus_border_color, 
	blur_border_color,
	text_color,
	max,
	text,
	mouse_variable,
	flags,
	size,
	pos,
	offset,
	cl_curs_x,
	cl_curs_y,
	shift,
	shift_old,
	ed_height,
	ed_char_width;
};

struct scroll_bar{
word w,
	x,
	h,
	y;
	dword btn_height,
	type,
	max_area,
	cur_area,
	position,
	bckg_col,
	frnt_col,
	line_col,
	redraw;
	word delta,
	delta2,
	r_size_x,
	r_start_x,
	r_size_y,
	r_start_y;
	dword m_pos,
	m_pos_2,
	m_keys,
	run_size,
	position2,
	work_size,
	all_redraw,
	ar_offset;
};

void load_edit_box()
{
	kol_struct_import *k = kol_cofflib_load(lib_path);

	if (k == NULL)
	{
		sprintf(debuf, "cannot load library %S", lib_path);
		rtlDebugOutString(debuf);
		return;
	}

	edit_box_draw  = (dword_func*)kol_cofflib_procload(k, aEdit_box_draw);
	edit_box_key   = (dword_func*)kol_cofflib_procload(k, aEdit_box_key);
	edit_box_mouse = (dword_func*)kol_cofflib_procload(k, aEdit_box_mouse);

	scrollbar_v_draw  = (dword_func*)kol_cofflib_procload(k, aScrollbar_v_draw);
	scrollbar_v_mouse = (dword_func*)kol_cofflib_procload(k, aScrollbar_v_mouse);
	scrollbar_h_draw  = (dword_func*)kol_cofflib_procload(k, aScrollbar_h_draw);
	scrollbar_h_mouse = (dword_func*)kol_cofflib_procload(k, aScrollbar_h_mouse);

	if (edit_box_draw == NULL || scrollbar_v_draw == NULL || scrollbar_h_draw == NULL)
		rtlDebugOutString("Some of EDITBOX functions have not been loaded!");
}
