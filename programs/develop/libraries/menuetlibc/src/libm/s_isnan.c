/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* @(#)s_isnan.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$Id: s_isnan.c,v 1.6 1994/08/18 23:06:54 jtc Exp $";
#endif

/*
 * isnan(x) returns 1 is x is nan, else 0;
 * no branching!
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	int isnan(double x)
#else
	int isnan(x)
	double x;
#endif
{
	int32_t hx,lx;
	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	hx |= (u_int32_t)(lx|(-lx))>>31;	
	hx = 0x7ff00000 - hx;
	return (int)((u_int32_t)(hx))>>31;
}
