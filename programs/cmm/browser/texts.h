char version[]="WebView 2.5b";

#ifdef LANG_RUS
char page_not_found[] = FROM "html\\page_not_found_ru.htm""\0";
char homepage[] = FROM "html\\homepage_ru.htm""\0";
char help[] = FROM "html\\help_ru.htm""\0";
char accept_language[]= "Accept-Language: ru\n";
char rmb_menu[] = 
"��ᬮ���� ��室���|Ctrl+U
������஢��� ��室���";
char main_menu[] = 
"������ 䠩�|Ctrl+O
����� ����|Ctrl+N
-
�����|Ctrl+H
�������� ����㧮�|Ctrl+J
������ ���
�������� ��㧥�";
char link_menu[] =
"������ � ����� �������
������ � ����� ����
-
����஢��� ��뫪�
������ ᮤ�ন��� ��뫪�";
char loading_text[] = "����㧪�...";

char update_param[] = "-download_and_exit http://builds.kolibrios.org/rus/data/programs/cmm/browser/WebView.com";
char update_download_error[] = "'WebView\n�訡�� �� ����祭�� ����������!' -tE";
char update_ok[] = "'WebView\n��㧥� �� �ᯥ譮 ��������!' -tO";
char update_is_current[] = "'WebView\n�� 㦥 �ᯮ���� ��᫥���� �����.' -tI";
char update_can_not_copy[] = "'WebView\n�� ���� ��६����� ����� ����� �� ����� Downloads �� Ramdisk. ��������, �� �����筮 ����.' -tE";
char clear_cache_ok[] = "'WebView\n��� ��饭.' -tI";
#else
char page_not_found[] = FROM "html\\page_not_found_en.htm""\0";
char homepage[] = FROM "html\\homepage_en.htm""\0";
char help[] = FROM "html\\help_en.htm""\0";
char accept_language[]= "Accept-Language: en\n";
char rmb_menu[] =
"View source|Ctrl+U
Edit source";
char main_menu[] = 
"Open local file|Ctrl+O
New window|Ctrl+N
-
History|Ctrl+H
Download Manager|Ctrl+J
Clear cache
Update browser";
char link_menu[] =
"Open in new tab
Open in new window
-
Copy link
Download link contents";
char loading_text[] = "Loading...";
char update_param[] = "-download_and_exit http://builds.kolibrios.org/eng/data/programs/cmm/browser/WebView.com";
char update_download_error[] = "'WebView\nError receiving an up to date information!' -tE";
char update_ok[] = "'WebView\nThe browser has been updated!' -tO";
char update_is_current[] = "'WebView\nThe browser is up to date.' -tI";
char update_can_not_copy[] = "'WebView\nError copying a new version from Downloads folder!\nProbably too litle space on Ramdisk.' -tE";
char clear_cache_ok[] = "'WebView\nThe cache has been cleared.' -tI";
#endif

#define URL_SERVICE_HISTORY "WebView:history"
#define URL_SERVICE_HOMEPAGE "WebView:home"
#define URL_SERVICE_HELP "WebView:help"

char webview_shared[] = "WEBVIEW";
