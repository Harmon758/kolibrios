//===================================================//
//                                                   //
//                   TRANSLATIONS                    //
//                                                   //
//===================================================//

#ifdef LANG_RUS

#define INTRO_TEXT "�� ���⮩ ��ᬮ��騪 ⥪��.\n���஡�� ������ �����-����� ⥪�⮢� 䠩�."
#define VERSION "Text Reader v1.41"
#define ABOUT "����: Leency, punk_joker
���: Leency, Veliant, KolibriOS Team

����稥 ������:
Ctrl+O - ������ 䠩�
Ctrl+I - �������� ���ଠ�� � 䠩��
Ctrl+���� - 㢥����� ����
Ctrl+����� - 㬥����� ����
Ctrl+Tab - ����� ����஢�� ⥪��
Ctrl+E - ������ 䠩� � ��㣮� �ணࠬ��

����:
Ctrl+F - ������ ������ ���᪠
F3 - �᪠�� �����
Esc - ����� ������ ���᪠
 
������ ���� �������..."

char color_scheme_names[] =
"���� �� �����
���� �� �஬   |RtfRead
���� �� �쭥    |Horst  
���� �� 宫��  |Pocket 
���� �� ���⮬  |Fb2Read
���� �� ⥬���1  |Godot  
���� �� ⥬���2  |Monokai";

#else

#define INTRO_TEXT "This is a plain Text Reader.\nTry to open some text file."
#define VERSION "Text Reader v1.41"
#define ABOUT "Idea: Leency, punk_joker
Code: Leency, Veliant, KolibriOS Team

Hotkeys:
Ctrl+O - open file
Ctrl+I - show file properties
Ctrl+Up - bigger font
Ctrl+Down - smaller font
Ctrl+Tab - select charset
Ctrl+E - reopen current file in another app

Search:
Ctrl+F - open search
F3 - search next
Esc - hide search bar
 
Press any key..."

char color_scheme_names[] =
"Black & White
Black & Grey    |RtfRead
Black & Linen   |Horst  
Black & Antique |Pocket 
Black & Lemon   |Fb2Read
Grey & DarkGrey |Godot  
Grey & DarkGrey |Monokai";

#endif

//===================================================//
//                                                   //
//                       DATA                        //
//                                                   //
//===================================================//

dword color_schemes[] = {
0xFFFfff, 0,
0xF0F0F0, 0,
0xFDF6E3, 0x101A21,
0xFCF0DA, 0x171501,
0xF0F0C7, 0,
0x282C34, 0xABB2BF,
0x282923, 0xD8D8D2
};

char default_dir[] = "/rd/1";
od_filter filter2 = { 8, "TXT\0\0" };