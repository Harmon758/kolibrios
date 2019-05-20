#define MEMSIZE 1024*160

#include "../lib/io.h"
#include "../lib/gui.h"
#include "../lib/copyf.h"
#include "../lib/obj/libini.h"
#include "../lib/patterns/restart_process.h"

char logo[] = "
����   ���� ���������� ����   ���� �����������
 ����   ��   ���         ��� ���   ��  ���  ��
 �����  ��   ���          �����    ��  ���  ��
 �� ��� ��   �������       ���         ���    
 ��  �����   ���          �����        ���    
 ��   ����   ���         ��� ���       ���    
����   ���  ���������� ����   ����    �����   
";

char intro[] = "���஡�� ����� ���㠫쭮� ��ଫ���� ������, ���஥ ࠭�� �뫮 ����㯭� ⮫쪮 � KolibriNext."; 

#define B_INSTALL 10

void main()
{
	word btn;
	load_dll(libini, #lib_init,1);
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
	system.color.get();
	DefineAndDrawWindow(screen.width-WINW/2,screen.height-WINH/2,
		WINW+9,WINH+skin_height,0x34,system.color.work,"KolibriN10",0);

	DrawLogo();

	DrawTextViewArea(30, 140, WINW-60, WINH-80, 
		#intro, -1, system.color.work_text);

	DrawCaptButton(WINW-110/2, WINH-70, 110, 28, B_INSTALL, 
		0x0092D8, 0xFFFfff, "��⠭�����");
}

void DrawLogo()
{
	#define LX -46*6+WINW/2
	#define LY 25
	WriteTextLines(LX-1, LY, 0x80, 0x9F87B8, #logo, 9);
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
}

void Operation_Draw_Progress(dword filename) { debug("copying: "); debugln(filename); }