/*
    IMGLIB:  An example image loading library for use with SDL
    Copyright (C) 1999  Sam Lantinga

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
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

/* A simple library to load images of various formats as SDL surfaces */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "SDL_image.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof((a)[0]))

/* Table of image detection and loading functions */
static struct {
	char *type;
	int (*is)(SDL_RWops *src);
	SDL_Surface *(*load)(SDL_RWops *src);
} supported[] = {
	/* keep magicless formats first */
	{ "TGA", 0,         IMG_LoadTGA_RW },
	{ "BMP", IMG_isBMP, IMG_LoadBMP_RW },
	{ "PNM", IMG_isPNM, IMG_LoadPNM_RW }, /* P[BGP]M share code */
	{ "XPM", IMG_isXPM, IMG_LoadXPM_RW },
	{ "XCF", IMG_isXCF, IMG_LoadXCF_RW },
	{ "PCX", IMG_isPCX, IMG_LoadPCX_RW },
	{ "GIF", IMG_isGIF, IMG_LoadGIF_RW },
	{ "JPG", IMG_isJPG, IMG_LoadJPG_RW },
	{ "TIF", IMG_isTIF, IMG_LoadTIF_RW },
	{ "PNG", IMG_isPNG, IMG_LoadPNG_RW }
};

/* Load an image from a file */
SDL_Surface *IMG_Load(const char *file)
{
    SDL_RWops *src = SDL_RWFromFile(file, "rb");
    char *ext = strrchr(file, '.');
    if(ext)
	ext++;
    return IMG_LoadTyped_RW(src, 1, ext);
}

/* Load an image from an SDL datasource (for compatibility) */
SDL_Surface *IMG_Load_RW(SDL_RWops *src, int freesrc)
{
    return IMG_LoadTyped_RW(src, freesrc, NULL);
}

/* Portable case-insensitive string compare function */
int IMG_string_equals(const char *str1, const char *str2)
{
	while ( *str1 && *str2 ) {
		if ( toupper((unsigned char)*str1) !=
		     toupper((unsigned char)*str2) )
			break;
		++str1;
		++str2;
	}
	return (!*str1 && !*str2);
}

/* Load an image from an SDL datasource, optionally specifying the type */
SDL_Surface *IMG_LoadTyped_RW(SDL_RWops *src, int freesrc, char *type)
{
	int i, start;
	SDL_Surface *image;

	/* Make sure there is something to do.. */
	if ( src == NULL ) {
		return(NULL);
	}

	/* See whether or not this data source can handle seeking */
	if ( SDL_RWseek(src, 0, SEEK_CUR) < 0 ) {
		IMG_SetError("Can't seek in this data source");
		return(NULL);
	}

	/* Detect the type of image being loaded */
	start = SDL_RWtell(src);
	image = NULL;
	for ( i=0; i < ARRAYSIZE(supported); ++i ) {
	        if( (supported[i].is
		     && (SDL_RWseek(src, start, SEEK_SET),
			 supported[i].is(src)))
		    || (type && IMG_string_equals(type, supported[i].type))) {
#ifdef DEBUG_IMGLIB
			SDL_printf("IMGLIB: Loading image as %s\n",
							supported[i].type);
#endif
			SDL_RWseek(src, start, SEEK_SET);
			image = supported[i].load(src);
			break;
		}
	}

	/* Clean up, check for errors, and return */
	if ( freesrc ) {
		SDL_RWclose(src);
	}
	if ( i == ARRAYSIZE(supported) ) {
		IMG_SetError("Unsupported image format");
	}
	return(image);
}

/* Invert the alpha of a surface for use with OpenGL
   This function is a no-op and only kept for backwards compatibility.
 */
int IMG_InvertAlpha(int on)
{
    return 1;
}
