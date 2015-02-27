#define MEMSIZE 0xA0000
#include "..\lib\kolibri.h" 
#include "..\lib\mem.h" 
#include "..\lib\strings.h"
#include "..\lib\file_system.h"

#include "..\lib\dll.h"
#include "..\lib\lib.obj\libio_lib.h"
#include "..\lib\lib.obj\libini.h"

#ifndef AUTOBUILD
#include "lang.h--"
#endif

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////              Program data                  ////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

struct ioctl_struct
{
	dword	handle;
	dword	io_code;
	dword	input;
	dword	inp_size;
	dword	output;
	dword	out_size;
};

#define DEV_ADD_DISK 1	//input = structure add_disk_struc
#define DEV_DEL_DISK 2	//input = structure del_disk_struc

struct add_disk_struc
{
	dword DiskSize; // in sectors, 1 sector = 512 bytes. Include FAT service data
	unsigned char DiskId; // from 0 to 9
};

struct del_disk_struc
{
	unsigned char DiskId; //from 0 to 9
};


ioctl_struct ioctl;
add_disk_struc add_disk;
del_disk_struc del_disk;

int driver_handle;

dword disk_sizes[10];

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////                    Code                    ////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#include "t_console.c"
#include "t_gui.c"


void main()
{   
	driver_handle = LoadDriver("tmpdisk");
	if (driver_handle==0)
	{
		notify("'TmpDisk\nError: /rd1/1/drivers/tmpdisk.obj driver loading failed\nvirtual disk wouldn't be added' -tE");
		ExitProcess();
	}

	GetDiskSizesFromIni();
	
	if (param)
		Console_Work();
	else
		Main_Window();
		
	SaveDiskSizesToIni();
	ExitProcess();
}


void GetDiskSizesFromIni()
{
	char i, key[2];
	if (load_dll2(libini, #lib_init,1)!=0) notify("Error: library doesn't exists - libini");
	for (i=0; i<=9; i++)
	{
		key[0]=i+'0';
		ini_get_int stdcall ("/sys/settings/tmpdisk.ini", "DiskSizes", #key, 0); 
		disk_sizes[i] = EAX;
	}
}

void SaveDiskSizesToIni()
{
	char i, key[2];
	for (i=0; i<=9; i++)
	{
		key[0]=i+'0';
		ini_set_int stdcall ("/sys/settings/tmpdisk.ini", "DiskSizes", #key, disk_sizes[i]);
	}
}


stop:
