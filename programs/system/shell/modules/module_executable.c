
/// ===========================================================

int executable_run(char cmd[], char args[])
{

char		exec[256];
#if LANG_ENG
	char		error_starting[]={"  No such command '%s'.\n\r"};
#elif LANG_RUS
	char		error_starting[]={"  ��� ⠪�� ������� - '%s'.\n\r"};
#endif
int		result;

if ( '/' == cmd[0]) // �᫨ ���� ��᮫���
	{
	strcpy(exec, cmd);

	if (  !file_check(exec) ) // �஢��塞 ����⢮����� 䠩��
		{
		printf(error_starting, cmd);
		return FALSE;
		}
	}

else 
	{
	strcpy(exec, cur_dir); // �஢��塞 䠩� � ⥪�饬 ��⠫���
//	strcat(exec, "/"); // add slash
	strcat(exec, cmd);
	
	if ( !file_check(exec) ) // �஢��塞 ����⢮����� 䠩��
		{
		strcpy(exec, "/rd/1/"); // �஢��塞 䠩� �� ����㠫쭮� ��᪥
		strcat(exec, cmd);
			if ( !file_check(exec) ) // �஢��塞 ����⢮����� 䠩��
				{
				printf(error_starting, cmd);
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
	printf(error_starting, cmd);
	return FALSE;
	}

}

/// ===========================================================
