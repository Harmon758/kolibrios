
#pragma once

typedef unsigned __int32 Dword;
typedef unsigned __int16 Word;
typedef unsigned __int8 Byte;
//typedef unsigned __int32 size_t;

#define NULL 0

#define MAX_PATH				256

#define FO_READ					0
#define FO_WRITE				2

//Process Events
#define EM_WINDOW_REDRAW		1
#define EM_KEY_PRESS			2
#define EM_BUTTON_CLICK			3
#define EM_APP_CLOSE			4
#define EM_DRAW_BACKGROUND		5
#define EM_MOUSE_EVENT			6
#define EM_IPC					7
#define EM_NETWORK				8
#define EM_DEBUG				9

//Event mask bits for function 40
#define EVM_REDRAW        1
#define EVM_KEY           2
#define EVM_BUTTON        4
#define EVM_EXIT          8
#define EVM_BACKGROUND    16
#define EVM_MOUSE         32
#define EVM_IPC           64
#define EVM_STACK         128
#define EVM_DEBUG         256
#define EVM_STACK2        512
#define EVM_MOUSE_FILTER  0x80000000
#define EVM_CURSOR_FILTER 0x40000000

//Button options
#define BT_DEL      0x80000000
#define BT_HIDE     0x40000000
#define BT_NOFRAME  0x20000000
#define BT_NODRAW   BT_HIDE+BT_NOFRAME

#define KM_CHARS				0
#define KM_SCANS				1

#define WRS_BEGIN				1
#define WRS_END					2

#define PROCESS_ID_SELF			-1

#define abs(a) (a<0?0-a:a)

extern "C" double __cdecl acos(double x);
extern "C" double __cdecl asin(double x);
extern "C" double __cdecl floor(double x);
extern "C" double __cdecl round(double x);
#pragma function(acos,asin)
#if _MSC_VER > 1200
#pragma function(floor)
#endif


struct kosFileInfo
{
	Dword rwMode;
	Dword OffsetLow;
	char* OffsetHigh;
	Dword dataCount;
	Byte *bufferPtr;
	char fileURL[MAX_PATH];
};


struct kosSysColors {
	Dword nonset1;
	Dword nonset2;
	Dword work_dark;
	Dword work_light;
	Dword window_title;
	Dword work;
	Dword work_button;
	Dword work_button_text;
	Dword work_text;
	Dword work_graph;
};


struct RGB
{
	Byte b;
	Byte g;
	Byte r;
	//
	RGB() {};
	//
	RGB( Dword value )
	{
		r = (Byte)(value >> 16);
		g = (Byte)(value >> 8);
		b = (Byte)value;
	};
	//
	bool operator != ( RGB &another )
	{
		return this->b != another.b || this->g != another.g || this->r != another.r;
	};
	//
	bool operator == ( RGB &another )
	{
		return this->b == another.b && this->g == another.g && this->r == another.r;
	};
};


#pragma pack(push, 1)
union sProcessInfo
{
	Byte rawData[1024];
	struct
	{
		Dword cpu_usage;
		Word window_stack_position;
		Word window_slot; //slot
		Word reserved1;
		char process_name[12];
		Dword memory_start;
		Dword used_memory;
		Dword PID;
		Dword x_start;
		Dword y_start;
		Dword width;
		Dword height;
		Word slot_state;
		Word reserved3;
		Dword work_left;
		Dword work_top;
		Dword work_width;
		Dword work_height;
		char status_window;
		Dword cwidth;
		Dword cheight;
	} processInfo;
};
#pragma pack(pop)

#ifndef AUTOBUILD
//
extern char *kosExePath;
#endif

