#define MEMSIZE 4096*25

#include "../lib/io.h"
#include "../lib/gui.h"
#include "../lib/list_box.h"
#include "../lib/kfont.h"

#include "../lib/obj/box_lib.h"
#include "../lib/obj/libini.h"
#include "../lib/obj/libimg.h"
#include "../lib/obj/iconv.h"
#include "../lib/obj/proc_lib.h"

#include "../lib/patterns/simple_open_dialog.h"

#define TOOLBAR_H 34
#define TOOLBAR_ICON_WIDTH  26
#define TOOLBAR_ICON_HEIGHT 24

#define DEFAULT_EDITOR "/sys/tinypad"

#define INTRO_TEXT "This is a plain Text Reader.\nTry to open some text file."
#define VERSION "Text Reader v1.21a"
#define ABOUT "Idea: Leency, punk_joker
Code: Leency, Veliant, KolibriOS Team

Hotkeys:
Ctrl+O - open file
Ctrl+I - show file properties
Ctrl+Up - bigger font
Ctrl+Down - smaller font
Ctrl+Tab - select charset
Ctrl+E - edit current document
 
Press any key..."

char default_dir[] = "/rd/1";
od_filter filter2 = { 8, "TXT\0\0" };

scroll_bar scroll = { 15,200,398,44,0,2,115,15,0,0xeeeeee,0xBBBbbb,0xeeeeee,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1};
llist list;

proc_info Form;
char title[4196];

bool help_opened = false;
int charsets_menu_left = 0;

enum {
	OPEN_FILE,
	MAGNIFY_MINUS,
	MAGNIFY_PLUS,
	CHANGE_ENCODING,
	RUN_EDIT,
	SHOW_INFO,
	SHOW_FILE_PROPERTIES
};

int encoding;

#include "ini.h"
#include "prepare_page.h"


