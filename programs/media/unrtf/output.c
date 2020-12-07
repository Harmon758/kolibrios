
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
 * Module name:    output
 * Author name:    Zach Smith
 * Create date:    18 Sep 01
 * Purpose:        Generalized output module
 *----------------------------------------------------------------------
 * Changes:
 * 22 Sep 01, tuorfa@yahoo.com: addition of functions to change font size
 * 22 Sep 01, tuorfa@yahoo.com: added function-level comment blocks 
 *--------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include "malloc.h"
#include "defs.h"
#include "error.h"
#include "output.h"
#include "main.h"
#include "convert.h"


/*========================================================================
 * Name:	op_create
 * Purpose:	Creates a blank output personality.
 * Args:	None.
 * Returns:	Output personality struct.
 *=======================================================================*/

OutputPersonality*
op_create ()
{
	OutputPersonality* new_op;

	new_op = (OutputPersonality*) my_malloc (sizeof(OutputPersonality));
	if (!new_op)
		error_handler ("cannot allocate output personality");

	bzero ((void*) new_op, sizeof (OutputPersonality));
	return new_op;
}



/*========================================================================
 * Name:	op_free
 * Purpose:	Deallocates an output personality, but none of the strings
 *		it points to since they are usually constants.
 * Args:	OutputPersonality.
 * Returns:	None.
 *=======================================================================*/

void
op_free (OutputPersonality *op)
{
	CHECK_PARAM_NOT_NULL(op);

	my_free ((void*) op);
}




/*========================================================================
 * Name:	op_translate_char
 * Purpose:	Performs a translation of a character in the context of
 *		a given output personality.
 * Args:	OutputPersonality, character set#, character.
 * Returns:	String.
 *=======================================================================*/

char *
op_translate_char (OutputPersonality *op, int charset, int ch)
{
	short start;
	char *result=NULL;

	CHECK_PARAM_NOT_NULL(op);

	if (ch >= 0x20 && ch < 0x80) {
		result = op->ascii_translation_table [ch - 0x20];
	}
	else
	if (charset != CHARSET_ANSI &&
	    charset != CHARSET_MAC &&
	    charset != CHARSET_CP437 &&
	    charset != CHARSET_CP850)
		error_handler ("invalid character set value, cannot translate character");
	else
	switch (charset) {
	case CHARSET_ANSI:
		start = op->ansi_first_char;
		if (ch >= start &&
		    ch <= op->ansi_last_char)
			result = op->ansi_translation_table [ch-start];
		break;
	case CHARSET_MAC:
		start = op->mac_first_char;
		if (ch >= start &&
		    ch <= op->mac_last_char)
			result = op->mac_translation_table [ch-start];
		break;
	case CHARSET_CP437:
		start = op->cp437_first_char;
		if (ch >= start &&
		    ch <= op->cp437_last_char)
			result = op->cp437_translation_table [ch-start];
		break;
	case CHARSET_CP850:
		start = op->cp850_first_char;
		if (ch >= start &&
		    ch <= op->cp850_last_char)
			result = op->cp850_translation_table [ch-start];
		break;
	}
	return result;
}


/*========================================================================
 * Name:	op_begin_std_fontsize 
 * Purpose:	Prints whatever is necessary to perform a change in the
 *		current font size.
 * Args:	OutputPersonality, desired size.
 * Returns:	None.
 *=======================================================================*/

void
op_begin_std_fontsize (OutputPersonality *op, int size)
{
	int found_std_expr = FALSE;

	CHECK_PARAM_NOT_NULL(op);

	/* Look for an exact match with a standard point size.
	 */
	switch (size) {
	case 8:
		if (op->fontsize8_begin) {
			printf (op->fontsize8_begin);
			found_std_expr = TRUE;
		}
		break;
	case 10:
		if (op->fontsize10_begin) {
			printf (op->fontsize10_begin);
			found_std_expr = TRUE;
		}
		break;
	case 12:
		if (op->fontsize12_begin) {
			printf (op->fontsize12_begin);
			found_std_expr = TRUE;
		}
		break;
	case 14:
		if (op->fontsize14_begin) {
			printf (op->fontsize14_begin);
			found_std_expr = TRUE;
		}
		break;
	case 18:
		if (op->fontsize18_begin) {
			printf (op->fontsize18_begin);
			found_std_expr = TRUE;
		}
		break;
	case 24:
		if (op->fontsize24_begin) {
			printf (op->fontsize24_begin);
			found_std_expr = TRUE;
		}
		break;
	case 36:
		if (op->fontsize36_begin) {
			printf (op->fontsize36_begin);
			found_std_expr = TRUE;
		}
		break;
	case 48:
		if (op->fontsize48_begin) {
			printf (op->fontsize48_begin);
			found_std_expr = TRUE;
		}
		break;
	}

	/* If no exact match, try to write out a change to the
	 * exact point size.
	 */
	if (!found_std_expr) {
		if (op->fontsize_begin) {
			char expr[16];
			sprintf (expr, "%d", size);
			printf (op->fontsize_begin, expr);
		} else {
			/* If we cannot write out a change for the exact
			 * point size, we must approximate to a standard
			 * size.
			 */
			if (size<9 && op->fontsize8_begin) {
				printf (op->fontsize8_begin);
			} else 
			if (size<11 && op->fontsize10_begin) {
				printf (op->fontsize10_begin);
			} else 
			if (size<13 && op->fontsize12_begin) {
				printf (op->fontsize12_begin);
			} else 
			if (size<16 && op->fontsize14_begin) {
				printf (op->fontsize14_begin);
			} else 
			if (size<21 && op->fontsize18_begin) {
				printf (op->fontsize18_begin);
			} else 
			if (size<30 && op->fontsize24_begin) {
				printf (op->fontsize24_begin);
			} else 
			if (size<42 && op->fontsize36_begin) {
				printf (op->fontsize36_begin);
			} else 
			if (size>40 && op->fontsize48_begin) {
				printf (op->fontsize48_begin);
			} else 
			/* If we can't even produce a good approximation,
			 * just try to get a font size near 12 point.
			 */
			if (op->fontsize12_begin)
				printf (op->fontsize12_begin);
			else
			if (op->fontsize14_begin)
				printf (op->fontsize14_begin);
			else
			if (op->fontsize10_begin)
				printf (op->fontsize10_begin);
			else
			if (op->fontsize18_begin)
				printf (op->fontsize18_begin);
			else
			if (op->fontsize8_begin)
				printf (op->fontsize8_begin);
			else
				error_handler ("output personality lacks sufficient font size change capability");
		}
	}
}


/*========================================================================
 * Name:	op_end_std_fontsize 
 * Purpose:	Prints whatever is necessary to perform a change in the
 *		current font size.
 * Args:	OutputPersonality, desired size.
 * Returns:	None.
 *=======================================================================*/

void
op_end_std_fontsize (OutputPersonality *op, int size)
{
	int found_std_expr = FALSE;

	CHECK_PARAM_NOT_NULL(op);

	/* Look for an exact match with a standard point size.
	 */
	switch (size) {
	case 8:
		if (op->fontsize8_end) {
			printf (op->fontsize8_end);
			found_std_expr = TRUE;
		}
		break;
	case 10:
		if (op->fontsize10_end) {
			printf (op->fontsize10_end);
			found_std_expr = TRUE;
		}
		break;
	case 12:
		if (op->fontsize12_end) {
			printf (op->fontsize12_end);
			found_std_expr = TRUE;
		}
		break;
	case 14:
		if (op->fontsize14_end) {
			printf (op->fontsize14_end);
			found_std_expr = TRUE;
		}
		break;
	case 18:
		if (op->fontsize18_end) {
			printf (op->fontsize18_end);
			found_std_expr = TRUE;
		}
		break;
	case 24:
		if (op->fontsize24_end) {
			printf (op->fontsize24_end);
			found_std_expr = TRUE;
		}
		break;
	case 36:
		if (op->fontsize36_end) {
			printf (op->fontsize36_end);
			found_std_expr = TRUE;
		}
		break;
	case 48:
		if (op->fontsize48_end) {
			printf (op->fontsize48_end);
			found_std_expr = TRUE;
		}
		break;
	}

	/* If no exact match, try to write out a change to the
	 * exact point size.
	 */
	if (!found_std_expr) {
		if (op->fontsize_end) {
			char expr[16];
			sprintf (expr, "%d", size);
			printf (op->fontsize_end, expr);
		} else {
			/* If we cannot write out a change for the exact
			 * point size, we must approximate to a standard
			 * size.
			 */
			if (size<9 && op->fontsize8_end) {
				printf (op->fontsize8_end);
			} else 
			if (size<11 && op->fontsize10_end) {
				printf (op->fontsize10_end);
			} else 
			if (size<13 && op->fontsize12_end) {
				printf (op->fontsize12_end);
			} else 
			if (size<16 && op->fontsize14_end) {
				printf (op->fontsize14_end);
			} else 
			if (size<21 && op->fontsize18_end) {
				printf (op->fontsize18_end);
			} else 
			if (size<30 && op->fontsize24_end) {
				printf (op->fontsize24_end);
			} else 
			if (size<42 && op->fontsize36_end) {
				printf (op->fontsize36_end);
			} else 
			if (size>40 && op->fontsize48_end) {
				printf (op->fontsize48_end);
			} else 
			/* If we can't even produce a good approximation,
			 * just try to get a font size near 12 point.
			 */
			if (op->fontsize12_end)
				printf (op->fontsize12_end);
			else
			if (op->fontsize14_end)
				printf (op->fontsize14_end);
			else
			if (op->fontsize10_end)
				printf (op->fontsize10_end);
			else
			if (op->fontsize18_end)
				printf (op->fontsize18_end);
			else
			if (op->fontsize8_end)
				printf (op->fontsize8_end);
			else
				error_handler ("output personality lacks sufficient font size change capability");
		}
	}
}


