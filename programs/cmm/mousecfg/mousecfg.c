// Mouse Configuration Utility ver 1.62

#ifndef AUTOBUILD
#include "lang.h--"
#endif

#define MEMSIZE 4096*11

#include "..\lib\strings.h" 
#include "..\lib\mem.h" 
#include "..\lib\fs.h"
#include "..\lib\gui.h"
#include "..\lib\obj\libio.h"
#include "..\lib\obj\libini.h"
#include "..\lib\patterns\restart_process.h"

// Translatiions
#ifdef LANG_RUS
	?define WINDOW_TITLE "�஢�ઠ � ����ன�� ��ࠬ��஢ ���"
	?define CHECK_MOUSE_1 "������ �� �⮩ ������"
	?define CHECK_MOUSE_2 "��� �஢�ન ������ ���"
	?define POINTER_SPEED "����⥫� ᪮��� 㪠��⥫� ���"
	?define ACCELERATION_TEXT "���⢨⥫쭮��� 㪠��⥫� ���"
	?define DOUBLE_CLICK_TEXT "����প� �������� ����� �����"
	?define MOUSE_EMULATION "��ࠢ����� 㪠��⥫�� ��� �१ ����������"
	?define MADMOUSE "������� ��� ����� ��஭� �࠭�"
	?define COMMOUSE "����㧨�� �ࠩ��� ��� ��� COM-���� (F10)"
	?define COMMOUSE_LOADED "'�ࠩ��� ��� COM ��� �� ����㦥�' -O"
	?define COMMOUSE_CAN_NOT_UNLOAD "'��⠭���� �ࠩ��� ����������' -W"
#else
	?define WINDOW_TITLE "Mouse testing and configuration"
	?define CHECK_MOUSE_1 "Click on this area to"
	?define CHECK_MOUSE_2 "check your mouse buttons"
	?define POINTER_SPEED "Mouse pointer speed divider"
	?define ACCELERATION_TEXT "Mouse pointer sensitivity"
	?define DOUBLE_CLICK_TEXT "Mouse double click delay"
	?define MOUSE_EMULATION "Enable mouse emulation using keyboard NumPad"
	?define MADMOUSE "Through screen sides for pointer"
	?define COMMOUSE "Load mouse driver for COM-port (F10)"
	?define COMMOUSE_LOADED "'Driver for COM mouse loaded' -O"
	?define COMMOUSE_CAN_NOT_UNLOAD "'Driver stop is impossible' -W"
#endif

#define FRAME_X 18
#define FRAME_Y 18
:block mouse_frame = { FRAME_X, FRAME_Y, NULL, 130 };
:more_less_box pointer_speed      = { NULL, 0, 64, POINTER_SPEED };
:more_less_box acceleration       = { NULL, 0, 64, ACCELERATION_TEXT };
:more_less_box double_click_delay = { NULL, 0, 999, DOUBLE_CLICK_TEXT, 8 };
:checkbox emulation = { MOUSE_EMULATION, NULL };
:checkbox madmouse = { MADMOUSE, NULL };
:checkbox com_mouse = { COMMOUSE, NULL };

_ini ini = { "/sys/settings/system.ini", "loaded drivers" };

void main() {
	proc_info Form;
	int id;
	
	load_dll(libini, #lib_init,1);
	
	LoadCfg();

	SetEventMask(EVM_REDRAW+EVM_KEY+EVM_BUTTON+EVM_MOUSE+EVM_MOUSE_FILTER);	

	loop() switch(WaitEvent())
	{
		case evMouse:
				mouse.get();
				IF (mouse_frame.hovered()) DrawMouseImage(mouse.lkm,mouse.pkm,mouse.mkm,mouse.vert);
				IF (mouse.click) || (mouse.up) DrawMouseImage(0,0,0,0);
				break;

		CASE evButton: 
				id = GetButtonID();
				IF (1 == id) ExitApp();
				else IF (pointer_speed.click(id)) ApplyCfg();
				else IF (acceleration.click(id)) ApplyCfg();
				else IF (double_click_delay.click(id)) ApplyCfg();
				else IF (emulation.click(id)) {
					IF (emulation.checked == true) RunProgram("/sys/mousemul", 0);
					ELSE KillProcessByName("mousemul", SINGLE);
					break;
				}
				else IF (madmouse.click(id)) {						
					IF (madmouse.checked == true) RunProgram("/sys/madmouse", 0);
					ELSE KillProcessByName("madmouse", SINGLE);
					break;
				}
				else IF (id == com_mouse.id) {
					EventClickComMouse();
				}
				break;

		case evKey:
				GetKeys();
				IF (key_scancode == SCAN_CODE_ESC) ExitApp();
				IF (key_scancode == SCAN_CODE_F10) EventClickComMouse();
				break;
			
		case evReDraw:
				system.color.get();
				DefineAndDrawWindow(430, 150, 424, 343+skin_height,0x34,system.color.work,WINDOW_TITLE,0);
				GetProcessInfo(#Form, SelfInfo);
				if (Form.status_window>2) break;
				mouse_frame.w = - FRAME_X * 2 + Form.cwidth;
				DefineButton(FRAME_X, FRAME_Y, mouse_frame.w, 
					mouse_frame.h, 99+BT_NOFRAME, 0xF0F2F3); //needed to handle mouse_up and refresh mouse image
				WriteText(FRAME_X + 110, FRAME_Y + 25, 0x90, 0x2C343C, CHECK_MOUSE_1);
				WriteText(FRAME_X + 110, FRAME_Y + 45, 0x90, 0x2C343C, CHECK_MOUSE_2);
				DrawMouseImage(0,0,0,0);
				DrawControls();
	}
}

:byte panels_img_data[] = FROM "mouse_image.raw";

#define red    0xff0000
#define yellow 0xfff600
#define white  0xffffff
#define dgrey  0x2d353d

:struct IMG_PAL{ dword back, shad1, contour, left,  right, middle, mwhite; }
         pal = { 0xF0F2F3,0xABB0B2, dgrey,   white, white, dgrey,  white  };

void DrawMouseImage(dword l,r,m,v) {
	#define IMG_W 59
	#define IMG_H 101

	IF (l) pal.left = red;
	IF (m) pal.middle = red;
	IF (r) pal.right = red;
	IF (v) pal.middle = yellow;

	PutPaletteImage(#panels_img_data,IMG_W,IMG_H,18+30,18+15,8,#pal);
	pal.left = pal.right = white;
	pal.middle = dgrey;
	IF (v) {
		pause(10);
		DrawMouseImage(l,r,m,0);
	}
}

void DrawControls() {
	incn y;
	y.n = FRAME_Y+115;
	pointer_speed.draw(FRAME_X, y.inc(30));
	acceleration.draw(FRAME_X, y.inc(30));
	double_click_delay.draw(FRAME_X, y.inc(30));
	emulation.draw(FRAME_X, y.inc(33));
	madmouse.draw(FRAME_X, y.inc(27));
	com_mouse.draw(FRAME_X, y.inc(27));
}

void LoadCfg() {
	acceleration.value = GetMouseAcceleration();
	pointer_speed.value = GetMouseSpeed();
	double_click_delay.value = GetMouseDoubleClickDelay();
	com_mouse.checked = ini.GetInt("com_mouse", 0);
	madmouse.checked = CheckProcessExists("MADMOUSE");
	emulation.checked = CheckProcessExists("MOUSEMUL");
}

void ExitApp() {
	ini.SetInt("com_mouse", com_mouse.checked);
	ExitProcess();
}

void ApplyCfg() {
	SetMouseSpeed(pointer_speed.value);
	SetMouseAcceleration(acceleration.value);
	SetMouseDoubleClickDelay(double_click_delay.value);
}

void EventClickComMouse()
{
	if (!com_mouse.checked) 
	{
		if (RunProgram("/sys/loaddrv", "COMMOUSE")>=0) {
			notify(COMMOUSE_LOADED);
			com_mouse.click(com_mouse.id);
		}
		else {
			notify("'Error running LOADDRV' -E");
		}
	}
	else
	{
		notify(COMMOUSE_CAN_NOT_UNLOAD);
	}
}

stop:
