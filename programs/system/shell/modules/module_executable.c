
/// ===========================================================

int executable_run(char cmd[], char args[])
{

	char	exec[FILENAME_MAX];
	int		result;

	if ( '/' == cmd[0]) // if path is absolute
	{
		strcpy(exec, cmd);
		if (!file_check(exec) ) // check file existense
		{
			file_not_found(cmd);
			return FALSE;
		}
	}
	else 
	{
		strcpy(exec, cur_dir); // check file in current directory
		if (exec[strlen(exec)-1] != '/') 
			strcat(exec, "/"); // add slash
		strcat(exec, cmd);
		
		if ( !file_check(exec) ) // check file existense
		{
			strcpy(exec, "/rd/1/"); // check file on virtual disk
			strcat(exec, cmd);
			if ( !file_check(exec) ) // check file existense
			{
				file_not_found(cmd);
				return FALSE;
			}
		}
	}

	// if file exists:

	// try to run as a program
	result = program_run(exec, args);
	if (result > 0)
	{
		if ( !program_console(result)  )
		{
				LAST_PID = result;
			#if LANG_ENG
				printf ("  '%s' started. PID = %d\n\r", cmd, result);
			#elif LANG_RUS
				printf ("  '%s' ����饭. PID = %d\n\r", cmd, result);
			#endif
		}
		return TRUE;
	}
	else	
	{
		if ( script_check(exec) ) // if file is a valid script
		{
			return script_run(exec, args);
		} else
		{
			#if LANG_ENG
				printf ("Error in '%s' : script must start with #SHS line\n\r", cmd);
			#elif LANG_RUS
				printf ("�訡�� � '%s' : �ਯ� ������ ��稭����� � ���窨 #SHS\n\r", cmd);
			#endif
			return FALSE;
		}
	}

}

/// ===========================================================

