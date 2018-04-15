
#ifdef LANG_RUS
	?define TITLE_SETT "����ன��"
	?define SHOW_DEVICE_CLASS "�뢮���� �������� ����� ���ன��"
	?define SHOW_REAL_NAMES "�����뢠�� ����� 䠩��� �� ����� ॣ����"
	?define SHOW_STATUS_BAR "�����뢠�� ����� ���"
	?define NOTIFY_COPY_END "���������� � �����襭�� ����஢����"
	?define SHOW_BREADCRUMBS "�ᯮ�짮���� '嫥��� ��誨'"
	?define USE_TWO_PANELS "��� ������"
	?define FONT_SIZE_LABEL "������ ����"
	?define LIST_LINE_HEIGHT "���� ��ப� � ᯨ᪥"
	?define SAVE_PATH_AS_DEFAULT "����騩 ����"
	?define SAVE_START_PATH_AS_DEFAULT "�������� ����"
	?define EDIT_FILE_ASSOCIATIONS "������஢��� ���樠樨 䠩���"
	?define START_PATH " ���⮢� ����: "
#else
	?define TITLE_SETT "Settings"
	?define SHOW_DEVICE_CLASS "Show device class name"
	?define SHOW_REAL_NAMES "Show file names in original case"
	?define SHOW_STATUS_BAR "Show status bar"
	?define NOTIFY_COPY_END "Notify when copying finished"
	?define SHOW_BREADCRUMBS "Show breadcrumbs"
	?define USE_TWO_PANELS "Two panels"
	?define FONT_SIZE_LABEL "Font size"
	?define LIST_LINE_HEIGHT "List line height"
	?define SAVE_PATH_AS_DEFAULT "�urrent path"
	?define SAVE_START_PATH_AS_DEFAULT "Typed path"
	?define EDIT_FILE_ASSOCIATIONS "Edit file associations"
	?define START_PATH " Start path: "
#endif

int WinX, WinY, WinW, WinH;

dword set_mouse_dd;
char path_start[4096]="\0";
edit_box path_start_ed = {290,50,57,0xffffff,0x94AECE,0xffffff,0xffffff,0x10000000,4098,
	                      #path_start,#set_mouse_dd, 100000000000010b,0,0};

void settings_dialog()
{   
	byte id;
	active_settings=1;
	SetEventMask(0x27);
	loop(){
		switch(WaitEvent())
		{
			case evMouse:
				edit_box_mouse stdcall (#path_start_ed);
				break;
				
			case evButton: 
				id=GetButtonID();
				if (id==1) { ExitSettings(); break; }
				else if (id==5)
				{
					RunProgram("tinypad", "/sys/settings/assoc.ini");
					break;
				}
				else if (id==6)
				{
					strcpy(#path_start,#path);
					path_start_ed.size = strlen(#path_start);
					path_start_ed.pos = strlen(#path_start);
					ini.SetString("DefaultPath", #path, strlen(#path));
					edit_box_draw stdcall (#path_start_ed);
					break;
				}
				else if (id==7)
				{
					ini.SetString("DefaultPath", #path_start,strlen(#path_start));
					break;
				}
				else if (id==20) show_dev_name ^= 1;
				else if (id==21) { action_buf=109; real_files_names_case ^= 1; }
				else if (id==22) info_after_copy ^= 1;
				else if (id==24) two_panels ^= true;
				else if (id==32) show_breadcrumb ^= true;
				else if (id==25) { files.item_h++; files_active.item_h = files_inactive.item_h = files.item_h; }
				else if (id==26) && (files.item_h>16) files_inactive.item_h = files.item_h = files.item_h-1;
				else if (id==27) show_status_bar ^= 1;
				else if (id==30) { kfont.size.pt++; IF(!kfont.changeSIZE()) kfont.size.pt--; BigFontsChange(); }
				else if (id==31) { kfont.size.pt--; IF(!kfont.changeSIZE()) kfont.size.pt++; BigFontsChange(); }
				EventRedrawWindow(Form.left,Form.top);
				break;
					
			case evKey:
				GetKeys();
				if (key_scancode==SCAN_CODE_ESC) ExitSettings();
				EAX= key_ascii << 8;
				edit_box_key stdcall (#path_start_ed);	
				break;
				
			case evReDraw:
				DefineAndDrawWindow(Form.cwidth-300/2+Form.left, Form.cheight-292/2+Form.top, 380, 
					390+skin_height,0x34,system.color.work,TITLE_SETT,0);
				DrawSettingsCheckBoxes();
		}
	}
}

void ExitSettings()
{
	active_settings = 0;
	settings_window = 0;
	cmd_free = 4;
	ExitProcess();
}

void DrawSettingsCheckBoxes()
{
	incn y;
	int x=11, frx=26, but_x;
	y.n = 0;
	CheckBox(x, y.inc(14), 20, SHOW_DEVICE_CLASS,  show_dev_name);
	CheckBox(x, y.inc(25), 21, SHOW_REAL_NAMES,  real_files_names_case);
	CheckBox(x, y.inc(25), 27, SHOW_STATUS_BAR,  show_status_bar);
	CheckBox(x, y.inc(25), 22, NOTIFY_COPY_END,  info_after_copy);
	CheckBox(x, y.inc(25), 32, SHOW_BREADCRUMBS,  show_breadcrumb);
	CheckBox(x, y.inc(25), 24, USE_TWO_PANELS,  two_panels);
	MoreLessBox(x, y.inc(31), 30, 31, kfont.size.pt, FONT_SIZE_LABEL);
	MoreLessBox(x, y.inc(31), 25, 26, files.item_h, LIST_LINE_HEIGHT);
	
	DrawFrame(x, y.inc(37), 340, 95, START_PATH);
	// START_PATH {
	path_start_ed.top = y.inc(21);
	path_start_ed.left = frx;
	DrawEditBox(#path_start_ed);
	but_x = DrawStandartCaptButton(frx, y.inc(34), 6, SAVE_PATH_AS_DEFAULT);
	DrawStandartCaptButton(frx+but_x, y.inc(0), 7, SAVE_START_PATH_AS_DEFAULT);
	// } START_PATH

	DrawStandartCaptButton(x, y.inc(52), 5, EDIT_FILE_ASSOCIATIONS);
}


void LoadIniSettings()
{
	ini.path = GetIni(#eolite_ini_path, "EOLITE.INI");
	ini.section = "Config";

	files.SetFont(6, 9, 10000000b);
	real_files_names_case = ini.GetInt("RealFileNamesCase", 1); 
	show_dev_name   = ini.GetInt("ShowDeviceName", 1); 
	show_status_bar = ini.GetInt("ShowStatusBar", 1); 
	info_after_copy = ini.GetInt("InfoAfterCopy", 0); 
	kfont.size.pt   = ini.GetInt("FontSize", 13); 
	two_panels      = ini.GetInt("TwoPanels", 0); 
	files.item_h    = ini.GetInt("LineHeight", 19);
	WinX = ini.GetInt("WinX", 200); 
	WinY = ini.GetInt("WinY", 50); 
	WinW = ini.GetInt("WinW", 550); 
	WinH = ini.GetInt("WinH", 506); 
	ini.GetString("DefaultPath", #path, 4096, "/rd/1");
	ini.GetString("DefaultPath", #path_start, 4096, "/rd/1");
	path_start_ed.size = strlen(#path_start);
	path_start_ed.pos = strlen(#path_start);

	ini_get_str stdcall ("/sys/SETTINGS/SYSTEM.INI", "system", "font file",#temp,4096,DEFAULT_FONT);
	kfont.init(#temp);
	ini_get_str stdcall ("/sys/SETTINGS/SYSTEM.INI", "system", "font smoothing",#temp,4096,"on");
	if(!strcmp(#temp,"off")) kfont.smooth = false; else kfont.smooth = true;
}


void SaveIniSettings()
{
	ini.SetInt("ShowDeviceName", show_dev_name);
	ini.SetInt("ShowStatusBar", show_status_bar);
	ini.SetInt("RealFileNamesCase", real_files_names_case);
	ini.SetInt("InfoAfterCopy", info_after_copy);
	ini.SetInt("FontSize", kfont.size.pt);
	ini.SetInt("TwoPanels", two_panels);
	ini.SetInt("LineHeight", files.item_h);
	ini.SetInt("WinX", Form.left);
	ini.SetInt("WinY", Form.top);
	ini.SetInt("WinW", Form.width);
	ini.SetInt("WinH", Form.height);
}



void Write_Error(int error_number)
{
	char error_message[500];
	dword ii;
	if (files.cur_y>=0) Line_ReDraw(0xFF0000, files.cur_y);
	pause(5);
	sprintf(#error_message,"\"%s\n%s\" -%s","Eolite",get_error(error_number),"tE");
	notify(#error_message);	
}


void SetAppColors()
{
	system.color.get();
	//system.color.work = 0xE4DFE1;
	//system.color.work_text = 0;
	//system.color.work_graph  = 0x7E87A3; //A0A0B8;
	//system.color.work_button = 0x7E87A3;
	//system.color.work_button_text = 0x000000
	col_work    = 0xE4DFE1;
	col_padding = 0xC8C9C9;
	col_selec   = 0x94AECE;
	col_lpanel  = 0x00699C;
	col_graph   = 0x7E87A3;
}


void BigFontsChange()
{
	files.item_h = kfont.size.pt + 4;
	if (files.item_h<18) files.item_h = 18;
	files_active.item_h = files_inactive.item_h = files.item_h;
}
