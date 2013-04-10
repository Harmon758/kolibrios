//HTML Viewer in C--
//Copyright 2007-2012 by Veliant & Leency
//Asper, lev, Lrz, Barsuk, Nable...
//home icon - rachel fu, GPL licence

//libraries
#define MEMSIZE 0x100000
#include "..\lib\kolibri.h"
#include "..\lib\strings.h"
#include "..\lib\figures.h"
#include "..\lib\encoding.h"
#include "..\lib\file_system.h"
#include "..\lib\mem.h"
#include "..\lib\dll.h"
//*.obj libraries
#include "..\lib\lib.obj\box_lib.h"
#include "..\lib\lib.obj\libio_lib.h"
#include "..\lib\lib.obj\libimg_lib.h"
#include "..\lib\lib.obj\truetype.h"
//images
#include "img\toolbar_icons.c"
#include "img\URLgoto.txt";


#define URL param
int use_truetype = 0;
char fontlol[64];

char editURL[sizeof(URL)],
	page_links[12000],
	header[2048];

struct lines{
	int visible, all, first, column_max;
};

int	mouse_dd;
edit_box edit1= {250,207,16,0xffffff,0x94AECE,0xffffff,0xffffff,0,sizeof(editURL),#editURL,#mouse_dd,2,19,19};
scroll_bar scroll1 = { 18,200,398, 44,18,0,115,15,0,0xeeeeee,0xD2CED0,0x555555,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1};


proc_info Form;
#define WIN_W 640
#define WIN_H 480


char stak[512];
mouse m;

#include "TWB.h"
#include "include\menu_rmb.h"


void main()
{
	int btn, key;
	int half_scroll_size;
	int scroll_used=0;
	
	mem_Init();
	if (load_dll2(boxlib, #box_lib_init,0)!=0) {notify("System Error: library doesn't exists /rd/1/lib/box_lib.obj"); ExitProcess();}
	if (load_dll2(libio, #libio_init,1)!=0) debug("Error: library doesn't exists - libio"w);
	if (load_dll2(libimg, #libimg_init,1)!=0) debug("Error: library doesn't exists - libimg"w);
	if (load_dll2(libtruetype, #truetype,0)!=0) {debug("Error: library doesn't exists - TrueType"w); use_truetype = 2; }
	else init_font(#fontlol);
	
	if (!URL) strcpy(#URL, "/sys/home.htm");
	strcpy(#editURL, #URL);
	
	Form.width=WIN_W;
	Form.height=WIN_H;
	
	SetElementSizes();

	WB1.OpenPage();

	SetEventMask(0x27);
	loop()
	{
		WaitEventTimeout(2);
		switch(EAX & 0xFF)
		{
			CASE evMouse:
				/*scrollbar_v_mouse (#scroll1);      //�������� ������ ��������������, ��� "����� ����"
				if (lines.first <> scroll1.position)
				{
					lines.first = scroll1.position;
					WB1.ParseHTML(buf, filesize);
					//break;
				};*/
				
				btn=GetProcessSlot(Form.ID); 
				IF (btn<>GetActiveProcess()) break; //���� ���� �� ������� �� ������� ���� �� ���������

				edit_box_mouse stdcall (#edit1);

				m.get();
				
				if (m.pkm) && (m.y>WB1.top) && (m.y<Form.height) && (filesize)
				{
					SwitchToAnotherThread();
					CreateThread(#menu_rmb,#stak); 
				}

				IF (m.vert==65535) //��������� ��������
				{
					IF (lines.first==0) break;
					IF (lines.first>3) lines.first-=2; ELSE lines.first=1;
					WB1.Scan(ID1);
					break;
				} 
				IF (m.vert==1)
				{
					IF(lines.visible+lines.first+3>=lines.all) WB1.Scan(181);
					ELSE	{
						lines.first+=2;
						WB1.Scan(ID2);
					}
					break;
				}
				
				if (!m.lkm) scroll_used=0;
				if (m.x>=WB1.width-14) && (m.x<=WB1.width+6) && (m.y>WB1.top+16)
				&& (m.y<WB1.top+WB1.height-16) && (lines.all>lines.visible) && (m.lkm)
					scroll_used=1;
				
				if (scroll_used)
				{
					half_scroll_size = WB1.height - 16 * lines.visible / lines.all - 3 /2;
					IF (half_scroll_size+WB1.top>m.y) || (m.y<0) || (m.y>4000) m.y=half_scroll_size+WB1.top; //���� ������ ��� �����
					btn=lines.first; //��������� ������ ����������
					lines.first = m.y -half_scroll_size -WB1.top * lines.all / WB1.height;
					if (lines.visible+lines.first>lines.all) lines.first=lines.all-lines.visible;
					if (btn<>lines.first) WB1.ParseHTML(buf); //���� ������ ��� �� ��������������
				}

				break;
			case evButton:
				btn=GetButtonID();
				if (btn==1)
				{
					KillProcess(downloader_id);
					ExitProcess();
				}
				ELSE
				{
					WB1.Scan(btn);
				}
				break;
			case evKey:
				key = GetKey();
				
				if (edit1.flags & 0b10) SWITCH(key) //���� ������� ������ ������ ���������� ��������� ������
					{ CASE 52: CASE 53: CASE 54: goto _EDIT_MARK; } 

				WB1.Scan(key);
				
				_EDIT_MARK:
				if (key<>0x0d) && (key<>183) && (key<>184) && (key<>173) {EAX=key<<8; edit_box_key stdcall(#edit1);} //�������� ������
				break;
			case evReDraw:
				Draw_Window();
				break;
			default:
				if (downloader_id<>0)
				{
					if (GetProcessSlot(downloader_id)<>0) break;
					downloader_id=0;
					lines.first = lines.all = 0;
					WB1.ReadHtml(_WIN);
					Draw_Window();
				}
		}
	}
}

void SetElementSizes()
{
	edit1.width = Form.width-266;
	WB1.top = 44;
	WB1.width = Form.cwidth - 4;
	WB1.height = Form.cheight - WB1.top;
	WB1.line_h = 10;
	lines.column_max = WB1.width - 30 / 6;
	lines.visible = WB1.height - 3 / WB1.line_h - 2;
}


void Draw_Window()
{
	int j;
	DefineAndDrawWindow(215,100,WIN_W,WIN_H,0x73,0x00E4DFE1,0,0);

	GetProcessInfo(#Form, SelfInfo);
	if (Form.status_window>2)
	{
		DrawTitle(#header);
		return;
	}
	if (Form.height<120) MoveSize(OLD,OLD,OLD,120);
	if (Form.width<280) MoveSize(OLD,OLD,280,OLD);
	
	PutPaletteImage(#toolbar,200,42,0,0,8,#toolbar_pal);
	if (GetProcessSlot(downloader_id)<>0) _PutImage(88,10, 24,24, #stop_btn);
	
	DrawBar(200,0,Form.cwidth-200,43,0xE4DFE1);
	DrawBar(0,42,Form.cwidth,1,0xE2DBDC);
	DrawBar(0,43,Form.cwidth,1,0xD2CED0);
	for (j=0; j<5; j++) DefineButton(j*37+11, 7, 29, 29, 300+j+BT_HIDE, 0xE4DFE1);
	_PutImage(Form.cwidth-48,14, 40,19, #URLgoto);
	DefineButton(Form.cwidth-28,15, 18, 16, GOTOURL+BT_HIDE, 0xE4DFE1);
	DefineButton(Form.cwidth-47,15, 17, 16, SEARCHWEB+BT_HIDE, 0xE4DFE1);
	DrawRectangle(205,14,Form.cwidth-205-49,18,0x94AECE); //around adress bar
	DrawRectangle(206,15,Form.cwidth-205-50,16,0xE4ECF3);

	SetElementSizes();
	WB1.ShowPage();

	DefineButton(scroll1.start_x+1, scroll1.start_y+1, 16, 16, ID1+BT_HIDE, 0xE4DFE1);
	DefineButton(scroll1.start_x+1, scroll1.start_y+scroll1.size_y-18, 16, 16, ID2+BT_HIDE, 0xE4DFE1);
}


stop:
