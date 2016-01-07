#define MEMSIZE 4096*30

#include "../lib/font.h"
#include "../lib/io.h"
#include "../lib/gui.h"
#include "../lib/list_box.h"
#include "../lib/menu.h"
#include "../lib/obj/box_lib.h"
#include "../lib/obj/libini.h"
#include "../lib/obj/iconv.h"
#include "../lib/obj/proc_lib.h"
#include "../lib/obj/http.h"
#include "../lib/patterns/libimg_load_skin.h"
#include "../lib/patterns/simple_open_dialog.h"
#include "../lib/patterns/history.h"

#include "../browser/http_downloader.h"
#include "parse_address.h"

char default_dir[] = "/rd/1";
od_filter filter2 = {0,0};

char accept_language[]= "Accept-Language: ru\n";

#define TOOLBAR_H 36
#define TOOLBAR_ICON_WIDTH  26
#define TOOLBAR_ICON_HEIGHT 24

#define DEFAULT_EDITOR "/sys/tinypad"
#define DEFAULT_PREVIEW_PATH "/tmp0/1/aelia_preview.txt"

//ATTENTION: each page must have '\0' character at the end of the file
char buidin_page_home[] = FROM "buidin_pages\\home.htm";
char buidin_page_about[] = FROM "buidin_pages\\about.htm";
char buidin_page_not_found[] = FROM "buidin_pages\\not_found.htm";

#define UML 4096*2

scroll_bar scroll = { 15,200,398,44,0,2,115,15,0,0xeeeeee,0xBBBbbb,0xeeeeee,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1};
llist list;

proc_info Form;
char title[4196];

enum {
	OPEN_FILE,
	MAGNIFY_MINUS,
	MAGNIFY_PLUS,
	CHANGE_ENCODING,
	RUN_EDIT,
	SHOW_INFO,
	GO_BACK,
	GO_FORWARD,
};

