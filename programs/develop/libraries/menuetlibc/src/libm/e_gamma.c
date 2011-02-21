/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* @(#)e_gamma.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

#if defined(LIBM_SCCS) && !defined(lint)
static char rcsid[] = "$Id: e_gamma.c,v 1.4 1994/08/10 20:30:51 jtc Exp $";
#endif

/* __ieee754_gamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call __ieee754_gamma_r
 */

#include "math.h"
#include "math_private.h"

extern int signgam;

#ifdef __STDC__
	double __ieee754_gamma(double x)
#else
	double __ieee754_gamma(x)
	double x;
#endif
{
	return __ieee754_gamma_r(x,&signgam);
}
