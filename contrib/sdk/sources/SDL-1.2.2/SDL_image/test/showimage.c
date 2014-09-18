/*
    SHOW:  A test application for the SDL image loading library.
    Copyright (C) 1999  Sam Lantinga

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    5635-34 Springhouse Dr.
    Pleasanton, CA 94588 (USA)
    slouken@devolution.com
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_image.h"


/* Draw a Gimpish background pattern to show transparency in the image */
void draw_background(SDL_Surface *screen)
{
    Uint8 *dst = screen->pixels;
    int x, y;
    int bpp = screen->format->BytesPerPixel;
    Uint32 col[2];
    col[0] = SDL_MapRGB(screen->format, 0x66, 0x66, 0x66);
    col[1] = SDL_MapRGB(screen->format, 0x99, 0x99, 0x99);
    for(y = 0; y < screen->h; y++) {
	for(x = 0; x < screen->w; x++) {
	    /* use an 8x8 checkerboard pattern */
	    Uint32 c = col[((x ^ y) >> 3) & 1];
	    switch(bpp) {
	    case 1:
		dst[x] = c;
		break;
	    case 2:
		((Uint16 *)dst)[x] = c;
		break;
	    case 3:
		if(SDL_BYTEORDER == SDL_LIL_ENDIAN) {
		    dst[x * 3] = c;
		    dst[x * 3 + 1] = c >> 8;
		    dst[x * 3 + 2] = c >> 16;
		} else {
		    dst[x * 3] = c >> 16;
		    dst[x * 3 + 1] = c >> 8;
		    dst[x * 3 + 2] = c;
		}
		break;
	    case 4:
		((Uint32 *)dst)[x] = c;
		break;
	    }
	}
	dst += screen->pitch;
    }
}

int app_main(int argc, char *argv[])
{
	Uint32 flags;
	SDL_Surface *screen, *image;
	int i, depth, done;
	SDL_Event event;

	argv[1]="../1/test.png";
	argc=2;
	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		SDL_printf("Couldn't initialize SDL: %s\n",SDL_GetError());
		return(255);
	}

	flags = SDL_SWSURFACE;
	for ( i=1; i<argc; ++i ) {
		SDL_ShowCursor(0);
		image = IMG_Load(argv[i]);
		if ( image == NULL ) {
			SDL_printf("Couldn't load %s: %s\n",
			        argv[i], SDL_GetError());
			continue;
		}
		{		 
		 static char xttl[128];
		 sprintf(xttl,"Showimage - filename is \"%s\"",argv[1]);
		 SDL_WM_SetCaption(xttl, "showimage");
	        }
		/* Create a display for the image */
		depth = SDL_VideoModeOK(image->w, image->h, 32, flags);
		/* Use the deepest native mode, except that we emulate 32bpp
		   for viewing non-indexed images on 8bpp screens */
		if ( depth == 0 ) {
			if ( image->format->BytesPerPixel > 1 ) {
				depth = 32;
			} else {
				depth = 8;
			}
		} else
		if ( (image->format->BytesPerPixel > 1) && (depth == 8) ) {
	    		depth = 32;
		}
		if(depth == 8)
			flags |= SDL_HWPALETTE;
		screen = SDL_SetVideoMode(image->w, image->h, depth, flags);
		if ( screen == NULL ) {
			SDL_printf("Couldn't set %dx%dx%d video mode: %s\n",
				image->w, image->h, depth, SDL_GetError());
			continue;
		}

		/* Set the palette, if one exists */
		if ( image->format->palette ) {
			SDL_SetColors(screen, image->format->palette->colors,
			              0, image->format->palette->ncolors);
		}

		/* Draw a background pattern if the surface has transparency */
		if(image->flags & (SDL_SRCALPHA | SDL_SRCCOLORKEY))
	    		draw_background(screen);

		/* Display the image */
		SDL_BlitSurface(image, NULL, screen, NULL);
		SDL_UpdateRect(screen, 0, 0, 0, 0);

		done = 0;
		while ( ! done ) {
			if ( SDL_PollEvent(&event) ) {
				switch (event.type) {
				    case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					    case SDLK_LEFT:
						if ( i > 1 ) {
							i -= 2;
							done = 1;
						}
						break;
					    case SDLK_RIGHT:
						if ( argv[i+1] ) {
							done = 1;
						}
						break;
					    case SDLK_ESCAPE:
					    case SDLK_q:
						argv[i+1] = NULL;
						/* Drop through to done */
					    case SDLK_SPACE:
					    case SDLK_TAB:
						done = 1;
						break;
					    default:
						break;
					}
					break;
                                    case SDL_QUIT:
					argv[i+1] = NULL;
					done = 1;
					break;
				    default:
					break;
				}
			} else {
				SDL_Delay(3);
			}
		}
		SDL_FreeSurface(image);
	}

	/* We're done! */
	SDL_Quit();
	return(0);
}
