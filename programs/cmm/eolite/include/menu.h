//Leency 2008-2013

//pay attension: >200 this is only file actions, not supported by folders
#ifdef LANG_RUS
char *file_captions[] = {
	"������",               "Enter",100,
	"������ � �������...",  "CrlEnt",201,
	"������ ��� ⥪��",     "F3",202,
	"������ ��� HEX",       "F4",203,
	"����஢���",            "Crl+C",104,
	"��१���",              "Crl+X",105,
	"��⠢���",              "Crl+V",106,
	"��२��������",         "F2",207,
	"�������",               "Del",108,
	"�������� �����",        "F5",109,
	"�����⢠",              "",110,
	0, 0, 0};
#elif LANG_EST
char *file_captions[] = {
	"Ava",            "Enter",100,
	"Ava ...",        "CrlEnt",201,
	"Vaata tekstina", "F3",202,
	"Vaata HEX",      "F4",203,
	"Kopeeri",        "Crl+C",104,
	"L�ika",          "Crl+X",105,
	"Aseta",          "Crl+V",106,
	"Nimeta �mber",   "F2",207,
	"Kustuta",        "Del",108,
	"V�rskenda",      "F5",109,
	"Properties",       "",110,
	0, 0, 0};
#else
char *file_captions[] = {
	"Open",          "Enter",100,
	"Open with...",  "CrlEnt",201,
	"View as text",  "F3",202,
	"View as HEX",   "F4",203,
	"Copy",          "Crl+C",104,
	"Cut",           "Crl+X",105,
	"Paste",         "Crl+V",106,
	"Rename",        "F2",207,
	"Delete",        "Del",108,
	"Refresh",       "F5",109,
	"Properties",       "",110,
	0, 0, 0};
#endif

llist menu;
int cur_action_buf;

void FileMenu()
{
	proc_info MenuForm;
	int index;

	menu.ClearList();
	menu.SetSizes(0,0,10,0,18);
	for (index=0; file_captions[index]!=0; index+=3)
	{
		if (itdir) && (file_captions[index+2]>=200) continue;
		if (strlen(file_captions[index])>menu.w) menu.w = strlen(file_captions[index]);
		menu.count++;
		menu.visible++;
	}
	menu.w = menu.w + 3 * 6 + 50;
	menu.h = menu.count*menu.line_h;
	SetEventMask(100111b);
	goto _MENU_DRAW;
	
	loop() switch(WaitEvent())
	{
		case evMouse:
				mouse.get();
				if (!CheckActiveProcess(MenuForm.ID)){ cmd_free=1; ExitProcess();}
				else if (mouse.move)&&(menu.ProcessMouse(mouse.x, mouse.y)) MenuListRedraw();
				else if (mouse.key&MOUSE_LEFT)&&(mouse.up) {action_buf = cur_action_buf; cmd_free=1; ExitProcess(); }
		break;
				
		case evKey:
				GetKeys();
				if (key_scancode==SCAN_CODE_ESC){cmd_free=1;ExitProcess();}
				else if (key_scancode == SCAN_CODE_ENTER) {action_buf = cur_action_buf; cmd_free=1; ExitProcess(); }
				if (menu.ProcessKey(key_scancode)) MenuListRedraw();
				break;
				
		case evReDraw: _MENU_DRAW:
				if (menu_call_mouse) DefineAndDrawWindow(mouse.x+Form.left+5, mouse.y+Form.top+GetSkinHeight(),menu.w+3,menu.h+6,0x01, 0, 0, 0x01fffFFF);
				else DefineAndDrawWindow(Form.left+files.x+15, files.line_h*files.current+files.y+Form.top+30,menu.w+3,menu.h+6,0x01, 0, 0, 0x01fffFFF);
				GetProcessInfo(#MenuForm, SelfInfo);
				DrawRectangle(0,0,menu.w+1,menu.h+2,system.color.work_graph);
				DrawBar(1,1,menu.w,1,0xFFFfff);
				DrawPopupShadow(1,1,menu.w,menu.h,0);
				MenuListRedraw();
	}
}

void MenuListRedraw()
{
	int start_y=0;
	int index;
	for (index=0; file_captions[index*3]!=0; index++)
	{
		if ((itdir) && (file_captions[index*3+2]>=200)) continue;
		DrawBar(1,start_y+2,1,menu.line_h,0xFFFfff);
		if (start_y/menu.line_h==menu.current)
		{
			cur_action_buf = file_captions[index*3+2];
			DrawBar(2,start_y+2,menu.w-1,menu.line_h,0xFFFfff);
		}
		else
		{
			DrawBar(2,start_y+2,menu.w-1,menu.line_h,system.color.work);
			WriteText(8,start_y+menu.text_y+3,0x80,0xf2f2f2,file_captions[index*3]);
		}
		WriteText(7,start_y+menu.text_y+2,0x80,system.color.work_text,file_captions[index*3]);
		WriteText(-strlen(file_captions[index*3+1])*6-6+menu.w,start_y+menu.text_y+2,0x80,0x888888,file_captions[index*3+1]);
		start_y+=menu.line_h;
	}	
}