
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
 * Module name:    malloc
 * Author name:    Zach Smith
 * Create date:    01 Aug 01
 * Purpose:        Memory management. Allows us to keep track of how
 *                 much memory is being used.
 *----------------------------------------------------------------------
 * Changes:
 * 14 Aug 01, tuorfa@yahoo.com: added Turbo C support.
 * 16 Aug 01, Lars Unger <l.unger@tu-bs.de>: added Amiga/GCC support.
 * 22 Sep 01, tuorfa@yahoo.com: added function-level comment blocks 
 * 28 Sep 01, tuorfa@yahoo.com: removed Turbo C support.
 *--------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>

#if AMIGA
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include "error.h"


static unsigned long count=0;



/*========================================================================
 * Name:	my_malloc
 * Purpose:	Internal version of malloc necessary for record keeping.
 * Args:	Amount.
 * Returns:	Pointer.
 *=======================================================================*/

char * 
my_malloc (unsigned long size) {
	char *ptr;

	ptr = malloc (size);
	if (ptr)
		count += size;

	return ptr;
}

/*========================================================================
 * Name:	my_free
 * Purpose:	Internal version of free necessary for record keeping.
 * Args:	Pointer.
 * Returns:	None.
 *=======================================================================*/

void 
my_free (char* ptr) {
	CHECK_PARAM_NOT_NULL(ptr);

	free (ptr);
}



/*========================================================================
 * Name:	total_malloced
 * Purpose:	Returns total amount of memory thus far allocated.
 * Args:	None.
 * Returns:	Amount.
 *=======================================================================*/

unsigned long 
total_malloced (void) {
	return count;
}



/*========================================================================
 * Name:	my_strdup
 * Purpose:	Internal version of strdup necessary for record keeping.
 * Args:	String.
 * Returns:	String.
 *=======================================================================*/

char *
my_strdup (char *src) {
	unsigned long len;
	char *ptr;

	CHECK_PARAM_NOT_NULL(src);

	len = strlen(src);
	ptr = my_malloc (len+1);
	if (!ptr)
		error_handler ("out of memory in strdup()");

	strcpy (ptr, src);
	return ptr;
}

