/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __dj_include_sys_timeb_h_
#define __dj_include_sys_timeb_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __dj_ENFORCE_ANSI_FREESTANDING

#ifndef __STRICT_ANSI__

#ifndef _POSIX_SOURCE

#include <sys/djtypes.h>

__DJ_time_t
#undef __DJ_time_t
#define __DJ_time_t

struct timeb
{
    time_t		time;		/* Seconds since the epoch	*/
    unsigned short	millitm;
    short		timezone;
    short		dstflag;
};

extern int	ftime(struct timeb *);

#endif /* !_POSIX_SOURCE */
#endif /* !__STRICT_ANSI__ */
#endif /* !__dj_ENFORCE_ANSI_FREESTANDING */

#ifndef __dj_ENFORCE_FUNCTION_CALLS
#endif /* !__dj_ENFORCE_FUNCTION_CALLS */

#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_sys_timeb_h_ */
