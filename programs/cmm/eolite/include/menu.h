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
char burger_menu_items[] = 
"����� ����|Ctrl+N
������ ���᮫�|Ctrl+G
�������� �����|F5
� �ணࠬ��|F1";

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
char burger_menu_items[] = 
"New window|Ctrl+N
Open console here|Ctrl+G
V�rskenda|F5
About|F1";

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
char burger_menu_items[] = 
"New window|Ctrl+N
Open console here|Ctrl+G
Refresh folder|F5
About|F1";
#endif


enum { MENU_DIR=1, MENU_FILE, MENU_NO_FILE, MENU_BURGER };

bool active_menu = false;

void EventMenuClick(dword _id)
{
	if (active_menu == MENU_NO_FILE) switch(_id) {
		case 1: EventPaste(); break;
	}
	if (active_menu == MENU_FILE) switch(_id) {
		case 1: Open(0); break;
		case 2: ShowOpenWithDialog(); break;
		case 3: EventCopy(NOCUT); break;
		case 4: EventCopy(CUT); break;
		case 5: EventPaste(); break;
		case 6: FnProcess(2); break;
		case 7: Del_Form(); break;
		case 8: FnProcess(8); break;
	}
	if (active_menu == MENU_DIR) switch(_id) {
		case 1: Open(0); break;
		case 2: EventCopy(NOCUT); break;
		case 3: EventCopy(CUT); break;
		case 4: EventPaste(); break;
		case 5: Del_Form(); break;
		case 6: FnProcess(8); break;
	}
	if (active_menu == MENU_BURGER) switch(_id) {
		case 1: EventOpenNewEolite(); break;
		case 2: EventOpenConsoleHere(); break;
		case 3: EventRefreshDisksAndFolders(); break;
		case 4: FnProcess(1); break;		
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
	open_lmenu(mouse.x+Form.left+5, mouse.y+Form.top+3
		+skin_height, MENU_ALIGN_TOP_LEFT, NULL, text);
}

void EventShowBurgerMenu()
{
	active_menu = MENU_BURGER;
	open_lmenu(Form.width-10+Form.left, 35+Form.top+skin_height,
		MENU_ALIGN_TOP_RIGHT, NULL, #burger_menu_items);
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

