/* Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details */
/* w_lgammaf_r.c -- float version of w_lgamma_r.c.
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
static char rcsid[] = "$Id: w_lgammaf_r.c,v 1.1 1994/08/10 20:35:05 jtc Exp $";
#endif

/* 
 * wrapper float lgammaf_r(float x, int *signgamp)
 */

#include "math.h"
#include "math_private.h"


#ifdef __STDC__
	float lgammaf_r(float x, int *signgamp) /* wrapper lgammaf_r */
#else
	float lgammaf_r(x,signgamp)              /* wrapper lgammaf_r */
        float x; int *signgamp;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_lgammaf_r(x,signgamp);
#else
        float y;
        y = __ieee754_lgammaf_r(x,signgamp);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!finitef(y)&&finitef(x)) {
            if(floorf(x)==x&&x<=(float)0.0)
	        /* lgamma pole */
                return (float)__kernel_standard((double)x,(double)x,115);
            else
	        /* lgamma overflow */
	        return (float)__kernel_standard((double)x,(double)x,114);
        } else
            return y;
#endif
}             
