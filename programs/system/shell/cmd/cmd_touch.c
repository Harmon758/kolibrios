
int cmd_touch(char file[])
{

kol_struct70	k70;
char		temp[256];
unsigned	result;

if (NULL == file)
	{
	#if LANG_ENG
		printf("  touch <filename>\n\r");
	#elif LANG_RUS
		printf("  touch <��� 䠩��>\n\r");
	#endif
	return TRUE;
	}

if (  ( 0 == strcmp(file, ".") ) || ( 0 == strcmp(file, "..") ) || ( 0 == strcmp(cur_dir, "/")) ) 
	{
	return FALSE;
	}

if ( '/' == file[0])
	{
	strcpy(temp, file);

	if ( !file_check(temp) )
		k70.p00 = 2;
	else
		k70.p00 = 3;
	}
else 
	{
	strcpy(temp, cur_dir);
	strcat(temp, file);
	if ( !file_check(temp) )
		k70.p00 = 2;
	else
		k70.p00 = 3;
	}

k70.p04 = 0;
k70.p08 = 0;
k70.p12 = 0;
k70.p16 = 0;
k70.p20 = 0;
k70.p21 = temp;

result = kol_file_70(&k70);

if (0 == result)
	return TRUE;
else 
	return FALSE;

}
