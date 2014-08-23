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
	int i;
	proc_info Process;

	mem_Init();
	if (GetFreeRAM()/1024<15) notify(T_LESS_RAM);

	for (i=0; i<1000; i++;)
	{
		GetProcessInfo(#Process, i);
		if (strcmpi(#Process.name, "@icon")==0) KillProcess(Process.ID);
		if (strcmpi(#Process.name, "@docky")==0) KillProcess(Process.ID);
	}
	SetAddApplDir("kolibrios", abspath("kolibrios")+1);
	RunProgram("/sys/REFRSCRN", NULL);
	copyf(abspath("sys"), "/rd/1");
	RunProgram("/sys/launcher", NULL);
	SetSystemSkin("/kolibrios/res/skins/Yeah.skn");
	notify(T_END);
	DeleteFile("/sys/3d/free3d04");
	DeleteFile("/sys/games/invaders");
	RunProgram("/sys/tmpdisk", "a0s10");
	pause(10);
	copyf(abspath("tmp"), "/tmp0/1");
	ExitProcess();
}


void copyf_Draw_Progress(dword filename) { return; }


stop: