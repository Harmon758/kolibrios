// Mouse Configuration Utility ver 1.3

#ifndef AUTOBUILD
#include "lang.h--"
#endif

#define MEMSIZE 0x23E80
#include "..\lib\strings.h" 
#include "..\lib\mem.h" 
#include "..\lib\file_system.h"
#include "..\lib\gui.h"
#include "..\lib\obj\libio_lib.h"
#include "..\lib\obj\box_lib.h"
#include "..\lib\obj\libini.h"
#include "..\lib\patterns\restart_process.h"

#include "kos_mouse_functions.h"

#ifdef LANG_RUS
	?define WINDOW_TITLE "�஢�ઠ � ����ன�� ��ࠬ��஢ ���"
	?define CHECK_MOUSE_1 "������ �� �⮩ ������"
	?define CHECK_MOUSE_2 "��� �஢�ન ������ ���"
	?define POINTER_SPEED "������� 㪠��⥫� ���"
	?define POINTER_DELAY "����প� 㪠��⥫� ���"
	?define MOUSE_EMULATION "������ �ࠢ����� 㪠��⥫�� �१ ����������"
	?define MADMOUSE "������� ��� ����� ��஭� �࠭�"
#else
	?define WINDOW_TITLE "Mouse testing and configuration"
	?define CHECK_MOUSE_1 "Click on this area to"
	?define CHECK_MOUSE_2 "check your mouse buttons"
	?define POINTER_SPEED "Mouse pointer speed"
	?define POINTER_DELAY "Mouse pointer delay"
	?define MOUSE_EMULATION "Enable mouse emulation using keyboard NumPad"
	?define MADMOUSE "Through screen sides for pointer"
	#endif

frame mouse_frame = { 0, 000, 14, 130, 14, 0x000111, 0xFFFfff, 0, 0, 0, 0, 6, 0x000111, 0xCCCccc };
char pos_x = 22;


