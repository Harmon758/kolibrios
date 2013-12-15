/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997, 1998, 1999, 2000, 2001  Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@devolution.com
*/

/* This file sets things up for C dynamic library function definitions,
   static inlined functions, and structures aligned at 4-byte alignment.
   If you don't like ugly C preprocessor code, don't look at this file. :)
*/

/* This shouldn't be nested -- included it around code only. */
#ifdef _begin_code_h
#error Nested inclusion of begin_code.h
#endif
#define _begin_code_h

/* Some compilers use a special export keyword */
#ifndef DECLSPEC
# ifdef __BEOS__
#  if defined(__GNUC__)
#   define DECLSPEC	__declspec(dllexport)
#  else
#   define DECLSPEC	__declspec(export)
#  endif
# else
# ifdef WIN32UNDEFINED
#  define DECLSPEC	__declspec(dllexport)
# else
#  define DECLSPEC
# endif
# endif
#endif

/* Force structure packing at 4 byte alignment.
   This is necessary if the header is included in code which has structure
   packing set to an alternate value, say for loading structures from disk.
   The packing is reset to the previous value in close_code.h
 */
#if defined(_MSC_VER) || defined(__MWERKS__) || defined(__WATCOMC__) || defined(__BORLANDC__)
#ifdef _MSC_VER
#pragma warning(disable: 4103)
#endif
#ifdef __BORLANDC__
#pragma nopackwarning
#endif
#pragma pack(push,4)
#endif /* Compiler needs structure packing set */

/* Set up compiler-specific options for inlining functions */
#ifndef SDL_INLINE_OKAY
#ifdef __GNUC__
#define SDL_INLINE_OKAY
#else
/* Add any special compiler-specific cases here */
#if defined(_MSC_VER)
#define __inline__	__inline
#define SDL_INLINE_OKAY
#else
#if !defined(__MRC__) && !defined(_SGI_SOURCE)
#define __inline__ inline
#define SDL_INLINE_OKAY
#endif /* Not a funky compiler */
#endif /* Visual C++ */
#endif /* GNU C */
#endif /* SDL_INLINE_OKAY */

/* If inlining isn't supported, remove "__inline__", turning static
   inlined functions into static functions (resulting in code bloat
   in all files which include the offending header files)
*/
#ifndef SDL_INLINE_OKAY
#define __inline__
#endif

