//Leency - 2012

#include "imgs\logo.txt"

#define BROWSER_PATH	"/sys/htmlv"
#define BROWSER_LINK	"http://kolibri-os.narod.ru"


void about_dialog()
{   
	byte p;
	mouse mm;
	byte id, letitclose=0;
	SetEventMask(100111b); 
	loop() switch(WaitEvent())
	{
		case evMouse:
				mm.get();
				//������
				IF (mm.x>85) && (mm.x<155) && (mm.y>190) && (mm.y<190+22)
				IF (mm.lkm) {DrawRegion_3D(86,191,68,20,0xC7C7C7,0xFFFFFF); letitclose=1;}
				ELSE {IF (letitclose) {DrawRegion_3D(86,191,68,20,0xFFFFFF,0xC7C7C7); Pause(7); ExitProcess();}}
				ELSE IF (letitclose) {letitclose=0; DrawRegion_3D(86,191,68,20,0xFFFFFF,0xC7C7C7);}
				break; 
				
		case evButton: 
				id=GetButtonID();
				IF (id==1) ExitProcess();
				IF (id==23) RunProgram(BROWSER_PATH, BROWSER_LINK);
				break;
				
		case evKey:
				IF (GetKey()==27) ExitProcess();
				break;
				
		case evReDraw:
				DefineAndDrawWindow(600,150,181,256,0x34,0x10EFEBEF,0,0,"About Eolite");
				DrawBar(0,0,172,50,0x8494C4); //������� �����
				PutPaletteImage(#logo,85,85,43,7,#logo_pal);
				WriteText(46,100,0x90,0xBF40BF,"Eolite v1.28",0);
					$add ebx, 1<<16
					$int 0x40
				WriteText(55,120,0x80,0,"Developers:",0); 
				WriteText(39,130,0x80,0,"Leency & Veliant",0); 
				WriteText(45,140,0x80,0,"KolibriOS Team",0);
				WriteText(61,150,0x80,0,"2008-2012",0);
				WriteText(12,170,0x80,0,"Visit",0);
				DrawLink(48,170,23, "kolibri-os.narod.ru"); //�����a
				DrawFlatButton(85,190,70,22,0,0xE4DFE1, "Close");
				
				//PutPaletteImage(3*16*15+#ficons,16,15,40,195,#ficons_pal);								
	}
}

void DrawLink(dword x,y,btn_id, inscription)
{
	WriteText(x,y,0x80,0x4E00E7,inscription,0);
	DrawBar(x,y+8,strlen(inscription)*6,1,0x4E00E7); //����������� ������
	DefineButton(x-1,y-1,strlen(inscription)*6,10,btn_id+BT_HIDE,0);
}