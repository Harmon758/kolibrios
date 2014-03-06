/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////                �������                     ////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#ifdef LANG_RUS
	?define ADD_DISK_TEXT "�஡�� �������� ����㠫�� ���"
	?define DELETE_DISK_TEXT "�஡�� 㤠���� ����㠫�� ���"
	?define DONT_KNOW_DISK_SIZE_TEXT "��� ࠧ��� �� 㪠���, 10% ᢮������ ��� �㤥� �ᯮ�짮����"
	?define NEW_DISK_TEXT "ࠧ��� ��᪠ �㤥�: "
	char *rezult_text[]={
	"������ �ᯥ譮 �����襭�",
	"��������� IOCTL, ������ ࠧ��� �।��⠢�塞�� ������...",
	"����� ��᪠ ������ ���� �� 0 �� 9",
	"ࠧ��� ᮧ��������� ��᪠ ᫨誮� �����",
	"ࠧ��� ᮧ��������� ��᪠ ᫨誮� ���",
	"�訡�� �뤥����� �����",
	"�������⭠� �訡�� O_o",
	0};
#else
	?define ADD_DISK_TEXT "trying to add disk"
	?define DELETE_DISK_TEXT "trying to delete virtual disk"
	?define DONT_KNOW_DISK_SIZE_TEXT "its size is not specified, 10% from free RAM will be used"
	?define NEW_DISK_TEXT "new DiskSize: "
	char *rezult_text[]={
	"operation completed successfully",
	"unknown IOCTL code, wrong input/output size...",
	"DiskId must be from 0 to 9",
	"DiskSize is too large",
	"DiskSize is too small, might be too little free RAM",
	"memory allocation failed",
	"unknown error O_o",
	0};
#endif

char Console_Work()
{
	unsigned int disk_size, driver_rezult;
	char size_t[256];
	dword size_tt;

	strlwr(#param);
//	debug(#param);
	
	switch (param[0])
	{
		case '?': //������ �� ���������
		case 'h':
			debug("tmpdisk command line parameters:");
			debug("a[number]s[size in MB] - add RAM disk");
			debug("d[number] - delete RAM disk");
			ExitProcess();
			break;
		case 'd': //������� ����
			debug(DELETE_DISK_TEXT);
			del_disk.DiskId = param[1]-'0';
			ioctl.handle   = driver_handle;
			ioctl.io_code  = DEV_DEL_DISK;
			ioctl.input    = #del_disk;
			ioctl.inp_size = sizeof(del_disk);
			ioctl.output   = 0;
			ioctl.out_size = 0;
			break;
		case 'a': //�������� ����
			debug(ADD_DISK_TEXT);
			disk_size= strchr(#param, 's');
			if (!disk_size)
			{
				add_disk.DiskSize = GetFreeRAM() / 5;
				debug(DONT_KNOW_DISK_SIZE_TEXT);
			}				
			else
				add_disk.DiskSize = atoi(#param+disk_size)*2048;
			strcpy(#size_t, NEW_DISK_TEXT);
			size_tt = itoa(add_disk.DiskSize/2048);
			strcat(#size_t, size_tt);
			strcat(#size_t, " MB");
			debug(#size_t);
			add_disk.DiskId = param[1]-'0';
			ioctl.handle   = driver_handle;
			ioctl.io_code  = DEV_ADD_DISK;
			ioctl.input    = #add_disk;
			ioctl.inp_size = sizeof(add_disk);
			ioctl.output   = 0;
			ioctl.out_size = 0;
			break;
		default:
			debug("unknown command line parameters");
			debug("use 'h' or '?' for help");
			ExitProcess();			
	}
	
	driver_rezult = RuleDriver(#ioctl);
	if (driver_rezult<7) debug(rezult_text[driver_rezult]);
	return driver_rezult;
}

