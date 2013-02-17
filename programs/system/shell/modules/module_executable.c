
/// ===========================================================

int executable_run(char cmd[], char args[])
{

char	exec[256];
int		result;

if ( '/' == cmd[0]) // �᫨ ���� ��᮫���
	{
	strcpy(exec, cmd);

	if (  !file_check(exec) ) // �஢��塞 ����⢮����� 䠩��
		{
		file_not_found(cmd);
		return FALSE;
		}
	}

else 
	{
	strcpy(exec, cur_dir); // �஢��塞 䠩� � ⥪�饬 ��⠫���
	if (exec[strlen(exec)-1] != '/') 
		strcat(exec, "/"); // add slash
	strcat(exec, cmd);
	
	if ( !file_check(exec) ) // �஢��塞 ����⢮����� 䠩��
		{
		strcpy(exec, "/rd/1/"); // �஢��塞 䠩� �� ����㠫쭮� ��᪥
		strcat(exec, cmd);
			if ( !file_check(exec) ) // �஢��塞 ����⢮����� 䠩��
				{
				file_not_found(cmd);
				return FALSE;
				}
		}
	}


if ( script_check(exec) )
	return script_run(exec, args);

/* ����� �ணࠬ�� */
result = program_run(exec, args);
if (result > 0)
	{
	
	if ( !program_console(result)  )
		{
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
	file_not_found(cmd);
	return FALSE;
	}

}

/// ===========================================================

