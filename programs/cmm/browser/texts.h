char version[]="WebView 2.2";

#ifdef LANG_RUS
char page_not_found[] = FROM "html\\page_not_found_ru.htm""\0";
char homepage[] = FROM "html\\homepage_ru.htm""\0";
char help[] = FROM "html\\help_ru.htm""\0";
char accept_language[]= "Accept-Language: ru\n";
char rmb_menu[] = 
"��ᬮ���� ��室���
������஢��� ��室���";
char main_menu[] = 
"������ 䠩�
����� ����
�����
�������� ����㧮�";
char link_menu[] =
"����஢��� ��뫪�
������ ᮤ�ন��� ��뫪�";
char loading_text[] = "����㧪�...";
#else
char page_not_found[] = FROM "html\\page_not_found_en.htm""\0";
char homepage[] = FROM "html\\homepage_en.htm""\0";
char help[] = FROM "html\\help_en.htm""\0";
char accept_language[]= "Accept-Language: en\n";
char rmb_menu[] =
"View source
Edit source";
char main_menu[] = 
"Open local file
New window
History
Download Manager";
char link_menu[] =
"Copy link
Download link contents";
char loading_text[] = "Loading...";
#endif

#define URL_SERVICE_HISTORY "WebView:history"
#define URL_SERVICE_HOMEPAGE "WebView:home"
#define URL_SERVICE_HELP "WebView:help"

