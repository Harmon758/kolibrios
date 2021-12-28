#ifdef LANG_RUS
	char t_skins[] =       "   �⨫� ����";
	char t_wallpapers[] =  "   ����";
	char t_screensaver[] =  "   ��ਭᥩ���";
	?define WINDOW_HEADER "����ன�� ��ଫ����"
	?define T_SELECT_FOLDER "����� �����"
	?define T_PICTURE_MODE " ��������� ���⨭�� "
	?define T_CHECKBOX_STRETCH "��������"
	?define T_CHECKBOX_TILED "��������"
	?define T_CHECKBOX_AUTO "��⮬���᪨"
	?define T_UPDATE_DOCK "��������� Dock-������"
	?define T_NO_FILES "'�����ন����� 䠩�� �� �������' -E"
	?define T_UI_PREVIEW " �ਬ�� ��������⮢ "
	?define T_SS_TIMEOUT "���ࢠ� � ������: %i "
	?define T_SS_PREVIEW "��ᬮ��"
	?define T_SS_SET "��⠭�����"
	?define T_NO_SS "[�몫�祭]"
	?define T_DEFAULT "[�� 㬮�砭��]"
#else
	char t_skins[] =       "   Skins";
	char t_wallpapers[] =  "   Wallpapers";
	char t_screensaver[] =  "   Screensaver";
	?define WINDOW_HEADER "Appearance"
	?define T_SELECT_FOLDER "Select folder"
	?define T_PICTURE_MODE " Picture Mode "
	?define T_CHECKBOX_STRETCH "Stretch"
	?define T_CHECKBOX_TILED "Tiled"
	?define T_CHECKBOX_AUTO "Auto"
	?define T_UPDATE_DOCK "Update Dock"
	?define T_NO_FILES "'No supported files were found' -E"
	?define T_UI_PREVIEW " Components Preview "
	?define T_SS_TIMEOUT "Wait in minutes: %i "
	?define T_SS_PREVIEW "View"
	?define T_SS_SET "Set"
	?define T_NO_SS "[Disable]"
	?define T_DEFAULT "[Default]"
#endif

#define WIN_W 560
#define WIN_H 450
#define LIST_W 260
#define PANEL_H 50
#define LP 6 //LIST_PADDING

#define RIGHTx LP + LIST_W + TAB_P + 30
#define RIGHTy PANEL_H
#define RIGHTw WIN_W - RIGHTx - LP - TAB_P
#define RIGHTh 215

enum {
	TAB_SKINS, 
	TAB_WALLPAPERS, 
	TAB_SCREENSAVERS
};

enum { 
	BASE_TAB_BUTTON_ID=3, 
	BTN_SELECT_WALLP_FOLDER=10,
	BTN_TEST_SCREENSAVER,
	BTN_SET_SCREENSAVER
};

_ini ini = { "/sys/settings/system.ini" };

char default_dir[] = "/rd/1";
od_filter filter2 = { 8, "TXT\0\0" };

_tabs tabs = { -sizeof(t_skins)-sizeof(t_wallpapers)-sizeof(t_screensaver)
	-3*8+WIN_W - TAB_P / 2, LP, NULL, BASE_TAB_BUTTON_ID };

scroll_bar ss_timeout = { RIGHTw-19,RIGHTx,15,RIGHTy+25,0,3,89,10,0,0xFFFfff,
	0xBBBbbb,0xeeeeee};