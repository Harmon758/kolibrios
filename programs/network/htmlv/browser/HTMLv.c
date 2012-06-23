//HTML Viewer in C--
//Copyright 2007-2012 by Veliant & Leency
//Asper, lev, Lrz, Barsuk, Nable.
//home icon - rachel fu, GPL licence

#include "..\lib\kolibri.h"
#include "..\lib\encoding.h"
#include "..\lib\file_system.h"
#include "..\lib\mem.h"
#include "..\lib\dll.h"
//����������
#include "..\lib\libio_lib.h"
#include "..\lib\libimg_lib.h"
#include "..\lib\edit_box_lib.h"
#include "..\lib\scroll_bar\scroll_lib.h"
#include "..\lib\ttf_fonts.h"
//��������
#include "img\toolbar_icons.c"
#include "img\URLgoto.txt";


//����������
char URL[4096],
	editURL[4096],
	page_links[12000],
	header[512];

struct lines{
	int visible, all, first, column_max;
};

int	mouse_dd;
edit_box edit1= {250,207,16,0xffffff,0x94AECE,0xffffff,0xffffff,0,248,#editURL,#mouse_dd,2,19,19};
scroll_bar scroll1 = { 18,200,398, 44,18,0,115,15,0,0xeeeeee,0xD2CED0,0x555555,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1}; //details in scroll_lib.h--

proc_info Form;
dword stak[100]; //���� ��� 
mouse m;

#include "TWB.h"
#include "include\menu_rmb.h"


void main()
{
	int btn;
	byte key;
	int half_scroll_size;
	
	mem_Init();
	load_dll2(libio, #libio_init,1);
	load_dll2(libimg, #libimg_init,1);
	load_dll2(boxlib, #edit_box_draw,0);
	load_dll2(#abox_lib, #boxlib_init,0);
	//load_dll2(libtruetype, #truetype,0);
	
	if (param) strcpy(#URL, #param);
		else strcpy(#URL, "/sys/index.htm");
	strcpy(#editURL, #URL);
	
	lines.column_max = 101;
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
				
				btn=GetSlot(Form.ID); 
				IF (btn<>ActiveProcess()) break; //���� ���� �� ������� �� ������� ���� �� ���������

				edit_box_mouse stdcall (#edit1);

				m.get();
				
				if (m.pkm) && (m.y>WB1.top) && (m.y<Form.height) && (filesize)
				{
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
				
				IF (lines.all<lines.visible) break;
				half_scroll_size = WB1.height - 16 * lines.visible / lines.all - 3 /2;
				if (m.x>=WB1.width-14) && (m.x<=WB1.width+6)
				&& (m.y>WB1.top+16) && (m.y<WB1.top+WB1.height-16)
				&& (lines.all>lines.visible) while (m.lkm)
				{
					IF (half_scroll_size+WB1.top>m.y) || (m.y<0) || (m.y>4000) m.y=half_scroll_size+WB1.top; //���� ������ ��� �����
					btn=lines.first; //��������� ������ ����������
					lines.first = m.y -half_scroll_size -WB1.top * lines.all / WB1.height;
					IF (lines.visible+lines.first>lines.all) lines.first=lines.all-lines.visible;
					IF (btn<>lines.first) WB1.ParseHTML(buf); //���� ������ ��� �� ��������������
					m.get();
				}

				break;
			case evButton:
				btn=GetButtonID();
				IF (btn==1)
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
				
				if (edit1.flags == 66) || (edit1.flags == 98) SWITCH(key) //���� ������� ������ ������ ���������� ��������� ������
					{ CASE 52: CASE 53: CASE 54: goto _EDIT_MARK; } 

				WB1.Scan(key);
				
				_EDIT_MARK:
				IF (key<>0x0d) && (key<>183) && (key<>184) && (key<>173) {EAX=key<<8; edit_box_key stdcall(#edit1);} //�������� ������
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


void Draw_Window()
{
	int j;
	DefineAndDrawWindow(215,100,640,480,0x73,0x00E4DFE1,0,0,0);

	GetProcessInfo(#Form, SelfInfo);
	if (Form.status_window>2) //���� �������� � ���������, ������ �� ������
	{
		DrawTitle(#header);
		return;
	}
	if (Form.height<120) MoveSize(OLD,OLD,OLD,120);
	if (Form.width<280) MoveSize(OLD,OLD,280,OLD);
	
	PutPaletteImage(#toolbar,200,42,0,0,8,#toolbar_pal);
	if (GetProcessSlot(downloader_id)<>0) PutImage(#stop_btn,24,24,88,10);
	
	DrawBar(200,0,onLeft(200,9),43,0xE4DFE1); //����������� ��� ��� ��������
	DrawBar(0,42,onLeft(5,4),1,0xE2DBDC); //����������
	DrawBar(0,43,onLeft(5,4),1,0xD2CED0); //����������
	for (j=0; j<5; j++) DefineButton(j*37+11, 7, 29, 29, 300+j+BT_HIDE, 0xE4DFE1);
	PutImage(#URLgoto,40,19,onLeft(57,0),14);
	DefineButton(onLeft(37,0),15, 18, 16, GOTOURL+BT_HIDE, 0xE4DFE1);
	DefineButton(onLeft(56,0),15, 17, 16, SEARCHWEB+BT_HIDE, 0xE4DFE1);
	DrawRegion(205,14,onLeft(58,205),18,0x94AECE); //������ ������ ������
	DrawRegion(206,15,onLeft(59,205),16,0xE4ECF3);

	edit1.width=Form.width-266;
	WB1.top=44;
	WB1.width=Form.width-13;
	WB1.height=onTop(43,5);
	lines.column_max = WB1.width - 30 / 6;
	lines.visible = WB1.height - 3 / 10 - 2;

	WB1.ShowPage();
	
	DefineButton(scroll1.start_x+1, scroll1.start_y+1, 16, 16, ID1+BT_HIDE, 0xE4DFE1);
	DefineButton(scroll1.start_x+1, scroll1.start_y+scroll1.size_y-18, 16, 16, ID2+BT_HIDE, 0xE4DFE1);
}
 
int onLeft(dword right,left) {return Form.width-right-left;}
int onTop(dword down,up) {return Form.height-GetSkinWidth()-down-up;}


stop:
