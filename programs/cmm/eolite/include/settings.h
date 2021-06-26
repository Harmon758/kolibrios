
#ifdef LANG_RUS
	?define TITLE_SETT "����ன��"
	?define SHOW_DEVICE_CLASS "�뢮���� �������� ����� ���ன��"
	?define SHOW_STATUS_BAR "�����뢠�� ����� ���"
	?define SHOW_BREADCRUMBS "�ᯮ�짮���� '嫥��� ��誨'"
	?define BIG_ICONS "�ᯮ�짮���� ����訥 ������"
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
	?define SHOW_BREADCRUMBS "Show breadcrumbs"
	?define BIG_ICONS "Big icons in list"
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
checkbox show_breadcrumb  = { SHOW_BREADCRUMBS };
checkbox big_icons        = { BIG_ICONS };
checkbox colored_lines    = { COLORED_LINES };


void settings_dialog()
{   
	proc_info Settings;
	int id;
	active_settings = true;
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
					RunProgram("/sys/@open", "/sys/settings/assoc.ini");
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
					365+skin_height,0x34,sc.work,TITLE_SETT,0);
				GetProcessInfo(#Settings, SelfInfo);
				DrawSettingsCheckBoxes();
		}
	}
}

void ExitSettings()
{
	active_settings = false;
	settings_window = 0;
	cmd_free = 4;
	ExitProcess();
}

void DrawSettingsCheckBoxes()
{
	incn y;
	int but_x;
	#define FRX 26
	#define XXX 11
	y.n = 0;
	show_dev_name.draw(XXX, y.inc(14));
	show_status_bar.draw(XXX, y.inc(25));
	show_breadcrumb.draw(XXX, y.inc(25));
	big_icons.draw(XXX, y.inc(25));
	colored_lines.draw(XXX, y.inc(25));
	font_size.draw(XXX, y.inc(31));
	line_height.draw(XXX, y.inc(31));
	
	DrawFrame(XXX, y.inc(37), 340, 95, START_PATH);
	// START_PATH {
	DrawEditBoxPos(FRX, y.inc(21), #path_start_ed);
	but_x = DrawStandartCaptButton(FRX, y.inc(34), 6, SAVE_PATH_AS_DEFAULT);
	DrawStandartCaptButton(FRX+but_x, y.inc(0), 7, SAVE_START_PATH_AS_DEFAULT);
	// } START_PATH

	DrawStandartCaptButton(XXX, y.inc(52), 5, EDIT_FILE_ASSOCIATIONS);
}


void LoadIniSettings()
{
	ini.path = GetIni(#eolite_ini_path, "app.ini");
	if (efm) ini.section = "EFM"; else ini.section = "Eolite";

	files.SetFont(6, 9, 10000000b);
	show_dev_name.checked   = ini.GetInt("ShowDeviceName", true); 
	show_status_bar.checked = ini.GetInt("ShowStatusBar", true); 
	big_icons.checked       = ini.GetInt("BigIcons", false); BigIconsSwitch();
	colored_lines.checked   = ini.GetInt("ColoredLines", true); 
	kfont.size.pt   = ini.GetInt("FontSize", 13); 
	files.item_h    = ini.GetInt("LineHeight", 19);
	Form.left   = ini.GetInt("WinX", 100); 
	Form.top    = ini.GetInt("WinY", 30); 
	Form.width  = ini.GetInt("WinW", efm*170+550); 
	Form.height = ini.GetInt("WinH", efm*100+517); 
	ini.GetString("DefaultPath", #path, 4096, "/rd/1");
	ini.GetString("DefaultPath", #path_start, 4096, "/rd/1");
	path_start_ed.size = path_start_ed.pos = strlen(#path_start);
	kfont.init(DEFAULT_FONT);
	ini_get_str stdcall ("/sys/SETTINGS/SYSTEM.INI", "system", "font smoothing",#temp,4096,"on");
	if(streq(#temp,"off")) kfont.smooth = false; else kfont.smooth = true;
}


void SaveIniSettings()
{
	ini.SetInt("ShowDeviceName", show_dev_name.checked);
	ini.SetInt("ShowStatusBar", show_status_bar.checked);
	ini.SetInt("BigIcons", big_icons.checked);
	ini.SetInt("ColoredLines", colored_lines.checked);
	ini.SetInt("FontSize", kfont.size.pt);
	ini.SetInt("LineHeight", files.item_h);
	if (Form.status_window<=2) {
		ini.SetInt("WinX", Form.left);
		ini.SetInt("WinY", Form.top);
		ini.SetInt("WinW", Form.width);
		ini.SetInt("WinH", Form.height);		
	}
}



void Write_Error(int error_number)
{
	char error_message[500];
	sprintf(#error_message,"\"%s\n%s\" -%s","Eolite",get_error(error_number),"tE");
	notify(#error_message);	
}


void LoadIcons()
{
	dword selected_shadow = MixColors(col.selec, 0, 200);
	dword non_white_shadow = MixColors(col.list_bg, 0, 200);
	if (big_icons.checked) 
	{
		icons32_default.load("/sys/icons32.png");
		icons32_selected.load("/sys/icons32.png");
		//FUUUUUU... the next ugly code may replace the previous line
		//we are copying raw data without loading and unpycking PNG
		//memmov(icons32_selected.imgsrc, icons32_default.imgsrc, icons32_default.h*32*4);
		//EDI = icons32_selected.image;
		//ESDWORD[EDI+04] = 32;
		//ESDWORD[EDI+08] = icons32_default.h;
		//ESDWORD[EDI+20] = IMAGE_BPP32;
		//ESDWORD[EDI+24] = icons32_selected.imgsrc;
		icons32_default.replace_color(0x00000000, col.list_bg);
		icons32_selected.replace_color(0x00000000, col.selec);
	} else {
		icons16_default.load("/sys/icons16.png");
		icons16_selected.load("/sys/icons16.png");
		icons16_selected.replace_2colors(0xffFFFfff, col.selec, 0xffCACBD6, selected_shadow);
		if (col.list_bg!=0xFFFfff) {
			icons16_default.replace_2colors(0xffFFFfff, col.list_bg, 0xffCACBD6, non_white_shadow);
		}
	}

}

void SetAppColors()
{
	int i;
	static dword bg_col;
	dword old_list_bg_color = col.list_bg;
	sc.get();
	sc.work_dark = MixColors(0, sc.work, 35);
	if (sc.work == 0) sc.work = 1;
	if (bg_col == sc.work) return;
	bg_col = sc.work;
	if (skin_is_dark()) 
	{
		//use dark colors
		col.def = false;
		col.list_bg = sc.work;
		col.list_gb_text = sc.work_text;
		col.list_text_hidden = 0xA6A6B7;
		col.list_vert_line = sc.work_graph;
		col.lpanel  = MixColors(sc.work_graph, sc.work, 65);
		col.selec = col.selec_active = sc.button;
		col.selec_text = sc.button_text;
		if (col.list_bg==col.selec) col.selec = sc.work_graph; //for fucking skins
		col.slider_bg_big = MixColors(0xCED0D0, sc.work, 35);
		col.odd_line = MixColors(0xFFFfff, sc.work, 15);
		for (i=0; blue_hl_pal[i]!=0; i++) waves_pal[i] = MixColors(col.lpanel, blue_hl_pal[i], 180);
	}
	else 
	{
		//use light colors
		col.def = true;
		col.list_bg = 0xFFFfff;
		col.list_gb_text = 0x000000;
		col.list_text_hidden = 0xA6A6B7;
		col.list_vert_line = 0xDDD7CF;
		col.lpanel  = 0x00699C;
		col.selec = col.selec_active = 0x92B1D9;
		col.selec_text = 0x000000;
		col.slider_bg_big = 0xCDCFCF;
		col.odd_line = 0xF4F5F5;
		for (i=0; blue_hl_pal[i]!=0; i++) waves_pal[i] = blue_hl_pal[i];
	}
	col.selec_inactive = MixColors(0xBBBbbb, col.list_bg, 65);
	col.slider_bg_left = MixColors(sc.work_graph, col.slider_bg_big, 10);
	for (i=0; i<20; i++) col.work_gradient[i] = MixColors(sc.work_light, sc.work, i*5);
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
	if (big_icons.checked) {
		icon_size = 32; 
		LoadIcons();
	} else {
		icon_size = icons16_default.w; 
	}
	BigFontsChange();
}

void SetDefaultPath(dword p)
{
	ini.SetString("DefaultPath", p, strlen(p));
	notify("'Default path has been set' -O");
}