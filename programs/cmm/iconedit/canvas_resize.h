
child_window Window_CanvasReSize = {#CanvasReSize_Thread};

//===================================================//
//                                                   //
//                       CODE                        //
//                                                   //
//===================================================//

char text_columns[4];
char text_rows[4];

edit_box edit_columns = {60,NULL,NULL,0xffffff,0x94AECE,0xFFFfff,0xffffff,
	0x10000000,sizeof(text_columns)-1,#text_columns,0, 1000000000000010b};
edit_box edit_rows = {60,NULL,NULL,0xffffff,0x94AECE,0xFFFfff,0xffffff,
	0x10000000,sizeof(text_rows)-1,#text_rows,0, 1000000000000000b};

#define BTN_APPLY 10

void CanvasReSize_Thread()
{
	int id, butw;

	sprintf(#text_columns, "%i", image.columns);
	sprintf(#text_rows, "%i", image.rows);
	edit_columns.size = edit_columns.pos = edit_columns.shift = edit_columns.shift_old = strlen(#text_columns);
	edit_rows.size = edit_rows.pos = edit_rows.shift = edit_rows.shift_old = strlen(#text_rows);

	SetEventMask(EVM_REDRAW+EVM_KEY+EVM_BUTTON+EVM_MOUSE+EVM_MOUSE_FILTER);
	loop() switch(WaitEvent())
	{
	case evMouse:
		edit_box_mouse stdcall (#edit_columns);
		edit_box_mouse stdcall (#edit_rows);
		break;

	case evKey:
		GetKeys();

		if (SCAN_CODE_ESC == key_scancode) ExitProcess();
		if (SCAN_CODE_ENTER == key_scancode) EventApplyClick();
		if (SCAN_CODE_TAB == key_scancode) EventTabClick();

		EAX= key_ascii << 8;
		edit_box_key stdcall (#edit_columns);	
		edit_box_key stdcall (#edit_rows);	
		break;

	case evButton:
		id = GetButtonID();
		if (CLOSE_BTN == id) ExitProcess();
		if (BTN_APPLY == id) EventApplyClick();
		break;

	case evReDraw:
		DefineAndDrawWindow(Form.left+canvas.x + 100, Form.top+skin_height+canvas.y+40, 
			200, 170, 0x34, system.color.work, "Canvas", 0);
		WriteText(20, 20, 0x90, system.color.work_text, "Width");
		WriteText(20, 60, 0x90, system.color.work_text, "Height");
		DrawStandartCaptButton(20, 100, BTN_APPLY, "OK");
		DrawEditBoxes();
	}
}

void DrawEditBoxes()
{
	DrawEditBoxPos(20+70, 20-4, #edit_columns);
	DrawEditBoxPos(20+70, 60-4, #edit_rows);
}

//===================================================//
//                                                   //
//                      EVENTS                       //
//                                                   //
//===================================================//

void EventApplyClick()
{
	int new_rows = atoi(#text_rows);
	int new_columns = atoi(#text_columns);
	if (new_columns>MAX_CELL_SIZE) || (new_rows>MAX_CELL_SIZE) {
		sprintf(#param, 
			"'Maximum icon size exceeded! Please, try\nsomething less or equal to %ix%i.' -E",
			MAX_CELL_SIZE, MAX_CELL_SIZE);
		notify(#param);
		return;
	}
	image.create(new_rows, new_columns);
	actionsHistory.init();
	ActivateWindow(GetProcessSlot(Form.ID));
	DrawEditArea();
	ExitProcess();
}

void EventTabClick()
{
	if (edit_columns.flags & 0b10) { edit_columns.flags -= 0b10; edit_rows.flags += 0b10; }
	else { edit_columns.flags += 0b10; edit_rows.flags -= 0b10; }
	DrawEditBoxes();
}