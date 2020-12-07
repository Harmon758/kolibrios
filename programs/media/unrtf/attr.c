
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
 * Create date:    01 Aug 01
 * Purpose:        Character attribute stack.
 *----------------------------------------------------------------------
 * Changes:
 * 01 Aug 01, tuorfa@yahoo.com: moved code over from convert.c
 * 06 Aug 01, tuorfa@yahoo.com: added several font attributes.
 * 18 Sep 01, tuorfa@yahoo.com: added AttrStack (stack of stacks) paradigm
 * 22 Sep 01, tuorfa@yahoo.com: added comment blocks
 *--------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "malloc.h"
#include "defs.h"
#include "error.h"
#include "attr.h"
#include "main.h"


extern void starting_body();
extern void starting_text();

extern int simulate_allcaps;
extern int simulate_smallcaps;


#define MAX_ATTRS (1000)



/* For each RTF text block (the text within braces) we must keep
 * an AttrStack which is a stack of attributes and their optional
 * parameter. Since RTF text blocks are nested, these make up a
 * stack of stacks. And, since RTF text blocks inherit attributes
 * from parent blocks, all new AttrStacks do the same from
 * their parent AttrStack.
 */
typedef struct _stack {
	unsigned char attr_stack [MAX_ATTRS];
	char *attr_stack_params [MAX_ATTRS]; 
	int tos;
	struct _stack *next;
}
AttrStack;

static AttrStack *stack_of_stacks = NULL;
static AttrStack *stack_of_stacks_top = NULL;




/*========================================================================
 * Name:	attr_express_begin
 * Purpose:	Print the HTML for beginning an attribute.
 * Args:	Attribute number, optional string parameter.
 * Returns:	None.
 *=======================================================================*/

void 
attr_express_begin (int attr, char* param) {
	switch(attr) 
	{
	case ATTR_BOLD: 
		printf (op->bold_begin); 
		break;
	case ATTR_ITALIC: 
		printf (op->italic_begin); 
		break;

	/* Various underlines, they all resolve to HTML's <u> */
	case ATTR_THICK_UL:
	case ATTR_WAVE_UL:
	case ATTR_DASH_UL:
	case ATTR_DOT_UL: 
	case ATTR_DOT_DASH_UL:
	case ATTR_2DOT_DASH_UL:
	case ATTR_WORD_UL: 
	case ATTR_UNDERLINE: 
		printf (op->underline_begin); 
		break;

	case ATTR_DOUBLE_UL: 
		printf (op->dbl_underline_begin); 
		break;

	case ATTR_FONTSIZE: 
		op_begin_std_fontsize (op, atoi (param));
		break;

	case ATTR_FONTFACE: 
		printf (op->font_begin,param); 
		break;

	case ATTR_FOREGROUND: 
		printf (op->foreground_begin, param); 
		break;

	case ATTR_BACKGROUND: 
		if (!simple_mode)
		  printf (op->foreground_begin,param); 
		break;

	case ATTR_SUPER: 
		printf (op->superscript_begin); 
		break;
	case ATTR_SUB: 
		printf (op->subscript_begin); 
		break;

	case ATTR_STRIKE: 
		printf (op->strikethru_begin); 
		break;

	case ATTR_DBL_STRIKE: 
		printf (op->dbl_strikethru_begin); 
		break;

	case ATTR_EXPAND: 
		printf (op->expand_begin, param); 
		break;

	case ATTR_OUTLINE: 
		printf (op->outline_begin); 
		break;
	case ATTR_SHADOW: 
		printf (op->shadow_begin); 
		break;
	case ATTR_EMBOSS: 
		printf (op->emboss_begin); 
		break;
	case ATTR_ENGRAVE: 
		printf (op->engrave_begin); 
		break;

	case ATTR_CAPS:
		if (op->simulate_all_caps)
			simulate_allcaps = TRUE;
		break;

	case ATTR_SMALLCAPS: 
		if (op->simulate_small_caps)
			simulate_smallcaps = TRUE;
		else {
			if (op->small_caps_begin)
				printf (op->small_caps_begin); 
		}
		break;
	}
}


/*========================================================================
 * Name:	attr_express_end
 * Purpose:	Print HTML to complete an attribute.
 * Args:	Attribute number.
 * Returns:	None.
 *=======================================================================*/

