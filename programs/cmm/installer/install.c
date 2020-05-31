#define MEMSIZE 1024*160

#include "../lib/io.h"
#include "../lib/gui.h"
#include "../lib/copyf.h"

#include "../lib/obj/libini.h"
#include "../lib/obj/libio.h"
#include "../lib/obj/libimg.h"

#include "../lib/patterns/restart_process.h"

#ifndef AUTOBUILD
#include "lang.h--"
#endif

char logo[] = "
����   ���� ���������� ����   ���� �����������
 ����   ��   ���         ��� ���   ��  ���  ��
 �����  ��   ���          �����    ��  ���  ��
 �� ��� ��   �������       ���         ���    
 ��  �����   ���          �����        ���    
 ��   ����   ���         ��� ���       ���    
����   ���  ���������� ����   ����    �����   
";

#ifdef LANG_RUS
#define T_INTRO "���஡�� ����� ���㠫쭮� ��ଫ���� ������, ���஥ ࠭�� �뫮 ����㯭� ⮫쪮 � KolibriNext."; 
#define T_INSTALL "��⠭�����"
#define T_COMPLETE "��⠭���� �����襭�"
#define T_EXIT "��室"
#else
#define T_INTRO "Try a new visual design of KolibriOS, which previously was available only in KolibriNext."; 
#define T_INSTALL "Install"
#define T_COMPLETE "Install complete"
#define T_EXIT "Exit"
#endif

#define B_INSTALL 10

bool install_complete = false;

void main()
{
	word btn;
	load_dll(libini, #lib_init,1);
	load_dll(libio, #libio_init,1);
	load_dll(libimg, #libimg_init,1);
	loop() switch(WaitEventTimeout(300) & 0xFF)
	{
		case evButton:
			btn = GetButtonID();               
			if (btn == 1) ExitProcess();
			if (btn == B_INSTALL) EventInstall();
			break;
	  
		case evKey:
			GetKeys();
			if (key_scancode == SCAN_CODE_ESC) ExitProcess();
			if (key_scancode == SCAN_CODE_ENTER) {
				if (install_complete) ExitProcess();
				else EventInstall();
			}
			break;
		 
		case evReDraw:
			draw_window();
			break;

		default:
			DrawLogo();
			DrawLogo();
	}
}

#define WINW 400
#define WINH 300
void draw_window()
{
	sc.get();
	DefineAndDrawWindow(screen.width-WINW/2,screen.height-WINH/2,
		WINW+9,WINH+skin_height,0x34,sc.work,"KolibriN10",0);
	DrawLogo();
	if (install_complete) DrawInstallComplete(); else DrawIntro();
}

void DrawIntro()
{
	DrawTextViewArea(30, 140, WINW-60, WINH-80, 
		T_INTRO, -1, sc.work_text);
	DrawCaptButton(WINW-110/2, WINH-70, 110, 28, B_INSTALL, 
		0x0092D8, 0xFFFfff, T_INSTALL);
}

void DrawInstallComplete()
{
	DrawIcon32(WINW-32/2, 140, sc.work, 49);
	WriteTextCenter(0,185, WINW, sc.work_text, T_COMPLETE);
	DrawCaptButton(WINW-110/2, WINH-70, 110, 28, CLOSE_BTN, 
		0x0092D8, 0xFFFfff, T_EXIT);
}

void DrawLogo()
{
	#define LX -46*6+WINW/2
	#define LY 25
	WriteTextLines(LX-2, LY, 0x80, 0xF497C0, #logo, 9);
	WriteTextLines(LX+3, LY, 0x80, 0x7ED1E3, #logo, 9);

	pause(1);

	WriteTextLines(LX+1, LY, 0x80, 0xEC008C, #logo, 9);
	WriteTextLines(LX,   LY, 0x80, 0xEC008C, #logo, 9);
}

void EventInstall()
{
	//#include "..\lib\added_sysdir.c";
	//SetAdditionalSystemDirectory("kolibrios", abspath("install/kolibrios")+1);
	ini_set_int stdcall ("/sys/settings/taskbar.ini", "Flags", "Attachment", 0);
	copyf("/kolibrios/KolibriNext/settings", "/sys/settings");

	RestartProcessByName("/sys/@icon", MULTIPLE);
	RestartProcessByName("/sys/@taskbar", SINGLE);
	RestartProcessByName("/sys/@docky", SINGLE);

	RunProgram("/sys/media/kiv", "\\S__/kolibrios/res/Wallpapers/Free yourself.jpg");

	install_complete = true;
	draw_window();
}

void Operation_Draw_Progress(dword filename) { }