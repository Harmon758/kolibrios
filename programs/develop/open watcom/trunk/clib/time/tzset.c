/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Platform independent tzset() implementation.
*
****************************************************************************/

#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "rtdata.h"
#include "timedata.h"
#include "exitwmsg.h"
#include "_time.h"

/*
 * TZSET - sets the values of the variables 'timezone', 'daylight'
 *         and 'tzname' according to the setting of the environment
 *         variable "TZ". The "TZ" variable has the format
 *          :characters
 *              or
 *          <std><offset>[<dst>[<offset>]][,<start>[/<time>],<end>[/<time>]]]
 */

//#define TZNAME_MAX    128     /* defined in <limits.h> */

struct tm       __start_dst = { /* start of daylight savings */
    0, 0, 2,                    /* M4.1.0/02:00:00 default */
    1, 3, 0,                    /* i.e., 1st Sunday of Apr */
    0, 0, 0
};

struct tm       __end_dst = {   /* end of daylight savings */
    0, 0, 1,                    /* M10.5.0/02:00:00 default */
    5, 9, 0,                    /* i.e., last Sunday of Oct */
    0, 0, 0                     /* note that this is specified*/
};                              /* in terms of EST */
/* i.e. 02:00 EDT == 01:00 EST*/

static char     stzone[TZNAME_MAX + 1] = "EST";       /* place to store names */
static char     dtzone[TZNAME_MAX + 1] = "EDT";       /* place to store names */

_WCRTLINKD char *tzname[2] = { stzone, dtzone };

_WCRTLINKD long timezone = 5L * 60L * 60L;      /* seconds from GMT */
_WCRTLINKD int  daylight = 1;                   /* d.s.t. indicator */
int             __dst_adjust = 60 * 60;         /* daylight adjustment */

static struct {
    unsigned    cache_OS_TZ : 1;
    unsigned    have_OS_TZ : 1;
}               tzFlag = { 1, 0 };

int __DontCacheOSTZ( void )
/*************************/
{
    int old_flag;

    old_flag           = tzFlag.cache_OS_TZ;
    tzFlag.cache_OS_TZ = 0;
    tzFlag.have_OS_TZ  = 0;
    return( old_flag );
}

int __CacheOSTZ( void )
/*********************/
{
    int old_flag;

    old_flag           = tzFlag.cache_OS_TZ;
    tzFlag.cache_OS_TZ = 1;
    tzFlag.have_OS_TZ  = 0;
    return( old_flag );
}

static char *parse_time( char *tz, int *val )
/*******************************************/
{
    int value;

    value = 0;
    while( *tz >= '0' && *tz <= '9' ) {
        value = value * 10 + *tz - '0';
        ++tz;
    }
    *val = value;
    return( tz );
}

static char *parse_offset( char *tz, char *name, long *offset )
/*************************************************************/
{
    int         hours;
    int         minutes;
    int         seconds;
    int         neg;
    int         len;
    char        ch;
    char const  *tzstart;

    if( *tz == ':' )
        tz++;
    /* remember where time zone name string begins */
    tzstart = tz;
    /* parse time zone name (should be 3 or more characters) */
    /* examples:    PST8, EDT+6, Central Standard Time+7:00:00 */
    for( ;; ) {
        ch = *tz;
        if( ch == '\0' )
            break;
        if( ch == ',' )
            break;
        if( ch == '-' )
            break;
        if( ch == '+' )
            break;
        if( ch >= '0' && ch <= '9' )
            break;
        ++tz;
    }
    len = tz - tzstart;
    if( len > TZNAME_MAX )
        len = TZNAME_MAX;
    memcpy( name, tzstart, ( size_t ) len );
    name[len] = '\0';

    neg = 0;
    if( ch == '-' ) {
        neg = 1;
        ++tz;
    } else if( ch == '+' )
        ++tz;
    ch = *tz;
    if( ch >= '0' && ch <= '9' ) {
        hours = minutes = seconds = 0;
        tz = parse_time( tz, &hours );
        if( *tz == ':' ) {
            tz = parse_time( tz + 1, &minutes );
            if( *tz == ':' )
                tz = parse_time( tz + 1, &seconds );
        }
        *offset = seconds + ( ( minutes + ( hours * 60 ) ) * 60L );
        if( neg )
            *offset = -*offset;
    }
    return( tz );
}

