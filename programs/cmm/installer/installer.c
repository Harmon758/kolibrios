#define MEMSIZE 0xA1000
#include "..\lib\kolibri.h"
#include "..\lib\strings.h"
#include "..\lib\file_system.h"
#include "..\lib\mem.h"
#include "..\lib\copyf.h"

#include "add_appl_dir.c";

?define T_END "\'��⠭���� KolibriN �ᯥ譮 �����襭�.\' -O"
?define T_LESS_RAM "���� ᢮������ ����⨢��� �����. ����� ���������� �஡����"

void main()
{
	mem_Init();
	SetAddApplDir("kolibrios", abspath("kolibrios")+1);
	RunProgram("/sys/media/kiv", "\\S__/kolibrios/res/Wallpapers/In the wind there is longing.png");
	notify(T_END);
	copyf(abspath("tmp"), "/tmp0/1");
	ExitProcess();
}


void copyf_Draw_Progress(dword filename) { return; }


stop: