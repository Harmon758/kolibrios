/*
 * Copyright 2008 Vincent Sanders <vince@simtec.co.uk>
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

#ifndef NETSURF_FB_FONT_FREETYPE_H
#define NETSURF_FB_FONT_FREETYPE_H

#include <ft2build.h>  
#include FT_FREETYPE_H 
#include FT_GLYPH_H

extern int ft_load_type;

FT_Glyph fb_getglyph(const plot_font_style_t *fstyle, uint32_t ucs4);


#define NETSURF_FB_FONT_SANS_SERIF "sans.ttf"
#define NETSURF_FB_FONT_SANS_SERIF_BOLD NULL
#define NETSURF_FB_FONT_SANS_SERIF_ITALIC NULL
#define NETSURF_FB_FONT_SANS_SERIF_ITALIC_BOLD NULL
#define NETSURF_FB_FONT_SERIF NULL
#define NETSURF_FB_FONT_SERIF_BOLD NULL
#define NETSURF_FB_FONT_MONOSPACE NULL
#define NETSURF_FB_FONT_MONOSPACE_BOLD NULL
#define NETSURF_FB_FONT_FANTASY NULL
#define NETSURF_FB_FONT_CURSIVE NULL

#endif /* NETSURF_FB_FONT_FREETYPE_H */
