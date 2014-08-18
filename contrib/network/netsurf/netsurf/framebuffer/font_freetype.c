/*
 * Copyright 2005 James Bursa <bursa@users.sourceforge.net>
 *           2008 Vincent Sanders <vince@simtec.co.uk>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <inttypes.h>
#include <assert.h>

#include <ft2build.h>
#include FT_CACHE_H

#include "css/css.h"
#include "css/utils.h"
#include "render/font.h"
#include "utils/filepath.h"
#include "utils/utf8.h"
#include "utils/log.h"
#include "desktop/options.h"

#include "framebuffer/gui.h"
#include "framebuffer/font.h"
#include "framebuffer/findfile.h"
#include "font_freetype.h"

/* glyph cache minimum size */
#define CACHE_MIN_SIZE (100 * 1024)

#define BOLD_WEIGHT 700

#ifdef DBG
#undef DBG
#endif
//#define DBG(s) __menuet__debug_out(s) /* For the debug messages in BOARD */
#define DBG(s) LOG((s))            /* So that we see debug in Netsurf's LOG files */

static FT_Library library; 
static FTC_Manager ft_cmanager;
static FTC_CMapCache ft_cmap_cache ;
static FTC_ImageCache ft_image_cache;

int ft_load_type;

/* cache manager faceID data to create freetype faceid on demand */
typedef struct fb_faceid_s {
        char *fontfile; /* path to font */
        int index; /* index of font */
        int cidx; /* character map index for unicode */
} fb_faceid_t;


enum fb_face_e {
	FB_FACE_SANS_SERIF = 0,
	FB_FACE_SANS_SERIF_BOLD = 1,
	FB_FACE_SANS_SERIF_ITALIC = 2,
	FB_FACE_SANS_SERIF_ITALIC_BOLD = 3,
	FB_FACE_SERIF = 4,
	FB_FACE_SERIF_BOLD = 5,
	FB_FACE_MONOSPACE = 6,
	FB_FACE_MONOSPACE_BOLD = 7,
	FB_FACE_CURSIVE = 8,
	FB_FACE_FANTASY = 9,
};

/* defines for accesing the faces */
#define FB_FACE_DEFAULT 0
#define FB_FACE_COUNT 10

static fb_faceid_t *fb_faces[FB_FACE_COUNT];


utf8_convert_ret utf8_to_local_encoding(const char *string, 
				       size_t len,
				       char **result)
{
	return utf8_to_enc(string, "UTF-8", len, result);
}

utf8_convert_ret utf8_from_local_encoding(const char *string,
					size_t len,
					char **result)
{
	*result = malloc(len + 1);
	if (*result == NULL) {
		return UTF8_CONVERT_NOMEM;
	}

	memcpy(*result, string, len);

	(*result)[len] = '\0';

	return UTF8_CONVERT_OK;
}

/* map cache manager handle to face id */
static FT_Error ft_face_requester(FTC_FaceID face_id, FT_Library  library, FT_Pointer request_data, FT_Face *face )
{
        FT_Error error;
        fb_faceid_t *fb_face = (fb_faceid_t *)face_id;
        int cidx;

        error = FT_New_Face(library, fb_face->fontfile, fb_face->index, face); 
        if (error) {
                LOG(("Could not find font (code %d)\n", error));
        } else {

                error = FT_Select_Charmap(*face, FT_ENCODING_UNICODE);
                if (error) {
                        LOG(("Could not select charmap (code %d)\n", error));
                } else {
                        for (cidx = 0; cidx < (*face)->num_charmaps; cidx++) {
                                if ((*face)->charmap == (*face)->charmaps[cidx]) {
                                        fb_face->cidx = cidx;
                                        break;
                                }
                        }
                }
        }
        LOG(("Loaded face from %s\n", fb_face->fontfile));

        return error;
}

/* create new framebuffer face and cause it to be loaded to check its ok */
static fb_faceid_t *
fb_new_face(const char *option, const char *resname, const char *fontname)
{
        fb_faceid_t *newf;
        FT_Error error;
        FT_Face aface;
	char buf[PATH_MAX];

        newf = calloc(1, sizeof(fb_faceid_t));

        if (option != NULL) {
                newf->fontfile = strdup(option);
        } else {
		filepath_sfind(respaths, buf, fontname);
                newf->fontfile = strdup(buf);
        }

        error = FTC_Manager_LookupFace(ft_cmanager, (FTC_FaceID)newf, &aface);
        if (error) {
                LOG(("Could not find font face %s (code %d)\n", fontname, error));
                free(newf);
                newf = NULL;
        }

        return newf;
}

#include <menuet/os.h>

/* initialise font handling */
bool fb_font_init(void)
{
        FT_Error error;
        FT_ULong max_cache_size;
        FT_UInt max_faces = 6;
	fb_faceid_t *fb_face;

LOG(("Freetype init..."));

	nsoptions.fb_font_monochrome = false;			
	nsoptions.fb_font_cachesize = 2048;			
	nsoptions.fb_face_sans_serif = NULL;			
	nsoptions.fb_face_sans_serif_bold = NULL;		
	nsoptions.fb_face_sans_serif_italic = NULL;		
	nsoptions.fb_face_sans_serif_italic_bold = NULL;		
	nsoptions.fb_face_serif = NULL;				
	nsoptions.fb_face_serif_bold = NULL;			
	nsoptions.fb_face_monospace = NULL;			
	nsoptions.fb_face_monospace_bold = NULL;			
	nsoptions.fb_face_cursive = NULL;			
	nsoptions.fb_face_fantasy = NULL;		


        /* freetype library initialise */
        error = FT_Init_FreeType( &library ); 
        if (error) {
                LOG(("Freetype could not initialised (code %d)\n", error));
                return false;
        }

        /* set the Glyph cache size up */
        max_cache_size = nsoption_int(fb_font_cachesize) * 1024; 

	if (max_cache_size < CACHE_MIN_SIZE) {
		max_cache_size = CACHE_MIN_SIZE;
	}

LOG(("Freetype cache..."));
DBG("Ft cache\n");
        /* cache manager initialise */
        error = FTC_Manager_New(library, 
                                max_faces, 
                                0, 
                                max_cache_size, 
                                ft_face_requester, 
                                NULL, 
                                &ft_cmanager);
        if (error) {
                LOG(("Freetype could not initialise cache manager (code %d)\n", error));
                FT_Done_FreeType(library);
                return false;
        }


LOG(("Freetype map cache..."));
DBG("Ft map cache\n");
        error = FTC_CMapCache_New(ft_cmanager, &ft_cmap_cache);

        error = FTC_ImageCache_New(ft_cmanager, &ft_image_cache);

	/* need to obtain the generic font faces */


LOG(("Freetype load fonts..."));
DBG("Ft load fonts\n");

	/* Start with the sans serif font */
	fb_face = fb_new_face(nsoption_charp(fb_face_sans_serif),
			      "sans_serif.ttf",
			      NETSURF_FB_FONT_SANS_SERIF);
	if (fb_face == NULL) {
		
LOG(("Freetype load fonts failed due SANS unavailable :(..."));
DBG("Ft Z:(((\n");
		/* The sans serif font is the default and must be found. */
                LOG(("Could not find the default font\n"));
                FTC_Manager_Done(ft_cmanager);
                FT_Done_FreeType(library);
                return false;
        } else {
		fb_faces[FB_FACE_SANS_SERIF] = fb_face;
	}

LOG(("Freetype loaded sans.."));
DBG("Ft sans loaded:)\n");

	/* Bold sans serif face */
	fb_face = fb_new_face(nsoption_charp(fb_face_sans_serif_bold),
                            "sans_serif_bold.ttf",
                            NETSURF_FB_FONT_SANS_SERIF_BOLD);
	if (fb_face == NULL) {
		/* seperate bold face unavailabe use the normal weight version */
		fb_faces[FB_FACE_SANS_SERIF_BOLD] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_SANS_SERIF_BOLD] = fb_face;
	}

	/* Italic sans serif face */
	fb_face = fb_new_face(nsoption_charp(fb_face_sans_serif_italic),
			      "sans_serif_italic.ttf",
			      NETSURF_FB_FONT_SANS_SERIF_ITALIC);
	if (fb_face == NULL) {
		/* seperate italic face unavailabe use the normal weight version */
		fb_faces[FB_FACE_SANS_SERIF_ITALIC] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_SANS_SERIF_ITALIC] = fb_face;
	}

	/* Bold italic sans serif face */
	fb_face = fb_new_face(nsoption_charp(fb_face_sans_serif_italic_bold), 
			      "sans_serif_italic_bold.ttf",
			      NETSURF_FB_FONT_SANS_SERIF_ITALIC_BOLD);
	if (fb_face == NULL) {
		/* seperate italic face unavailabe use the normal weight version */
		fb_faces[FB_FACE_SANS_SERIF_ITALIC_BOLD] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_SANS_SERIF_ITALIC_BOLD] = fb_face;
	}

	/* serif face */
	fb_face = fb_new_face(nsoption_charp(fb_face_serif),
                            "serif.ttf",
			      NETSURF_FB_FONT_SERIF);
	if (fb_face == NULL) {
		/* serif face unavailabe use the default */
		fb_faces[FB_FACE_SERIF] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_SERIF] = fb_face;
	}

	/* bold serif face*/
	fb_face = fb_new_face(nsoption_charp(fb_face_serif_bold),
			      "serif_bold.ttf",
			      NETSURF_FB_FONT_SERIF_BOLD);
	if (fb_face == NULL) {
		/* bold serif face unavailabe use the normal weight */
		fb_faces[FB_FACE_SERIF_BOLD] = fb_faces[FB_FACE_SERIF];
	} else {
		fb_faces[FB_FACE_SERIF_BOLD] = fb_face;
	}


	/* monospace face */
	fb_face = fb_new_face(nsoption_charp(fb_face_monospace),
			      "monospace.ttf",
			      NETSURF_FB_FONT_MONOSPACE);
	if (fb_face == NULL) {
		/* serif face unavailabe use the default */
		fb_faces[FB_FACE_MONOSPACE] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_MONOSPACE] = fb_face;
	}

	/* bold monospace face*/
	fb_face = fb_new_face(nsoption_charp(fb_face_monospace_bold),
			      "monospace_bold.ttf",
			      NETSURF_FB_FONT_MONOSPACE_BOLD);
	if (fb_face == NULL) {
		/* bold serif face unavailabe use the normal weight */
		fb_faces[FB_FACE_MONOSPACE_BOLD] = fb_faces[FB_FACE_MONOSPACE];
	} else {
		fb_faces[FB_FACE_MONOSPACE_BOLD] = fb_face;
	}

	/* cursive face */
	fb_face = fb_new_face(nsoption_charp(fb_face_cursive),
			      "cursive.ttf",
			      NETSURF_FB_FONT_CURSIVE);
	if (fb_face == NULL) {
		/* cursive face unavailabe use the default */
		fb_faces[FB_FACE_CURSIVE] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_CURSIVE] = fb_face;
	}

	/* fantasy face */
	fb_face = fb_new_face(nsoption_charp(fb_face_fantasy),
			      "fantasy.ttf",
			      NETSURF_FB_FONT_FANTASY);
	if (fb_face == NULL) {
		/* fantasy face unavailabe use the default */
		fb_faces[FB_FACE_FANTASY] = fb_faces[FB_FACE_SANS_SERIF];
	} else {
		fb_faces[FB_FACE_FANTASY] = fb_face;
	}

LOG(("Freetype fonts ready..."));
DBG("Ft ready :)\n");
        
        /* set the default render mode */
        if (nsoption_bool(fb_font_monochrome) == true)
                ft_load_type = FT_LOAD_MONOCHROME; /* faster but less pretty */
        else
                ft_load_type = 0;
        
        return true;
}

bool fb_font_finalise(void)
{
        FTC_Manager_Done(ft_cmanager );
        FT_Done_FreeType(library);
        return true;
}

static void fb_fill_scalar(const plot_font_style_t *fstyle, FTC_Scaler srec)
{
        int selected_face = FB_FACE_DEFAULT;

	switch (fstyle->family) {
                                
	case PLOT_FONT_FAMILY_SERIF:
		if (fstyle->weight >= BOLD_WEIGHT) {
                        selected_face = FB_FACE_SERIF_BOLD;
		} else {
                        selected_face = FB_FACE_SERIF;
                }
		break;

	case PLOT_FONT_FAMILY_MONOSPACE:
		if (fstyle->weight >= BOLD_WEIGHT) {
			selected_face = FB_FACE_MONOSPACE_BOLD;
		} else {
			selected_face = FB_FACE_MONOSPACE;
                }
		break;

	case PLOT_FONT_FAMILY_CURSIVE:
                selected_face = FB_FACE_CURSIVE;
		break;

	case PLOT_FONT_FAMILY_FANTASY:
                selected_face = FB_FACE_FANTASY;
		break;

	case PLOT_FONT_FAMILY_SANS_SERIF:
	default:
		if ((fstyle->flags & FONTF_ITALIC) || 
		    (fstyle->flags & FONTF_OBLIQUE)) {
			if (fstyle->weight >= BOLD_WEIGHT) {
                                selected_face = FB_FACE_SANS_SERIF_ITALIC_BOLD;
			} else {
                                selected_face = FB_FACE_SANS_SERIF_ITALIC;
			}
		} else {
			if (fstyle->weight >= BOLD_WEIGHT) {
                                selected_face = FB_FACE_SANS_SERIF_BOLD;
                        } else {
                                selected_face = FB_FACE_SANS_SERIF;
			}
                }
	}

        srec->face_id = (FTC_FaceID)fb_faces[selected_face];

	srec->width = srec->height = (fstyle->size * 64) / FONT_SIZE_SCALE;
	srec->pixel = 0;

	srec->x_res = srec->y_res = FIXTOINT(nscss_screen_dpi);
}

FT_Glyph fb_getglyph(const plot_font_style_t *fstyle, uint32_t ucs4)
{
        FT_UInt glyph_index;
        FTC_ScalerRec srec;
        FT_Glyph glyph;
        FT_Error error;
        fb_faceid_t *fb_face; 

        fb_fill_scalar(fstyle, &srec);

        fb_face = (fb_faceid_t *)srec.face_id;

        glyph_index = FTC_CMapCache_Lookup(ft_cmap_cache, srec.face_id,
			fb_face->cidx, ucs4);

        error = FTC_ImageCache_LookupScaler(ft_image_cache, 
                                            &srec, 
                                            FT_LOAD_RENDER | 
                                            FT_LOAD_FORCE_AUTOHINT | 
                                            ft_load_type, 
                                            glyph_index, 
                                            &glyph, 
                                            NULL);
	if (error != 0)
		return NULL;

        return glyph;
}


/**
 * Measure the width of a string.
 *
 * \param  fstyle  style for this text
 * \param  string  UTF-8 string to measure
 * \param  length  length of string
 * \param  width   updated to width of string[0..length)
 * \return  true on success, false on error and error reported
 */
static bool nsfont_width(const plot_font_style_t *fstyle,
                         const char *string, size_t length,
                         int *width)
{
        uint32_t ucs4;
        size_t nxtchr = 0;
        FT_Glyph glyph;

        *width = 0;
        while (nxtchr < length) {
                ucs4 = utf8_to_ucs4(string + nxtchr, length - nxtchr);
                nxtchr = utf8_next(string, length, nxtchr);

                glyph = fb_getglyph(fstyle, ucs4);
                if (glyph == NULL)
                        continue;

                *width += glyph->advance.x >> 16;
        }

	return true;
}

/**
 * Find the position in a string where an x coordinate falls.
 *
 * \param  fstyle       style for this text
 * \param  string       UTF-8 string to measure
 * \param  length       length of string
 * \param  x            x coordinate to search for
 * \param  char_offset  updated to offset in string of actual_x, [0..length]
 * \param  actual_x     updated to x coordinate of character closest to x
 * \return  true on success, false on error and error reported
 */

static bool nsfont_position_in_string(const plot_font_style_t *fstyle,
		const char *string, size_t length,
		int x, size_t *char_offset, int *actual_x)
{
        uint32_t ucs4;
        size_t nxtchr = 0;
        FT_Glyph glyph;
        int prev_x = 0;

        *actual_x = 0;
        while (nxtchr < length) {
                ucs4 = utf8_to_ucs4(string + nxtchr, length - nxtchr);

                glyph = fb_getglyph(fstyle, ucs4);
                if (glyph == NULL)
                        continue;

                *actual_x += glyph->advance.x >> 16;
                if (*actual_x > x)
                        break;

                prev_x = *actual_x;
                nxtchr = utf8_next(string, length, nxtchr);
        }

        /* choose nearest of previous and last x */
        if (abs(*actual_x - x) > abs(prev_x - x))
                *actual_x = prev_x;

        *char_offset = nxtchr;
	return true;
}


/**
 * Find where to split a string to make it fit a width.
 *
 * \param  fstyle       style for this text
 * \param  string       UTF-8 string to measure
 * \param  length       length of string
 * \param  x            width available
 * \param  char_offset  updated to offset in string of actual_x, [0..length]
 * \param  actual_x     updated to x coordinate of character closest to x
 * \return  true on success, false on error and error reported
 *
 * On exit, [char_offset == 0 ||
 *           string[char_offset] == ' ' ||
 *           char_offset == length]
 */

static bool nsfont_split(const plot_font_style_t *fstyle,
		const char *string, size_t length,
		int x, size_t *char_offset, int *actual_x)
{
        uint32_t ucs4;
        size_t nxtchr = 0;
        int last_space_x = 0;
        int last_space_idx = 0;
        FT_Glyph glyph;

        *actual_x = 0;
        while (nxtchr < length) {
                ucs4 = utf8_to_ucs4(string + nxtchr, length - nxtchr);

                glyph = fb_getglyph(fstyle, ucs4);
                if (glyph == NULL)
                        continue;

                if (ucs4 == 0x20) {
                        last_space_x = *actual_x;
                        last_space_idx = nxtchr;
                }

                *actual_x += glyph->advance.x >> 16;
                if (*actual_x > x) {
                        /* string has exceeded available width return previous
                         * space 
                         */
                        *actual_x = last_space_x;
                        *char_offset = last_space_idx;
                        return true;
                }

                nxtchr = utf8_next(string, length, nxtchr);
        }

        *char_offset = nxtchr;

	return true;
}

const struct font_functions nsfont = {
	nsfont_width,
	nsfont_position_in_string,
	nsfont_split
};

/*
 * Local Variables:
 * c-basic-offset:8
 * End:
 */
