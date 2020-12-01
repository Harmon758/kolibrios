#define MEMSIZE 1024*50

#include "../lib/io.h"
#include "../lib/gui.h"
#include "../lib/copyf.h"

#include "../lib/obj/libini.h"
#include "../lib/obj/libio.h"
#include "../lib/obj/libimg.h"
#include "../lib/obj/http.h"
#include "../lib/obj/network.h"

#include "../lib/patterns/restart_process.h"
#include "../lib/patterns/http_downloader.h"

#ifndef AUTOBUILD
#include "lang.h--"
#endif

bool install_complete = false;
_http http;

#define WINW 460
#define WINH 380

//#define LANG_RUS 1

#ifdef LANG_RUS
#define T_WINDOW_TITLE "������ ���������� KolibriOS"
#define T_TITLE_H1 "������ ����������"
#define T_INTRO "������ �ਫ������ ᪠砥� ��᫥���� ����� KolibriOS � �ᯠ��� �� �� RAM-���. �� �⮬ �� �� �㤥� ��१���饭�, ��� �⮣� ����室��� ��࠭��� ��ࠧ � ��१���㧨���. ��������, ���ன� �� ������ �ਫ������ ��। ��砫�� ����������.
��������: �� ������� 䠩�� �� RAM-��᪥ ���� ��१���ᠭ�!"; 
#define T_INSTALL "��������"
#define T_COMPLETE "���������� �����襭�"
#define T_EXIT "��室"
#define IMG_URL "http://builds.kolibrios.org/rus/data/data/kolibri.img"
#define KS "���࠭��� ����ன��"
#define RA "��१������� �� �ணࠬ��"
#else
#define T_WINDOW_TITLE "KolibriOS Online Updater"
#define T_TITLE_H1 "ONLINE UPDATE"
#define T_INTRO "This app will download the latest KolibriOS dirsto and update your RAM-disk with it. Kernel won't be restarted. 
Please close all opened apps before start.
Note that all changes on RAM-disk will be lost."; 
#define T_INSTALL "Update"
#define T_COMPLETE "Update complete"
#define T_EXIT "Exit"
#define IMG_URL "http://builds.kolibrios.org/eng/data/data/kolibri.img"
#define KS "Keep settings folder"
#define RA "Restart all apps"
#endif
char accept_language[]="en"; //not used, necessary for http.get()
void Operation_Draw_Progress(dword f) {} //not used, necessary for copyf()

checkbox keep_settings = { KS, true };
checkbox restart_apps = { RA, true };

void main()
{
	int btn;
	sensor progress;
	load_dll(libio, #libio_init,1);
	load_dll(libimg, #libimg_init,1);
	load_dll(libHTTP,   #http_lib_init,1);
	@SetEventMask(EVM_REDRAW + EVM_KEY + EVM_BUTTON + EVM_STACK);
	loop() switch(@WaitEventTimeout(300))
	{
		case evButton: 
			btn = @GetButtonID();
			if (btn<=2) ExitProcess();
			if (btn==9) goto _INSTALL; 
			keep_settings.click(btn);
			restart_apps.click(btn);
			break;

		case evKey:
			switch (@GetKeyScancode()) {
				case SCAN_CODE_ESC: ExitProcess();
				case SCAN_CODE_ENTER: 
					if (install_complete) ExitProcess();
					else {
						_INSTALL: 
						http.get(IMG_URL); 
						goto _DRAW_WINDOW;
						}
			}
			break;

		case evReDraw:
			_DRAW_WINDOW:
			sc.get();
			DefineAndDrawWindow(screen.width-WINW/2,screen.height-WINH/2,
				WINW+9,WINH+skin_height,0x34,sc.work,T_WINDOW_TITLE,0);
			WriteText(30, 20, 0x81, 0xEC008C, T_TITLE_H1);
			if (!install_complete) {
					DrawTextViewArea(30, 50, WINW-60, WINH-80, T_INTRO, -1, sc.work_text);
					progress.set_size(30, WINH-130, WINW-60, 20);
					if (http.transfer<=0) {
						DrawCaptButton(WINW-110/2, WINH-70, 110, 28, 9, 0x0092D8, 0xFFFfff, T_INSTALL);
						keep_settings.draw(30, WINH - 210);
						restart_apps.draw(30, WINH - 185);
					}
			} else {
					DrawIcon32(WINW-32/2, 140, sc.work, 49);
					WriteTextCenter(0,185, WINW, sc.work_text, T_COMPLETE);
					DrawCaptButton(WINW-110/2, WINH-70, 110, 28, 2, 
						0x0092D8, 0xFFFfff, T_EXIT);
			}

		case evNetwork:
			if (http.transfer <= 0) break;
			http.receive();
			if (http.content_length) {
				progress.draw_progress(http.content_length - http.content_received 
					* progress.w / http.content_length);
			}
			if (!http.receive_result) {
				CreateFile(http.content_received, 
					http.content_pointer, "/tmp0/1/latest.img");
				http.hfree();
				EventDownloadComplete();
			}
	}
}

dword GetFreeSpaceOfRamdisk()
{
	dword rdempty = malloc(1440*1024);
	CreateFile(0, 1440*1024, rdempty, "/rd/1/rdempty");
	free(rdempty);
	file_size stdcall ("/rd/1/rdempty");
	rdempty = EBX;
	DeleteFile("/rd/1/rdempty");
	return rdempty;
}

signed CheckFreeSpace(dword _latest, _combined)
{
	dword cur_size, new_size, empty;
	DIR_SIZE dir_size;

	dir_size.get("/sys");
	cur_size = dir_size.bytes;

	copyf("/sys", _combined);
	copyf(_latest, _combined);
	dir_size.get(_combined);
	new_size = dir_size.bytes;

	empty = GetFreeSpaceOfRamdisk();

	return cur_size + empty - new_size / 1024;
}

void EventDownloadComplete()
{
	dword unimg_id, slot_n;
	signed space_delta;
	int i=0;

	char osupdate[32];
	char latest[40];
	char backup[40];
	char combined[40];
	char exract_param[64];
	char backup_settings[64];

	do  { sprintf(#osupdate, "/tmp0/1/osupdate%d", i); i++;
	} while (dir_exists(#osupdate));
	CreateDir(#osupdate);

	sprintf(#latest, "%s/latest", #osupdate);
	sprintf(#backup, "%s/rdbackup", #osupdate);
	sprintf(#combined, "%s/combined", #osupdate);
	sprintf(#backup_settings, "%s/settings", #backup);
	sprintf(#exract_param, "/tmp0/1/latest.img %s -e", #latest);


	unimg_id = RunProgram("/sys/unimg", #exract_param);
	do {
		slot_n = GetProcessSlot(unimg_id);
		pause(10);
	} while (slot_n!=0);

	space_delta = CheckFreeSpace(#latest, #combined);
	if (space_delta<0) {
		sprintf(#param, "'Not enought free space! You need %d Kb more.'E", -space_delta);
		notify(#param);
	} else {
		copyf("/sys", #backup);
		copyf(#latest, "/sys");
		if (keep_settings.checked) copyf(#backup_settings, "/rd/1/settings");
		if (restart_apps.checked) RestartAllProcess();
		install_complete = true;		
	}
}



