dword bufpointer;
dword o_bufpointer;
dword bufsize;

char URL[10000];

scroll_bar scroll_wv = { 15,200,398,44,0,2,115,15,0,0xeeeeee,0xBBBbbb,0xeeeeee,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1};

char header[2048];

struct TWebBrowser {
	llist list;
	dword draw_line_width;
	DrawBufer DrawBuf;
	void Prepare();
	void Parse();
	void SetTextStyle();
	void DrawPage();
	void DrawScroller();
	void NewLine();
	void Perenos();
	byte end_parsing;
} WB1;

byte b_text, i_text, u_text, s_text, pre_text, blq_text, li_text, li_tab, 
	link, ignor_text, cur_encoding, text_align, t_html, t_body;
byte condition_text_active, condition_text_val, condition_href, condition_max;

enum { _WIN, _DOS, _KOI, _UTF, _DEFAULT };

enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT};

dword text_colors[300];
dword text_color_index;
dword link_color_inactive;
dword link_color_active;
dword bg_color;

int stroka;
int stolbec;
int tab_len;
int anchor_line_num;

char line[500];
char tag[100];
char tagparam[10000];
char parametr[1200];
char options[4096];
char anchor[256];

#include "..\TWB\history.h"
#include "..\TWB\links.h"
#include "..\TWB\colors.h"
#include "..\TWB\unicode_tags.h"
#include "..\TWB\img_cache.h"
#include "..\TWB\parce_tag.h"
#include "..\TWB\table.h"


//=======================================================================


