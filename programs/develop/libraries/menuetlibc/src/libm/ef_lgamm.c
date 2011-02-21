/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* e_lgammaf.c -- float version of e_lgamma.c.
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
static char rcsid[] = "$Id: e_lgammaf.c,v 1.1 1994/08/10 20:31:08 jtc Exp $";
#endif

/* __ieee754_lgammaf(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_lgammaf_r
 */

#include "math.h"
#include "math_private.h"

extern int signgam;

#ifdef __STDC__
	float __ieee754_lgammaf(float x)
#else
	float __ieee754_lgammaf(x)
	float x;
#endif
{
	return __ieee754_lgammaf_r(x,&signgam);
}