void 
attr_express_end (int attr, char *param)
{
	switch(attr) 
	{
	case ATTR_BOLD: 
		printf (op->bold_end); 
		break;
	case ATTR_ITALIC: 
		printf (op->italic_end); 
		break;

	/* Various underlines, they all resolve to HTML's </u> */
	case ATTR_THICK_UL:
	case ATTR_WAVE_UL:
	case ATTR_DASH_UL:
	case ATTR_DOT_UL: 
	case ATTR_DOT_DASH_UL:
	case ATTR_2DOT_DASH_UL: 
	case ATTR_WORD_UL: 
	case ATTR_UNDERLINE: 
		printf (op->underline_end); 
		break;

	case ATTR_DOUBLE_UL: 
		printf (op->dbl_underline_end); 
		break;

	case ATTR_FONTSIZE: 
		op_end_std_fontsize (op, atoi (param));
		break;

	case ATTR_FONTFACE: 
		printf (op->font_end); 
		break;

	case ATTR_FOREGROUND: 
		printf (op->foreground_end); 
		break;
	case ATTR_BACKGROUND: 
		if (!simple_mode)
		  printf (op->background_end);
		break;

	case ATTR_SUPER: 
		printf (op->superscript_end); 
		break;
	case ATTR_SUB: 
		printf (op->subscript_end); 
		break;

	case ATTR_STRIKE: 
		printf (op->strikethru_end); 
		break;

	case ATTR_DBL_STRIKE: 
		printf (op->dbl_strikethru_end); 
		break;

	case ATTR_OUTLINE: 
		printf (op->outline_end); 
		break;
	case ATTR_SHADOW: 
		printf (op->shadow_end); 
		break;
	case ATTR_EMBOSS: 
		printf (op->emboss_end); 
		break;
	case ATTR_ENGRAVE: 
		printf (op->engrave_end); 
		break;

	case ATTR_EXPAND: 
		printf (op->expand_end); 
		break;

	case ATTR_CAPS:
		if (op->simulate_all_caps)
			simulate_allcaps = FALSE;
		break;

	case ATTR_SMALLCAPS: 
		if (op->simulate_small_caps)
			simulate_smallcaps = FALSE;
		else {
			if (op->small_caps_end)
				printf (op->small_caps_end); 
		}
		break;
	}
}



/*========================================================================
 * Name:	attr_push
 * Purpose:	Pushes an attribute onto the current attribute stack.
 * Args:	Attribute number, optional string parameter.
 * Returns:	None.
 *=======================================================================*/

void 
attr_push(int attr, char* param) 
{
	AttrStack *stack = stack_of_stacks_top;
	if (!stack) {
		warning_handler ("no stack to push attribute onto");
		return;
	}

	if (stack->tos>=MAX_ATTRS) { fprintf (stderr,"Too many attributes!\n"); return; }

	/* Make sure it's understood we're in the <body> section. */
	/* KLUDGE */
	starting_body();
	starting_text();

	++stack->tos;
	stack->attr_stack [stack->tos]=attr;
	if (param) 
		stack->attr_stack_params [stack->tos]=my_strdup(param);
	else
		stack->attr_stack_params [stack->tos]=NULL;

	attr_express_begin (attr, param);
}


/*========================================================================
 * Name:	attrstack_copy_all
 * Purpose:	Routine to copy all attributes from one stack to another.
 * Args:	Two stacks.
 * Returns:	None.
 *=======================================================================*/

void 
attrstack_copy_all (AttrStack *src, AttrStack *dest) 
{
	int i;
	int total;

	CHECK_PARAM_NOT_NULL(src);
	CHECK_PARAM_NOT_NULL(dest);

	total = src->tos + 1;

	for (i=0; i<total; i++)
	{
		int attr=src->attr_stack [i];
		char *param=src->attr_stack_params [i];

		dest->attr_stack[i] = attr;
		if (param)
			dest->attr_stack_params[i] = my_strdup (param);
		else
			dest->attr_stack_params[i] = NULL;
	}

	dest->tos = src->tos;
}

/*========================================================================
 * Name:	attrstack_unexpress_all
 * Purpose:	Routine to un-express all attributes heretofore applied,
 * 		without removing any from the stack.
 * Args:	Stack whost contents should be unexpressed.
 * Returns:	None.
 * Notes:	This is needed by attrstack_push, but also for \cell, which
 * 		often occurs within a brace group, yet HTML uses <td></td> 
 *		which clear attribute info within that block.
 *=======================================================================*/

void 
attrstack_unexpress_all (AttrStack *stack)
{
	int i;

	CHECK_PARAM_NOT_NULL(stack);

	i=stack->tos;
	while (i>=0)
	{
		int attr=stack->attr_stack [i];
		char *param=stack->attr_stack_params [i];

		attr_express_end (attr, param);
		i--;
	}
}


/*========================================================================
 * Name:	attrstack_push
 * Purpose:	Creates a new attribute stack, pushes it onto the stack
 *		of stacks, performs inheritance from previous stack.
 * Args:	None.
 * Returns:	None.
 *=======================================================================*/
void 
attrstack_push () 
{
	AttrStack *new_stack;
	AttrStack *prev_stack;

	new_stack = (AttrStack*) my_malloc (sizeof (AttrStack));
	bzero ((void*) new_stack, sizeof (AttrStack));

	prev_stack = stack_of_stacks_top;

	if (!stack_of_stacks) {
		stack_of_stacks = new_stack;
	} else {
		stack_of_stacks_top->next = new_stack;
	}
	stack_of_stacks_top = new_stack;
	new_stack->tos = -1;

	if (prev_stack) {
		attrstack_unexpress_all (prev_stack);
		attrstack_copy_all (prev_stack, new_stack);
		attrstack_express_all ();
	}
}



