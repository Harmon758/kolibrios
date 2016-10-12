#ifndef KOLIBRI_BUF2D_H
#define KOLIBRI_BUF2D_H

/*ToDo
 * voxel function
 */

extern int kolibri_buf2d_init(void);

typedef struct __attribute__ ((__packed__)) {
	unsigned int *buf_pointer;
	uint16_t left;
	uint16_t top;
	unsigned int width;
	unsigned int height;
	unsigned int bgcolor;
	uint8_t color_bit;
}buf2d_struct;

enum BUF2D_ALGORITM_FILTR {
	SIERRA_LITE,
	FLOYD_STEINBERG,
	BURKERS,
	HEAVYIRON_MOD,
	ATKINSON
};

enum BUF2D_OPT_CROP {
	BUF2D_OPT_CROP_TOP = 1,
	BUF2D_OPT_CROP_LEFT = 2,
	BUF2D_OPT_CROP_BOTTOM = 4,
	BUF2D_OPT_CROP_RIGHT = 8
};

extern void (*buf2d_create_asm)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_curve_bezier_asm)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));

buf2d_struct* buf2d_create(uint16_t tlx, uint16_t tly, unsigned int sizex, unsigned int sizey, unsigned int font_bgcolor, uint8_t color_bit)
{
    buf2d_struct *new_buf2d_struct = (buf2d_struct *)malloc(sizeof(buf2d_struct));
    new_buf2d_struct -> left = tlx;
	new_buf2d_struct -> top = tly;
	new_buf2d_struct -> width = sizex;
	new_buf2d_struct -> height = sizey;
	new_buf2d_struct -> bgcolor = font_bgcolor;
	new_buf2d_struct -> color_bit = color_bit;
	buf2d_create_asm(new_buf2d_struct);
    return new_buf2d_struct;
}

void buf2d_curve_bezier(buf2d_struct *buf, unsigned int p0_x, unsigned int p0_y, unsigned int p1_x, unsigned int p1_y, unsigned int p2_x, unsigned int p2_y, unsigned int color)
{
	buf2d_curve_bezier_asm(buf, (p0_x<<16)+p0_y, (p1_x<<16)+p1_y, (p2_x<<16)+p2_y, color);
}

extern void (*buf2d_draw)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_clear)(buf2d_struct *, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_delete)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_rotate)(buf2d_struct *, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_resize)(buf2d_struct *, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_line)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_line_sm)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_rect_by_size)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_filled_rect_by_size)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_circle)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_img_hdiv2)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_img_wdiv2)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_conv_24_to_8)(buf2d_struct *, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_conv_24_to_32)(buf2d_struct *, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_bit_blt_transp)(buf2d_struct *, unsigned int, unsigned int, buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_bit_blt_alpha)(buf2d_struct *, unsigned int, unsigned int, buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_convert_text_matrix)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_draw_text)(buf2d_struct *, buf2d_struct *, const char *, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_crop_color)(buf2d_struct *, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_offset_h)(buf2d_struct *, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_flood_fill)(buf2d_struct *, unsigned int, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_set_pixel)(buf2d_struct *, unsigned int, unsigned int, unsigned int) __attribute__((__stdcall__));
extern unsigned int (*buf2d_get_pixel)(buf2d_struct *, unsigned int, unsigned int) __attribute__((__stdcall__));
extern void (*buf2d_flip_h)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_flip_v)(buf2d_struct *) __attribute__((__stdcall__));
extern void (*buf2d_filter_dither)(buf2d_struct *, unsigned int) __attribute__((__stdcall__));
#endif /* KOLIBRI_BUF2D_H */
