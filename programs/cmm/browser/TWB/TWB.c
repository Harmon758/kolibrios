#include "TWB\colors.h"
#include "TWB\anchors.h"
#include "TWB\parse_tag.h"
#include "TWB\special.h"
#include "TWB\tag_list.h"
#define DEFAULT_BG_COL 0xffEBE8E9;
dword link_color_default;
dword link_color_active;
#include "TWB\links.h"

#define BODY_MARGIN 6
#define BASIC_CHAR_W 8
#define BASIC_LINE_H 18

struct STYLE {
	bool
	b, u, s, h,
	font,
	pre,
	blq,
	button,
	image;
	LIST tag_list;
	dword title;
	dword cur_line_h;
	void reset();
};

void STYLE::reset()
{
	b = u = s = h = blq = pre = title = false;
	font = false;
	cur_line_h = NULL;
	tag_list.reset();
}

struct TWebBrowser {
	llist list;
	STYLE style;
	dword draw_y, draw_x, draw_w, left_gap;
	dword o_bufpointer;
	int cur_encoding, custom_encoding;
	bool link, t_html, t_body;
	dword bufpointer;
	dword bufsize;
	dword is_html;
	collection img_url;
	char header[150];
	char line[500];
	char redirect[URL_SIZE];

	void SetStyle();
	void RenderTextbuf();
	void RenderLine();
	bool RenderImage();

	void SetPageDefaults();
	void ParseHtml();
	void NewLine();
	void ChangeEncoding();
	void AddCharToTheLine();
	void DrawPage();

	void tag_a();
	void tag_p();
	void tag_img();
	void tag_div();
	void tag_h1234_caption();
	void tag_ol_ul_dt();
	void tag_li();
	void tag_q();
	void tag_hr();
	void tag_code();
	void tag_meta_xml();
	void tag_body();
	void tag_iframe();
	void tag_title();
	void tag_font();
	void tag_table();
	void tag_td();
	void tag_tr();
};

