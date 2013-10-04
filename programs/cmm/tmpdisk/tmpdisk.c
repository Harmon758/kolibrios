#define MEMSIZE 0xA0000
#include "..\lib\kolibri.h" 
#include "..\lib\strings.h" 
#include "..\lib\file_system.h"

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

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////                    Code                    ////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#include "t_console.c"
#include "t_window.c"


void main()
{   
	driver_handle = LoadDriver("tmpdisk");
	if (driver_handle==0)
	{
		notify("error: /rd1/1/drivers/tmpdisk.obj driver loading failed");
		notify("virtual disk wouldn't be added");
		ExitProcess();
	}

	
	if (param)
		Console_Work();
	else
		Main_Window();
		
	
	ExitProcess();
}

stop:
