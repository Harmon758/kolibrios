#define MAX_LINE_CHARS 256

#define CHBOX 12
#define CHECKBOX_ID 50
unsigned char checkbox[sizeof(file "checkbox.raw")]= FROM "checkbox.raw";

#define COL_BG_ACTIVE 0xFFF0A9
#define COL_BG_INACTIVE 0xFFFFFF

//===================================================//
//                                                   //
//                       LINE                        //
//                                                   //
//===================================================//

struct NOTE_LINE
{
	bool state;
	char data[MAX_LINE_CHARS];
	void Delete();
};

void NOTE_LINE::Delete()
{
	state=false;
	data[0]=' ';
	data[1]=NULL;
}

//===================================================//
//                                                   //
//                       LIST                        //
//                                                   //
//===================================================//

struct NOTES : llist {
	char txt_path[4096];
	char txt_data[MAX_LINE_CHARS*LINES_COUNT];

	NOTE_LINE lines[LINES_COUNT]; 

	char edit_active;
	int OpenTxt();
	int SaveTxt();
	void DeleteNode();
	void DrawList();
	dword DrawLine(int line_n, draw_h);
} notes;


int NOTES::OpenTxt(dword file_path)
{
	int i=0, linepos=0;
	int item_n=-1;

	strcpy(#txt_path, file_path);
	ReadFile(0, 4096, #txt_data, #txt_path);
	if (!txt_data) || (strncmp(#txt_data, "notes", 5)!=0)
	{
		notify("'Notes\nData file does not exists or is not valid' -tE");
		return 0;
	}
	else
	{
		i+=5; //skip "notes" indefinier
		while (txt_data[i]) 
		{
			if (txt_data[i]=='\n') {
				item_n++;
				i+=2;
				if (txt_data[i]=='-') lines[item_n].state=false; else lines[item_n].state=true;
				i+=2;
				linepos = 0;
				continue;
			}
			if (linepos<MAX_LINE_CHARS) lines[item_n].data[linepos] = txt_data[i];
			linepos++;				
			i++;
		}
		while (item_n < LINES_COUNT)
		{
			//lines[item_n].Delete();
			item_n++;
		}
		count = LINES_COUNT;
		return 1;
	}
}

int NOTES::SaveTxt()
{
	int i;
	dword tm;
	strcpy(#txt_data, "notes");
	for (i=0; i<=count; i++)
	{
		if (lines[i].state==false) strcat(#txt_data, "\n- "); else strcat(#txt_data, "\n+ ");
		tm = #lines[i].data;
		strcat(#txt_data, #lines[i].data);
	}
	WriteFile(0, strlen(#txt_data), #txt_data, #txt_path);
}

void NOTES::DrawList()
{
	int i;
	for (i=0; i<visible; i++) DrawLine(i, item_h);
}


dword NOTES::DrawLine(int line_n, draw_h) {
	dword 
		COL_BOTTOM_LINE=0xE8EFF4,
		COL_BG,
		cur_text;
	char line_text[4096];
	if (line_n<0) return;
	x = 1;
	if (line_n==cur_y) COL_BG = COL_BG_ACTIVE; else COL_BG = COL_BG_INACTIVE;
	DrawBar(x, line_n*item_h+y, RED_LINE_X, draw_h-1, COL_BG_INACTIVE);
	DrawBar(x+RED_LINE_X+1, line_n*item_h+y, w-RED_LINE_X-1, draw_h-1, COL_BG);

	cur_text = #lines[line_n].data;

	if (draw_h!=item_h) 
	{
		COL_BOTTOM_LINE=COL_BG;
	}
	else
	{
		DefineButton(RED_LINE_X-CHBOX/2+x, item_h*line_n+5+y, CHBOX-1,CHBOX-1, CHECKBOX_ID+line_n+BT_HIDE, 0); //checkbox
		_PutImage(RED_LINE_X-CHBOX/2+x, item_h*line_n+5+y, CHBOX,CHBOX, lines[line_n].state*CHBOX*CHBOX*3+#checkbox);
		if (cur_text) WriteText(x+RED_LINE_X+6, item_h*line_n+7+y, 0x80, lines[line_n].state*0x777777, cur_text);
		if (lines[line_n].state == true) DrawBar(x+RED_LINE_X+6, item_h*line_n+11+y, strlen(cur_text)*6, 1, 0x444444); //strike
	}
	DrawBar(x, line_n*item_h+draw_h-1+y, w, 1, COL_BOTTOM_LINE);
	DrawBar(x+RED_LINE_X, line_n*item_h+y, 1, draw_h, COL_RED_LINE);
	x = RED_LINE_X;
	return cur_text;
}
