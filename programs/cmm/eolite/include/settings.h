
#ifdef LANG_RUS
	?define TITLE_SETT "����ன��"
	?define SHOW_DEVICE_CLASS "�뢮���� �������� ����� ���ன��"
	?define SHOW_STATUS_BAR "�����뢠�� ����� ���"
	?define NOTIFY_COPY_END "���������� � �����襭�� ����஢����"
	?define SHOW_BREADCRUMBS "�ᯮ�짮���� '嫥��� ��誨'"
	?define BIG_ICONS "�ᯮ�짮���� ����訥 ������"
	?define USE_TWO_PANELS "��� ������"
	?define COLORED_LINES "���ᢥ稢��� ��� ����� � ᯨ᪥"
	?define FONT_SIZE_LABEL "������ ����"
	?define LIST_LINE_HEIGHT "���� ��ப� � ᯨ᪥"
	?define SAVE_PATH_AS_DEFAULT "����騩 ����"
	?define SAVE_START_PATH_AS_DEFAULT "�������� ����"
	?define EDIT_FILE_ASSOCIATIONS "������஢��� ���樠樨 䠩���"
	?define START_PATH " ���⮢� ����: "
#else
	?define TITLE_SETT "Settings"
	?define SHOW_DEVICE_CLASS "Show device class name"
	?define SHOW_STATUS_BAR "Show status bar"
	?define NOTIFY_COPY_END "Notify when copying finished"
	?define SHOW_BREADCRUMBS "Show breadcrumbs"
	?define BIG_ICONS "Big icons in list"
	?define USE_TWO_PANELS "Two panels"
	?define COLORED_LINES "Highlight even lines in list"
	?define FONT_SIZE_LABEL "Font size"
	?define LIST_LINE_HEIGHT "List line height"
	?define SAVE_PATH_AS_DEFAULT "Current path"
	?define SAVE_START_PATH_AS_DEFAULT "Typed path"
	?define EDIT_FILE_ASSOCIATIONS "Edit file associations"
	?define START_PATH " Start path: "
#endif

