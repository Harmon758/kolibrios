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
	?define T_SCREENSAVER_PREVIEW "��ᬮ��"
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
	?define T_SCREENSAVER_PREVIEW "Preview"
#endif

#define WIN_W 600
#define WIN_H 420
#define LIST_W 280
#define PANEL_H 50
#define LP 6 //LIST_PADDING

#define RIGHTx LP + TAB_PADDING + LIST_W + TAB_PADDING + 30
#define RIGHTy PANEL_H
#define RIGHTw 226
#define RIGHTh 215

enum {
	TAB_SKINS, 
	TAB_WALLPAPERS, 
	TAB_SCREENSAVERS
};

_ini ini = { "/sys/settings/system.ini" };

char default_dir[] = "/rd/1";
od_filter filter2 = { 8, "TXT\0\0" };