/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* @(#)er_gamma.c 5.1 93/09/24 */
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
static char rcsid[] = "$Id: e_gamma_r.c,v 1.4 1994/08/10 20:30:52 jtc Exp $";
#endif

/* __ieee754_gamma_r(x, signgamp)
 * Reentrant version of the logarithm of the Gamma function 
 * with user provide pointer for the sign of Gamma(x). 
 *
 * Method: See __ieee754_lgamma_r
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	double __ieee754_gamma_r(double x, int *signgamp)
#else
	double __ieee754_gamma_r(x,signgamp)
	double x; int *signgamp;
#endif
{
	return __ieee754_lgamma_r(x,signgamp);
}
