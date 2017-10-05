#ifndef AUTOBUILD
#include "lang.h--"
#endif

#define MEMSIZE 0x23E80
#include "..\lib\kolibri.h" 
#include "..\lib\strings.h" 
#include "..\lib\mem.h" 
#include "..\lib\file_system.h"
#include "..\lib\dll.h"
#include "..\lib\gui.h"

#include "..\lib\obj\libio.h"
#include "..\lib\obj\libini.h"
#include "..\lib\obj\box_lib.h"

#include "..\lib\patterns\restart_process.h"

#ifdef LANG_RUS
	?define WINDOW_TITLE "����ன�� ������ ����� � ����"
    ?define TASK_FRAME_T " ������ ����� "
	?define DOCK_FRAME_T " ��� "
	?define MIN_LEFT_BUTTON "������ ����� ᫥��"
	?define MIN_RIGHT_BUTTON "������ ����� �ࠢ�"
	?define SOFTEN_UP   "����������� ᢥ���"
	?define SOFTEN_DOWN "����������� ᭨��"
	?define CLOCK    "����"
	?define CPU_USAGE "����㧪� ��"
	?define CHANGE_LANG "��� �����"
	?define MENU_BUTTON "������ ����"
	?define PANEL_HEIGHT "���� ������"
	?define SOFTEN_HEIGHT "���� ᣫ��������"
	?define BUTTON_OFFSET "���� ����� ������"
	?define FSIZE "����� ������"
	?define ASHOW "�� ��뢠��"
	?define CHANGE_POS "������ �� ����ࠦ���� ��� ᬥ�� ����樨"
#else
	?define WINDOW_TITLE "Taskbar and Docky configuration"
    ?define TASK_FRAME_T " Taskbar "
	?define DOCK_FRAME_T " Docky "
	?define MIN_LEFT_BUTTON "Min Left Button"
	?define MIN_RIGHT_BUTTON "Min Right Button"
	?define SOFTEN_UP   "Soften Up"
	?define SOFTEN_DOWN "Soften Down"
	?define CLOCK    "Clock"
	?define CPU_USAGE "Cpu Usage"
	?define CHANGE_LANG "Change Language"
	?define MENU_BUTTON "Menu Button"
	?define PANEL_HEIGHT "Panel Height"
	?define SOFTEN_HEIGHT "Soften Height"
	?define BUTTON_OFFSET "Button Offset"
	?define FSIZE "Full width"
	?define ASHOW "Always show"
	?define CHANGE_POS "Click on image to change position"
#endif


frame taskbar_frame = { 0, NULL, 10, NULL, 16, NULL, 0xFFFfff, 1, TASK_FRAME_T, 0, 1, 12, 0x000111, 0xCCCccc };
frame docky_frame   = { 0, NULL, 10, NULL, NULL, NULL, 0xFFFfff, 1, DOCK_FRAME_T, 0, 1, 12, 0x000111, 0xCCCccc };