static char *parse_rule( char *tz, struct tm *timeptr )
/*****************************************************/
{
    int         date_form;
    int         days;
    int         hours;
    int         minutes;
    int         seconds;

    date_form = -1;                         /* n  0-365 */
    if( *tz == 'J' ) { /* Jn 1-365 (no leap days) */
        date_form = 1;
        tz++;
    }
    if( *tz == 'M' ) { /* Mm.n.d n'th day of month */
        date_form = 0;
        tz++;
    }
    timeptr->tm_isdst = date_form;
    tz = parse_time( tz, &days );
    if( date_form != 0 )
        timeptr->tm_yday = days;
    else {
        timeptr->tm_mon = days - 1;             /* 1-12 for M form */
        if( *tz == '.' ) {
            tz = parse_time( tz + 1, &days );   /* 1-5 for M form */
            timeptr->tm_mday = days;
            if( *tz == '.' ) {
                tz = parse_time( tz + 1, &days );/* 0-6 for M form */
                timeptr->tm_wday = days;
            }
        }
        timeptr->tm_yday = 0;
    }

    hours = 2;
    minutes = seconds = 0;
    if( *tz == '/' ) {
        tz = parse_time( tz + 1, &hours );
        if( *tz == ':' ) {
            tz = parse_time( tz + 1, &minutes );
            if( *tz == ':' )
                tz = parse_time( tz + 1, &seconds );
        }
    }
    timeptr->tm_sec = seconds;
    timeptr->tm_min = minutes;
    timeptr->tm_hour = hours;
    return( tz );
}

void __parse_tz( char * tz )
/**************************/
{
    long        dayzone;

    _RWD_daylight = 0;
    tz = parse_offset( tz, stzone, &_RWD_timezone );
    if( *tz == '\0' ) {
        dtzone[0] = '\0';
        return;
    }
    _RWD_daylight = 1;
    dayzone = _RWD_timezone - ( 60*60 );              /* 16-aug-91 */
    tz = parse_offset( tz, dtzone, &dayzone );
    _RWD_dst_adjust = _RWD_timezone - dayzone;

    /* parse daylight changing rules */
    if( *tz == ',' )
        tz = parse_rule( tz + 1, &__start_dst );
    if( *tz == ',' ) {
        tz = parse_rule( tz + 1, &__end_dst );
        /* convert rule to be in terms of Standard Time */
        /* rather than Daylight Time */
        __end_dst.tm_hour -= _RWD_dst_adjust / 3600;
        __end_dst.tm_min -= ( _RWD_dst_adjust / 60 ) % 60;
        __end_dst.tm_sec -= _RWD_dst_adjust % 60;
    }
}

static int tryOSTimeZone( const char *tz )
/****************************************/
{
    if( tz == NULL ) {
        /* calling OS can be expensive; many programs don't care */
        if( tzFlag.cache_OS_TZ && tzFlag.have_OS_TZ )
            return( 1 );
        /* Assume that even if we end up not getting the TZ from OS,
            we won't have any better luck if we try later. */
        tzFlag.have_OS_TZ = 1;
    } else {
        tzFlag.have_OS_TZ = 0;
        return( 0 );
    }

    // assume Eastern (North America) time zone
    _RWD_timezone = 5L * 60L * 60L;
    _RWD_dst_adjust = 60L * 60L;
    return( 1 );
}

_WCRTLINK void tzset( void )
/**************************/
{
    #ifndef __NETWARE__
    char        *tz;

    tz = getenv( "TZ" );
    if( !tryOSTimeZone( tz ) && tz != NULL )
        __parse_tz( tz );
    #endif
}