//
void crtStartUp();
//
int __cdecl _purecall();
//
int __cdecl atexit( void (__cdecl *func )( void ));
//
void rtlSrand( Dword seed );
Dword rtlRand( void );
//
char * __cdecl strcpy( char *target, const char *source );
int __cdecl strlen( const char *line );
char * __cdecl strrchr( const char * string, int c );
//
// if you have trouble here look at old SVN revisions for alternatives
void memcpy( void *dst, const void *src, size_t bytesCount );
void memset( Byte *dst, Byte filler, Dword count );
//
void sprintf( char *Str, char* Format, ... );
//
Dword rtlInterlockedExchange( Dword *target, Dword value );
// function -1 ���������� ��������
void kos_ExitApp();
// function 0
void kos_DefineAndDrawWindow(
	Word x, Word y,
	Word sizeX, Word sizeY,
	Byte mainAreaType, Dword mainAreaColour,
	Byte headerType, Dword headerColour,
	Dword borderColour
	);
// function 1 ��������� �����
void kos_PutPixel( Dword x, Dword y, Dword colour );
// function 2 �������� ��� ������� �������
bool kos_GetKey( Byte &keyCode );
// function 3 �������� �����
Dword kos_GetSystemClock();
// function 4
void __declspec(noinline) kos_WriteTextToWindow(
	Word x, Word y,
	Byte fontType,
	Dword textColour,
	char *textPtr,
	Dword textLen
	);
// function 7 ���������� �����������
void kos_PutImage( RGB * imagePtr, Word sizeX, Word sizeY, Word x, Word y );
// function 8 ���������� ������
void __declspec(noinline) kos_DefineButton( Word x, Word y, Word sizeX, Word sizeY, Dword buttonID, Dword colour );
//
void __declspec(noinline) kos_UnsaveDefineButton( Word x, Word y, Word sizeX, Word sizeY, Dword buttonID, Dword colour );
// function 5 �����, � ����� ����� �������
void kos_Pause( Dword value );
// function 9 - ���������� � ��������
Dword kos_ProcessInfo( sProcessInfo *targetPtr, Dword processID = PROCESS_ID_SELF );
// function 10
Dword kos_WaitForEvent();
// function 11
Dword kos_CheckForEvent();
// function 12
void kos_WindowRedrawStatus( Dword status );
// function 13 ���������� ������
void __declspec(noinline) kos_DrawBar( Word x, Word y, Word sizeX, Word sizeY, Dword colour );
// function 17
bool kos_GetButtonID( Dword &buttonID );
// function 23
Dword kos_WaitForEventTimeout( Dword timeOut );
//
enum eNumberBase
{
	nbDecimal = 0,
	nbHex,
	nbBin
};
// ��������� ���������� � ��������� "����" function 37
void kos_GetMouseState( Dword & buttons, int & cursorX, int & cursorY );
// function 38
void kos_DrawLine( Word x1, Word y1, Word x2, Word y2, Dword colour, Dword invert );
// function 40 ���������� ����� �������
void kos_SetMaskForEvents( Dword mask );
// function 47 ������� � ���� ���������� �����
void kos_DisplayNumberToWindow(
   Dword value,
   Dword digitsNum,
   Word x,
   Word y,
   Dword colour,
   eNumberBase nBase = nbDecimal,
   bool valueIsPointer = false
   );
// 48.3: get system colors
bool kos_GetSystemColors( kosSysColors *sc );
// function 63
void kos_DebugOutChar( char ccc );
//
void rtlDebugOutString( char *str );
//
void kos_DebugValue(char *str, int n);
// function 64 �������� ��������� ����, �������� == -1 �� ��������
void kos_ChangeWindow( Dword x, Dword y, Dword sizeX, Dword sizeY );
// function 67 ��������� ���������� ������, ���������� ��� ���������
bool kos_ApplicationMemoryResize( Dword targetSize );
// function 66 ����� ��������� ������ �� ����������
void kos_SetKeyboardDataMode( Dword mode );
// 68.11: init heap
void kos_InitHeap();
// function 70 ������ � �������� �������
Dword kos_FileSystemAccess( kosFileInfo *fileInfo );
// 70.7: run Kolibri application with param
int kos_AppRun(char* app_path, char* param);
//
void kos_Main();
