//11.03.12 - start!
//ver 2.3

#ifndef AUTOBUILD
	?include "lang.h--"
#endif

#define MEMSIZE 0xFE800
#include "..\lib\mem.h"
#include "..\lib\strings.h"
#include "..\lib\io.h"
#include "..\lib\list_box.h"
#include "..\lib\gui.h"

#include "..\lib\obj\box_lib.h"
#include "..\lib\obj\proc_lib.h"
#include "..\lib\obj\libini.h"

#include "..\lib\patterns\select_list.h"
#include "..\lib\patterns\simple_open_dialog.h"

#include "ui_elements_preview.h"

//===================================================//
//                                                   //
//                       DATA                        //
//                                                   //
//===================================================//

#ifdef LANG_RUS
	?define WINDOW_HEADER "����ன�� ��ଫ����"
	?define T_SKINS       "�⨫� ����"
	?define T_WALLPAPERS  "����"
	?define T_SELECT_FOLDER "����� �����"
	?define MENU_LIST "������ 䠩�   Enter\n������� 䠩�     Del"
	?define T_PICTURE_MODE " ��������� ���⨭�� "
	?define T_CHECKBOX_STRETCH "��������"
	?define T_CHECKBOX_TILED "��������"
	?define T_UPDATE_DOCK "��������� Dock-������"
#else
	?define WINDOW_HEADER "Appearance"
	?define T_SKINS       "Skins"
	?define T_WALLPAPERS  "Wallpapers"
	?define T_SELECT_FOLDER "Select folder"
	?define MENU_LIST "Open file      Enter\nDelete file      Del"
	?define T_PICTURE_MODE " Picture Mode "
	?define T_CHECKBOX_STRETCH "Stretch"
	?define T_CHECKBOX_TILED "Tiled"
	?define T_UPDATE_DOCK "Update Dock"
#endif

#define PANEL_H 40
#define LP 10 //LIST_PADDING
char skins_folder_path[4096];
char wallp_folder_path[4096];

signed int active_skin=-1, active_wallpaper=-1;
enum { 
	SKINS=2, 
	WALLPAPERS,
	BTN_SELECT_WALLP_FOLDER };

char folder_path[4096];
char cur_file_path[4096];
char cur_skin_path[4096];
char temp_filename[4096];
int files_mas[400];

int cur;

proc_info Form;
block skp;

_tabs tabs = { SKINS, LP, LP, NULL, NULL };

checkbox update_docky = { T_UPDATE_DOCK, false };

char default_dir[] = "/rd/1";
od_filter filter2 = { 8, "TXT\0\0" };

checkbox optionbox_stretch = { T_CHECKBOX_STRETCH, true };
checkbox optionbox_tiled = { T_CHECKBOX_TILED, false };

//===================================================//
//                                                   //
//                       CODE                        //
//                                                   //
//===================================================//