/*========================================================================
 * Name:	attr_pop 
 * Purpose:	Removes and undoes the effect of the top attribute of
 *		the current AttrStack.
 * Args:	The top attribute's number, for verification.
 * Returns:	Success/fail flag.
 *=======================================================================*/

int 
attr_pop (int attr) 
{
	AttrStack *stack = stack_of_stacks_top;

	if (!stack) {
		warning_handler ("no stack to pop attribute from");
		return FALSE;
	}

	if(stack->tos>=0 && stack->attr_stack[stack->tos]==attr)
	{
		char *param = stack->attr_stack_params [stack->tos];

		attr_express_end (attr, param);

		if (param) my_free(param);

		stack->tos--;

		return TRUE;
	}
	else
		return FALSE;
}



/*========================================================================
 * Name:	attr_read
 * Purpose:	Reads but leaves in place the top attribute of the top
 * 		attribute stack.
 * Args:	None.
 * Returns:	Attribute number.
 *=======================================================================*/

int 
attr_read() {
	AttrStack *stack = stack_of_stacks_top;
	if (!stack) {
		warning_handler ("no stack to read attribute from");
		return FALSE;
	}

	if(stack->tos>=0)
	{
		int attr = stack->attr_stack [stack->tos];
		return attr;
	}
	else
		return ATTR_NONE;
}


/*========================================================================
 * Name:	attr_drop_all 
 * Purpose:	Undoes all attributes that an AttrStack contains.
 * Args:	None.
 * Returns:	None.
 *=======================================================================*/

void 
attr_drop_all () 
{
	AttrStack *stack = stack_of_stacks_top;
	if (!stack) {
		warning_handler ("no stack to drop all attributes from");
		return;
	}

	while (stack->tos>=0) 
	{
		char *param=stack->attr_stack_params [stack->tos];
		if (param) my_free(param);
		stack->tos--;
	}
}


/*========================================================================
 * Name:	attrstack_drop
 * Purpose:	Removes the top AttrStack from the stack of stacks, undoing
 *		all attributes that it had in it.
 * Args:	None.
 * Returns:	None.
 *=======================================================================*/

void 
attrstack_drop () 
{
	AttrStack *stack = stack_of_stacks_top;
	AttrStack *prev_stack;
	if (!stack) {
		warning_handler ("no attr-stack to drop");
		return;
	}

	attr_pop_all ();

	prev_stack = stack_of_stacks;
	while(prev_stack && prev_stack->next && prev_stack->next != stack)
		prev_stack = prev_stack->next;

	if (prev_stack) {
		stack_of_stacks_top = prev_stack;
		prev_stack->next = NULL;
	} else {
		stack_of_stacks_top = NULL;
		stack_of_stacks = NULL;
	}
	my_free ((void*) stack);

	attrstack_express_all ();
}

/*========================================================================
 * Name:	attr_pop_all
 * Purpose:	Routine to undo all attributes heretofore applied, 
 *		also reversing the order in which they were applied.
 * Args:	None.
 * Returns:	None.
 *=======================================================================*/

void 
attr_pop_all() 
{
	AttrStack *stack = stack_of_stacks_top;
	if (!stack) {
		warning_handler ("no stack to pop from");
		return;
	}

	while (stack->tos>=0) {
		int attr=stack->attr_stack [stack->tos];
		char *param=stack->attr_stack_params [stack->tos];
		attr_express_end (attr,param);
		if (param) my_free(param);
		stack->tos--;
	}
}


/*========================================================================
 * Name:	attrstack_express_all
 * Purpose:	Routine to re-express all attributes heretofore applied.
 * Args:	None.
 * Returns:	None.
 * Notes:	This is needed by attrstack_push, but also for \cell, which
 * 		often occurs within a brace group, yet HTML uses <td></td> 
 *		which clear attribute info within that block.
 *=======================================================================*/

void 
attrstack_express_all() {
	AttrStack *stack = stack_of_stacks_top;
	int i;

	if (!stack) {
		warning_handler ("no stack to pop from");
		return;
	}

	i=0;
	while (i<=stack->tos) 
	{
		int attr=stack->attr_stack [i];
		char *param=stack->attr_stack_params [i];
		attr_express_begin (attr, param);
		i++;
	}
}


/*========================================================================
 * Name:	attr_pop_dump
 * Purpose:	Routine to un-express all attributes heretofore applied.
 * Args:	None.
 * Returns:	None.
 * Notes:	This is needed for \cell, which often occurs within a 
 *		brace group, yet HTML uses <td></td> which clear attribute 
 *		info within that block.
 *=======================================================================*/

void 
attr_pop_dump() {
	AttrStack *stack = stack_of_stacks_top;
	int i;

	if (!stack) return;

	i=stack->tos;
	while (i>=0) 
	{
		int attr=stack->attr_stack [i];
		attr_pop (attr);
		i--;
	}
}

