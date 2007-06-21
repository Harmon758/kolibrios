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
* Description:  prototypes for arguments processing variables 
*
****************************************************************************/


#ifndef _INITARG_H_INCLUDED
#define _INITARG_H_INCLUDED

extern int         __F_NAME(_argc,_wargc);    /* argument count  */
extern CHAR_TYPE **__F_NAME(_argv,_wargv);    /* argument vector */

extern void __F_NAME(__Init_Argv,__wInit_Argv)( void );
extern void __F_NAME(__Fini_Argv,__wFini_Argv)( void );

_WCRTLINKD extern int        __F_NAME(___Argc,___wArgc); /* argument count */
_WCRTLINKD extern CHAR_TYPE**__F_NAME(___Argv,___wArgv); /* argument vector */

_WCRTLINKD extern char   *_LpCmdLine;    /* pointer to command line */
_WCRTLINKD extern char   *_LpPgmName;    /* pointer to program name */
           extern char   *_LpDllName;    /* pointer to dll name */

_WCRTLINKD extern wchar_t *_LpwCmdLine;  /* pointer to wide command line */
_WCRTLINKD extern wchar_t *_LpwPgmName;  /* pointer to wide program name */
           extern wchar_t *_LpwDllName;  /* pointer to wide dll name */

#if defined(__WINDOWS_386__)
extern char     __Is_DLL;
#else
extern int      __Is_DLL;
#endif

#endif
