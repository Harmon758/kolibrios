//CODED by Veliant, Leency, Nable, Pavelyakov. GNU GPL licence.

#ifndef INCLUDE_KOLIBRI_H
#define INCLUDE_KOLIBRI_H

#pragma option OST
#pragma option ON
#pragma option cri-
#pragma option -CPA
#initallvar 0
#jumptomain FALSE

#startaddress 0

#code32 TRUE

char   os_name[8]   = {'M','E','N','U','E','T','0','1'};
dword  os_version   = 0x00000001;
dword  start_addr   = #______INIT______;
dword  final_addr   = #______STOP______+32;
dword  alloc_mem    = MEMSIZE;
dword  x86esp_reg   = MEMSIZE;
dword  I_Param      = #param;
dword  I_Path       = #program_path;
char param[4096];
char program_path[4096];

#define bool      int

#define NULL      0
#define OLD      -1
#define true      1
#define false     0

//Process Events
#define evReDraw  1
#define evKey     2
#define evButton  3
#define evExit    4
#define evDesktop 5
#define evMouse   6
#define evIPC     7
#define evNetwork 8
#define evDebug   9

//Event mask bits for function 40
#define EVM_REDRAW                1b
#define EVM_KEY                  10b
#define EVM_BUTTON              100b
#define EVM_EXIT               1000b
#define EVM_BACKGROUND        10000b
#define EVM_MOUSE            100000b
#define EVM_IPC             1000000b
#define EVM_STACK          10000000b
#define EVM_DEBUG         100000000b
#define EVM_STACK2       1000000000b
#define EVM_MOUSE_FILTER  0x80000000
#define EVM_CURSOR_FILTER 0x40000000

//Button options
#define BT_DEL      0x80000000
#define BT_HIDE     0x40000000
#define BT_NOFRAME  0x20000000

#define CLOSE_BTN 1

//-------------------------------------------------------------------------

#include "../lib/system.h"
#include "../lib/mouse.h"
#include "../lib/keyboard.h"

inline fastcall dword calc(EAX) { return EAX; }

:struct raw_image {
	dword w, h, data;
};

//------------------------------------------------------------------------------
:dword wait_event_code;
inline fastcall dword WaitEvent()
{
	$mov eax,10
	$int 0x40
	wait_event_code = EAX;
	//if(wait_event_code==evMouse) MOUSE.get();
	//return wait_event_code;
}

inline fastcall dword CheckEvent()
{
	$mov eax,11
	$int 0x40
	wait_event_code = EAX;
}

:dword WaitEventTimeout(dword time)
{
	EAX = 23;
	EBX = time;
	$int 0x40
	wait_event_code = EAX;
} 
 
inline fastcall dword SetEventMask(EBX)
{
	$mov eax,40
	$int 0x40
}


inline fastcall pause(EBX)
{
	$mov eax, 5
	$int 0x40
}

inline fastcall word GetButtonID()
{
	$mov eax,17
	$int  0x40
	$shr eax,8
}

inline fastcall dword GetFreeRAM()
{
	$mov eax, 18
	$mov ebx, 16
	$int 0x40
	//return eax = free RAM size in Kb
}

inline fastcall dword GetTotalRAM()
{
	$mov eax, 18
	$mov ebx, 17
	$int 0x40
	//return eax = total RAM size in Kb
}

inline fastcall int GetCpuIdleCount()
{
	EAX = 18;
	EBX = 4;
	$int 0x40
}

inline fastcall int GetCpuFrequency()
{
	EAX = 18;
	EBX = 5;
	$int 0x40
}

inline fastcall dword LoadDriver(ECX) //ECX - èìÿ äðàéâåðà
{
	$mov eax, 68
	$mov ebx, 16
	$int 0x40
	//return 0 - íåóäà÷à, èíà÷å eax = õýíäë äðàéâåðà 
}

inline fastcall dword RuleDriver(ECX) //óêàçàòåëü íà óïðàâëÿþùóþ ñòðóêòóðó
{
	$mov eax, 68
	$mov ebx, 17
	$int 0x40
	//return eax = îïðåäåëÿåòñÿ äðàéâåðîì
}

struct proc_info
{
	#define SelfInfo -1
	dword	use_cpu;
	word	pos_in_stack,slot,rezerv1;
	unsigned char name[11];
	char	rezerv2;
	dword	adress,use_memory,ID,left,top,width,height;
	word	status_slot,rezerv3;
	dword	work_left,work_top,work_width,work_height;
	char	status_window;
	dword   cwidth,cheight;
	byte    reserved[1024-71-8];
};

:void GetProcessInfo(dword _process_struct_pointer, _process_id)
{
	skin_height = GetSkinHeight();
	EAX = 9;
	EBX = _process_struct_pointer;
	ECX = _process_id;
	$int  0x40
	DSDWORD[EBX+71] = DSDWORD[EBX+42] - 9; //set cwidth
	DSDWORD[EBX+75] = DSDWORD[EBX+46] - skin_height - 4; //set cheight
}

inline fastcall int GetPointOwner( EBX, ECX) //ebx=m.x, ecx=m.y
{
	$mov eax,34
	$int 0x40
}

inline fastcall int GetProcessSlot( ECX)
{
	EAX = 18;
	EBX = 21;
	$int 0x40
}

inline fastcall int GetActiveProcess()
{
	EAX = 18;
	EBX = 7;
	$int 0x40
}

:int CheckActiveProcess(int Form_ID)
{
	int id9=GetProcessSlot(Form_ID);
	if (id9==GetActiveProcess()) return 1;
	return 0;
}

inline fastcall void ActivateWindow( ECX) //ECX - slot
{
	EAX = 18;
	EBX = 3;
	$int 0x40
}

:void RefreshWindow(dword ID_REFRESH,ID_ACTIVE)
{
	ActivateWindow(ID_REFRESH);
	ActivateWindow(ID_ACTIVE);
}

inline fastcall int MinimizeWindow()
{
	EAX = 18;
	EBX = 10;
	$int 0x40
}

inline fastcall int CreateThread(ECX,EDX)
{
	$mov eax,51
	$mov ebx,1
	$int 0x40
}

inline fastcall void SwitchToAnotherThread()
{
	$mov eax,68
	$mov ebx,1
	$int 0x40
}

inline fastcall int SendWindowMessage( ECX, EDX)
{
	$mov eax, 72
	$mov ebx, 1
	$int 0x40
}

inline fastcall int KillProcess( ECX)
{
	$mov eax,18;
	$mov ebx,18;
	$int 0x40
}

#define TURN_OFF 2
#define REBOOT 3
#define KERNEL 4
inline fastcall int ExitSystem( ECX)
{
	$mov eax, 18
	$mov ebx, 9
	$int 0x40
}

inline fastcall ExitProcess()
{
	$mov eax,-1;
	$int 0x40
}

//------------------------------------------------------------------------------

//eax = ÿçûê ñèñòåìû (1=eng, 2=fi, 3=ger, 4=rus)
#define SYS_LANG_ENG 1
#define SYS_LANG_FIN 2
#define SYS_LANG_GER 3
#define SYS_LANG_RUS 4
inline fastcall int GetSystemLanguage()
{
	EAX = 26;
	EBX = 5;
	$int 0x40
}

#define BUTTON_STYLE_FLAT 0
#define BUTTON_STYLE_GRADIENT 1
inline fastcall SetButtonStyle(ECX) //ECX - button style
{
	$mov eax,48
	$mov ebx,1
	int 64
}

inline fastcall GetSkinHeight()
{
	$push ebx
	$mov  eax,48
	$mov  ebx,4
	$int 0x40
	$pop  ebx
}

inline fastcall void SetSystemSkin( ECX)
{
	EAX = 48;
	EBX = 8;
	$int 0x40
}

inline fastcall int GetScreenWidth()
{
	$mov eax, 14
	$int 0x40
	$shr eax, 16
}

inline fastcall int GetScreenHeight()
{
	$mov eax, 14
	$int 0x40
	$and eax,0x0000FFFF
}

inline fastcall int GetClientTop()
{
	$mov eax, 48
	$mov ebx, 5
	$int 0x40
	$mov eax, ebx
	$shr eax, 16
}

inline fastcall int GetClientHeight()
{
	$mov eax, 48
	$mov ebx, 5
	$int 0x40
	$mov eax, ebx
}

inline fastcall dword LoadLibrary( ECX)
{
	$mov eax, 68 
	$mov ebx, 19
	$int  0x40
}

inline fastcall int TestBit( EAX, CL)
{
	$shr eax,cl
	$and eax,1
}

:void SetClientScreenArea(dword _left, _right, _top, _bottom)
{
	EAX = 48;
	EBX = 6;
	ECX = _left * 65536 + _right;
	EDX = _top * 65536 + _bottom;
	$int 64;
}

//------------------------------------------------------------------------------

:void DefineAndDrawWindow(dword _x, _y, _w, _h, _window_type, _bgcolor, _title, _flags)
{
	EAX = 12;              // function 12:tell os about windowdraw
	EBX = 1;
	$int 0x40
	
	$xor EAX,EAX
	EBX = _x << 16 + _w; 
	ECX = _y << 16 + _h;
	EDX = _window_type << 24 | _bgcolor;
	EDI = _title;
	ESI = _flags;
	$int 0x40


	EAX = 12;              // function 12:tell os about windowdraw
	EBX = 2;
	$int 0x40
}

inline fastcall MoveSize( EBX,ECX,EDX,ESI)
{
	$mov eax, 67
	$int 0x40
}

inline fastcall void DrawTitle( ECX)
{
	EAX = 71;
	EBX = 1;
	$int 0x40;
}

// @EDX is a process id, -1 for self
// @ESI is a new LayerBehaviour
// @RETURN: EAX, 0 is fail, 1 is success
#define ZPOS_DESKTOP     -2
#define ZPOS_ALWAYS_BACK -1
#define ZPOS_NORMAL      0
#define ZPOS_ALWAYS_TOP  1
inline fastcall dword SetWindowLayerBehaviour(EDX, ESI)
{
	EAX = 18;
	EBX = 25;
	ECX = 2;
	$int 64
}

:void WriteTextB(dword x,y,byte fontType, dword color, str_offset)
{
	EAX = 4;
	EBX = x<<16+y;
	ECX = fontType<<24+color;
	EDX = str_offset;
	ESI = 0;
	$int 0x40;
	$add ebx, 1<<16
	$int 0x40
}

:void WriteText(dword x,y,byte fontType, dword color, str_offset)
{
	EAX = 4;
	EBX = x<<16+y;
	ECX = fontType<<24+color;
	EDX = str_offset;
	$int 0x40;
}

:dword WriteBufText(dword x,y,byte fontType, dword color, str_offset, buf_offset)
{
	EDI = buf_offset;
	WriteText(x,y, fontType, color, str_offset);
}

:void WriteTextWithBg(dword x,y,byte fontType, dword color, str_offset, bgcolor)
{
	EDI = bgcolor;
	WriteText(x,y, fontType, color, str_offset);
}

:void WriteNumber(dword x,y,byte fontType, dword color, count, number_or_offset)
{
	EAX = 47;
	EBX = count<<16;
	ECX = number_or_offset;
	EDX = x<<16+y;
	ESI = fontType<<24+color;
	$int 0x40;
}

:void CopyScreen(dword dst_offset, x, y, w, h)
{
  EAX = 36;
  EBX = dst_offset;
  ECX = w << 16 + h;
  EDX = x << 16 + y;
  $int  0x40;
}

:dword GetPixelColorFromScreen(dword _x, _y)
{
	EAX = 35;
	EBX = _y * screen.width + _x;
	$int 64
}

#define DRAW_DESKTOP_BG_TILE 1
#define DRAW_DESKTOP_BG_STRETCH 2
:void SetBackgroundImage(dword w,h, image, mode)
{
	$mov     eax,15      // SF_BACKGROUND_SET 15 - work with desktop background graphics
	$mov     ebx,4       // SSF_MODE_BG 4 - set drawing mode for the background
	$mov     ecx,mode    // drawing mode - tile (1), stretch (2)
	$int     64

	$mov     eax,15      // SF_BACKGROUND_SET 15 - work with desktop background graphics
	$mov     ebx,1       // SSF_SIZE_BG=1 - set a size of the background image
	$mov     ecx,w       // width
	$mov     edx,h       // height
	$int     64

	$mov     eax,15      // SF_BACKGROUND_SET 15 - work with desktop background graphics
	$mov     ebx,5       // SSF_IMAGE_BG=5 - put block of pixels on the background image
	$mov     ecx,image   // pointer to the data in the format BBGGRRBBGGRR...
	$mov     edx,0       // offset in data of the background image
	ESI      = 3*w*h;    // size of data in bytes = 3 * number of pixels
	$int     64

	$mov     eax,15      // SF_BACKGROUND_SET 15 - work with desktop background graphics
	$mov     ebx,3       // SSF_REDRAW_BG=3 - redraw background
	$int     64
}

:void _PutImage(dword x,y, w,h, data_offset)
{
	EAX = 7;
	EBX = data_offset;
	ECX = w<<16+h;
	EDX = x<<16+y;
	$int 0x40
}

:void PutPaletteImage(dword inbuf,w,h,x,y,bits,pal)
{
	EAX = 65;
	EBX = inbuf;
	ECX = w<<16+h;
	EDX = x<<16+y;
	ESI = bits;
	EDI = pal;
	EBP = 0;
	$int 0x40
}

inline fastcall void PutPixel( EBX,ECX,EDX)
{
  EAX=1;
  $int 0x40
}

:void DrawBar(dword x,y,w,h,color)
{
	if (h<=0) || (h>60000) || (w<=0) || (w>60000) return; //bad boy :)
	EAX = 13;
	EBX = x<<16+w;
	ECX = y<<16+h;
	EDX = color;
	$int 0x40
}

:void DefineButton(dword x,y,w,h,id,color)
{
	EAX = 8;
	EDX = id + BT_DEL;
	$int 0x40;
	EDX = id;
	ESI = color;
	EBX = x<<16+w;
	ECX = y<<16+h;
	$int 0x40
}

:void UnsafeDefineButton(dword x,y,w,h,id,color)
{
	EAX = 8;
	EBX = x<<16+w;
	ECX = y<<16+h;
	EDX = id;
	ESI = color;
	$int 0x40
}

:void DefineDragableWindow(dword _x, _y, _w, _h)
{
	DefineAndDrawWindow(_x, _y, _w, _h, 0x41,0x000000,NULL,0b);
}

:void DefineUnDragableWindow(dword _x, _y, _w, _h)
{
	DefineAndDrawWindow(_x, _y, _w, _h, 0x01, 0, 0, 0x01fffFFF);
}

:void EventDragWindow()
{
	proc_info Form1;
	dword tmp_x,tmp_y;
	dword z1,z2;
	tmp_x = mouse.x;
	tmp_y = mouse.y;
	do {
		GetProcessInfo(#Form1, SelfInfo);
		mouse.get();
		if (tmp_x!=mouse.x) || (tmp_y!=mouse.y) 
		{
			z1 = Form1.left + mouse.x - tmp_x;
			z2 = Form1.top + mouse.y - tmp_y;
			if(z1<=10) || (z1>20000) z1=0; else if(z1>screen.width-Form1.width-10)z1=screen.width-Form1.width;
			if(z2<=10) || (z2>20000) z2=0; else if(z2>screen.height-Form1.height-10)z2=screen.height-Form1.height;
			MoveSize(z1 , z2, OLD, OLD);
			draw_window();
		}
		pause(1);
	} while (mouse.lkm);
}

:void DefineHiddenButton(dword _x, _y, _w, _h, _id)
{
	DefineButton(_x, _y, _w, _h, _id + BT_HIDE, 0);
}

inline fastcall void DeleteButton( EDX)
{
	EAX = 8;
	EDX += BT_DEL;
	$int 0x40
}

inline fastcall dword GetStartTime()
{
	$mov eax,26
	$mov ebx,9
	$int 0x40
}

:dword X_EventRedrawWindow,Y_EventRedrawWindow;
:void _EventRedrawWindow()
{
	DefineAndDrawWindow(X_EventRedrawWindow,Y_EventRedrawWindow,1,1,0x34,0xFFFFFF,NULL,0);
	//pause(10);
	ExitProcess();
}
:char REDRAW_BUFF_EVENT_[4096];
:void EventRedrawWindow(dword x,y)
{
	X_EventRedrawWindow = x;
	Y_EventRedrawWindow = y;
	CreateThread(#_EventRedrawWindow,#REDRAW_BUFF_EVENT_+4092);
}

:struct _screen
{
	dword width,height;
} screen;

:byte skin_height;

:void DrawDate(dword x, y, color, in_date)
{
	EDI = in_date;
	EAX = 47;
	EBX = 2<<16;
	EDX = x<<16+y;
	ESI = 0x90<<24+color;
	ECX = EDI.date.day;
	$int 0x40;
	EDX += 20<<16;
	ECX = EDI.date.month;
	$int 0x40;
	EDX += 20<<16;
	EBX = 4<<16;
	ECX = EDI.date.year;
	$int 0x40;
	DrawBar(x+17,y+10,2,2,color);
	DrawBar(x+37,y+10,2,2,color);
}

:void __path_name__(dword BUF,PATH)
{
	dword beg = PATH;
	dword pos = PATH;
	dword sav = PATH;
	dword i;
	while(DSBYTE[pos])
	{
		if(DSBYTE[pos]=='/')sav = pos;
		pos++;
	}
	i = sav-beg;
	while(i)
	{
		DSBYTE[BUF] = DSBYTE[beg];
		beg++;
		BUF++;
		i--;
	}
	/*while(DSBYTE[beg])
	{
		DSBYTE[BUF1] = DSBYTE[beg];
		beg++;
		BUF1++;
	}*/
	//DSBYTE[BUF1] = 0;
	DSBYTE[BUF] = 0;
}
char __BUF_DIR__[4096];
:struct SELF
{
	dword dir;
	dword file;
	dword path;
} self;

dword __generator;  // random number generator - äëÿ ãåíåðàöèè ñëó÷àéíûõ ÷èñåë

:dword program_path_length;

//The initialization of the initial data before running
void ______INIT______()
{
	//if (program_path[0]!='/') I_Path++;
	
	self.dir = #__BUF_DIR__;
	self.file = 0;
	self.path = I_Path;
	__path_name__(#__BUF_DIR__,I_Path);
	
	skin_height   = GetSkinHeight();
	screen.width  = GetScreenWidth()+1;
	screen.height = GetScreenHeight()+1;
	
	__generator = GetStartTime();
	
	mem_init();


	main();
	ExitProcess();
}
______STOP______:
#endif

#ifndef INCLUDE_MEM_H
#include "../lib/mem.h"
#endif

#ifndef INCLUDE_DEBUG_H
#include "../lib/debug.h"
#endif