void TWebBrowser::DrawPage()
{
	int start_x, start_y, line_length, stolbec_len, magrin_left=5;
	dword font_type;
	
	if (!header)
	{
		strcpy(#header, #line);
		strcat(#header, " -");
		strcat(#header, #version);
		line = 0;
		return;
	}
	if (t_html) && (!t_body) return;
	
	if (stroka >= 0) && (stroka - 2 < list.visible) && (line) && (!anchor)
	{
		start_x = stolbec * 6 + magrin_left * DrawBuf.zoom + list.x;
		start_y = stroka * list.line_h + magrin_left + list.y;
		stolbec_len = strlen(#line);
		line_length = stolbec_len * 6 * DrawBuf.zoom;

		if (DrawBuf.zoom==1) font_type = 0x88; else font_type = 0x89;

		WriteBufText(start_x, 0, font_type, text_colors[text_color_index], #line, buf_data);
		if (b_text)	WriteBufText(start_x+1, 0, font_type, text_colors[text_color_index], #line, buf_data);
		if (i_text) { stolbec++; DrawBuf.Skew(start_x, 0, line_length, list.line_h); } // bug with zoom>1
		if (s_text) DrawBuf.DrawBar(start_x, list.line_h / 2 - DrawBuf.zoom, line_length, DrawBuf.zoom, text_colors[text_color_index]);
		if (u_text) DrawBuf.DrawBar(start_x, list.line_h - DrawBuf.zoom - DrawBuf.zoom, line_length, DrawBuf.zoom, text_colors[text_color_index]);
		if (link) {
			DrawBuf.DrawBar(start_x, list.line_h - DrawBuf.zoom - DrawBuf.zoom, line_length, DrawBuf.zoom, text_colors[text_color_index]);
			UnsafeDefineButton(start_x-2, start_y-1, line_length + 3, DrawBuf.zoom * 10, PageLinks.count + 400 + BT_HIDE, 0xB5BFC9);
			PageLinks.AddText(#line, line_length, list.line_h, UNDERLINE);
		}
		stolbec += stolbec_len;
	}
}
//=======================================================================


void BufEncode(int set_new_encoding)
{
	int bufpointer_realsize;
	cur_encoding = set_new_encoding;
	if (o_bufpointer==0)
	{
		o_bufpointer = malloc(bufsize);
		strcpy(o_bufpointer, bufpointer);
	}
	else
	{
		strcpy(bufpointer, o_bufpointer);
	}
	if (set_new_encoding==_WIN) bufpointer = ChangeCharset("CP1251",  "CP866", bufpointer);
	if (set_new_encoding==_UTF) bufpointer = ChangeCharset("UTF-8",   "CP866", bufpointer);
	if (set_new_encoding==_KOI) bufpointer = ChangeCharset("KOI8-RU", "CP866", bufpointer);
}

void TWebBrowser::Prepare(dword bufpos, in_filesize){
	bufsize = in_filesize;
	bufpointer = bufpos;
	Parse();
}


void TWebBrowser::Parse(){
	word bukva[2];
	int j;
	byte ignor_param;
	char temp[768];
	dword bufpos = bufpointer;
	int line_len;
	
	b_text = i_text = u_text = s_text = blq_text = t_html = t_body =
	li_text = link = ignor_text = text_color_index = text_colors[0] = li_tab = 
	condition_text_val = condition_text_active = 0; //�������� ����
	end_parsing = false;
	condition_max = 255;
	text_align = ALIGN_LEFT;
	link_color_inactive = 0x0000FF;
	link_color_active = 0xFF0000;
	bg_color = 0xFFFFFF;
	DrawBuf.Fill(bg_color);
	PageLinks.Clear();
	strcpy(#header, #version);
	stroka = -list.first;
	stolbec = 0;
	line = 0;

	draw_line_width = list.w * DrawBuf.zoom;

	if (pre_text<>2)
	{
		pre_text=0;
		if (!strcmp(#URL + strlen(#URL) - 4, ".txt")) pre_text = 1;
		if (!strcmp(#URL + strlen(#URL) - 4, ".mht")) ignor_text = 1;
	}
	
	for ( ; (bufpointer+bufsize > bufpos) && (ESBYTE[bufpos]!=0); bufpos++;)
	{
		if (end_parsing) break;
		bukva = ESBYTE[bufpos];
		if (ignor_text) && (bukva!='<') continue;
		if (condition_text_active) && (condition_text_val != condition_href) && (bukva!='<') continue;
		switch (bukva)
		{
		case 0x0a:
			if (pre_text)
			{
				chrcat(#line, ' ');
				bukva = temp = NULL;
				Perenos();
				break;
			}
		case '\9':
			if (pre_text) //����� ��� �� 0x0d	
			{
				tab_len = strlen(#line) % 4;
				if (!tab_len) tab_len = 4;
				for (j=0; j<tab_len; j++;) chrcat(#line,' ');
				break;
			}
			goto DEFAULT_MARK;		
		case '=': //quoted printable
			if (strcmp(#URL + strlen(#URL) - 4, ".mht")<>0) goto DEFAULT_MARK;

			temp[0] = ESBYTE[bufpos+1];
			temp[1] = ESBYTE[bufpos+2];
			temp[2] = '\0';
			if (bukva = Hex2Symb(#temp))
			{
				bufpos+=2;
				goto DEFAULT_MARK;
			}
			break;
			
		case '&': //&nbsp; and so on
			bufpos++;
			tag=0;
			for (j=0; (ESBYTE[bufpos]<>';') && (j<7);   j++, bufpos++;)
			{
				bukva = ESBYTE[bufpos];
				chrcat(#tag, bukva);
			}
			if (bukva = GetUnicodeSymbol()) goto DEFAULT_MARK;
			break;
		case '<':
			bufpos++; //��������� ������ <
			tag = parametr = tagparam = ignor_param = NULL;
			if (ESBYTE[bufpos] == '!') //���������� ������ <!-- -->, ������
			{
				bufpos++;
				if (ESBYTE[bufpos] == '-')
				{
				HH_:
					do
					{
						bufpos++;
						if (bufpointer + bufsize <= bufpos) break 2;
					}
					while (ESBYTE[bufpos] <>'-');
					
					bufpos++;
					if (ESBYTE[bufpos] <>'-') goto HH_;
				}
			}
			while (ESBYTE[bufpos] !='>') && (bufpos < bufpointer + bufsize) //�������� ��� � ��� ���������
			{
				bukva = ESBYTE[bufpos];
				if (bukva == '\9') || (bukva == '\x0a') || (bukva == '\x0d') bukva = ' ';
				if (!ignor_param) && (bukva <>' ')
				{
					if (strlen(#tag)<sizeof(tag)) chrcat(#tag, bukva);
				}
				else
				{
					ignor_param = true;
					if (!ignor_text) && (strlen(#tagparam)+1<sizeof(tagparam)) strcat(#tagparam, #bukva);
				}
				bufpos++;
			}
			strlwr(#tag);

			if (condition_text_active) && (condition_text_val != condition_href) 
			{
				if (strcmp(#tag, "/condition")!=0) break;
			}
			if (tag[strlen(#tag)-1]=='/') tag[strlen(#tag)-1]=NULL; //for br/
			if (tagparam) GetNextParam();

			if (stolbec + strlen(#line) > list.column_max) Perenos();
			DrawPage();
			line = NULL;
			if (tag) SetTextStyle(WB1.DrawBuf.zoom * 5 + list.x, stroka * list.line_h + list.y + 5); //��������� �����
			tag = parametr = tagparam = ignor_param = NULL;
			break;
		default:
			DEFAULT_MARK:
			if (bukva<=15) bukva=' ';
			line_len = strlen(#line);
			if (!pre_text) && (bukva == ' ')
			{
				if (line[line_len-1]==' ') break; //no double spaces
				if (!stolbec) && (!line) break; //no paces at the beginning of the line
			}
			if (line_len < sizeof(line)) chrcat(#line, bukva);
			if (stolbec + line_len > list.column_max) Perenos();
		}
	}
	DrawPage();
	NewLine();
	DrawBar(list.x, stroka * list.line_h + list.y + 5, draw_line_width, -stroka * list.line_h + list.h - 5, bg_color);
	DrawBar(list.x, list.visible * list.line_h + list.y + 4, draw_line_width, -list.visible * list.line_h + list.h - 4, bg_color);
	if (list.first == 0) list.count = stroka;
	if (anchor) //���� ������� ������ �������� ����� ����� - ����� ����������� ����
	{
		anchor=NULL;
		list.first=anchor_line_num;
		Parse();
	}
	DrawScroller();
}

void TWebBrowser::Perenos()
{
	int perenos_num;
	char new_line_text[4096];
	perenos_num = strrchr(#line, ' ');
	if (!perenos_num) && (strlen(#line)>list.column_max) perenos_num=list.column_max;
	strcpy(#new_line_text, #line + perenos_num);
	line[perenos_num] = 0x00;
	if (stroka-1 > list.visible) && (list.first <>0) end_parsing=true;
	DrawPage();
	strcpy(#line, #new_line_text);
	NewLine();
}


char oldtag[100];
void TWebBrowser::SetTextStyle(int left1, top1) {
	dword hr_color;
	byte opened;
	byte meta_encoding;
	//��������� ��� ����������� ��� �����������
	if (tag[0] == '/') 
	{
		 opened = 0;
		 strcpy(#tag, #tag+1);
	}
	else opened = 1;
		
	if (isTag("html"))
	{
		IF(!strcmp(#URL + strlen(#URL) - 4, ".mht")) IF (opened==0) ignor_text = 1; ELSE ignor_text = 0;
		t_html = opened;
		return;
	}

	if (isTag("script")) || (isTag("style")) || (isTag("binary")) || (isTag("select")) ignor_text = opened;

	if(isTag("title"))
	{
		if (opened) header=NULL;
		else if (!stroka) DrawTitle(#header); //��� �������� - ������ ������
		return;
	}

	if (ignor_text) return;
	
	IF(isTag("q"))
	{
		if (opened)
		{
			NewLine();
			strcat(#line, ' \"');
		}
		if (!opened)
		{
			chrcat(#line, '\"');
			NewLine();
		} 
	}

	if (anchor) && (!strcmp(#parametr, "id=")) //����� �����!!! ������ ��� ���� �� ��������� ���, �������� �� �����
	{
		if (!strcmp(#anchor, #options))	anchor_line_num=list.first+stroka;
	}
	
	if (isTag("body"))
	{
		t_body = opened;
		do{
			if (!strcmp(#parametr, "condition_max=")) condition_max = atoi(#options);
			if (!strcmp(#parametr, "link=")) link_color_inactive = GetColor(#options);
			if (!strcmp(#parametr, "alink=")) link_color_active = GetColor(#options);
			if (!strcmp(#parametr, "text=")) text_colors[0]=GetColor(#options);
			if (!strcmp(#parametr, "bgcolor="))
			{
				bg_color=GetColor(#options);
				DrawBuf.Fill(bg_color);
			}
		} while(GetNextParam());
		if (opened) && (cur_encoding==_DEFAULT)
		{
			debugln("Document has no information about encoding, UTF will be used");
			BufEncode(_UTF);
		}
		return;
	}

	if (isTag("a"))
	{
		if (opened)
		{
			if (link) IF(text_color_index > 0) text_color_index--; //���� ���������� ��� � �� ��� ������

			do{
				if (!strcmp(#parametr, "href="))
				{
					if (stroka - 1 > list.visible) || (stroka < -2) return;
					
					text_color_index++;
					text_colors[text_color_index] = text_colors[text_color_index-1];
					
					link = 1;
					text_colors[text_color_index] = link_color_inactive;
					PageLinks.AddLink(#options, DrawBuf.zoom * stolbec*6+left1, top1-DrawBuf.zoom);
				}
				if (anchor) && (!strcmp(#parametr, "name="))
				{
					if (!strcmp(#anchor, #options))
					{
						anchor_line_num=list.first+stroka;
					}
				}
			} while(GetNextParam());
		}
		else {
			link = 0;
			IF(text_color_index > 0) text_color_index--;
		}
		return;
	}

	if (isTag("font"))
	{
		if (opened)
		{
			text_color_index++;
			text_colors[text_color_index] = text_colors[text_color_index-1];
		
			do{
				if (strcmp(#parametr, "color=") == 0) //&& (parametr[1] == '#')
				{
					text_colors[text_color_index] = GetColor(#options);
				}
			} while(GetNextParam());
		}
		else
			if (text_color_index > 0) text_color_index--;
		return;
	}
	if (isTag("br")) {
		NewLine();
		return;
	}
	if (isTag("div")) || (isTag("header")) || (isTag("article")) || (isTag("footer")) {
		IF(oldtag[0] <>'h') NewLine();
		if (!strcmp(#parametr, "bgcolor="))
		{
			bg_color=GetColor(#options);
			DrawBuf.Fill(bg_color);
		}
		return;
	}
	if (isTag("p")) {
		IF(oldtag[0] == 'h') return;
		NewLine();
		IF(opened) NewLine();
		return;
	}

	if(isTag("table")) {
		table.active = opened;
		NewLine();
		if (opened)	table.NewTable();
	}

	if(isTag("td")) {
		if (opened)
		{
			table.cur_col++;
			table.row_h = 0;
			do {
				if (!strcmp(#parametr, "width="))
				{
					table.col_w[table.cur_col] = atoi(#options);
					// NewLine();
					// strcpy(#line, #options);
					// NewLine();
				}
			} while(GetNextParam());
		}
		else
		{
			if (table.row_h > table.row_max_h) table.row_max_h = table.row_h;
		}
	}

	if(isTag("tr")) {
		if (opened)
		{
			table.cur_col = 0;
			table.row_max_h = 0;
			table.row_start = stroka;
		}
		else
		{
			NewLine();
			if (table.cur_row == 0) table.max_cols = table.cur_col;
			table.cur_row++;
			table.max_cols = table.cur_col;
		}
	}

	/*
	if (isTag("center"))
	{
		if (opened) text_align = ALIGN_CENTER;
		if (!opened)
		{
			NewLine();
			text_align = ALIGN_LEFT;
		}
		return;
	}
	if (isTag("right"))
	{
		if (opened) text_align = ALIGN_RIGHT;
		if (!opened)
		{
			NewLine();
			text_align = ALIGN_LEFT;
		}
		return;
	}
	*/
	if (isTag("h1")) || (isTag("h2")) || (isTag("h3")) || (isTag("h4")) {
		NewLine();
		if (opened) && (stroka>1) NewLine();
		strcpy(#oldtag, #tag);
		if (opened)
		{
			if (!strcmp(#parametr, "align=")) && (!strcmp(#options,"center")) text_align = ALIGN_CENTER;
			if (!strcmp(#parametr, "align=")) && (!strcmp(#options,"right")) text_align = ALIGN_RIGHT;
			b_text = 1;
		}
		if (!opened)
		{
			text_align = ALIGN_LEFT;
			b_text = 0;
		}
		return;
	}
	else
		oldtag=NULL;
		
	if (isTag("b")) || (isTag("strong")) || (isTag("big")) {
		b_text = opened;
		return;
	}
	if(isTag("i")) || (isTag("em")) || (isTag("subtitle")) {
		i_text = opened;
		return;
	}	
	if (isTag("dt"))
	{
		li_text = opened;
		IF(opened == 0) return;
		NewLine();
		return;
	}
	if (isTag("condition"))
	{
		condition_text_active = opened;
		if (opened) && (!strcmp(#parametr, "show_if=")) condition_text_val = atoi(#options);
		return;
	}
	if (isTag("li")) || (isTag("dt")) //���� ������� ��������� ������
	{
		li_text = opened;
		if (opened)
		{
			NewLine();
			if (stroka > -1) && (stroka - 2 < list.visible) 
				DrawBuf.DrawBar(li_tab * 5 * 6 * DrawBuf.zoom + list.x, list.line_h / 2 - DrawBuf.zoom - DrawBuf.zoom, DrawBuf.zoom*2, DrawBuf.zoom*2, 0x555555);
		}
		return;
	}
	if (isTag("u")) || (isTag("ins")) u_text = opened;
	if (isTag("s")) || (isTag("strike")) || (isTag("del")) s_text = opened;
	if (isTag("ul")) || (isTag("ol")) IF(!opened)
	{
		li_text = opened;
		li_tab--;
		NewLine();
	} ELSE li_tab++;
	if (isTag("dd")) stolbec += 5;
	if (isTag("blockquote")) blq_text = opened;
	if (isTag("pre")) || (isTag("code")) pre_text = opened; 
	if (isTag("hr"))
	{
		if (anchor) || (stroka < -1)
		{
			stroka+=2;
			return;
		}
		if (strcmp(#parametr, "color=") == 0) hr_color = GetColor(#options); else hr_color = 0x999999;
		NewLine();
		DrawBuf.DrawBar(5, list.line_h/2, list.w-10, 1, hr_color);
		NewLine();
	}
	if (isTag("img"))
	{
		ImgCache.Images( left1, top1, WB1.list.w);
		return;
	}
	if (isTag("meta")) || (isTag("?xml"))
	{
		do{
			if (!strcmp(#parametr, "charset=")) || (!strcmp(#parametr, "content=")) || (!strcmp(#parametr, "encoding="))
			{
				strcpy(#options, #options[strrchr(#options, '=')]); //����� � content=
				strlwr(#options);
				meta_encoding = _DEFAULT;
				if (!strcmp(#options, "utf-8"))  || (!strcmp(#options,"utf8")) meta_encoding = _UTF;
				if (!strcmp(#options, "koi8-r")) || (!strcmp(#options, "koi8-u")) meta_encoding = _KOI;
				if (!strcmp(#options, "windows-1251")) || (!strcmp(#options, "windows1251")) meta_encoding = _WIN;
				if (!strcmp(#options, "dos"))    || (!strcmp(#options, "cp-866"))   meta_encoding = _DOS;
				if (cur_encoding==_DEFAULT) BufEncode(meta_encoding);
				return;
			}
		} while(GetNextParam());
		return;
	}
}

void TWebBrowser::DrawScroller()
{
	scroll_wv.max_area = list.count;
	scroll_wv.cur_area = list.visible;
	scroll_wv.position = list.first;

	scroll_wv.all_redraw = 0;
	scroll_wv.start_x = list.x + list.w;
	scroll_wv.start_y = list.y;

	scroll_wv.size_y = list.h;
	scroll_wv.start_x = list.w * DrawBuf.zoom + list.x;

	scrollbar_v_draw(#scroll_wv);
}


void TWebBrowser::NewLine()
{
	int onleft, ontop;

	onleft = list.x + 5;
	ontop = stroka * list.line_h + list.y + 5;
	if (!stroka) DrawBar(list.x, list.y, draw_line_width, 5, bg_color);
	if (t_html) && (!t_body) return;
	if (stroka * list.line_h + 5 >= 0) && ( stroka + 1 * list.line_h + 5 < list.h) && (!anchor)
	{
		if (text_align == ALIGN_CENTER) && (DrawBuf.zoom==1) DrawBuf.AlignCenter(onleft,ontop,list.w,list.line_h,stolbec * 6);
		if (text_align == ALIGN_RIGHT) && (DrawBuf.zoom==1)  DrawBuf.AlignRight(onleft,ontop,list.w,list.line_h,stolbec * 6);
		DrawBuf.bufy = ontop;
		DrawBuf.Show();
		DrawBuf.Fill(bg_color);
	}
	stroka++;
	if (blq_text) stolbec = 6; else stolbec = 0;
	if (li_text) stolbec = li_tab * 5;
}



int isTag(dword text) 
{ 
	if (!strcmp(#tag,text)) return 1; else return 0;
}


:dword Hex2Symb(char* htmlcolor)
{
  dword j=0, symbol=0;
  char ch=0x00;
  for (;j<2;j++)
  {
    ch=ESBYTE[htmlcolor+j];
    if (ch==0x0d) || (ch=='\9') RETURN 0;
    if ((ch>='0') && (ch<='9')) ch -= '0';
    if ((ch>='A') && (ch<='F')) ch -= 'A'-10;
    if ((ch>='a') && (ch<='f')) ch -= 'a'-10;
    symbol = symbol*0x10 + ch;
  }
  AL=symbol;
}

