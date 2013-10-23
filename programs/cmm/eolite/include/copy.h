//Leency 2008-2013


byte copy_from[4096], copy_to[4096], cut_active=0;
enum {NOCUT, CUT, COPY_PASTE_END};
#define WIN_W 300
#define WIN_H 50


void Copy(dword pcth, char cut)
{
	strcpy(#copy_from, pcth);
	cut_active = cut;
}

void copyf_Draw_Progress(dword filename) {
	DefineAndDrawWindow(Form.left+Form.width-200,Form.top+90,WIN_W,GetSkinHeight()+WIN_H-1,0x34,sc.work,T_PASTE_WINDOW);
	WriteText(5,8, 0x80, sc.work_text, T_PASTE_WINDOW_TEXT);
	DrawBar(5, 26, WIN_W-10, 10, sc.work);
	WriteText(5,26, 0x80, sc.work_text, filename);
	if (CheckEvent()==evButton) 
	{
		notify(T_CANCEL_PASTE);
		ExitProcess();
	} 
}


void Paste()
{
	char copy_rezult;
	
	if (!copy_from) ExitProcess();
	strcpy(#copy_to, #path);
	strcat(#copy_to, #copy_from+strrchr(#copy_from,'/'));
	if (!strcmp(#copy_from,#copy_to))
	{
		strcpy(#copy_to, #path);
		strcat(#copy_to, "new_");
		strcat(#copy_to, #copy_from+strrchr(#copy_from,'/'));
	}
	if (strstr(#copy_to, #copy_from))
	{
		notify("Copy directory into itself is a bad idea...");
		ExitProcess();
	}
	if (copy_rezult = copyf(#copy_from,#copy_to))
	{
		Write_Error(copy_rezult);
	}
	else if (cut_active)
	{
		strcpy(#file_path, #copy_from);
		Del_File(true);
		copy_from=NULL;
		cut_active=false;
	}
	action_buf = COPY_PASTE_END;
	ActivateWindow(GetProcessSlot(Form.ID));
	ExitProcess();
}