unsigned char panels_img_data[] = FROM "mouse_image.raw";
raw_image panels_img = { 59, 101, #panels_img_data };
char system_ini_path[] = "/sys/settings/system.ini";
char mouse_category[] = "mouse";

proc_info Form;


struct mouse_cfg1 {
	char pointer_speed, 
	pointer_delay,
	emulation, 
	madmouse, 
	button_clicked;
} mouse_cfg;


void main() {
	char id, old_button_clicked;

	load_dll(libini, #lib_init,1);
	load_dll(boxlib, #box_lib_init,0);

	LoadCfg();

	SetEventMask(0x27);
	loop() switch(WaitEvent())
	{
		case evMouse:
				mouse.get();
				if (mouse.y <= mouse_frame.start_y) || (mouse.y >= mouse_frame.start_y + mouse_frame.size_y) 
				|| (mouse.x >= mouse_frame.start_x + mouse_frame.size_x) || (mouse.x <= mouse_frame.start_x) break;
				old_button_clicked = mouse_cfg.button_clicked;
				if (mouse.lkm) mouse_cfg.button_clicked=1;
				else if (mouse.pkm) mouse_cfg.button_clicked=2;
				else if (mouse.mkm) mouse_cfg.button_clicked=3;
				else mouse_cfg.button_clicked=0;
				if (mouse_cfg.button_clicked != old_button_clicked) DrawMouseImage();
				break;

		case evButton: 
				id=GetButtonID();
				if (id==1)
				{
					ExitApp();
				}
				if (id==99) 
				{
					mouse_cfg.button_clicked=0;
					DrawMouseImage();
					break;
				}
				if (id==100)
				{
					if (mouse_cfg.emulation==true) KillProcessByName("mousemul", SINGLE);
					else RunProgram("/sys/mousemul", 0);
					mouse_cfg.emulation ^= 1;
				}
				if (id==101) 
				{
					if (mouse_cfg.madmouse==true) KillProcessByName("madmouse", SINGLE);
					else RunProgram("/sys/madmouse", 0);
					mouse_cfg.madmouse ^= 1;
				}
				if (id==120) 
				{
					mouse_cfg.pointer_speed++;
					SetMouseSpeed(mouse_cfg.pointer_speed);
				}
				if (id==121) && (mouse_cfg.pointer_speed>1)
				{
					mouse_cfg.pointer_speed--;
					SetMouseSpeed(mouse_cfg.pointer_speed);
				}
				if (id==122)
				{
					mouse_cfg.pointer_delay++;
					SetMouseDelay(mouse_cfg.pointer_delay);
				}
				if (id==123) && (mouse_cfg.pointer_delay>0)
				{
					mouse_cfg.pointer_delay--;
					SetMouseDelay(mouse_cfg.pointer_delay);
				}
				DrawControls();
				break;
				
		case evKey:
				if (GetKey()==27) ExitApp();
				break;
			
		case evReDraw:
				system.color.get();
				DefineAndDrawWindow(430, 150, 360, 280+GetSkinHeight(),0x34,system.color.work,WINDOW_TITLE);
				GetProcessInfo(#Form, SelfInfo);
				if (Form.status_window>2) break;
				SetFrameColors();
				DefineButton(mouse_frame.start_x+2, mouse_frame.start_y+2, mouse_frame.size_x-4, 
					mouse_frame.size_y-4, 99+BT_NOFRAME, 0xF0F2F3); //needed to handle mouse_up and refresh mouse image
				frame_draw stdcall (#mouse_frame);
				WriteText(pos_x + 110, mouse_frame.start_y + 25, 0x90, 0x2C343C, CHECK_MOUSE_1);
				WriteText(pos_x + 110, mouse_frame.start_y + 45, 0x90, 0x2C343C, CHECK_MOUSE_2);
				DrawMouseImage();
				DrawControls();
	}
}

void PanelCfg_CheckBox(dword x, y, id, text, byte value) {
	CheckBox(x, y, 14, 14, id, text, system.color.work_graph, system.color.work_text, value);
}

void PanelCfg_MoreLessBox(dword x, y, id_more, id_less; byte value; dword text) {
	MoreLessBox(x, y, 18, id_more, id_less, #system.color, value, text);
}

void DrawMouseImage() {
	_PutImage(mouse_frame.start_x+30, mouse_frame.start_y + 15,  panels_img.w, panels_img.h, 
		mouse_cfg.button_clicked * panels_img.w * panels_img.h * 3 + panels_img.data);
}

void DrawControls() {
	DrawBar(pos_x, mouse_frame.start_y + 142, Form.cwidth - pos_x, 120, system.color.work);
	PanelCfg_MoreLessBox(pos_x, mouse_frame.start_y + 142, 120, 121, mouse_cfg.pointer_speed, POINTER_SPEED);
	PanelCfg_MoreLessBox(pos_x, mouse_frame.start_y + 170, 122, 123, mouse_cfg.pointer_delay, POINTER_DELAY);
	PanelCfg_CheckBox(pos_x, mouse_frame.start_y + 202, 100, MOUSE_EMULATION, mouse_cfg.emulation);
	PanelCfg_CheckBox(pos_x, mouse_frame.start_y + 226, 101, MADMOUSE, mouse_cfg.madmouse);
}

void SetFrameColors() {
	mouse_frame.size_x = - mouse_frame.start_x * 2 + Form.cwidth;
	mouse_frame.font_color = system.color.work_text;
	mouse_frame.font_backgr_color = system.color.work;
	mouse_frame.ext_col = system.color.work_graph;
}

void LoadCfg() {
	ini_get_int stdcall (#system_ini_path, #mouse_category, "delay", GetMouseDelay());   mouse_cfg.pointer_delay = EAX;
	ini_get_int stdcall (#system_ini_path, #mouse_category, "speed", GetMouseSpeed());   mouse_cfg.pointer_speed = EAX;
	mouse_cfg.madmouse = CheckProcessExists("MADMOUSE");
	mouse_cfg.emulation = CheckProcessExists("MOUSEMUL");
}

void ExitApp() {
	ini_set_int stdcall (#system_ini_path, #mouse_category, "delay", mouse_cfg.pointer_delay);
	ini_set_int stdcall (#system_ini_path, #mouse_category, "speed", mouse_cfg.pointer_speed);
	ExitProcess();
}



stop: