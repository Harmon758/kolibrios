//===================================================//
//                                                   //
//                     CYRILLIC                      //
//                                                   //
//===================================================//
#ifdef LANG_RUS
#define T_FILE "���"
#define T_TYPE "���"
#define T_SIZE "������"
#define T_NEW_FOLDER "����� �����"
#define T_NEW_FILE "���� 䠩�"
#define T_DELETE_FILE "�� ����⢨⥫쭮 ��� 㤠����"
#define T_YES "��"
#define T_NO "���"
#define T_CANCEL "�⬥��"
#define T_CREATE "�������"
#define T_RENAME "��७������"
#define T_GOPATH "��३�"
#define FS_ITEM_ALREADY_EXISTS "'������� � ⠪�� ������ 㦥 �������' -E"
#define T_NOTIFY_APP_PARAM_WRONG "'��ࠬ��� ��� ����᪠ Eolite �� ��७: ����� �� �������!' -E"
#define T_COPY_WINDOW_TITLE "�������..."
#define T_MOVE_WINDOW_TITLE "��६���..."
#define T_DELETE_WINDOW_TITLE "������..."
#define T_ABORT_WINDOW_BUTTON "��ࢠ��"
#define T_SELECT_APP_TO_OPEN_WITH "�롥�� �ணࠬ�� ��� ������ 䠩��"
#define DEL_MORE_FILES_1 "��࠭�� ������ ("
#define DEL_MORE_FILES_2 " ��.)?"
#define T_STATUS_EVEMENTS "�����: %d  ������: %d"
#define T_STATUS_SELECTED "�뤥�����: %d"
#define COPY_PATH_STR "'���� ����� ᪮��஢�� � ���� ������' -I"
#define T_ABOUT "� �ணࠬ��"
#define T_USE_SHIFT_ENTER "'�ᯮ���� Shift+Enter �⮡� ������ �� �뤥����� 䠩��.' -I"

char *kfm_func = {
	"���",
	"��२���.",
	"��ᬮ��",
	"������.",
	"����஢���",
	"��६�����",
	"�����",
	"�������",
	"����",
	" ����ன��"
};

#define T_DEVICES "���ன�⢠"
#define T_ACTIONS "����⢨�"
char *actions[] = {
	59, "���� 䠩�", "F9",
	57, "����� �����", "F7",
	60, "����ன��", "F10",
	0,0,0
};
#define T_PROG "�ணࠬ�� "
#define T_SYS  "���⥬� "
#define T_CD   "CD-ROM "
#define T_FD   "��᪥� "
#define T_HD   "���⪨� ��� "
#define T_SATA "SATA ��� "
#define T_USB  "USB ��� "
#define T_RAM  "RAM ��� "
#define T_UNC  "�������⭮ "
//===================================================//
//                                                   //
//                      ENGLISH                      //
//                                                   //
//===================================================//
#else
#define T_FILE "Name"
#define T_TYPE "Type"
#define T_SIZE "Size"
#define T_NEW_FOLDER "New folder"
#define T_NEW_FILE "New file"
#define T_DELETE_FILE "Do you really want to delete"
#define T_YES "Yes"
#define T_NO "No"
#define T_CANCEL "Cancel"
#define T_CREATE "Create"
#define T_RENAME "Rename"
#define T_GOPATH "Go to"
#define FS_ITEM_ALREADY_EXISTS "'An item with that name already exists' -E"
#define WAIT_DELETING_FOLDER "Deleting folder. Please, wait..."
#define T_NOTIFY_APP_PARAM_WRONG "'Eolite param is wrong: directory does not exist!' -E"
#define T_COPY_WINDOW_TITLE "Copying..."
#define T_MOVE_WINDOW_TITLE "Moving..."
#define T_DELETE_WINDOW_TITLE "Deleting..."
#define T_ABORT_WINDOW_BUTTON "Abort"	
#define T_SELECT_APP_TO_OPEN_WITH "Select application to open file"
#define DEL_MORE_FILES_1 "selected items("
#define DEL_MORE_FILES_2 " pcs.)?"
#define T_STATUS_EVEMENTS "Dirs: %d  Files: %d"
#define T_STATUS_SELECTED "Selected: %d"
#define COPY_PATH_STR "'Directory path copied to clipboard' -I"
#define T_ABOUT "About"
#define T_USE_SHIFT_ENTER "'Use Shift+Enter to open all selected files.' -I"

char *kfm_func = {
	"Info",
	"Rename",
	"View",
	"Edit",
	"Copy",
	"Move",
	"Folder",
	"Delete",
	"File",
	" Settings"
};

#define T_DEVICES "Devices"
#define T_ACTIONS "Actions"
char *actions[] = {
	59, "New file", "F9",
	57, "New folder", "F7",
	60, "Settings", "F10",
	0,0,0
};
#define T_PROG "Programs "
#define T_SYS  "System "
#define T_CD   "CD-ROM "
#define T_FD   "Floppy disk "
#define T_HD   "Hard disk "
#define T_SATA "SATA disk "
#define T_USB  "USB disk "
#define T_RAM  "RAM disk "
#define T_UNC  "Unknown "
#endif

//===================================================//
//                                                   //
//                      GLOBAL                       //
//                                                   //
//===================================================//

//Button IDs
enum {
	POPUP_BTN1 = 201,
	POPUP_BTN2 = 202,
	KFM_DEV_DROPDOWN_1 = 205,
	KFM_DEV_DROPDOWN_2 = 207,
	BTN_PATH = 210,
	BTN_BREADCRUMB = 214,
	BREADCRUMB_ID = 300,

	BACK_BTN = 400,
	FWRD_BTN,
	GOUP_BTN,
	COPY_BTN,
	CUT_BTN,
	PASTE_BTN,
	KFM_FUNC_ID = 450
};

//OpenDir options
enum {
	WITH_REDRAW, 
	ONLY_OPEN
};

//OpenDir options
enum {
	POPIN_PATH=1,
	POPIN_NEW_FILE,
	POPIN_NEW_FOLDER,
	POPIN_RENAME,
	POPIN_DELETE,
	POPIN_DISK,
	POPIN_BREADCR
};


char *devinfo = {
	"sy", 0, T_SYS,
	"rd", 0, T_SYS,
	"ko", 1, T_PROG,
	"fd", 2, T_FD,
	"cd", 3, T_CD,
	"hd", 4, T_HD,
	"bd", 4, T_HD,
	"sd", 4, T_SATA,
	"tm", 5, T_RAM,
	"us", 6, T_USB,
	0
};

#define DEV_H 17
#define DDW 110
#define KFM2_DEVH 20
#define SELECTY 10
#define POPIN_W 260

int toolbar_buttons_x[7]={9,46,85,134,167,203};