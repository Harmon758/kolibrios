
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
 * Module name:    parse
 * Author name:    Zach Smith
 * Create date:    01 Sep 00
 * Purpose:        Parsing of the RTF file into a structure of Word objects.
 *----------------------------------------------------------------------
 * Changes:
 * 15 Oct 00, tuorfa@yahoo.com: parse.c created with functions taken from word.c
 * 15 Oct 00, tuorfa@yahoo.com: backslash before newline is now \par
 * 08 Apr 01, tuorfa@yahoo.com: removed limit on word length
 * 03 Aug 01, tuorfa@yahoo.com: added input buffering
 * 19 Sep 01, tuorfa@yahoo.com: cleaned up read_word()
 * 22 Sep 01, tuorfa@yahoo.com: moved word_dump() to word.c
 * 22 Sep 01, tuorfa@yahoo.com: added function-level comment blocks 
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "defs.h"
#include "parse.h"
#include "malloc.h"
#include "main.h"
#include "error.h"
#include "word.h"
#include "hash.h"



/* local to getchar stuff */
static int ungot_char=-1;
static int ungot_char2=-1;
static int ungot_char3=-1;



/*========================================================================
 * Name:	my_unget_char
 * Purpose:	My own unget routine, handling up to 3 ungot characters.
 * Args:	Character.
 * Returns:	None.
 *=======================================================================*/

static void my_unget_char (int ch)
{
	if (ungot_char>=0 && ungot_char2>=0 && ungot_char3>=0) 
		error_handler ("more than 3 ungot chars");

	ungot_char3 = ungot_char2;
	ungot_char2 = ungot_char;
	ungot_char = ch;
}


static int last_returned_ch=0;


#define READ_BUF_LEN 2048
static int buffer_size = 0;
static char *read_buf = NULL;
static int read_buf_end = 0;
static int read_buf_index = 0;





/*========================================================================
 * Name:	my_getchar
 * Purpose:	Gets a character: either an ungot one, or a buffered one.
 * Args:	Input file.
 * Returns:	Character, or EOF.
 *=======================================================================*/

static int my_getchar (FILE* f)
{
	int ch;

	CHECK_PARAM_NOT_NULL(f);

	if (ungot_char>=0) {
		ch = ungot_char; 
		ungot_char=ungot_char2; 
		ungot_char2=ungot_char3;
		ungot_char3=-1;
		last_returned_ch = ch;
		return ch;
	}
	do {
		if (read_buf_index >= read_buf_end) {
			if (!read_buf) {
				buffer_size = READ_BUF_LEN;
				read_buf = my_malloc (buffer_size);
				if (!read_buf) {
					buffer_size /= 4;
					read_buf = my_malloc (buffer_size);
					if (!read_buf) 
						error_handler ("cannot allocate read buffer");
				}
			}
			read_buf_end = fread (read_buf, 1, buffer_size, f);
			read_buf_index = 0;
			if (!read_buf_end)
				return EOF;
		}
		ch = read_buf [read_buf_index++];

		if (ch=='\n') {
			lineno++;
			/* Convert \(newline) into \par here */
			if (last_returned_ch=='\\') {
				my_unget_char (' ');
				my_unget_char ('r');
				my_unget_char ('a');
				ch = 'p';
				break;
			}
		}
	} 
	while (ch=='\r' /* || ch=='\n' */ );

	if (ch=='\t') ch = ' ';

	last_returned_ch = ch;
	return ch;
}


/* local to read_word */
static char *input_str = NULL;
static unsigned long current_max_length = 1;



/*========================================================================
 * Name:	expand_word_buffer
 * Purpose:	Expands the buffer used to store an incoming word.
 *		This allows us to remove the limit on word length.
 * Args:	None.
 * Returns:	None.
 *=======================================================================*/

static int
expand_word_buffer ()
{
	char *new_ptr;
	unsigned long old_length;
	if (!input_str)
		error_handler ("no input buffer allocated");
	old_length = current_max_length;
	current_max_length *= 2;
	new_ptr = my_malloc (current_max_length);
	if (!new_ptr)
		error_handler ("out of memory while resizing buffer");
	
	memcpy (new_ptr, input_str, old_length);
	my_free (input_str);
	input_str = new_ptr;
	return TRUE;
}




/*========================================================================
 * Name:	read_word
 * Purpose:	The core of the parser, this reads a word.
 * Args:	Input file.
 * Returns:	Number of characters in the word, or zero.
 * Note:	The word buffer is static and local to this file.
 *=======================================================================*/