void InitDlls()
{
	load_dll(boxlib,    #box_lib_init,   0);
	load_dll(libio,     #libio_init,     1);
	load_dll(libimg,    #libimg_init,    1);
	load_dll(libini,    #lib_init,       1);
	load_dll(iconv_lib, #iconv_open,     0);
	load_dll(Proc_lib,  #OpenDialog_init,0);
}


void main()
{   	
	InitDlls();	
	OpenDialog_init stdcall (#o_dialog);
	LoadIniSettings();
	kfont.init(DEFAULT_FONT);
	Libimg_LoadImage(#skin, abspath("toolbar.png"));
	OpenFile(#param);
	list.no_selection = true;
	SetEventMask(EVM_REDRAW + EVM_KEY + EVM_BUTTON + EVM_MOUSE + EVM_MOUSE_FILTER);
	loop()
	{
		switch(WaitEvent())
		{
			case evMouse:
				HandleMouseEvent();
				break;
			case evKey:
				HandleKeyEvent();
				break;
			case evButton:
				HandleButtonEvent();
				break;
			case evReDraw:
				if (menu.list.cur_y) {
					encoding = menu.list.cur_y - 10;
					OpenFile(#param); 
					PreparePage();
					menu.list.cur_y = NULL;
				};
				draw_window();
		}
	}
}


void HandleButtonEvent()
{
	
	byte btn = GetButtonID();
	if (btn==1) {
		SaveIniSettings();
		ExitProcess();
	}
	btn-=10;
	switch(btn)
	{
		case OPEN_FILE:
			EventOpenFile();
			break;
		case SHOW_FILE_PROPERTIES:
			EventShowFileProperties();
			break;
		case MAGNIFY_PLUS:
			EventMagnifyPlus();
			break;
		case MAGNIFY_MINUS:
			EventMagnifyMinus();
			break;
		case CHANGE_ENCODING:
			EventChangeEncoding();
			break;
		case RUN_EDIT:
			EventRunEdit();
			break;
		case SHOW_INFO:
			EventShowInfo();
			break;
	}
}


void HandleKeyEvent()
{
	if (help_opened) {
		help_opened = false;
		DrawPage();
		return; 
	}
	GetKeys();
	if (key_scancode == SCAN_CODE_F1) {
		EventShowInfo();
		return;
	}
	if (key_modifier & KEY_LCTRL) || (key_modifier & KEY_RCTRL) {
		switch (key_scancode)
		{
			case SCAN_CODE_KEY_O:
				EventOpenFile();
				break;
			case SCAN_CODE_KEY_I:
				EventShowFileProperties();
				break;
			case SCAN_CODE_UP:
				EventMagnifyPlus();
				break;
			case SCAN_CODE_DOWN:
				EventMagnifyMinus();
				break;
			case SCAN_CODE_KEY_E:
				EventRunEdit();
				break;
			case SCAN_CODE_TAB:
				EventChangeEncoding();
				break;
		}
		return;
	}
	if (list.ProcessKey(key_scancode))
		DrawPage();
}


void HandleMouseEvent()
{
	mouse.get();
	list.wheel_size = 7;
	if (list.MouseScroll(mouse.vert)) {
		DrawPage(); 
		return; 
	}
	scrollbar_v_mouse (#scroll);
	if (list.first != scroll.position) {
		list.first = scroll.position;
		DrawPage(); 
	}
}


/* ----------------------------------------------------- */

void EventOpenFile()
{
	OpenDialog_start stdcall (#o_dialog);
	if (o_dialog.status) {
		OpenFile(#openfile_path);
		PreparePage();
	}
}

void EventShowFileProperties()
{
	char ss_param[4096];
	if (!param) return;
	sprintf(#ss_param, "-p %s", #param);
	io.run("/sys/File managers/Eolite", #ss_param);
}

void EventMagnifyPlus()
{
	kfont.size.pt++;
	if(!kfont.changeSIZE())
		kfont.size.pt--;
	else
		PreparePage();
}

void EventMagnifyMinus()
{
	kfont.size.pt--;
	if(!kfont.changeSIZE())
		kfont.size.pt++;
	else
		PreparePage();
}

void EventRunEdit()
{
	io.run(DEFAULT_EDITOR, #param);
}

void EventChangeEncoding()
{
	menu.selected = encoding + 1;
	menu.show(Form.left+5 + charsets_menu_left, Form.top+29+skin_height, 130,
		"UTF-8\nKOI8-RU\nCP1251\nCP1252\nISO8859-5\nCP866", 10);
}

void EventShowInfo() {
	help_opened = true;
	DrawBar(list.x, list.y, list.w, list.h, 0xFFFfff);
	WriteText(list.x + 10, list.y + 10, 10000001b, 0x555555, VERSION);
	WriteTextLines(list.x + 10, list.y+40, 10110000b, 0, ABOUT, 20);
}

/* ------------------------------------------- */


void OpenFile(dword f_path) 
{
	int tmp;
	if (ESBYTE[f_path]) {
		strcpy(#param, f_path);
		io.read(#param);
		strcpy(#title, #param);
		strcat(#title, " - Text Reader"); 
	}
	else {
		if (list.count) return;
		io.buffer_data = INTRO_TEXT;
		strcpy(#title, "Text Reader"); 
	}
	if (encoding!=CH_CP866) ChangeCharset(charsets[encoding], "CP866", io.buffer_data);
	list.KeyHome();
	list.ClearList();
}

void draw_window()
{
	#define PADDING 6
	#define TOOLBAR_BUTTON_WIDTH 26
	incn x;
	DefineAndDrawWindow(Form.left,Form.top,Form.width,Form.height,0x73,0,#title,0);
	GetProcessInfo(#Form, SelfInfo);
	if (Form.status_window>2) return;

	if (Form.width  < 200) { MoveSize(OLD,OLD,200,OLD); return; }
	if (Form.height < 200) { MoveSize(OLD,OLD,OLD,200); return; }
	
	DrawBar(0, 0, Form.cwidth, TOOLBAR_H - 1, 0xe1e1e1);
	DrawBar(0, TOOLBAR_H - 1, Form.cwidth, 1, 0x7F7F7F);
	
	x.n = 0;
	DrawToolbarButton(OPEN_FILE,       x.inc(8));
	DrawToolbarButton(SHOW_FILE_PROPERTIES, x.inc(TOOLBAR_BUTTON_WIDTH + PADDING));
	DrawToolbarButton(MAGNIFY_MINUS,   x.inc(TOOLBAR_BUTTON_WIDTH + PADDING + PADDING));
	DrawToolbarButton(MAGNIFY_PLUS,    x.inc(TOOLBAR_BUTTON_WIDTH - 1));
	DrawToolbarButton(CHANGE_ENCODING, x.inc(TOOLBAR_BUTTON_WIDTH + PADDING + PADDING));
		charsets_menu_left = x.n;
	DrawToolbarButton(RUN_EDIT,        x.inc(TOOLBAR_BUTTON_WIDTH + PADDING));
	DrawToolbarButton(SHOW_INFO,       Form.cwidth - 34);
	
	
	if ((Form.cwidth-scroll.size_x-1 == list.w) && 
		(Form.cheight-TOOLBAR_H == list.h) && 
		(list.count) 
	)
	{
		DrawPage(); 
	} else {
		PreparePage();
	}
	DrawRectangle(scroll.start_x, scroll.start_y, scroll.size_x, scroll.size_y-1, scroll.bckg_col);
}

void DrawPage()
{
	kfont.ShowBufferPart(list.x, list.y, list.w, list.h, list.first*list.item_h*list.w);
	DrawScroller();
}

void DrawToolbarButton(char image_id, int x)
{
	DefineButton(x+1, 6, TOOLBAR_ICON_WIDTH-2, TOOLBAR_ICON_HEIGHT-2, 10+image_id + BT_HIDE, 0);
	img_draw stdcall(skin.image, x, 5, TOOLBAR_ICON_WIDTH, TOOLBAR_ICON_HEIGHT, TOOLBAR_ICON_WIDTH-1*image_id, 0);
}

void DrawScroller()
{
	scroll.max_area = list.count;
	scroll.cur_area = list.visible;
	scroll.position = list.first;
	scroll.all_redraw = 0;
	scroll.start_x = list.x + list.w;
	scroll.start_y = list.y;
	scroll.size_y = list.h;
	scrollbar_v_draw(#scroll);
}