char address[UML]="http://";
int	mouse_address_box;
edit_box address_box = {250,56,34,0xffffff,0x94AECE,0xffffff,0xffffff,0,UML,#address,#mouse_address_box,2,19,19};

#include "ini.h"
#include "gui.h"
#include "prepare_page.h"

void InitDlls()
{
	load_dll(boxlib,    #box_lib_init,   0);
	load_dll(libHTTP,   #http_lib_init,  1);
	load_dll(libio,     #libio_init,     1);
	load_dll(libimg,    #libimg_init,    1);
	//load_dll(libini,    #lib_init,       1);
	load_dll(iconv_lib, #iconv_open,     0);
	load_dll(Proc_lib,  #OpenDialog_init,0);
}


void main()
{   	
	InitDlls();	
	OpenDialog_init stdcall (#o_dialog);
	label.init(DEFAULT_FONT);
	Libimg_LoadImage(#skin, abspath("toolbar.png"));
	LoadIniSettings();
	list.no_selection = true;
	SetEventMask(10000000000000000000000001100111b);
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
					debugln("evReDraw: charset changed");
					EventOpenAddress(history.current());
				}
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
	switch(btn-10)
	{
		case GO_BACK:
			EventGoBack();
			break;
		case GO_FORWARD:
			EventGoForward();
			break;
		case OPEN_FILE:
			EventOpenDialog();
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
	GetKeys();
	switch (key_scancode) 
	{
		case 059:
			EventShowInfo();
			return;
		case SCAN_CODE_ENTER:
			EventOpenAddress(#address);
			return;
		case SCAN_CODE_BS:
			if (! address_box.flags & 0b10) {
				EventGoBack();
				return;
			}
	}
	if (key_modifier & KEY_LCTRL) || (key_modifier & KEY_RCTRL) {
		switch (key_scancode)
		{
			case 024: //O
				EventOpenDialog();
				break;
			case SCAN_CODE_UP:
				EventMagnifyPlus();
				break;
			case SCAN_CODE_DOWN:
				EventMagnifyMinus();
				break;
			case 018: //E
				EventRunEdit();
				break;
			case 035: //H
				EventShowHistory();
				break;
			case SCAN_CODE_TAB:
				EventChangeEncoding();
				break;
		}
		return;
	}
	if (list.ProcessKey(key_scancode)) {
		DrawPage();
		return;
	}
	if (key_ascii != 0x0d)
	&& (key_ascii != ASCII_KEY_PGDN) 
	&& (key_ascii != ASCII_KEY_PGUP) {
		EAX = key_ascii << 8; 
		edit_box_key stdcall(#address_box);
	}
}


void HandleMouseEvent()
{
	edit_box_mouse stdcall (#address_box);
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

void EventOpenDialog()
{
	OpenDialog_start stdcall (#o_dialog);
	if (o_dialog.status) EventOpenAddress(#openfile_path);
}

void EventOpenAddress(dword _new_address)
{
char temp[UML];
	if (!ESBYTE[_new_address]) return;
	debugln("====================================");
	debug("address: ");
	debugln(_new_address);
	strlcpy(#address, _new_address, UML);
	strlwr(#address);
	DrawAddressBox();

	/*
	There could be several possible types of addresses:
	- build in page
	- local file
	- url
	So we need to detect what incoming address is
	and then halndle it in the propper way.
	*/

	io.buffer_data = 0;

	// - build in page
	if (!strncmp(#address,"aelia:",6)) {
		debugln("this is buildin page");
		if (!strcmp(#address,"aelia:home")) io.buffer_data = #buidin_page_home;
		if (!strcmp(#address,"aelia:about")) io.buffer_data = #buidin_page_about;
		if (!strcmp(#address,"aelia:history")) io.buffer_data = MakePageWithHistory();
	}
	// - local file
	else if (check_is_the_adress_local(#address)==true) {
		debugln("this is local address");
		io.read(#address);
	}
	// - url
	else {
		debugln("this is url");
		if (strncmp(#address,"http://",7)!=0) {
			strcpy(#temp, "http://");
			strlcpy(#temp, #address, UML);
			strlcpy(#address, #temp, UML);
			DrawAddressBox();
		}
		if (!downloader.Start(#address)) {
			downloader.Stop();
		} else {		
			while (downloader.state!=STATE_COMPLETED)
			{ 
				downloader.MonitorProgress(); 
				if (downloader.data_full_size>0)
					DrawProgress(STEP_2_COUNT_PAGE_HEIGHT-STEP_1_DOWNLOAD_PAGE*downloader.data_downloaded_size/downloader.data_full_size); 
				else
					DrawProgress(STEP_2_COUNT_PAGE_HEIGHT-STEP_1_DOWNLOAD_PAGE/2);
			}
			io.buffer_data = downloader.bufpointer;
		}
	}

	if (!io.buffer_data) {
		debugln("page not found");
		io.buffer_data = #buidin_page_not_found;
	}

	history.add(#address);

	/* 
	Great! So we have the page in our buffer.
	We don't know is it a plain text or html.
	So we need to parse it and draw.
	*/

	list.KeyHome();
	list.ClearList();
	PreparePage();
}

void EventMagnifyPlus()
{
	label.size.pt++;
	if(!label.changeSIZE())
		label.size.pt--;
	else
		PreparePage();
}

void EventMagnifyMinus()
{
	label.size.pt--;
	if(!label.changeSIZE())
		label.size.pt++;
	else
		PreparePage();
}

void EventRunEdit()
{
	if (check_is_the_adress_local(history.current())==true) {
		io.run(DEFAULT_EDITOR, history.current());
	}
	else {
		//io.write(strlen(io.buffer_data), io.buffer_data, DEFAULT_PREVIEW_PATH); // <--- doesn't work, smth odd, need to check
		WriteFile(strlen(io.buffer_data), io.buffer_data, DEFAULT_PREVIEW_PATH);
		io.run(DEFAULT_EDITOR, DEFAULT_PREVIEW_PATH);
	}
}

void EventChangeEncoding()
{
	menu.selected = encoding + 1;
	menu.show(Form.left+Form.cwidth-97,Form.top+TOOLBAR_H+skin_height-6, 130, "UTF-8\nKOI8-RU\nCP1251\nCP1252\nISO8859-5\nCP866", 10);
}

void EventShowInfo() {
	EventOpenAddress("aelia:about");
}

void EventShowHistory()
{
	EventOpenAddress("aelia:history");
}

void EventGoBack()
{
	if (history.back()) EventOpenAddress(history.current());
}

void EventGoForward()
{
	if (history.forward()) EventOpenAddress(history.current());
}

/* ------------------------------------------- */


void draw_window()
{
	DefineAndDrawWindow(Form.left,Form.top,Form.width,Form.height,0x73,0,#title);
	GetProcessInfo(#Form, SelfInfo);
	if (Form.status_window>2) return;

	if (Form.width  < 200) { MoveSize(OLD,OLD,200,OLD); return; }
	if (Form.height < 200) { MoveSize(OLD,OLD,OLD,200); return; }
	
	DrawBar(0, 0, Form.cwidth, TOOLBAR_H - 2, 0xe1e1e1);
	DrawBar(0, TOOLBAR_H - 2, Form.cwidth, 1, 0xcecece);
	DrawBar(0, TOOLBAR_H - 1, Form.cwidth, 1, 0x7F7F7F);
	
	DrawToolbarButton(GO_BACK,         8);
	DrawToolbarButton(GO_FORWARD,      33);
	DrawToolbarButton(OPEN_FILE,       68);
	DrawToolbarButton(MAGNIFY_PLUS,    Form.cwidth - 161);
	DrawToolbarButton(MAGNIFY_MINUS,   Form.cwidth - 136);
	DrawToolbarButton(CHANGE_ENCODING, Form.cwidth - 102);
	DrawToolbarButton(RUN_EDIT,        Form.cwidth - 68);
	DrawToolbarButton(SHOW_INFO,       Form.cwidth - 34);

	DrawAddressBox();

	if ((Form.cwidth-scroll.size_x-1 == list.w) && 
		(Form.cheight-TOOLBAR_H == list.h) && 
		(list.count) 
	) 
	{
		DrawPage();
	}
	else
	{
		if (!label.raw) {                           //this code need to be run
			if (param) EventOpenAddress(#param);    //only once at browser sturtup
			else EventOpenAddress("aelia:home");
		}
		else PreparePage();
	}

	DrawRectangle(scroll.start_x, scroll.start_y, scroll.size_x, scroll.size_y-1, scroll.bckg_col);
}

void DrawPage()
{
	list.CheckDoesValuesOkey();
	if (list.count) _PutImage(list.x,list.y,list.w,list.h,list.first*list.item_h*list.w*3 + label.raw);
	DrawScroller();
}

void DrawAddressBox()
{
	address_box.left = 97;
	address_box.top = 11;
	address_box.width = Form.cwidth - address_box.left - 172;
	DrawRectangle(address_box.left-4, address_box.top-5, address_box.width+6, 23, 0x8C8C8C);
	DrawWideRectangle(address_box.left-3, address_box.top-3, address_box.width+5, 21, 4, address_box.color);
	address_box.size = address_box.pos = address_box.shift = address_box.shift_old = strlen(#address);
	address_box.offset = 0;
	edit_box_draw stdcall(#address_box);
}