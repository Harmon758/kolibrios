#ifndef KOLIBRI_TREELIST_H
#define KOLIBRI_TREELIST_H

/*
el_focus dd tedit0
mouse_dd dd 0
tree1 tree_list 264,count_of_dir_list_files+2, tl_key_no_edit+tl_draw_par_line+tl_list_box_mode,\
    16,16, 0x8080ff,0x0000ff,0xffffff, 0,70,TED_PANEL_WIDTH-17,120, 0,0,0, el_focus,\
    ws_dir_lbox,0

tree3 tree_list MAX_COLOR_WORD_LEN,3,tl_key_no_edit,\
	16,16, 0x8080ff,0x0000ff,0xffffff, 5,30,300,160, 16, 0,0, el_focus, w_scr_t3,0

tree_file_struct:
  dd 1
  dd 0,0,count_of_dir_list_files
  dd dir_mem
  db 0
  dd file_name ;sys_path
*/
/*
;struct TreeList
;  type dw ? ;+ 0 ��� ��������, ��� ������ ������ ��� ����
;  lev db ?  ;+ 2 ������� ��������
;  clo db ?  ;+ 3 ���� ��������, ��� �������� (����� ����� ��� ������������� ����)
;  perv dd ? ;+ 4 ������ ����������� ��������
;  next dd ? ;+ 8 ������ ������������ ��������
;  tc dd ?   ;+12 ����. ��������
;  td dd ?   ;+16 ����. ��������
;ends

struc tree_list info_size,info_max_count,style, img_cx,img_cy,\
    col_bkg,col_zag,col_txt, box_l,box_t,box_w,box_h, capt_cy,info_capt_offs,\
    info_capt_len,el_focus, p_scroll,on_press {
.box_left    dd box_l
.box_top     dd box_t
.box_width   dd box_w
.box_height  dd box_h
.data_info   dd 0
.info_size   dw info_size
.info_max_count dd info_max_count
.style       dd style
.data_nodes  dd 0
.data_img    dd 0
.img_cx      dw img_cx
.img_cy      dw img_cy
.data_img_sys dd 0
.ch_tim      dd 0
.tim_undo    dd 0
.cur_pos     dd 0
.col_bkg     dd col_bkg
.col_zag     dd col_zag
.col_txt     dd col_txt
.capt_cy     dw capt_cy
.info_capt_offs dw info_capt_offs
.info_capt_len dw info_capt_len
.el_focus    dd el_focus
.p_scroll    dd p_scroll
.on_press    dd on_press
}
*/
// ��������� �����
enum tl_style {
    TL_KEY_NO_EDIT  = 1,    // ������� ������ ������������� �� ���������� (�������� ������, �������)
    TL_DRAW_PAR_LINE = 2,   // �������� ����� � ������������� ����
    TL_LISTBOX_MODE  = 4    //����� �� ���������� ������ (��� � ListBox ��� ������ ������)
};

typedef struct __attribute__ ((__packed__)) {
    uint32_t    left;
    uint32_t    top;
    uint32_t    width;
    uint32_t    height;
    void       *data_info;  // ��������� �� �������� �����
    uint16_t    info_size;  // ������ ������ ���������� ��� ������� ���� (����������������� ������ + ����� ��� �������)
    uint32_t    info_max_count; // ������������ ���������� �����, ������� ����� �������� � �������
    uint32_t    style;      // ����� ��������
    void       *data_nodes; // ��������� �� ��������� �����
    void       *data_img;   // ��������� �� ����������� � �������� �����
    uint16_t    img_cx;     // ������ ������
    uint16_t    img_cy;     // ������ ������
    void       *data_img_sys;//��������� �� ��������� ����������� (�������, �������)
    uint32_t    ch_tim;     // ���������� ��������� � �����
    uint32_t    tim_undo;   // ���������� ���������� ��������
    uint32_t    cur_pos;    // ������� �������
    color_t     col_bkg;    // ���� ����
    color_t     tl_col_zag; // ���� ���������
    color_t     tl_col_txt; // ���� ������
    uint16_t    capt_cy;    // ������ �������
    uint16_t    info_capt_offs;//����� ��� ������ ������ (������� ����)
    uint16_t    info_capt_len;//����� ������ ������� ���� (���� = 0 �� �� ����� ���������)
    void       *el_focus;   // ��������� �� ��������� �������� � ������
    void       *p_scroll;   // ��������� �� ��������� ����������
    void       *on_press;   // +84 ��������� �� �������, ������� ���������� ��� ������� Enter
} treelist;

/*
;��������� ��� �������
tl_err_save_memory_size equ  10b ;�� ������� ������ ��� ���������� ��������
tl_err_load_caption     equ   1b ;� ������ ��� ��������� 'tree'
tl_err_load_info_size   equ 100b ;�� ��������� ������ �������������� ��������� ��� ��������
tl_load_mode_add        equ 0x20000 ;����� ���������� � ������ ���������� ����������
tl_save_load_heder_size equ 26 ;������ ��������� ��� ������/������ ���������
*/

#endif //KOLIBRI_TREELIST_H
