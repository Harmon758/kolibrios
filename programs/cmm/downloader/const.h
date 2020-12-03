//Copyright 2020 by Leency

#ifndef AUTOBUILD
	#include "lang.h--"
#endif

#ifdef LANG_RUS
	#define DL_WINDOW_HEADER "�������� ����㧮�"
	#define T_DOWNLOAD "������"
	#define T_CANCEL "�⬥��"
	#define T_OPEN_DIR "�������� � �����"
	#define T_RUN "������ 䠩�"
	#define FILE_SAVED_AS "'�������� ����㧮�\n���� ��࠭�� ��� %s' -Dt"
	#define FILE_NOT_SAVED "'�������� ����㧮�\n�訡��! ���� �� ����� ���� ��࠭�� ���\n%s' -Et"
	#define KB_RECEIVED "���� ᪠稢����... %s ����祭�"
	#define T_ERROR_STARTING_DOWNLOAD "'���������� ����� ᪠稢����.\n��������, �஢���� �������� ���� � ᮥ������� � ���୥⮬.' -E"
	#define T_AUTOCLOSE "��⮧����⨥"
	char accept_language[]= "Accept-Language: ru\n";
#else
	#define DL_WINDOW_HEADER "Download Manager"
	#define T_DOWNLOAD "Download"
	#define T_CANCEL "Cancel"
	#define T_OPEN_DIR " Show in folder "
	#define T_RUN "Open file"
	#define FILE_SAVED_AS "'Download manager\nFile saved as %s' -Dt"
	#define FILE_NOT_SAVED "'Download manager\nError! Can\96t save file as %s' -Et"
	#define KB_RECEIVED "Downloading... %s received"
	#define T_ERROR_STARTING_DOWNLOAD "'Error while starting download process.\nPlease, check entered path and Internet connection.' -E"
	#define T_AUTOCLOSE "Autoclose"
	char accept_language[]= "Accept-Language: en\n";
#endif

#define GAPX 15
#define WIN_W 580
#define WIN_H 100

char save_to[] = "/tmp0/1/Downloads";
char dl_shared[] = "DL";

enum { 
	BTN_EXIT=1,
	BTN_START,
	BTN_STOP,
	BTN_DIR, 
	BTN_RUN, 
};