char path_start[4096];
edit_box path_start_ed = {290,50,57,0xffffff,0x94AECE,0xffffff,0xffffff,0x10000000,4098,
	                      #path_start,0, 100000000000010b,0,0};

more_less_box font_size   = { NULL, 9, 22, FONT_SIZE_LABEL };
more_less_box line_height = { NULL, 16, 64, LIST_LINE_HEIGHT };
checkbox show_dev_name    = { SHOW_DEVICE_CLASS };
checkbox show_status_bar  = { SHOW_STATUS_BAR };
checkbox info_after_copy  = { NOTIFY_COPY_END };
checkbox show_breadcrumb  = { SHOW_BREADCRUMBS };
checkbox big_icons        = { BIG_ICONS };
checkbox two_panels       = { USE_TWO_PANELS };
checkbox colored_lines    = { COLORED_LINES };


void settings_dialog()
{   
	proc_info Settings;
	int id;
	active_settings=1;
	font_size.value = kfont.size.pt;
	line_height.value = files.item_h;
	SetEventMask(0x27);
	loop(){
		switch(WaitEvent())
		{
			case evMouse:
				edit_box_mouse stdcall (#path_start_ed);
				break;
				
			case evButton: 
				id=GetButtonID();
				if (1==id) { ExitSettings(); break; }
				else if (id==5)
				{
					RunProgram("/sys/tinypad", "/sys/settings/assoc.ini");
					break;
				}
				else if (id==6)
				{
					strcpy(#path_start,#path);
					path_start_ed.size = path_start_ed.pos = strlen(#path_start);
					ini.SetString("DefaultPath", #path, strlen(#path));
					edit_box_draw stdcall (#path_start_ed);
					break;
				}
				else if (id==7)
				{
					SetDefaultPath(#path_start);
					break;
				}
				show_dev_name.click(id);
				info_after_copy.click(id);
				two_panels.click(id);
				show_breadcrumb.click(id);
				show_status_bar.click(id);
				colored_lines.click(id);
				if (font_size.click(id)) { 
					kfont.size.pt = font_size.value; 
					kfont.changeSIZE(); 
					BigFontsChange(); 
				}
				if (line_height.click(id)) files.item_h = files_inactive.item_h = line_height.value; 
				if (big_icons.click(id)) BigIconsSwitch();
				EventRedrawWindow(Form.left,Form.top);
				//RefreshWindow(Form.slot, Settings.slot);
				break;
					
			case evKey:
				GetKeys();
				if (key_scancode==SCAN_CODE_ESC) ExitSettings();
				EAX= key_ascii << 8;
				edit_box_key stdcall (#path_start_ed);	
				break;
				
			case evReDraw:
				DefineAndDrawWindow(Form.cwidth-300/2+Form.left, Form.cheight-292/2+Form.top, 400, 
					435+skin_height,0x34,system.color.work,TITLE_SETT,0);
				GetProcessInfo(#Settings, SelfInfo);
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
	show_dev_name.draw(x, y.inc(14));
	show_status_bar.draw(x, y.inc(25));
	info_after_copy.draw(x, y.inc(25));
	show_breadcrumb.draw(x, y.inc(25));
	big_icons.draw(x, y.inc(25));
	two_panels.draw(x, y.inc(25));
	colored_lines.draw(x, y.inc(25));
	font_size.draw(x, y.inc(31));
	line_height.draw(x, y.inc(31));
	
	DrawFrame(x, y.inc(37), 340, 95, START_PATH);
	// START_PATH {
	DrawEditBoxPos(frx, y.inc(21), #path_start_ed);
	but_x = DrawStandartCaptButton(frx, y.inc(34), 6, SAVE_PATH_AS_DEFAULT);
	DrawStandartCaptButton(frx+but_x, y.inc(0), 7, SAVE_START_PATH_AS_DEFAULT);
	// } START_PATH

	DrawStandartCaptButton(x, y.inc(52), 5, EDIT_FILE_ASSOCIATIONS);
}


void LoadIniSettings()
{
	ini.path = GetIni(#eolite_ini_path, "app.ini");
	ini.section = "Eolite";

	files.SetFont(6, 9, 10000000b);
	show_dev_name.checked   = ini.GetInt("ShowDeviceName", true); 
	show_status_bar.checked = ini.GetInt("ShowStatusBar", true); 
	info_after_copy.checked = ini.GetInt("InfoAfterCopy", false); 
	big_icons.checked       = ini.GetInt("BigIcons", false); BigIconsSwitch();
	two_panels.checked      = ini.GetInt("TwoPanels", false); 
	colored_lines.checked   = ini.GetInt("ColoredLines", false); 
	kfont.size.pt   = ini.GetInt("FontSize", 13); 
	files.item_h    = ini.GetInt("LineHeight", 19);
	Form.left   = ini.GetInt("WinX", 200); 
	Form.top    = ini.GetInt("WinY", 50); 
	Form.width  = ini.GetInt("WinW", 550); 
	Form.height = ini.GetInt("WinH", 506); 
	ini.GetString("DefaultPath", #path, 4096, "/rd/1");
	ini.GetString("DefaultPath", #path_start, 4096, "/rd/1");
	path_start_ed.size = path_start_ed.pos = strlen(#path_start);

	ini_get_str stdcall ("/sys/SETTINGS/SYSTEM.INI", "system", "font file",#temp,4096,DEFAULT_FONT);
	kfont.init(#temp);
	ini_get_str stdcall ("/sys/SETTINGS/SYSTEM.INI", "system", "font smoothing",#temp,4096,"on");
	if(!strcmp(#temp,"off")) kfont.smooth = false; else kfont.smooth = true;
}


void SaveIniSettings()
{
	ini.SetInt("ShowDeviceName", show_dev_name.checked);
	ini.SetInt("ShowStatusBar", show_status_bar.checked);
	ini.SetInt("InfoAfterCopy", info_after_copy.checked);
	ini.SetInt("BigIcons", big_icons.checked);
	ini.SetInt("TwoPanels", two_panels.checked);
	ini.SetInt("ColoredLines", colored_lines.checked);
	ini.SetInt("FontSize", kfont.size.pt);
	ini.SetInt("LineHeight", files.item_h);
	ini.SetInt("WinX", Form.left);
	ini.SetInt("WinY", Form.top);
	ini.SetInt("WinW", Form.width);
	ini.SetInt("WinH", Form.height);
}



void Write_Error(int error_number)
{
	char error_message[500];
	sprintf(#error_message,"\"%s\n%s\" -%s","Eolite",get_error(error_number),"tE");
	notify(#error_message);	
}


void LoadIcons()
{
	//ICONS16.PNG
	Libimg_LoadImage(#icons16_default, "/sys/icons16.png");
	Libimg_LoadImage(#icons16_selected, "/sys/icons16.png");
	Libimg_ReplaceColor(icons16_selected.image, icons16_selected.w, icons16_selected.h, 0xffFFFfff, col.selec);
	Libimg_ReplaceColor(icons16_selected.image, icons16_selected.w, icons16_selected.h, 0xffCACBD6, MixColors(col.selec, 0, 200));
	if (col.list_bg!=0xFFFfff) {
		Libimg_ReplaceColor(icons16_default.image, icons16_selected.w, icons16_selected.h, 0xffFFFfff, col.list_bg);
		Libimg_ReplaceColor(icons16_default.image, icons16_selected.w, icons16_selected.h, 0xffCACBD6, MixColors(col.list_bg, 0, 200));		
	}
	//ICONS32.PNG
	Libimg_LoadImage(#icons32_default, "/sys/icons32.png");
	Libimg_LoadImage(#icons32_selected, "/sys/icons32.png");
	Libimg_ReplaceColor(icons32_default.image, icons32_selected.w, icons32_selected.h, 0x00000000, col.list_bg);
	Libimg_ReplaceColor(icons32_selected.image, icons32_selected.w, icons32_selected.h, 0x00000000, col.selec);	
}

void SetAppColors()
{
	int i;
	dword bg_col, old_list_bg_color;
	system.color.get();
	if (col.work == system.color.work) return;
	old_list_bg_color = col.list_bg;
	bg_col = system.color.work;
	if (skin_is_dark()) 
	{
		//use dark colors
		col.def = false;
		col.list_bg = system.color.work;
		col.list_gb_text = system.color.work_text;
		col.list_text_hidden = 0xA6A6B7;
		col.list_vert_line = system.color.work_graph;
		col.work = system.color.work;
		col.graph = system.color.work_graph;
		col.lpanel  = MixColors(system.color.work_graph, system.color.work, 65);
		col.selec = col.selec_active = system.color.work_button;
		col.selec_text = system.color.work_button_text;
		system.color.work_dark = MixColors(0, system.color.work, 35);
		if (col.list_bg==col.selec) col.selec = system.color.work_graph; //for fucking skins
		col.slider_bg_big = MixColors(0xCED0D0, system.color.work, 35);
		col.odd_line = MixColors(0xFFFfff, system.color.work, 15);
	}
	else 
	{
		//use light colors
		col.def = true;
		col.list_bg = 0xFFFfff;
		col.list_gb_text = 0x000000;
		col.list_text_hidden = 0xA6A6B7;
		col.list_vert_line = 0xDDD7CF;
		col.work = system.color.work;
		col.graph = system.color.work_graph;
		col.lpanel  = 0x00699C;
		col.selec = col.selec_active = 0x94AECE;
		col.selec_text = 0x000000;
		system.color.work_dark = MixColors(0, system.color.work, 35);
		col.slider_bg_big = 0xCDCFCF;
		col.odd_line = 0xF1F1F1;
	}
	col.selec_inactive = MixColors(0xBBBbbb, col.list_bg, 65);
	col.slider_bg_left = MixColors(col.graph, col.slider_bg_big, 10);
	for (i=0; i<=20; i++) col.work_gradient[20-i] = MixColors(0, system.color.work, i);
	if (old_list_bg_color!=col.list_bg) LoadIcons();
}


void BigFontsChange()
{
	files.item_h = kfont.size.pt + 4;
	if (files.item_h<icon_size+3) {
		files.item_h = icon_size+3;
		line_height.value = files.item_h;
		line_height.redraw();
	}
	files_active.item_h = files_inactive.item_h = files.item_h;
}

void BigIconsSwitch()
{
	if (big_icons.checked) icon_size=32; else icon_size=16; 
	BigFontsChange();
}

void SetDefaultPath(dword p)
{
	ini.SetString("DefaultPath", p, strlen(p));
	notify("'Default path has been set' -O");
}