static int 
read_word (FILE *f) 
{
	int ch, ch2, ix=0;
	int have_whitespace=FALSE;
	int is_control_word=FALSE;
	int has_numeric_param=FALSE; /* if is_control_word==TRUE */
	int need_unget=FALSE;

	CHECK_PARAM_NOT_NULL(f);

	current_max_length = 10; /* XX */

	/* Get some storage for a word.
	 */
	input_str = my_malloc (current_max_length);
	if (!input_str)
		error_handler("cannot allocate word storage");

	do {
		ch = my_getchar(f);
	} 
	while (ch=='\n');

	if (ch==' ')
	{
		/* Compress multiple space chars down to one.
		 */
		while (ch == ' ') {
			ch = my_getchar(f);
			have_whitespace=TRUE;
		}
		if (have_whitespace) {
			my_unget_char (ch);
			input_str[0]=' '; 
			input_str[1]=0;
			return 1;
		}
	}

	switch(ch) 
	{
	case EOF: 
		return 0;

	case '\\':
		ch2 = my_getchar(f);

		/* Look for two-character command words.
		 */
		switch (ch2) 
		{
		case '\n':
			strcpy (input_str, "\\par");
			return 4;
		case '~':
		case '{':
		case '}':
		case '\\':
		case '_':
		case '-':
			input_str[0] = '\\';
			input_str[1] = ch2;
			input_str[2] = 0;
			return 2;
		case '\'':
			/* Preserve \'## expressions (hex char exprs) for later.
			 */
			input_str[0]='\\'; 
			input_str[1]='\'';
			ix=2;
			if(ix==current_max_length) {
				if (!expand_word_buffer ())
					error_handler("word too long");
			}
			ch = my_getchar(f);
			input_str[ix++]=ch;
			if(ix==current_max_length) {
				if (!expand_word_buffer ())
					error_handler("word too long");
			}
			ch = my_getchar(f);
			input_str[ix++]=ch;
			if(ix==current_max_length) {
				if (!expand_word_buffer ())
					error_handler("word too long");
			}
			input_str[ix]=0;
			return ix;
		}

		is_control_word=TRUE;
		ix=1;
		input_str[0]=ch;
		ch=ch2;
		break;

	case '\t':
		/* In RTF, a tab char is the same as \tab.
		 */
		strcpy (input_str, "\\tab");
		return 4;

	case '{':
	case '}':
	case ';':
		input_str[0]=ch; 
		input_str[1]=0;
		return 1;

	}

	while (ch!=EOF)
	{
		/* Several chars always ends a word, and we need to save them.
		 */
		if (ch=='\t' || ch=='{' || ch=='}' || ch=='\\') {
			need_unget=TRUE;
			break;
		}

		/* A newline always ends a command word; we don't save it. 
		 * A newline is ignored if this is not a command word.
		 */
		if (ch=='\n') { 
			if (is_control_word) 
				break;
			ch = my_getchar(f); 
			continue; 
		}

		/* A semicolon always ends a command word; we do save it. 
		 * A semicolon never ends a regular word.
		 */
		if (ch==';') {
			if (is_control_word) {
				need_unget=TRUE;
				break;
			}
		}

		/* In this parser, a space character terminates
		 * any word, and if it does not follow a command,
		 * then it is a word in itself.
		 */
		if (ch==' ') {
			if (!is_control_word)
				need_unget=TRUE;
			break;
		}

		/* Identify a control word's numeric parameter.
		 */
		if (is_control_word) {
			if (!has_numeric_param && (isdigit(ch) || ch=='-'))
				has_numeric_param = TRUE;
			else
			if (has_numeric_param && !isdigit(ch)) {
				if (ch!=' ')
					need_unget=TRUE;
				break;
			}
		}
		
		input_str[ix++] = ch;
		if (ix==current_max_length) {
			if (!expand_word_buffer ())
				error_handler("word too long");
		}
		ch = my_getchar (f);
	}

	if (need_unget)
		my_unget_char(ch);

	input_str[ix]=0;
	return ix;
}



/*========================================================================
 * Name:	word_read
 * Purpose:	This is the recursive metareader which pieces together the 
 *		structure of Word objects.
 * Args:	Input file.
 * Returns:	Tree of Word objects.
 *=======================================================================*/

Word *
word_read (FILE* f) {
	Word * prev_word = NULL;
	Word * first_word = NULL;
	Word * new_word = NULL; /* temp */

	CHECK_PARAM_NOT_NULL(f);

	do {
		if (!read_word(f)) {
			return first_word;
		}


		if (input_str[0] == '{') {
			/* Process subwords */

#if 0
printf ("processing subword...\n");
#endif

			/* Create a dummy word to point to a sublist */
			new_word = word_new (NULL);
			if (!new_word)
				error_handler ("cannot allocate word");

			/* Get the sublist */
			new_word->child = word_read (f);
			if (!new_word->hash_index && !new_word->child)
			{
				/* printf ("unable to read children!\n"); */
			}

		} else if (input_str[0] == '}') {
#if 0
printf ("returning from word_read.\n");
#endif
			return first_word;
		} else {
			new_word = word_new (input_str);
		}

		if (prev_word) prev_word->next = new_word;

		if (!first_word) first_word = new_word;

		prev_word = new_word;

		/* Free up the memory allocated by read_word.
		 */
		my_free (input_str);
		input_str = NULL;
	}
	while(1);

}