char taskbar_ini_path[] = "/sys/settings/taskbar.ini";
_ini taskbar_flags_ini = { #taskbar_ini_path, "Flags" };
_ini taskbar_vars_ini = { #taskbar_ini_path, "Variables" };

_ini docky_ini = { "/sys/settings/docky.ini", "@" };

unsigned char panels_img_data[] = FROM "panels_image.raw";
raw_image panels_img = { 37, 27, #panels_img_data };

proc_info Form;

word dkFsize;
byte dkLocation, dkAshow;

byte tbAttachment, tbPanelHeight, tbSoftenHeight, tbButtonOffset,
     tbSoftenUp, tbSoftenDown, tbMinLeftButton, tbMinRightButton, tbMenuButton,
     tbRunApplButton, tbClnDeskButton, tbClock, tbCpuUsage, tbChangeLang;

enum {
	TASKBAR,
	DOCKY,
	ALL
};


void main()
{
	dword id, key;

	load_dll(libini, #lib_init,1);
	load_dll(boxlib, #box_lib_init,0);

	LoadCfg();

	loop() switch(WaitEvent())
	{
		case evButton: 
				id=GetButtonID();
				if (id==1) ExitProcess();
				//taskbar buttons
				if (id>=100) && (id<200)
				{
					if (id==100) tbAttachment ^= 1;
					if (id==105) tbSoftenUp ^= 1;
					if (id==106) tbSoftenDown ^= 1;
					if (id==107) tbMinLeftButton ^= 1;
					if (id==108) tbMinRightButton ^= 1;
					if (id==109) tbRunApplButton ^= 1;
					if (id==110) tbClnDeskButton ^= 1;
					if (id==111) tbClock ^= 1;
					if (id==112) tbCpuUsage ^= 1;
					if (id==113) tbChangeLang ^= 1;
					if (id==114) tbMenuButton ^= 1;
					if (id==120) tbPanelHeight++;
					if (id==121) && (tbPanelHeight>6) tbPanelHeight--;
					if (id==122) tbSoftenHeight++;
					if (id==123) && (tbSoftenHeight>0) tbSoftenHeight--;
					if (id==124) tbButtonOffset++;
					if (id==125) && (tbButtonOffset>0) tbButtonOffset--;
					DrawWindowContent(TASKBAR);
					SaveCfg(TASKBAR);
					RestartProcess(TASKBAR);
				}
				//docky buttons			
				if (id>=200)
				{
					if (id==200)
					{
						dkLocation++;
						if (dkLocation>3) dkLocation=1;
					}
					if (id==201) dkFsize ^= 1;
					if (id==202) dkAshow ^= 1;
					DrawWindowContent(DOCKY);
					SaveCfg(DOCKY);
					RestartProcess(DOCKY);
				}
				break;
				
		case evKey:
				key = GetKey();
				if (key==27) ExitProcess();
				break;
			
		case evReDraw:
				system.color.get();
				DefineAndDrawWindow(130, 150, 465, 398 + skin_height, 0x34, system.color.work, WINDOW_TITLE, 0);
				GetProcessInfo(#Form, SelfInfo);
				if (Form.status_window>2) break;
				taskbar_frame.size_x = docky_frame.size_x = - taskbar_frame.start_x * 2 + Form.cwidth;
				taskbar_frame.font_color = docky_frame.font_color = system.color.work_text;
				taskbar_frame.font_backgr_color = docky_frame.font_backgr_color = system.color.work;
				taskbar_frame.ext_col = docky_frame.ext_col = system.color.work_graph;
				DrawWindowContent(ALL);
	}
}


void DrawWindowContent(byte panel_type)
{
	word win_center_x = Form.cwidth / 2 + 20;
	incn y;

	if (panel_type==ALL) || (panel_type==TASKBAR)
	{
		y.n = taskbar_frame.start_y;
		DefineButton(22, y.inc(18), panels_img.w-1, 27-1, 100 + BT_HIDE, 0);
		_PutImage(22, y.n, 37, 27, tbAttachment * 37 * 27 * 3 + panels_img.data);
		WriteText(68, y.inc(7), 0x90, system.color.work_text, CHANGE_POS);
		CheckBox(22, y.inc(35), 105, SOFTEN_UP, tbSoftenUp);
		CheckBox(win_center_x, y.n, 111, CLOCK, tbClock);
		CheckBox(22, y.inc(24), 106, SOFTEN_DOWN, tbSoftenDown);
		CheckBox(win_center_x, y.n, 112, CPU_USAGE, tbCpuUsage);
		CheckBox(22, y.inc(24), 107, MIN_LEFT_BUTTON, tbMinLeftButton);
		CheckBox(win_center_x, y.n, 113, CHANGE_LANG, tbChangeLang);
		CheckBox(22, y.inc(24), 108, MIN_RIGHT_BUTTON, tbMinRightButton);
		CheckBox(win_center_x, y.n, 114, MENU_BUTTON, tbMenuButton);	
		MoreLessBox(22, y.inc(28), 120, 121, tbPanelHeight, PANEL_HEIGHT);
		MoreLessBox(22, y.inc(32), 122, 123, tbSoftenHeight, SOFTEN_HEIGHT);
		MoreLessBox(22, y.inc(32), 124, 125, tbButtonOffset, BUTTON_OFFSET);
		taskbar_frame.size_y = y.inc(32) - taskbar_frame.start_y;
	}
	if (panel_type==ALL) || (panel_type==DOCKY)
	{
		docky_frame.start_y = y.inc(20);
		DefineButton(22, y.inc(18), panels_img.w-1, 27-1, 200 + BT_HIDE, 0);
		_PutImage(22, y.n,  37, 27, dkLocation + 1 * 37 * 27 * 3 + panels_img.data);
		WriteText(68, y.inc(7), 0x90, system.color.work_text, CHANGE_POS);
		CheckBox(22, y.inc(35), 201, FSIZE,  dkFsize);
		CheckBox(win_center_x, y.n, 202, ASHOW, dkAshow);
		docky_frame.size_y = y.inc(30) - docky_frame.start_y;
	}
	if (panel_type==ALL)
	{
		frame_draw stdcall (#taskbar_frame);
		frame_draw stdcall (#docky_frame);
	}
}

void LoadCfg()
{ 
	tbAttachment     = taskbar_flags_ini.GetInt("Attachment", 1);    
	tbSoftenUp       = taskbar_flags_ini.GetInt("SoftenUp", 1);      
	tbSoftenDown     = taskbar_flags_ini.GetInt("SoftenDown", 1);    
	tbMinLeftButton  = taskbar_flags_ini.GetInt("MinLeftButton", 1); 
	tbMinRightButton = taskbar_flags_ini.GetInt("MinRightButton", 1);
	tbClock          = taskbar_flags_ini.GetInt("Clock", 1);         
	tbCpuUsage       = taskbar_flags_ini.GetInt("CpuUsage", 1);      
	tbChangeLang     = taskbar_flags_ini.GetInt("ChangeLang", 1);    
	tbMenuButton     = taskbar_flags_ini.GetInt("MenuButton", 1);    
	tbPanelHeight  = taskbar_vars_ini.GetInt("PanelHeight", 18);   
	tbSoftenHeight = taskbar_vars_ini.GetInt("SoftenHeight", 4);   
	tbButtonOffset = taskbar_vars_ini.GetInt("ButtonTopOffset", 3);
	tbButtonOffset = taskbar_vars_ini.GetInt("ButtonBotOffset", 3);

	dkLocation = docky_ini.GetInt("location", 0);
	dkFsize = docky_ini.GetInt("fsize", 0);   
	dkAshow = docky_ini.GetInt("ashow", 0);   
}

void SaveCfg(byte panel_type)
{
	if (panel_type==TASKBAR) {
		taskbar_flags_ini.SetInt("Attachment", tbAttachment);
		taskbar_flags_ini.SetInt("SoftenUp", tbSoftenUp);
		taskbar_flags_ini.SetInt("SoftenDown", tbSoftenDown);
		taskbar_flags_ini.SetInt("MinLeftButton", tbMinLeftButton);
		taskbar_flags_ini.SetInt("MinRightButton", tbMinRightButton);
		taskbar_flags_ini.SetInt("RunApplButton", tbRunApplButton);
		taskbar_flags_ini.SetInt("ClnDeskButton", tbClnDeskButton);
		taskbar_flags_ini.SetInt("Clock", tbClock);
		taskbar_flags_ini.SetInt("CpuUsage", tbCpuUsage);
		taskbar_flags_ini.SetInt("ChangeLang", tbChangeLang);
		taskbar_flags_ini.SetInt("MenuButton", tbMenuButton);
		taskbar_vars_ini.SetInt("PanelHeight", tbPanelHeight);
		taskbar_vars_ini.SetInt("SoftenHeight", tbSoftenHeight);
		taskbar_vars_ini.SetInt("ButtonTopOffset", tbButtonOffset);
		taskbar_vars_ini.SetInt("ButtonBottOffset", tbButtonOffset);
	}
	if (panel_type==DOCKY) {
		docky_ini.SetInt("location", dkLocation);
		docky_ini.SetInt("fsize", dkFsize);
		docky_ini.SetInt("ashow", dkAshow);
	}
}

void RestartProcess(byte panel_type)
{
	dword proc_name1;
	if (panel_type == TASKBAR)
	{
		RestartProcessByName("@taskbar", SINGLE);
		pause(50);
	}
	else
	{
		RestartProcessByName("@docky", SINGLE);
		pause(120);
	}
	GetProcessInfo(#Form, SelfInfo);
	ActivateWindow(GetProcessSlot(Form.ID));
}



stop: