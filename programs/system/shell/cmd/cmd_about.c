
int cmd_about(char param[])
{

#if LANG_ENG
char message[] = {
"\
\n\r\
Shell for KolibriOS\n\r\
version %s\n\r\n\r\
  author: Oleksandr Bogomaz aka Albom\n\r\
  e-mail: albom85@yandex.ru\n\r\
    site: http://albom85.narod.ru/\n\r\n\r\
"};
#elif LANG_RUS
char message[] = {
"\
\n\r\
Shell ��� KolibriOS\n\r\
����� %s\n\r\n\r\
   ����: ����ᠭ�� ������� aka Albom\n\r\
  e-mail: albom85@yandex.ru\n\r\
    ᠩ�: http://albom85.narod.ru/\n\r\n\r\
"};
#endif

printf(message, SHELL_VERSION); 
return TRUE;
}
