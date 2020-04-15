#ifdef LANG_RUS

char file_actions[]= 
"������           |Enter
������ � �������  |CrlEnt
-
����஢���         |Crl+C
��१���           |Crl+X
��⠢���           |Crl+V
-
��२��������      |F2
�������            |Del
�����⢠           |F8";
char folder_actions[]=
"������       |Enter
-
����஢���     |Crl+C
��१���       |Crl+X
��⠢���       |Crl+V
-
�������        |Del
�����⢠       |F8";
char empty_folder_actions[]=
"��⠢���      |Crl+V";

#elif LANG_EST
char file_actions[]= 
"Ava           |Enter
Ava ...        |CrlEnt
-
Kopeeri        |Crl+C
L�ika          |Crl+X
Aseta          |Crl+V
-
Nimeta �mber   |F2
Kustuta        |Del
Properties     |F8";
char folder_actions[]=
"Ava           |Enter
-
Kopeeri        |Crl+C
L�ika          |Crl+X
Aseta          |Crl+V
-
Kustuta        |Del
Properties     |F8";
char empty_folder_actions[]=
"Aseta         |Crl+V";

#else
char file_actions[]= 
"Open         |Enter 
Open with...  |CrlEnt
-
Copy          |Crl+C
Cut           |Crl+X
Paste         |Crl+V
-
Rename        |F2
Delete        |Del
Properties    |F8";
char folder_actions[]=
"Open        |Enter 
-
Copy         |Crl+C
Cut          |Crl+X
Paste        |Crl+V
-
Delete       |Del
Properties   |F8";
char empty_folder_actions[]=
"Paste        |Crl+V";

#endif

//�������� �����      |F5
//V�rskenda      |F5
//Refresh      |F5

enum { MENU_DIR=1, MENU_FILE, MENU_NO_FILE, MENU_BURGER };

bool active_menu = false;

void EventMenuClick(dword _id)
{
	if (active_menu == MENU_NO_FILE) switch(_id) {
		case 1: Paste(); break;
	}
	if (active_menu == MENU_FILE) switch(_id) {
		case 1: Open(0); break;
		case 2: ShowOpenWithDialog(); break;
		case 3: Copy(#file_path, NOCUT); break;
		case 4: Copy(#file_path, CUT); break;
		case 5: Paste(); break;
		case 6: FnProcess(2); break;
		case 7: Del_Form(); break;
		case 8: FnProcess(8); break;
	}
	if (active_menu == MENU_DIR) switch(_id) {
		case 1: Open(0); break;
		case 2: Copy(#file_path, NOCUT); break;
		case 3: Copy(#file_path, CUT); break;
		case 4: Paste(); break;
		case 5: Del_Form(); break;
		case 6: FnProcess(8); break;
	}
	active_menu = NULL;
}

void EventShowListMenu()
{
	dword text;

	pause(3);

	if (!files.count) {
		text = #empty_folder_actions;
		active_menu = MENU_NO_FILE;
	} else if (itdir) {
		text = #folder_actions;
		active_menu = MENU_DIR;
	} else {
		text = #file_actions;
		active_menu = MENU_FILE;
	}
	open_lmenu(mouse.x+Form.left+5, mouse.y+Form.top+3+skin_height, MENU_ALIGN_TOP_LEFT, NULL, text);
}

bool GetMenuClick()
{
	dword click_id;
	if (active_menu) && (click_id = get_menu_click()) {
		EventMenuClick(click_id);
		return false;
	}
	return true;
}

