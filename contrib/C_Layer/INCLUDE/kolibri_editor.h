#ifndef KOLIBRI_EDITOR_H
#define KOLIBRI_EDITOR_H

// (���������� �������� � ����� ��������� + 2)
#define TE_MAXCHARS 100002
// ������������ ������ ����� ����������
#define TE_MAX_SYNTAX_FILESIZE 410000
// buffer for copy|paste
#define TE_BUF_SIZE  4096
typedef void (*editor_callback)(void);

struct __attribute__ ((__packed__)) editor_symbol {
	uint8_t     c;      //  +0 ������
	uint8_t     col;    //  +1 ����
	struct editor_symbol   *prev;   //  +2
	struct editor_symbol   *next;   //  +6 ���������
	uint32_t    tc;     //  +10 ����. ��������
	uint32_t    td;     // +14 ����. ��������
};

/// ��������� ���������� ���������
typedef struct __attribute__ ((__packed__)) {
    uint32_t    x_pos;  //0
    uint32_t    y_pos;  //50
    uint32_t    width;  //440
    uint32_t    hight;  //150
    uint32_t    w_pane;      //30 ������ ������ � ����
    uint32_t    h_pane;      //25 ������ ������ � ����
    uint32_t    width_sym;  //9 ������ ������� (����������) � ����
    uint32_t    hight_sym;  //16 ������ ������� (����������) � ����
	uint8_t     drag_m;     // ��������� �� ����
	uint8_t     drag_k;     // ��������� �� ����������
	uint32_t    sel_x0;     // ��������� ���������
	uint32_t    sel_y0;
	uint32_t    sel_x1;
	uint32_t    sel_y1;
	uint32_t    seln_x0;    //�������������� ��������� ���������
	uint32_t    seln_y0;
	uint32_t    seln_x1;
	uint32_t    seln_y1;
	struct editor_symbol   *tex;    // text memory pointer
	struct editor_symbol   *tex_1;  // text first symbol pointer
	struct editor_symbol   *tex_end;// text end memory pointer
	uint32_t    cur_x;      //���������� x �������
	uint32_t    cur_y;      //���������� y �������
	uint32_t    max_chars;  // TE_MAXCHARS ;+86 ������������ ����� �������� � ����� ���������
	uint32_t    count_colors_text; // 1 ;+90 ���������� ������ ������
	uint32_t    count_key_words;   //+94 ����������� �������� ����
	color_t     color_cursor;   // 0x808080 ;+98 ���� �������
	color_t     color_wnd_capt; // 0x80 ;+102 ���� ����� ������ ����
	color_t     color_wnd_work; // 0x0 ;+106 ���� ���� ����
	color_t     color_wnd_bord; //0xd0d0d0 ;+110 ���� ������ �� �����
	color_t     color_select;   // 0x208080 ;+114 ���� ���������
	color_t     color_cur_text; // 0xff0000 ;+118 ���� ������� ��� ��������
	color_t     color_wnd_text; // 0xffff00 ;+122 ���� ������ � ����
	char       *syntax_file;    // ��������� �� ������ ����� ����������
	uint32_t    syntax_file_size;   // TE_MAX_SYNTAX_FILESIZE ;+130 ������������ ������ ����� ����������
	void       *text_colors;    // ��������� �� ������ ������ ������
	char       *help_text_f1;   // ��������� �� ����� ������� (�� ������� F1)
	int         help_id;	    // -1 ;+142 ������������� ��� �������
	void       *key_words_data; // ��������� �� ��������� �������� ���� TexColViv
	uint32_t    tim_ch;     // ���������� ��������� � �����
	uint32_t    tim_undo;   // ���������� ���������� ��������
	uint32_t    tim_ls;     // ����� ���������� ����������
	uint32_t    tim_co;     // ����� ��������� �������� ��������
	void       *el_focus;   // ��������� �� ���������� �������� � ������ pointer to pointer**
	uint8_t     err_save;   // ������ ���������� �����
	uint8_t     panel_id;   // ����� �������� ������
	uint8_t     key_new;    // ������, ������� ����� ���������� � ����������
	uint8_t     symbol_new_line; // ascii 20 ������ ���������� ������
	scrollbar  *scr_w;      // ������������ ��������� != NULL
	scrollbar  *scr_h;      // �������������� ��������� != NULL
	char       *arr_key_pos;// ��������� �� ������ ������� �������� ����
	char       *buffer;     // ��������� �� ����� �����������/������� != NULL, also size
	char       *buffer_find;// ��������� �� ����� ��� ������ (sized 302)
	uint8_t     cur_ins;    // 1 ����� ������ ������� (������� ��� ������)
	uint8_t     mode_color; // 1 ����� ��������� ���� ������ (0-����. 1-���.)
	uint8_t     mode_invis; // 0 ����� ������ ������������ ��������
	uint8_t     gp_opt;     // 0 ����� ������������ �������� ted_get_pos_by_cursor
	editor_callback fun_on_key_ctrl_o;  // ��������� �� ������� ���������� ��� ������� Ctrl+O (�������� �����), can be NULL
	editor_callback fun_on_key_ctrl_f;  // ... Ctrl+F (������/������� ������ ������)
	editor_callback fun_on_key_ctrl_n;  // ... Ctrl+N (�������� ������ ���������)
	editor_callback fun_on_key_ctrl_s;  // ... Ctrl+S
	uint32_t    buffer_size;// BUF_SIZE ������ ������ �����������/�������
	editor_callback fun_find_err;       // ��������� �� ������� ���������� ���� ����� ���������� ��������
	editor_callback fun_init_synt_err;  // unused ��������� �� ������� ���������� ��� ��������� �������� ����� ����������
	editor_callback fun_draw_panel_buttons; // ��������� �� ������� ��������� ������ � ��������
	editor_callback fun_draw_panel_find;    // ��������� �� ������� ��������� ������ ������
	editor_callback fun_draw_panel_syntax;  // ��������� �� ������� ��������� ������ ����������
	editor_callback fun_save_err;   // ��������� �� ������� ���������� ���� ���������� ����� ����������� ��������
	uint32_t    increase_size;  //200 ����� �������� �� ������� ����� ����������� ������ ��� ��������
	void       *ptr_free_symb;  // ������������ ������ �������� ��� ��������� ������� ������
	uint32_t    font_size;       // ;+250 ��������� ��� ������� ������ // binary OR mask for ECX SysFn4
} editor;

struct __attribute__ ((__packed__)) editor_color_item
{
    char        word[40];   // ����� ��� ���������
    uint32_t    f1_offset;  // �������� � ������� F1 ���������
	uint8_t     flags;      // ����� ������������ ��� ���������
	uint8_t     endc;       // ������ ����� ��������� (������������ ��� flags&4)
	uint8_t     escc;       // ������������ ������ (������������ ��� flags&4)  ascii(34) ������� ������������� \r \n \t
    uint8_t     color;      // ���� �� ������� ������
};

struct editor_syntax_file
{
    uint32_t    count_colors_text;  // ���-�� ������ ������ dd (text-color_wnd_text)/4
    uint32_t    count_key_words;    // ���-�� �������� ���� dd (f1-text)/48
    color_t     color_cursor;       // dd 0xf1fcd0
    color_t     color_wnd_capt;     // dd 0x080808
    color_t     color_wnd_work;     // dd 0x1C1C1C
    color_t     color_wnd_bord;     // dd 0xc0c0c0
    color_t     color_select;       // dd 0x3E3D32
    color_t     color_cur_text;     // dd 0x808080
    color_t     color_wnd_text[];   // ������� ������ �������� count_colors_text
    // editor_color_item[];         // ������� �������� ������������ ���� �������� count_key_words
};


static struct editor_syntax_file default_syntax = {
    10,     //count_colors_text
    1,      // count_key_words dd (f1-text)/48, minimum 1
    0xf1fcd0, //color_cursor dd 0xf1fcd0
    0x080808, //color_wnd_capt dd 0x080808
    0x1C1C1C, //color_wnd_work dd 0x1C1C1C
    0xc0c0c0, //color_wnd_bord dd 0xc0c0c0
    0x3E3D32, //color_select dd 0x3E3D32
    0x808080, //color_cur_text dd 0x808080
	{0xD0D0D0, 0xffff00, 0x00ff00, 0x00ffff, 0x808080, 0xff40ff, 0x4080ff, 0xff0000, 0x8080ff, 0x00ccff}
};
// next structure must follow syntax definition, at least has 1 element !!!
static struct editor_color_item  word1 = {
    "siemargl", 1, 0, 0, 1  // test word colored as 1st in table
};
// next structure preferably follow syntax definition, at least has 1 element !!!
static char f1_table[] = {
    "\0"
    "First\0"
    "Last\0"
};


extern void (*ted_draw)(editor *) __attribute__((__stdcall__));
extern void (*ted_init_scroll_bars)(editor *, int opt) __attribute__((__stdcall__));
/// opt bits = 1 - ������ ���� �����������, 2 - ���������� ������� ����, 4 - ���������� ������� ���������
extern void (*ted_init_syntax_file)(editor *) __attribute__((__stdcall__));
extern void (*ted_mouse)(editor *) __attribute__((__stdcall__));
extern void (*ted_text_add)(editor *, char *text, int textlen, int opt) __attribute__((__stdcall__));
/// add text to cursor pos
/// opt == ted_opt_ed_change_time, ted_opt_ed_move_cursor
///ted_opt_ed_move_cursor equ 1 ;������� ������ ����� ���������� ������
///ted_opt_ed_change_time equ 2 ;��������� ��������� ��� �������������� ������
extern void (*ted_but_select_word)(editor *) __attribute__((__stdcall__));
/// select word under cursor
extern void (*ted_but_copy)(editor *) __attribute__((__stdcall__));
extern void (*ted_but_paste)(editor *) __attribute__((__stdcall__));

extern void (*ted_but_find_next)(editor *) __attribute__((__stdcall__));
///move cursor to <ed_buffer_find>, calls ted_fun_find_err() if exist



extern void (*ted_but_sumb_upper_asm)(editor *) __attribute__((__stdcall__));
static inline void editor_selected_toupper(editor *ed)
{
    __asm__ __volatile__ (
             "push %%edi \n\t"
             "push %%esi \n\t":::);

    (*ted_but_sumb_upper_asm)(ed);

    __asm__ __volatile__ (
             "pop %%esi \n\t"
             "pop %%edi \n\t":::);
}

extern void (*ted_but_sumb_lover_asm)(editor *) __attribute__((__stdcall__));
static inline void editor_selected_tolower(editor *ed)
{
    __asm__ __volatile__ (
             "push %%edi \n\t"
             "push %%esi \n\t":::);

    (*ted_but_sumb_lover_asm)(ed);

    __asm__ __volatile__ (
             "pop %%esi \n\t"
             "pop %%edi \n\t":::);
}


extern void (*ted_but_convert_by_table_asm)(editor *, char* table) __attribute__((__stdcall__));
static inline void editor_convert_by_table(editor *ed, char* table)
{
    __asm__ __volatile__ (
             "push %%edi \n\t"
             "push %%esi \n\t":::);

    (*ted_but_convert_by_table_asm)(ed, table);

    __asm__ __volatile__ (
             "pop %%esi \n\t"
             "pop %%edi \n\t":::);
}

extern int (*ted_can_save_asm)(editor *) __attribute__((__stdcall__));
static inline int editor_can_save(editor *ed)
/// return 1 if need to be saved (has changes), 0 otherwise
{
    int ret;
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_can_save_asm)(ed);

    __asm__ __volatile__ (
             "pop %%edi \n\t":"=a"(ret)::);
    return ret;
}

extern void (*ted_clear_asm)(editor *, int) __attribute__((__stdcall__));
static inline void editor_clear(editor *ed, int all)
/// all==1 - clear all memory
{
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_clear_asm)(ed, all);

    __asm__ __volatile__ (
             "pop %%edi \n\t":::);
}

extern void (*ted_delete_asm)(editor *) __attribute__((__stdcall__));
static inline void editor_delete(editor *ed)
/// frees all memory (destroy)
{
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_delete_asm)(ed);

    __asm__ __volatile__ (
             "pop %%edi \n\t":::);
    free(ed->scr_w);
    free(ed->scr_h);
    free(ed->buffer);
    free(ed->buffer_find);
}

extern void (*ted_init_asm)(editor *) __attribute__((__stdcall__));
static inline void editor_init(editor *ed)
/// allocate memory
{
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_init_asm)(ed);

    __asm__ __volatile__ (
             "pop %%edi \n\t":::);
}

extern int (*ted_is_select)(editor *) __attribute__((__stdcall__));
static inline int editor_is_select(editor *ed)
/// return 1 if have selection
{
    int ret;
    __asm__ __volatile__ (
             "push %%ebx \n\t":::);

    (*ted_is_select)(ed);

    __asm__ __volatile__ (
             "pop %%ebx \n\t":"=a"(ret)::);
    return ret;
}

enum control_keys {
    KM_SHIFT = 0x00010000,
    KM_CTRL = 0x00020000,
    KM_ALT = 0x00040000,
    KM_NUMLOCK = 0x00080000
};

extern void (*ted_key_asm)(editor *, char* table, int control) __attribute__((__stdcall__));
static inline void editor_keyboard(editor *ed, char* table, enum control_keys control, int ch)
/// control is KM_SHIFT, KM_ALT, KM_CTRL, KM_NUMLOCK,
/// ch = GET_KEY
/// table = SF_SYSTEM_GET,SSF_KEYBOARD_LAYOUT
{
    __asm__ __volatile__ (
             "nop \n\t"::"a"(ch):);

    (*ted_key_asm)(ed, table, control);
}

extern void (*ted_open_file_asm)(editor *, struct fs_dirinfo*, char *fname) __attribute__((__stdcall__));
static inline int editor_openfile(editor *ed, char *fname, int *readbytes)
/// returns errcode as SysFn70
{
    int     ret;
    struct fs_dirinfo   di;
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_open_file_asm)(ed, &di, fname);

    __asm__ __volatile__ (
             "pop %%edi \n\t":"=b"(*readbytes), "=a"(ret)::);
    return ret;
}

extern void (*ted_save_file_asm)(editor *, struct fs_dirinfo*, char *fname) __attribute__((__stdcall__));
static inline int editor_savefile(editor *ed, char *fname)
/// returns errcode, calls fun_save_err() if exists
{
    struct fs_dirinfo   di;

    (*ted_save_file_asm)(ed, &di, fname);

    return ed->err_save;
}

extern void (*ted_but_cut)(editor *) __attribute__((__stdcall__));
static inline void editor_cut(editor *ed)
{
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_but_cut)(ed);

    __asm__ __volatile__ (
             "pop %%edi \n\t":::);
}

extern void (*ted_but_undo)(editor *) __attribute__((__stdcall__));
static inline void editor_undo(editor *ed)
{
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_but_undo)(ed);

    __asm__ __volatile__ (
             "pop %%edi \n\t":::);
}

extern void (*ted_but_redo)(editor *) __attribute__((__stdcall__));
static inline void editor_redo(editor *ed)
{
    __asm__ __volatile__ (
             "push %%edi \n\t":::);

    (*ted_but_redo)(ed);

    __asm__ __volatile__ (
             "pop %%edi \n\t":::);
}

extern void (*ted_but_reverse)(editor *) __attribute__((__stdcall__));
static inline void editor_reverse(editor *ed)
{
    __asm__ __volatile__ (
             "push %%edi \n\t"
             "push %%ebx\n\t":::);

    (*ted_but_reverse)(ed);

    __asm__ __volatile__ (
             "pop %%ebx \n\t"
             "pop %%edi \n\t":::);
}

extern void (*ted_text_colored_asm)() __attribute__((__stdcall__));
static inline void editor_text_colored(editor *ed)
{
    __asm__ __volatile__ (
             "nop \n\t"::"D"(ed):);

    (*ted_text_colored_asm)();
}

static inline
uint32_t get_control_keys(void)
{
    uint32_t ctrl;

    __asm__ __volatile__(
    "int $0x40 \n\t"
    :"=a"(ctrl)
    :"a"(66),"b"(3));

    return ctrl;
};

static inline
int get_keyboard_layout(int opt, char* buf)
/// 128 byte buffer
/// opt: 1 - normal, 2 - shifted, 3 - alted, or 9 - return language
{
    uint32_t lang;

    __asm__ __volatile__(
    "int $0x40 \n\t"
    :"=a"(lang)
    :"a"(26),"b"(2), "c"(opt), "d"(buf));

    return lang;
};


static void editor_key(editor* ed)
// callback for gui
{
    uint32_t control = get_control_keys();
    enum control_keys ed_ctrl = 0;
    int ly_opt = 1;
    if (control & 3) { ed_ctrl |= KM_SHIFT; ly_opt = 2; }
    if (control & 0xC) ed_ctrl |= KM_CTRL;
    if (control & 0x30){ ed_ctrl |= KM_ALT; ly_opt = 3; }
    if (control & 0x80) ed_ctrl |= KM_NUMLOCK;

    char conv_table[128];
    get_keyboard_layout(ly_opt, conv_table);

    editor_keyboard(ed, conv_table, ed_ctrl, get_key().val);
}

static inline void gui_add_editor(kolibri_window *wnd, editor* e)
{
    kolibri_window_add_element(wnd, KOLIBRI_EDITOR, e);
}

static inline editor* kolibri_new_editor(uint32_t x_w, uint32_t y_h, uint32_t font, uint32_t max_chars, editor **editor_interlock)
/// font - 0b10SSS 8x16 size multiply (SSS+1), 0xSSS - 6x9 multiply (SSS+1)

{
    editor *ed = (editor *)calloc(1, sizeof(editor));
    ed->x_pos = x_w >> 16;
    ed->width = x_w & 0xFFFF;
    ed->y_pos = y_h >> 16;
    ed->hight = y_h & 0xFFFF;

    // no panel, w_pane, h_pane == 0
    // font
    if (font == 0) font = 0x10;  // default 16 = 8x16
    int font_multipl = (font & 7) + 1;
    ed->font_size = font << 24;
    if (font & 0x10) // 8x16
    {
        ed->width_sym = 8 * font_multipl;
        ed->hight_sym = 16 * font_multipl;
    } else   // 6x9
    {
        ed->width_sym = 6 * font_multipl;
        ed->hight_sym = 9 * font_multipl;
    }
    // memory sizing for text & syntax
    ed->max_chars = max_chars;
    ed->increase_size = max_chars / 2;
    ed->syntax_file_size = sizeof (default_syntax);

/* // loaded auto from syntax
    ed->color_cursor = 0x808080;
    ed->color_wnd_capt = 0x80;
    ed->color_wnd_bord = 0xd0d0d0;
    ed->color_select = 0x208080;
    ed->color_cur_text = 0xff0000;
    ed->color_wnd_text = 0xffff00;
*/
    ed->symbol_new_line = 20;  // ascii(20)

    ed->scr_w = kolibri_new_scrollbar_def(X_Y(50, 16), X_Y(50, 300), 100, 30, 0);
    ed->scr_h = kolibri_new_scrollbar_def(X_Y(0, 150), X_Y(50, 16), 100, 30, 0);

    ed->buffer_size = TE_BUF_SIZE;
    ed->buffer = malloc(TE_BUF_SIZE);
    ed->buffer_find = malloc(TE_BUF_SIZE / 8);  //where to store text to search

    ed->cur_ins = 1; // insert mode default
    ed->mode_color = 1; // can select text
    ed->mode_invis = 1; // show nonprinted symbols

    // ??? saveregs ax,cx,di
    editor_init(ed);  // memory allocation, cleaning
    ed->syntax_file = (char*)&default_syntax;
    (*ted_init_syntax_file)(ed); // load colors and syntax highlight
    ed->help_text_f1 = f1_table; // override if not aligned immediately after syntax words
    ed->key_words_data = &word1;

    return ed;
}


#endif // KOLIBRI_EDITOR_H