#include "TWB\render.h"
#include "TWB\set_style.h"
//============================================================================================
void TWebBrowser::SetPageDefaults()
{
	t_html = t_body = link = false;
	style.reset();
	link_color_default = 0x0000FF;
	link_color_active = 0xFF0000;
	style.cur_line_h = list.item_h;
	links.clear();
	anchors.clear();
	img_url.drop();
	text_colors.drop();
	text_colors.add(0);
	bg_colors.drop();
	bg_colors.add(DEFAULT_BG_COL);
	canvas.Fill(0, DEFAULT_BG_COL);
	header = NULL;
	cur_encoding = CH_CP866;
	draw_y = BODY_MARGIN;
	draw_x = left_gap = BODY_MARGIN;
	draw_w = list.w - BODY_MARGIN;
	line = 0;
	redirect = '\0';
	//hold original buffer
	if (o_bufpointer) o_bufpointer=free(o_bufpointer);
	o_bufpointer = malloc(bufsize);
	memmov(o_bufpointer, bufpointer, bufsize);
	if (custom_encoding != -1) {
		cur_encoding = custom_encoding;
		bufpointer = ChangeCharset(cur_encoding, "CP866", bufpointer);
	}
	list.SetFont(8, 14, 10011000b);
}
//============================================================================================
void TWebBrowser::ParseHtml(dword _bufpointer, _bufsize){
	int tab_len;
	dword bufpos;
	bufsize = _bufsize;

	if (list.w!=canvas.bufw) canvas.Init(list.x, list.y, list.w, 400*20);

	if (bufpointer != _bufpointer) {
		bufpointer = malloc(bufsize);
		memmov(bufpointer, _bufpointer, bufsize);
	} else {
		custom_encoding = CH_CP866;	
	}
	SetPageDefaults();
	is_html = true;
	if (!strstri(bufpointer, "<body")) {
		t_body = true;
		if (!strstri(bufpointer, "<html")) && (!strstr(bufpointer, "<?xml")) && (!strstr(bufpointer, "<xml")) {
			style.pre = true; //show linebreaks for a plaint text
			is_html = false;
		}
	} 
	for (bufpos=bufpointer ; (bufpos < bufpointer+bufsize) && (ESBYTE[bufpos]!=0) ; bufpos++;)
	{
		switch (ESBYTE[bufpos])
		{
		case 0x0a:
			if (style.pre) {
				RenderTextbuf();
				NewLine();
			} else {
				goto _DEFAULT;
			}
			break;
		case 0x09:
			if (style.pre) {
				tab_len = draw_x - left_gap / list.font_w + strlen(#line) % 4;
				if (!tab_len) tab_len = 4; else tab_len = 4 - tab_len;
				while (tab_len) {chrcat(#line,' '); tab_len--;}
			} else {
				goto _DEFAULT;
			}
			break;
		case '&': //&nbsp; and so on
			bufpos = GetUnicodeSymbol(#line, sizeof(TWebBrowser.line), bufpos+1, bufpointer+bufsize);
			break;
		case '<':
			if (!is_html) goto _DEFAULT;
			if (!strchr("!/?abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", ESBYTE[bufpos+1])) goto _DEFAULT;
			bufpos++;
			if (tag.parse(#bufpos, bufpointer + bufsize)) {
				RenderTextbuf();
				$push cur_encoding
				SetStyle();
				$pop eax
				// The thing is that UTF if longer than other encodings.
				// So if encoding was changed from UTF to DOS than $bufpos position got wrong,
				// and we have to start parse from the very beginning
				if (EAX != cur_encoding) && (cur_encoding == CH_UTF8) {
					ParseHtml(bufpointer, bufsize);
					return;
				}
			}
			break;
		default:
			_DEFAULT:
			AddCharToTheLine(ESBYTE[bufpos]);
		}
	}
	RenderTextbuf();
	list.count = draw_y + style.cur_line_h;

	canvas.bufh = math.max(list.visible, list.count);
	buf_data = realloc(buf_data, canvas.bufh * canvas.bufw * 4 + 8);

	list.CheckDoesValuesOkey();
	anchors.current = NULL;
	custom_encoding = -1;
	if (!header) {
		strncpy(#header, #version, sizeof(TWebBrowser.header)-1);
		DrawTitle(#header);
	}
}
//============================================================================================
void TWebBrowser::AddCharToTheLine(unsigned char _char)
{
	dword line_len = strlen(#line);
	if (_char<=15) _char=' ';
	if (!style.pre) && (_char == ' ')
	{
		if (line[line_len-1]==' ') return; //no double spaces
		if (draw_x==left_gap) && (!line) return; //no paces at the beginning of the line
		if (link) && (line_len==0) return;
	}
	if (line_len < sizeof(TWebBrowser.line)) chrcat(#line+line_len, _char);
	if (line_len+1 * list.font_w + draw_x >= draw_w) RenderTextbuf();
}
//============================================================================================
void TWebBrowser::NewLine()
{
	static bool empty_line = true;

	if (draw_x==left_gap) && (draw_y==BODY_MARGIN) return;
	if (t_html) && (!t_body) return;
	
	if (draw_x == style.tag_list.level * 5 * list.font_w + left_gap) { 
		if (!empty_line) empty_line=true; else return;
	} else {
		empty_line = false;
	}

	draw_y += style.cur_line_h;
	style.cur_line_h = list.item_h;
	if (style.blq) draw_x = 6 * list.font_w; else draw_x = 0;
	draw_x += style.tag_list.level * 5 * list.font_w + left_gap;
}
//============================================================================================
void TWebBrowser::ChangeEncoding(int _new_encoding)
{
	if (cur_encoding == _new_encoding) return;
	cur_encoding = _new_encoding;
	bufpointer = ChangeCharset(cur_encoding, "CP866", bufpointer);
	if (header) {
		ChangeCharset(cur_encoding, "CP866", #header);
		DrawTitle(#header);
	}
}
//============================================================================================
scroll_bar scroll_wv = { 15,NULL,NULL,NULL,
  0,2,NULL,0,0,0xeeeeee,0xBBBbbb,0xeeeeee};

void TWebBrowser::DrawPage()
{
	if (list.w!=canvas.bufw) {
		ParseHtml(bufpointer, bufsize);
	}
	canvas.Show(list.first, list.h);

	scroll_wv.max_area = list.count;
	scroll_wv.cur_area = list.visible;
	scroll_wv.position = list.first;
	scroll_wv.all_redraw = 0;
	scroll_wv.start_x = list.x + list.w;
	scroll_wv.start_y = list.y;
	scroll_wv.size_y = list.h;

	if (list.count <= list.visible) {
		DrawBar(scroll_wv.start_x, scroll_wv.start_y, scroll_wv.size_x, 
		scroll_wv.size_y, bg_colors.get(0) & 0x00FFFFFF);
	} else {
		scrollbar_v_draw(#scroll_wv);		
	}
}