/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* e_gammaf_r.c -- float version of e_gamma_r.c.
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
static char rcsid[] = "$Id: e_gammaf_r.c,v 1.1 1994/08/10 20:30:54 jtc Exp $";
#endif

/* __ieee754_gammaf_r(x, signgamp)
 * Reentrant version of the logarithm of the Gamma function 
 * with user provide pointer for the sign of Gamma(x). 
 *
 * Method: See __ieee754_lgammaf_r
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	float __ieee754_gammaf_r(float x, int *signgamp)
#else
	float __ieee754_gammaf_r(x,signgamp)
	float x; int *signgamp;
#endif
{
	return __ieee754_lgammaf_r(x,signgamp);
}
