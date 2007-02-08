// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: m_bbox.c,v 1.1 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "doomdef.h"
#include "m_misc.h"
#include "i_video.h"
#include "i_sound.h"

#include "d_net.h"
#include "g_game.h"


#include "i_system.h"

#include "kolibri.h"

int     mb_used = 8;

void
I_Tactile
( int   on,
  int   off,
  int   total )
{
  // UNUSED.
  on = off = total = 0;
}

ticcmd_t        emptycmd;
ticcmd_t*       I_BaseTiccmd(void)
{
    return &emptycmd;
}


int  I_GetHeapSize (void)
{
 return mb_used*1024*1024;
}

byte* I_ZoneBase (int*  size)
{
 *size = mb_used*1024*1024;
 return (byte *) UserAlloc(*size);
}


//
// I_GetTime
// returns time in 1/35 second tics
//

int  I_GetTime (void)
{
 unsigned int tm;
 _asm
 {  push ebx
    mov eax, 26
    mov ebx, 9
    int 0x40
    mov dword ptr [tm], eax
    pop ebx
 };
 
 return (tm*TICRATE)/100;
}



//
// I_Init
//
void I_Init (void)
{
  I_InitGraphics();
 // I_InitSound();
}

//
// I_Quit
//
void I_Quit (void)
{
//    __libclog_printf("Calling I_Quit from %x\n",__builtin_return_address(0));
    D_QuitNetGame ();
 //   I_ShutdownSound();
 //   I_ShutdownMusic();
    M_SaveDefaults ();
    I_ShutdownGraphics();
    exit(0);
}

void I_WaitVBL(int count)
{
 //__menuet__delay100((count*10)/7);
}

void I_BeginRead(void)
{
}

void I_EndRead(void)
{
}

/***********
byte*   I_AllocLow(int length)
{
    byte*       mem;
        
    mem = (byte *)malloc (length);
    memset (mem,0,length);
    return mem;
}
************/

//
// I_Error
//
extern boolean demorecording;

void I_Error (char *error, ...)
{
    va_list     argptr;

    // Message first.
    va_start (argptr,error);
    printf ("Error:  ");
    printf (error,argptr);
    printf ("\n\r");
    va_end (argptr);

    // Shutdown. Here might be other errors.
    if (demorecording)
        G_CheckDemoStatus();

    D_QuitNetGame ();
    I_ShutdownGraphics();
//    getch();
    exit(-1);
}
