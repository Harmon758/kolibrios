#include "..\lib\draw_buf.h"

int	downloader_id;

dword buf;
dword filesize;
dword blink;

#define URL param

int	mouse_twb;
edit_box address_box= {250,207,16,0xffffff,0x94AECE,0xffffff,0xffffff,0,sizeof(URL),#editURL,#mouse_twb,2,19,19};
scroll_bar scroll_wv = { 18,200,398, 44,18,0,115,15,0,0xeeeeee,0xD2CED0,0x555555,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1};

char editURL[sizeof(URL)];
char page_links[12000];
char header[2048];


char download_path[]="/rd/1/.download";

struct TWebBrowser {
	llist list; //need #include "..\lib\list_box.h"
	DrawBufer DrawBuf;
	void GetNewUrl();
	void ReadHtml();
	void ShowPage();
	void ParseHTML();
	void WhatTextStyle();
	void DrawPage();
	void DrawScroller();
	void TextGoDown();
};

TWebBrowser WB1;

byte rez, b_text, i_text, u_text, s_text, pre_text, blq_text, li_text, li_tab, 
	link, ignor_text, cur_encoding, text_align;
byte condition_text_active, condition_text_val, condition_href, condition_max;


enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT};

dword text_colors[300];
dword text_color_index;
dword link_color;
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
#include "..\TWB\colors.h"
#include "..\TWB\unicode_tags.h"
#include "..\TWB\img_cache.h"
#include "..\TWB\some_code.h"
#include "..\TWB\parce_tag.h"



//=======================================================================


void TWebBrowser::DrawPage()
{
	int start_x, start_y, line_length, magrin_left=5;
	
	if (!header)
	{
		strcpy(#header, #line);
		strcat(#header, " -");
		strcat(#header, #version);
		line = 0;
		return;
	}
	
	if (stroka >= 0) && (stroka - 2 < list.visible) && (line) && (!anchor)
	{
		start_x = stolbec * 6 + list.x + magrin_left;
		start_y = stroka * 10 + list.y + magrin_left;
		line_length = strlen(#line) * 6;

		WriteBufText(start_x, 0, 0x88, text_colors[text_color_index], #line, buf_data);
		IF (b_text)	WriteBufText(start_x+1, 0, 0x88, text_colors[text_color_index], #line, buf_data);
		IF (i_text) { stolbec++; DrawBuf.Skew(start_x, 0, line_length, list.line_h); }
		IF (s_text) DrawBuf.DrawBar(start_x, 4, line_length, 1, text_colors[text_color_index]);
		IF (u_text) DrawBuf.DrawBar(start_x, 8, line_length, 1, text_colors[text_color_index]);
		IF (link) {
			UnsafeDefineButton(start_x-2, start_y, line_length + 3, 9, blink + BT_HIDE, 0xB5BFC9);
			DrawBuf.DrawBar(start_x, 8, line_length, 1, text_colors[text_color_index]);
		}
		stolbec += strlen(#line);
	}
}
//=======================================================================



char *ABSOLUTE_LINKS[]={ "http:", "mailto:", "ftp:", "/sys/", "/kolibrios/", "/rd/", "/bd", "/hd", "/cd", "/tmp", "/usbhd", 0};
//dword TWebBrowser::GetNewUrl(dword CUR_URL, NEW_URL){
void TWebBrowser::GetNewUrl(){
	int i, len;
	
	for (i=0; ABSOLUTE_LINKS[i]; i++)
	{
		len=strlen(ABSOLUTE_LINKS[i]);
		if (!strcmpn(#URL, ABSOLUTE_LINKS[i], len)) return;
	}
		
	IF (!strcmpn(#URL,"./", 2)) strcpy(#URL, #URL+2); //������� :)
	strcpy(#editURL, BrowserHistory.CurrentUrl()); //������ ����� ������� ��������

	if (URL[0] == '/')
	{
		i = strchr(#editURL+8, '/');
		editURL[i+7]=0;
		strcpy(#URL, #URL+1);
	}
		
	_CUT_ST_LEVEL_MARK:
		
		if (editURL[strrchr(#editURL, '/')-2]<>'/')  // ���� �� http://
		{
			editURL[strrchr(#editURL, '/')] = 0x00; //�������� � ��� �� ���������� /
		}
		
		IF (!strcmp(get_URL_part(3),"../")) //�� ������� �����
		{
			strcpy(#URL,#URL+3);
			editURL[strrchr(#editURL, '/')-1] = 0x00; //�������� � ��� �� ���������� /
			goto _CUT_ST_LEVEL_MARK;
		}
		
		if (editURL[strlen(#editURL)-1]<>'/') strcat(#editURL, "/"); 
		
		strcat(#editURL, #URL); //����� ����� �����
		strcpy(#URL, #editURL);
}


	
void TWebBrowser::ReadHtml(byte encoding)
{
	if (!strcmp(get_URL_part(5),"http:"))) 
		file_size stdcall (#download_path);
	else
		file_size stdcall (#URL);
	
	filesize = EBX;
	if (!filesize) return;
	
	mem_Free(buf);
	buf = mem_Alloc(filesize);
	if (!strcmp(get_URL_part(5),"http:"))) 
		ReadFile(0, filesize, buf, #download_path);
	else
		ReadFile(0, filesize, buf, #URL);
		
	cur_encoding = encoding;
	if (encoding==_WIN) wintodos(buf);
	if (encoding==_UTF) utf8rutodos(buf);
	if (encoding==_KOI) koitodos(buf);
}


void TWebBrowser::ShowPage()
{
	address_box.size = address_box.pos = strlen(#editURL);
	address_box.offset=0;
	edit_box_draw stdcall(#address_box);

	if (!filesize)
	{
		DrawBar(list.x, list.y, list.w+scroll_wv.size_x+1, list.h, 0xFFFFFF); //fill all
		if (GetProcessSlot(downloader_id)<>0) WriteText(list.x + 10, list.y + 18, 0x80, 0, "Loading...");
		else
		{
			WriteText(list.x + 10, list.y + 18, 0x80, 0, "Page not found. May be, URL contains some errors.");
			if (!strcmp(get_URL_part(5),"http:"))) WriteText(list.x + 10, list.y + 32, 0x80, 0, "Or Internet unavilable for your configuration.");
		}
		//return;
	}
	else
		ParseHTML(buf);

	if (!header) strcpy(#header, #version);
	if (!strcmp(#version, #header)) DrawTitle(#header);
}



void TWebBrowser::ParseHTML(dword bword){
	word bukva[2];
	int j, perenos_num;
	byte ignor_param;
	char temp[768];
	
	if (blink<400) blink=400; else for ( ; blink>400; blink--;) DeleteButton(blink);
	b_text = i_text = u_text = s_text = blq_text = 
	li_text = link = ignor_text = text_color_index = text_colors[0] = li_tab = 
	condition_text_val = condition_text_active = 0; //�������� ����
	condition_max = 255;
	text_align = ALIGN_LEFT;
	link_color = 0x0000FF;
	bg_color = 0xFFFFFF;
	DrawBuf.Fill(bg_color);
	strcpy(#page_links,"|");
	strcpy(#header, #version);
	stroka = -list.first;
	stolbec = 0;
	line = 0;

	if (pre_text<>2)
	{
		pre_text=0;
		if (!strcmp(#URL + strlen(#URL) - 4, ".txt")) pre_text = 1;
		if (!strcmp(#URL + strlen(#URL) - 4, ".mht")) ignor_text = 1;
	}
	
	for ( ; buf+filesize > bword; bword++;)
	{
		bukva = ESBYTE[bword];
		if (ignor_text) && (bukva!='<') continue;
		if (condition_text_active) && (condition_text_val != condition_href) && (bukva!='<') continue;
		switch (bukva)
		{
		case 0x0a:
			if (pre_text)
			{
				bukva = temp = NULL;
				goto NEXT_MARK;
			}
		case '\9':
			if (pre_text) //����� ��� �� 0x0d	
			{
				tab_len=strlen(#line)/8;
				tab_len=tab_len*8;
				tab_len=8+tab_len-strlen(#line);
				for (j=0; j<tab_len; j++;) chrcat(#line,' ');
				break;
			}
			goto DEFAULT_MARK;		
		case '=': //quoted printable
			if (strcmp(#URL + strlen(#URL) - 4, ".mht")<>0) goto DEFAULT_MARK;

			temp[0] = ESBYTE[bword+1];
			temp[1] = ESBYTE[bword+2];
			temp[2] = '\0';
			if (bukva = Hex2Symb(#temp))
			{
				bword+=2;
				goto DEFAULT_MARK;
			}
			break;
			
		case '&': //&nbsp; and so on
			bword++;
			tag=0;
			for (j=0; (ESBYTE[bword]<>';') && (j<7);   j++, bword++;)
			{
				bukva = ESBYTE[bword];
				chrcat(#tag, bukva);
			}
			if (bukva = GetUnicodeSymbol()) goto DEFAULT_MARK;
			break;
		case '<':
			bword++; //��������� ������ <
			tag = parametr = tagparam = ignor_param = NULL;
			if (ESBYTE[bword] == '!') //���������� ������ <!-- -->, ������
			{
				bword++;
				if (ESBYTE[bword] == '-')
				{
				HH_:
					do
					{
						bword++;
						if (buf + filesize <= bword) break 2;
					}
					while (ESBYTE[bword] <>'-');
					
					bword++;
					if (ESBYTE[bword] <>'-') goto HH_;
				}
			}
			while (ESBYTE[bword] !='>') && (bword < buf + filesize) //�������� ��� � ��� ���������
			{
				bukva = ESBYTE[bword];
				if (bukva == '\9') || (bukva == '\x0a') || (bukva == '\x0d') bukva = ' ';
				if (!ignor_param) && (bukva <>' ')
				{
					if (strlen(#tag)<sizeof(tag)) strcat(#tag, #bukva);
				}
				else
				{
					ignor_param = true;
					if (!ignor_text) && (strlen(#tagparam)+1<sizeof(tagparam)) strcat(#tagparam, #bukva);
				}
				bword++;
			}
			strlwr(#tag);
			strlwr(#tagparam);

			if (condition_text_active) && (condition_text_val != condition_href) 
			{
				if (strcmp(#tag, "/condition")!=0) break;
			}
			if (tag[strlen(#tag)-1]=='/') tag[strlen(#tag)-1]=NULL; //for br/
			if (tagparam) && (strlen(#tagparam) < 4000) GetNextParam();

			if (stolbec + strlen(#line) > list.column_max) //============the same as NEXT_MARK
			{
				perenos_num = strrchr(#line, ' ');
				if (!perenos_num) && (strlen(#line)>list.column_max) perenos_num=list.column_max;
				strcpy(#temp, #line + perenos_num); //������� �� ������
				line[perenos_num] = 0x00;
				if (stroka-1 > list.visible) && (list.first <>0) break 1; //������...
				DrawPage();
				strcpy(#line, #temp);				
				TextGoDown(list.x + 5, stroka * 10 + list.y + 5, list.w - 20); //����������� �������� ������
			}
			DrawPage();
			line=NULL;

			if (tag) WhatTextStyle(list.x + 5, stroka * 10 + list.y + 5, list.w - 20); //��������� �����

			tag = parametr = tagparam = ignor_param = NULL;
			break;
		default:
			DEFAULT_MARK:
			if (bukva<=15) bukva=' ';
			if (!pre_text) && (bukva == ' ')
			{
				if (line[strlen(#line)-1]==' ') break; //������ 2 ������� ������
				if (!stolbec) && (!line) break; //������ �� ����� ���������� � �������
			}
			if (strlen(#line)<sizeof(line)) chrcat(#line, bukva);

			if (stolbec + strlen(#line) > list.column_max)
			{
			NEXT_MARK:
				perenos_num = strrchr(#line, ' ');
				if (!perenos_num) && (strlen(#line)>list.column_max) perenos_num=list.column_max;
				strcpy(#temp, #line + perenos_num); //������� �� ������
				line[perenos_num] = 0x00;
				if (stroka-1 > list.visible) && (list.first <>0) break 1; //������...
				DrawPage();
				strcpy(#line, #temp);			
				TextGoDown(list.x + 5, stroka * 10 + list.y + 5, list.w - 20); //����������� �������� ������
			}
		}
	}

	DrawPage(); //������ ��������� ������, ����� ��� ���� ������, ������������� ���
	TextGoDown(list.x + 5, stroka * 10 + list.y + 5, list.w - 20); //����������� �������� ������

	if (list.visible * 10 + 25 <= list.h)
		DrawBar(list.x, list.visible * 10 + list.y + 25, list.w, -list.visible * 10 + list.h - 25, bg_color);
	if (stroka * 10 + 5 <= list.h)
		DrawBar(list.x, stroka * 10 + list.y + 5, list.w, -stroka * 10 + list.h - 5, bg_color); //����������� �� �� �����
	if (list.first == 0) list.count = stroka;
	if (anchor) //���� ������� ������ �������� ����� ����� - ����� ����������� ����
	{
		anchor=NULL;
		list.first=anchor_line_num;
		ParseHTML(buf);
	}
	DrawScroller();
}



char oldtag[100];
void TWebBrowser::WhatTextStyle(int left1, top1, width1) {
	dword hr_color;

	//��������� ��� ����������� ��� �����������
	if (tag[0] == '/') 
	{
		 rez = 0;
		 strcpy(#tag, #tag+1);
	}
	else rez = 1;
		
	if (!chTag("html"))
	{
		IF(!strcmp(#URL + strlen(#URL) - 4, ".mht")) IF (rez==0) ignor_text = 1; ELSE ignor_text = 0;
		return;
	}

	if (!chTag("script")) || (!chTag("style")) || (!chTag("binary")) ignor_text = rez;

	if(!chTag("title"))
	{
		if (rez) header=NULL;
		else if (!stroka) DrawTitle(#header); //��� �������� - ������ ������
		return;
	}

	if (ignor_text) return;


	
	IF(!chTag("q")) chrcat(#line, '\"');

	if (anchor) && (!strcmp(#parametr, "id=")) //����� �����!!! ������ ��� ���� �� ��������� ���, �������� �� �����
	{
		if (!strcmp(#anchor, #options))	anchor_line_num=list.first+stroka;
	}
	
	if (!chTag("body"))
	{
		do{
			if (!strcmp(#parametr, "condition_max=")) { condition_max = atoi(#options); debugi(condition_max); }
			if (!strcmp(#parametr, "link=")) link_color = GetColor(#options);
			if (!strcmp(#parametr, "text=")) text_colors[0]=GetColor(#options);
			if (!strcmp(#parametr, "bgcolor="))
			{
				bg_color=GetColor(#options);
				DrawBuf.Fill(bg_color);
			}
		} while(GetNextParam());
		return;
	}

	if (!chTag("a"))
	{
		if (rez)
		{
			if (link) IF(text_color_index > 0) text_color_index--; //���� ���������� ��� � �� ��� ������

			do{
				if (!strcmp(#parametr, "href="))
				{
					if (stroka - 1 > list.visible) || (stroka < -2) return;
					
					text_color_index++;
					text_colors[text_color_index] = text_colors[text_color_index-1];
					
					link = 1;
					blink++;
					text_colors[text_color_index] = link_color;
					strcat(#page_links, #options);
					strcat(#page_links, "|");
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

	if (!chTag("font"))
	{
		if (rez)
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
	if(!chTag("tr")) || (!chTag("br")) {
		TextGoDown(left1, top1, width1);
		return;
	}
	if (!chTag("div")) {
		IF(oldtag[0] <>'h') TextGoDown(left1, top1, width1);
		return;
	}
	if (!chTag("p")) {
		IF(oldtag[0] == 'h') return;
		TextGoDown(left1, top1, width1);
		IF(rez) TextGoDown(left1, top1 + 10, width1);
		return;
	}
	/*
	if (!chTag("center"))
	{
		if (rez) text_align = ALIGN_CENTER;
		if (!rez)
		{
			TextGoDown(left1, top1, width1);
			text_align = ALIGN_LEFT;
		}
		return;
	}
	if (!chTag("right"))
	{
		if (rez) text_align = ALIGN_RIGHT;
		if (!rez)
		{
			TextGoDown(left1, top1, width1);
			text_align = ALIGN_LEFT;
		}
		return;
	}
	*/
	if (!chTag("h1")) || (!chTag("h2")) || (!chTag("h3")) || (!chTag("h4")) {
		TextGoDown(left1, top1, width1);
		if (rez) && (stroka>1) TextGoDown(left1, top1 + 10, width1);
		strcpy(#oldtag, #tag);
		if (rez)
		{
			if (!strcmp(#parametr, "align=")) && (!strcmp(#options,"center")) text_align = ALIGN_CENTER;
			if (!strcmp(#parametr, "align=")) && (!strcmp(#options,"right")) text_align = ALIGN_RIGHT;
			b_text = 1;
		}
		if (!rez)
		{
			text_align = ALIGN_LEFT;
			b_text = 0;
		}
		return;
	}
	else
		oldtag=NULL;
		
	if (!chTag("b")) || (!chTag("strong")) || (!chTag("big")) {
		b_text = rez;
		return;
	}
	if(!chTag("i")) || (!chTag("em")) || (!chTag("subtitle")) {
		i_text = rez;
		return;
	}	
	if (!chTag("dt"))
	{
		li_text = rez;
		IF(rez == 0) return;
		TextGoDown(left1, top1, width1);
		return;
	}
	if (!chTag("condition"))
	{
		condition_text_active = rez;
		if (rez) && (!strcmp(#parametr, "show_if=")) condition_text_val = atoi(#options);
		return;
	}
	if(!chTag("li")) || (!chTag("dt")) //���� ������� ��������� ������
	{
		li_text = rez;
		if (rez)
		{
			TextGoDown(left1, top1, width1);
			if (stroka > -1) && (stroka - 2 < list.visible) DrawBuf.DrawBar(li_tab * 5 * 6 + left1 - 5, list.line_h/2-3, 2, 2, 0x555555);
		}
		return;
	}
	if (!chTag("u")) || (!chTag("ins")) u_text = rez;
	if (!chTag("s")) || (!chTag("strike")) || (!chTag("del")) s_text = rez;
	if (!chTag("ul")) || (!chTag("ol")) IF(!rez)
	{
		li_text = rez;
		li_tab--;
		TextGoDown(left1, top1, width1);
	} ELSE li_tab++;
	if (!chTag("dd")) stolbec += 5;
	if (!chTag("blockquote")) blq_text = rez;
	if (!chTag("pre")) pre_text = rez; 
	if (!chTag("hr"))
	{
		if (anchor) || (stroka < -1)
		{
			stroka+=2;
			return;
		}
		if (strcmp(#parametr, "color=") == 0) hr_color = GetColor(#options); else hr_color = 0x999999;
		TextGoDown(left1, top1, width1);
		DrawBuf.DrawBar(5, list.line_h/2, list.w-10, 1, hr_color);
		TextGoDown(left1, top1+list.line_h, width1);
	}
	if (!chTag("img"))
	{
		ImgCache1.Images( left1, top1, width1);
		return;
	}
	if (!chTag("meta")) || (!chTag("?xml"))
	{
		do{
			if (!strcmp(#parametr, "charset=")) || (!strcmp(#parametr, "content=")) || (!strcmp(#parametr, "encoding="))
			{
				strcpy(#options, #options[strrchr(#options, '=')]); //����� � content=
				if (!strcmp(#options, "utf-8"))  || (!strcmp(#options,"utf8"))      ReadHtml(_UTF);
				if (!strcmp(#options, "koi8-r")) || (!strcmp(#options, "koi8-u"))   ReadHtml(_KOI);
				if (!strcmp(#options, "dos"))    || (!strcmp(#options, "cp-866"))   ReadHtml(_DOS);
			}
		} while(GetNextParam());
		return;
	}
}


void TWebBrowser::DrawScroller() //�� ����������� ���������, �� ���� � ����� �����
{
	scroll_wv.max_area = list.count;
	scroll_wv.cur_area = list.visible;
	scroll_wv.position = list.first;

	scroll_wv.all_redraw=1;
	scroll_wv.start_x = list.x + list.w;
	scroll_wv.size_y=list.h;

	scrollbar_v_draw(#scroll_wv);
}


void TWebBrowser::TextGoDown(int left1, top1, width1)
{
	if (!stroka) DrawBar(list.x, list.y, list.w, 5, bg_color); //����������� ��� ��� ������ �������
	if (top1>=list.y) && ( top1 < list.h+list.y-10)  && (!anchor)
	{
		if (text_align == ALIGN_CENTER) DrawBuf.AlignCenter(left1,top1,list.w,list.line_h,stolbec * 6);
		if (text_align == ALIGN_RIGHT) DrawBuf.AlignRight(left1,top1,list.w,list.line_h,stolbec * 6);
		PutPaletteImage(buf_data+8, list.w, list.line_h, left1-5, top1, 32,0);
		DrawBuf.Fill(bg_color);
	}
	stroka++;
	if (blq_text) stolbec = 8; else stolbec = 0;
	if (li_text) stolbec = li_tab * 5;
}