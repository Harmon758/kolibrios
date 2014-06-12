/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* @(#)w_sqrt.c 5.1 93/09/24 */
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
static char rcsid[] = "$Id: w_sqrt.c,v 1.4 1994/08/10 20:35:48 jtc Exp $";
#endif

/* 
 * wrapper sqrt(x)
 */

#include "math.h"
#include "math_private.h"

#ifdef __STDC__
	double sqrt(double x)		/* wrapper sqrt */
#else
	double sqrt(x)			/* wrapper sqrt */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_sqrt(x);
#else
	double z;
	z = __ieee754_sqrt(x);
	if(_LIB_VERSION == _IEEE_ || isnan(x)) return z;
	if(x<0.0) {
	    return __kernel_standard(x,x,26); /* sqrt(negative) */
	} else
	    return z;
#endif
}