void GetRealFolderPathes()
{
	char real_skin_path[4096];
	SetCurDir("/kolibrios");
	GetCurDir(#real_skin_path, sizeof(real_skin_path));
	sprintf(#skins_folder_path, "%s/res/skins", #real_skin_path);
	sprintf(#wallp_folder_path, "%s/res/wallpapers", #real_skin_path);
}

void main()
{   
	int id, mouse_clicked;

	GetRealFolderPathes();

	load_dll(boxlib, #box_lib_init,0);
	load_dll(libini, #lib_init,1);
	load_dll(Proc_lib, #OpenDialog_init,0);
	o_dialog.type = 2; //select folder
	OpenDialog_init stdcall (#o_dialog);

	EventTabClick(SKINS);

	SetEventMask(EVM_REDRAW + EVM_KEY + EVM_BUTTON + EVM_MOUSE + EVM_MOUSE_FILTER);
	loop() switch(WaitEvent()) 
	{
	  	case evMouse:
			SelectList_ProcessMouse();

			if (tabs.active_tab == SKINS) {
				edit_box_mouse stdcall (#edit_cmm);
				edit_box_mouse stdcall (#edit_st);
			}

	  		if (mouse.pkm)&&(select_list.MouseOver(mouse.x, mouse.y)) {
	  			select_list.ProcessMouse(mouse.x, mouse.y);
				SelectList_Draw();
				EventSetNewCurrent();
	  			menu.show(Form.left+mouse.x, Form.top+mouse.y+skin_height, 146, MENU_LIST, 10); 
	  		}
	  		break;

		case evButton:
			id=GetButtonID();
			if (id==1) EventExit();
			if (id==SKINS) EventTabClick(SKINS);
			if (id==WALLPAPERS) EventTabClick(WALLPAPERS);
			if (id==BTN_SELECT_WALLP_FOLDER) EventSelectWallpFolder();
			checkbox1.click(id);
			spinbox1.click(id);
			if (update_docky.click(id)) EventUpdateDocky();
			if (!optionbox_stretch.checked) && (optionbox_stretch.click(id)) EventSetWallpMode_Stretch();
			if (!optionbox_tiled.checked) && (optionbox_tiled.click(id)) EventSetWallpMode_Tiled();
			break;
	  
		case evKey:
			GetKeys(); 
			if (select_list.ProcessKey(key_scancode)) EventApply();
			if (key_scancode==SCAN_CODE_ENTER) EventOpenFile();
			if (key_scancode==SCAN_CODE_TAB) {
				if (tabs.active_tab==SKINS) EventTabClick(WALLPAPERS); 
				else EventTabClick(SKINS);				
			}
			if (key_scancode==SCAN_CODE_DEL) EventDeleteFile();

			if (! edit_cmm.flags & ed_focus) && (! edit_st.flags & ed_focus)
			for (id=select_list.cur_y+1; id<select_list.count; id++)
			{
				strcpy(#temp_filename, io.dir.position(files_mas[id]));
				if (temp_filename[0]==key_ascii) || (temp_filename[0]==key_ascii-32)
				{
					select_list.cur_y = id - 1;
					select_list.KeyDown();
					EventApply();
					break;
				}
			}

			if (tabs.active_tab == SKINS) {
				EAX = key_ascii << 8;
				edit_box_key stdcall (#edit_cmm);
				edit_box_key stdcall (#edit_st);				
			}
			break;
		 
		 case evReDraw:		
			draw_window();
	 		if (menu.cur_y) {
				if (menu.cur_y == 10) EventOpenFile();
				if (menu.cur_y == 11) EventDeleteFile();
				menu.cur_y = 0;
			};
   }
}

void draw_window()
{
	system.color.get();	
	DefineAndDrawWindow(screen.width-600/2,80,630,504+skin_height,0x34,system.color.work,WINDOW_HEADER,0);
	GetProcessInfo(#Form, SelfInfo);
	IF (Form.status_window>=2) return;
	DrawWindowContent();
}

void DrawWindowContent()
{
	int id;
	int list_w;

	system.color.get();	

	if (tabs.active_tab == SKINS) list_w=250; else list_w=350;

	tabs.w = Form.cwidth-LP-LP;
	tabs.h = Form.cheight-LP-LP;
	tabs.draw_wrapper();
	tabs.draw_button(tabs.x+TAB_PADDING, SKINS, T_SKINS);	
	tabs.draw_button(strlen(T_SKINS)*8+tabs.x+TAB_PADDING+TAB_PADDING, WALLPAPERS, T_WALLPAPERS);

	id = select_list.cur_y;
	SelectList_Init(
		tabs.x+TAB_PADDING,
		tabs.y+TAB_HEIGHT+TAB_PADDING, 
		list_w, 
		tabs.h - TAB_PADDING - TAB_PADDING - TAB_HEIGHT, 
		false
		);
	select_list.cur_y = id;

	skp.set_size(
		select_list.x + select_list.w + TAB_PADDING + scroll1.size_x + 20,
		select_list.y + 30 + 50,
		list_w,
		230 //select_list.h - 50 - 50
	);

	SelectList_Draw();
	SelectList_DrawBorder();
	//DrawWideRectangle(0, 0, Form.cwidth, Form.cheight, LP, system.color.work);

	if (tabs.active_tab == SKINS)
	{
		update_docky.draw(skp.x, select_list.y+15);
		DrawFrame(skp.x, skp.y, skp.w, skp.h, " Components Preview ");
		DrawUiElementsPreview(skp.x+20, skp.y, skp.h);
	}
	if (tabs.active_tab == WALLPAPERS)
	{
		skp.x -= TAB_PADDING + 3;
		DrawStandartCaptButton(skp.x, select_list.y, BTN_SELECT_WALLP_FOLDER, T_SELECT_FOLDER);
		DrawFrame(skp.x, select_list.y+50, 180, 80, T_PICTURE_MODE);
		optionbox_stretch.draw(skp.x+14, select_list.y+70);
		optionbox_tiled.draw(skp.x+14, select_list.y+97);
	}
}


void Open_Dir()
{
	int j;
	select_list.count = 0;
	if(io.dir.buffer)free(io.dir.buffer);
	io.dir.load(#folder_path,DIR_ONLYREAL);
	for (j=0; j<io.dir.count; j++)
	{
		strcpy(#temp_filename, io.dir.position(j));
		strlwr(#temp_filename);
		if (tabs.active_tab==SKINS) {
			if (strcmpi(#temp_filename+strlen(#temp_filename)-4,".skn")!=0) continue;
		}
		if (tabs.active_tab==WALLPAPERS) {
			if (strcmpi(#temp_filename+strlen(#temp_filename)-4,".png")!=0)
			&& (strcmpi(#temp_filename+strlen(#temp_filename)-4,".jpg")!=0) 
			&& (strcmpi(#temp_filename+strlen(#temp_filename)-5,".jpeg")!=0)
			&& (strcmpi(#temp_filename+strlen(#temp_filename)-4,".gif")!=0) continue;
		}
		cur = select_list.count;
		files_mas[cur]=j;
		if (!strcmpi("default.skn",#temp_filename)) files_mas[0]><files_mas[select_list.count];
		select_list.count++;
	}
}

void SelectList_DrawLine(dword i)
{
	int yyy, list_last;

	cur = select_list.first + i;
	strcpy(#temp_filename, io.dir.position(files_mas[cur]));
	temp_filename[strlen(#temp_filename)-4] = 0;
	yyy = i*select_list.item_h+select_list.y;
	
	if (select_list.cur_y-select_list.first==i)
	{
		DrawBar(select_list.x, yyy, select_list.w, select_list.item_h, system.color.work_button);
		WriteText(select_list.x+12,yyy+select_list.text_y,select_list.font_type,system.color.work_button_text, #temp_filename);
	}
	else
	{
		DrawBar(select_list.x,yyy,select_list.w, select_list.item_h, 0xFFFfff);
		WriteText(select_list.x+12,yyy+select_list.text_y,select_list.font_type,0, #temp_filename);
	}
}

void SelectList_LineChanged() 
{
	EventApply();
}

//===================================================//
//                                                   //
//                     EVENTS                        //
//                                                   //
//===================================================//

void EventTabClick(int N)
{
	tabs.click(N);
	if (tabs.active_tab == SKINS) 
	{
		active_wallpaper = select_list.cur_y;
		strcpy(#folder_path, #skins_folder_path);
		select_list.ClearList();
		Open_Dir();
		if (!select_list.count) notify("'No skins were found' -E");
		select_list.cur_y = active_skin;
	}
	if (tabs.active_tab == WALLPAPERS)
	{
		active_skin = select_list.cur_y;
		strcpy(#folder_path, #wallp_folder_path);
		select_list.ClearList();
		Open_Dir();
		if (!select_list.count) notify("'No wallpapers were found' -E");
		select_list.cur_y = active_wallpaper;
	}
	if (select_list.cur_y>select_list.visible) select_list.first=select_list.cur_y; select_list.CheckDoesValuesOkey();
	if (select_list.w) draw_window();
}

void EventDeleteFile()
{
	io.del(#cur_file_path);
	Open_Dir();
	EventApply();
}

void EventSetNewCurrent()
{
	cur = select_list.cur_y;
	sprintf(#cur_file_path,"%s/%s",#folder_path,io.dir.position(files_mas[cur]));
}

void EventSelectWallpFolder()
{
	OpenDialog_start stdcall (#o_dialog);
	if (o_dialog.status) {
		strcpy(#wallp_folder_path, #opendir_path);
		EventTabClick(WALLPAPERS);
	}
}

void EventSetWallpMode_Stretch()
{
	optionbox_tiled.checked = false;
	optionbox_tiled.redraw();
	EventApply();
}

void EventSetWallpMode_Tiled()
{
	optionbox_stretch.checked = false;
	optionbox_stretch.redraw();
	EventApply();
}

#include "..\lib\patterns\restart_process.h"
void EventApply()
{
	char kivpath[4096+10];
	EventSetNewCurrent();
	if (tabs.active_tab==SKINS)
	{
		cur = select_list.cur_y;
		SetSystemSkin(#cur_file_path);
		SelectList_Draw();
		strcpy(#cur_skin_path, #cur_file_path);
		EventUpdateDocky();
	} 
	if (tabs.active_tab==WALLPAPERS)
	{
		SelectList_Draw();
		if (optionbox_stretch.checked) strcpy(#kivpath, "\\S__");
		if (optionbox_tiled.checked) strcpy(#kivpath, "\\T__");
		strcat(#kivpath, #cur_file_path);
		RunProgram("/sys/media/kiv", #kivpath);
	}
}

void EventUpdateDocky()
{
	if (!update_docky.checked) return;
	KillProcessByName("@docky", MULTIPLE);
	RunProgram("/sys/@docky",NULL);
	pause(50);
	ActivateWindow(GetProcessSlot(Form.ID));
}

void EventOpenFile()
{
	if (tabs.active_tab==SKINS) RunProgram("/sys/skincfg", #cur_file_path);
	if (tabs.active_tab==WALLPAPERS) RunProgram("/sys/media/kiv", #cur_file_path);
}

_ini ini = { "/sys/settings/eskin.ini", "main" };
void EventExit()
{
	if (cur_skin_path) ini.SetString("skin", #cur_skin_path, strlen(#cur_skin_path));
	ExitProcess();
}

stop:
