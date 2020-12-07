
/*=============================================================================
   GNU UnRTF, a command-line program to convert RTF documents to other formats.
   Copyright (C) 2000,2001 Zachary Thayer Smith

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   The author is reachable by electronic mail at tuorfa@yahoo.com.
=============================================================================*/


/*----------------------------------------------------------------------
 * Module name:    attr
 * Author name:    Zach Smith
 * Create date:    1 Aug 2001
 * Purpose:        Definitions for attribute stack module.
 *----------------------------------------------------------------------
 * Changes:
 * 01 Aug 01, tuorfa@yahoo.com: moved code over from convert.c
 * 06 Aug 01, tuorfa@yahoo.com: added several attributes
 * 18 Sep 01, tuorfa@yahoo.com: updates for AttrStack paradigm
 *--------------------------------------------------------------------*/

enum {
	ATTR_NONE=0,
	ATTR_BOLD, ATTR_ITALIC,

	ATTR_UNDERLINE, ATTR_DOUBLE_UL, ATTR_WORD_UL, 

	ATTR_THICK_UL, ATTR_WAVE_UL, 

	ATTR_DOT_UL, ATTR_DASH_UL, ATTR_DOT_DASH_UL, ATTR_2DOT_DASH_UL,

	ATTR_FONTSIZE, ATTR_STD_FONTSIZE,
	ATTR_FONTFACE,
	ATTR_FOREGROUND, ATTR_BACKGROUND,
	ATTR_CAPS,
	ATTR_SMALLCAPS,

	ATTR_SHADOW,
	ATTR_OUTLINE, 
	ATTR_EMBOSS, 
	ATTR_ENGRAVE, 

	ATTR_SUPER, ATTR_SUB, 
	ATTR_STRIKE, 
	ATTR_DBL_STRIKE, 

	ATTR_EXPAND,
	/* ATTR_CONDENSE */
};



extern void attr_push_core (int attr, char* param);

extern void attr_pop_core (int attr);

extern void attr_push(int attr, char* param);

extern void attrstack_push();
extern void attrstack_drop();
extern void attrstack_express_all();

extern int attr_pop(int attr);

extern int attr_read();

extern void attr_drop_all ();

extern void attr_pop_all();

extern void attr_pop_dump();



