int del_error;
int Del_File2(dword way, sh_progr)
{    
	dword dirbuf, fcount, i, filename;
	int error;
	char del_from[4096];
	if (dir_exists(way))
	{
		if (error = GetDir(#dirbuf, #fcount, way, DIRS_ONLYREAL)) del_error = error;
		for (i=0; i<fcount; i++)
		{
			if (CheckEvent()==evReDraw) draw_window();
			filename = i*304+dirbuf+72;
			sprintf(#del_from,"%s/%s",way,filename);
			if ( TestBit(ESDWORD[filename-40], 4) )
			{
				Del_File2(#del_from, 1);
			}
			else
			{
				if (sh_progr) Operation_Draw_Progress(filename);
				if (error = DeleteFile(#del_from)) del_error = error;
			}
		}
	}
	if (error = DeleteFile(way)) del_error = error;
}

void Del_File_Thread()
{   
	byte del_from[4096];
	dword selected_offset2;
	int tst, count, i;
	
	file_count_copy = 0;
	copy_bar.value = 0; 
	operation_flag = DELETE_FLAG;
	
	if (selected_count)
	{
	   for (i=0; i<files.count; i++) 
		{
			selected_offset2 = file_mas[i]*304 + buf+32 + 7;
			if (ESBYTE[selected_offset2]) {
				sprintf(#del_from,"%s/%s",#path,file_mas[i]*304+buf+72);
				GetFileInfo(#del_from, #file_info_count);
				if ( file_info_count.isfolder ) DirFileCount(#del_from);
				else file_count_copy++;
			}
		}
	}
	else
	{
		if (itdir) DirFileCount(#file_path);
		else file_count_copy++;
	}
	
	copy_bar.max = file_count_copy;
	
	del_error = 0;
	DisplayOperationForm();
	if (selected_count)
	{
		for (i=0; i<files.count; i++) 
		{
			selected_offset2 = file_mas[i]*304 + buf+32 + 7;
			if (ESBYTE[selected_offset2]) {
				sprintf(#del_from,"%s/%s",#path,file_mas[i]*304+buf+72);
				Del_File2(#del_from, 1);
			}
		}
	}
	else
	{
		Del_File2(#file_path, 1);			
	}
	if (del_error) Write_Error(del_error);
	cmd_free = 6;
	DialogExit();
}

void Del_File(byte dodel) {
	del_active=0;
	if (dodel)
	{
		delete_stak = malloc(40000);
		CreateThread(#Del_File_Thread,delete_stak+40000-4);
	}
	else draw_window();
}