const command_t COMMANDS[]=
{
	{"about",   "  �뢮��� ���ଠ�� � �ணࠬ�� Shell\n\r", &cmd_about},
	{"alias",   "  �����뢠�� ᯨ᮪ alias'��\n\r", &cmd_alias},
	{"ccpuid",  "  �뢮��� ���ଠ�� � CPU\n\r", &cmd_ccpuid},
	{"cd",      "  ������� ⥪���� ��ਪ���. �ᯮ�짮�����:\n\r    cd <��४���>\n\r", &cmd_cd},
	{"clear",   "  ��頥� ��࠭\n\r", &cmd_clear},
	{"date",    "  �����뢠�� ⥪���� ���� � �६�\n\r", &cmd_date},
	{"echo",    "  �뢮��� ����� �� ��࠭. �ᯮ�짮�����:\n\r    echo <�����>\n\r", &cmd_echo},
	{"exit",    "  ��室 �� ���᮫�\n\r", &cmd_exit},
	{"memory",  "  �����뢠�� ���� �����: �ᥩ, ᢮������ � �ᯮ��㥬��\n\r", &cmd_memory},
	{"help",    "  ��ࠢ�� �� ��������. �ᯮ�짮�����:\n\r    help ;ᯨ᮪ ��� ������\n\r    help <�������> ;�ࠢ�� �� �������\n\r", &cmd_help},
	{"kill",    "  ������� �����. �ᯮ�짮�����:\n\r    kill <PID �����>\n\r", &cmd_kill},
	{"ls",      "  �뢮��� ᯨ᮪ 䠩���. �ᯮ�짮�����:\n\r    ls ;ᯨ᮪ 䠩��� � ⥪�饬 ��⠫���\n\r    ls <��४���> ;ᯨ᮪ 䠩��� �� �������� ��४�ਨ\n\r", &cmd_ls},
	{"mkdir",   "  ������� ��⠫��. �ᯮ�짮�����:\n\r    mkdir <��� �����> ;ᮧ���� ����� � ⥪�饬 ��⠫���\n\r    mkdir <����><��� �����> ;ᮧ���� ����� �� 㪠������� ���\n\r", &cmd_mkdir},
	{"more",    "  �뢮��� ᮤ�ন��� 䠩�� �� ��࠭. �ᯮ�짮�����:\n\r    more <��� 䠩��>\n\r", &cmd_more},
	{"ps",      "  �뢮��� ᯨ᮪ ����ᮢ\n\r", &cmd_ps},
	{"pwd",     "  �����뢠�� ��� ⥪�饩 ��४�ਨ\n\r", &cmd_pwd},
	{"reboot",  "  ��१���㦠�� �������� ��� �� KoOS. �ᯮ�짮�����:\n\r    reboot ;��१���㧨�� ��\n\r    reboot kernel ;��१������� �� Kolibri\n\r", &cmd_reboot},
	{"rm",      "  ������ 䠩�. �ᯮ�짮�����:\n\r    rm <��� 䠩��>\n\r", &cmd_rm},
	{"rmdir",   "  ������ �����. �ᯮ�짮�����:\n\r    rmdir <��४���>\n\r", &cmd_rmdir},
	{"sleep",   "  ��⠭�������� ࠡ��� Shell'� �� �������� �६�. �ᯮ�짮�����:\n\r    sleep <���ࢠ� � ���� ���� ᥪ㭤�>\n\r  �ਬ��:\n\r    sleep 500 ;��㧠 �� 5 ᥪ.\n\r", &cmd_sleep},
	{"shutdown","  �몫�砥� ��������\n\r", &cmd_shutdown},
	{"touch",   "  ������� ���⮩ 䠩� ��� ������� ����/�६� ᮧ����� 䠩��. �ᯮ�짮�����:\n\r    touch <��� 䠩��>\n\r", &cmd_touch},
	{"uptime",  "  �����뢠�� uptime\n\r", &cmd_uptime},
	{"ver",     "  �����뢠�� �����. �ᯮ�짮�����:\n\r    ver ;����� Shell'�\n\r    ver kernel ;����� � ����� ॢ���� �� ����\n\r", &cmd_ver},
};