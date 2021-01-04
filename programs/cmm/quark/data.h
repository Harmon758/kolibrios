//===================================================//
//                                                   //
//                   TRANSLATIONS                    //
//                                                   //
//===================================================//

char short_app_name[] = "Quark";

#ifdef LANG_RUS

char intro[] = "�� ���⮩ ��ᬮ��騪 ⥪��.
���஡�� ������ ⥪�⮢� 䠩�.";

char copied_chars[] = "�����஢��� ᨢ����: %i";

char about[] = "Quark Text v0.9
����: ��ਫ� ����⮢ aka Leency
����: http://aspero.pro

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
Esc - ����� ������ ���᪠";

char color_scheme_names[] = "���஦��\n��ᬮ�   ";
?define FILE_SAVED_WELL "'���� �ᯥ譮 ��࠭��'O"
?define FILE_NOT_SAVED "'�訡�� �� ��࠭���� 䠩��!'E"

char rmb_menu[] = 
"��१���|Ctrl+X
����஢���|Ctrl+C
��⠢���|Ctrl+V
-
������ � �����
����஢��� ���� 䠩��";

?define T_MATCHES "�������: %i   "
?define T_FIND_NEXT "���� �����"

#else

char intro[] = "Quark is a simple text viewer.
Try to open some text file.";

char copied_chars[] = "Copied %i chars";

char about[] = "Quark Text v0.91
Author: Kiril Lipatov aka Leency 
Website: http://aspero.pro

Hotkeys:
Ctrl+O - open file
Ctrl+I - show file properties
Ctrl+Plus - bigger font
Ctrl+Down - smaller font
Ctrl+Tab - select charset
Ctrl+E - reopen current file in another app

Search:
Ctrl+F - open search
F3 - search next
Esc - hide search bar";

char color_scheme_names[] = "Dairy\nCosmos   ";
?define FILE_SAVED_WELL "'File saved'O"
?define FILE_NOT_SAVED "'Error saving file!'E"

char rmb_menu[] = 
"Cut|Ctrl+X
Copy|Ctrl+C
Paste|Ctrl+V
-
Reveal in folder
Copy file path";

?define T_MATCHES "Matches: %i   "
?define T_FIND_NEXT " Find next "

#endif

//===================================================//
//                                                   //
//                       DATA                        //
//                                                   //
//===================================================//

dword color_schemes[] = {
//bg,     text,     scroll,   selected, cursor
0xFCF0DA, 0x171501, 0xB2ACA0, 0xD8CAA7, 0xFF0000, 0xFEC53A, //Dairy
0x282923, 0xD8D8D2, 0x555551, 0x5A574A, 0xFFFfff, 0x9D7E00 //Cosmos
};

struct THEME
{
	dword bg, text, cursor, found;
} theme;

char default_dir[] = "/rd/1";
od_filter filter2 = { 33, "TXT\0ASM\0HTM\0HTML\0C\0H\0C--\0H--\0CPP\0\0" };

CANVAS canvas;

dword cursor_pos=0;

collection_int lines = {0};

//===================================================//
//                                                   //
//                     SETTINGS                      //
//                                                   //
//===================================================//


_ini ini = { "/sys/settings/app.ini", "Quark" };

void LoadIniSettings()
{
	font_size     = ini.GetInt("FontSize", 'M');
	user_encoding = ini.GetInt("Encoding", CH_AUTO);
	curcol_scheme = ini.GetInt("ColorScheme", 0);
	Form.left     = ini.GetInt("WinX", 150); 
	Form.top      = ini.GetInt("WinY", 50); 
	Form.width    = ini.GetInt("WinW", 640); 
	Form.height   = ini.GetInt("WinH", 563);
}

void SaveIniSettings()
{
	ini.SetInt("FontSize", font_size);
	ini.SetInt("Encoding", user_encoding);
	ini.SetInt("ColorScheme", curcol_scheme);
	ini.SetInt("WinX", Form.left);
	ini.SetInt("WinY", Form.top);
	ini.SetInt("WinW", Form.width);
	ini.SetInt("WinH", Form.height);
}


