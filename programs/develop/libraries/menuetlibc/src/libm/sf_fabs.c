/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* s_fabsf.c -- float version of s_fabs.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

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
static char rcsid[] = "$Id: s_fabsf.c,v 1.2 1994/08/18 23:06:43 jtc Exp $";
#endif

/*
 * fabsf(x) returns the absolute value of x.
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	float fabsf(float x)
#else
	float fabsf(x)
	float x;
#endif
{
	u_int32_t ix;
	GET_FLOAT_WORD(ix,x);
	SET_FLOAT_WORD(x,ix&0x7fffffff);
        return x;
}
