/*
 * Copyright (C) 2010 LunarG Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Chia-I Wu <olv@lunarg.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "render.h"
#include "eglutint.h"
#include <kos32sys.h>

static struct eglut_state _eglut_state = {
   .api_mask = EGLUT_OPENGL_BIT,
   .window_width = 300,
   .window_height = 300,
   .verbose = 0,
   .num_windows = 0,
};

struct eglut_state *_eglut = &_eglut_state;

void
_eglutFatal(char *format, ...)
{
  va_list args;

  va_start(args, format);

  fprintf(stderr, "EGLUT: ");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);

  exit(1);
}

/* return current time (in milliseconds) */
int
_eglutNow(void)
{
   struct timeval tv;
#ifdef __VMS
   (void) gettimeofday(&tv, NULL );
#else
   struct timezone tz;
   (void) gettimeofday(&tv, &tz);
#endif
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static void
_eglutDestroyWindow(struct eglut_window *win)
{
   eglDestroySurface(_eglut->dpy, win->surface);
   _eglutNativeFiniWindow(win);
   eglDestroyContext(_eglut->dpy, win->context);
}

static EGLConfig
_eglutChooseConfig(void)
{
   EGLConfig config;
   EGLint config_attribs[32];
   EGLint num_configs, i;

   i = 0;
   config_attribs[i++] = EGL_RED_SIZE;
   config_attribs[i++] = 1;
   config_attribs[i++] = EGL_GREEN_SIZE;
   config_attribs[i++] = 1;
   config_attribs[i++] = EGL_BLUE_SIZE;
   config_attribs[i++] = 1;
   config_attribs[i++] = EGL_DEPTH_SIZE;
   config_attribs[i++] = 1;

   config_attribs[i++] = EGL_SURFACE_TYPE;
   config_attribs[i++] = EGL_WINDOW_BIT;

   config_attribs[i++] = EGL_RENDERABLE_TYPE;
   config_attribs[i++] = EGL_OPENGL_BIT;
   config_attribs[i] = EGL_NONE;

//   renderable_type = 0x0;
//   if (_eglut->api_mask & EGLUT_OPENGL_BIT)
//      renderable_type |= EGL_OPENGL_BIT;
//   if (_eglut->api_mask & EGLUT_OPENGL_ES1_BIT)
//      renderable_type |= EGL_OPENGL_ES_BIT;
//   if (_eglut->api_mask & EGLUT_OPENGL_ES2_BIT)
//      renderable_type |= EGL_OPENGL_ES2_BIT;
//   if (_eglut->api_mask & EGLUT_OPENVG_BIT)
//      renderable_type |= EGL_OPENVG_BIT;

   if (!eglChooseConfig(_eglut->dpy,
            config_attribs, &config, 1, &num_configs) || !num_configs)
      _eglutFatal("failed to choose a config");

   return config;
}

static struct eglut_window *
_eglutCreateWindow(const char *title, int x, int y, int w, int h)
{
    struct eglut_window *win;

    win = calloc(1, sizeof(*win));
    if (!win)
        _eglutFatal("failed to allocate window");

    win->config = _eglutChooseConfig();

    eglBindAPI(EGL_OPENGL_API);
    win->context = eglCreateContext(_eglut->dpy, win->config, EGL_NO_CONTEXT, NULL);
    if (!win->context)
        _eglutFatal("failed to create context");

    _eglutNativeInitWindow(win, title, x, y, w, h);
    switch (_eglut->surface_type) {
        case EGL_WINDOW_BIT:
            win->surface = eglCreateWindowSurface(_eglut->dpy,
                           win->config, win->native.u.window, NULL);
        break;
        default:
            break;
    }
    if (win->surface == EGL_NO_SURFACE)
        _eglutFatal("failed to create surface");

    return win;
}

void
eglutInitAPIMask(int mask)
{
   _eglut->api_mask = mask;
}

void
eglutInitWindowSize(int width, int height)
{
   _eglut->window_width = width;
   _eglut->window_height = height;
}

void
eglutInit(int argc, char **argv)
{
   int i;

   for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-display") == 0)
         _eglut->display_name = argv[++i];
      else if (strcmp(argv[i], "-info") == 0) {
         _eglut->verbose = 1;
      }
   }

   _eglutNativeInitDisplay();
   _eglut->dpy = eglGetDisplay(_eglut->native_dpy);

   if (!eglInitialize(_eglut->dpy, &_eglut->major, &_eglut->minor))
      _eglutFatal("failed to initialize EGL display");

   _eglut->init_time = _eglutNow();

   printf("EGL_VERSION = %s\n", eglQueryString(_eglut->dpy, EGL_VERSION));
   if (_eglut->verbose) {
      printf("EGL_VENDOR = %s\n", eglQueryString(_eglut->dpy, EGL_VENDOR));
      printf("EGL_EXTENSIONS = %s\n",
            eglQueryString(_eglut->dpy, EGL_EXTENSIONS));
      printf("EGL_CLIENT_APIS = %s\n",
            eglQueryString(_eglut->dpy, EGL_CLIENT_APIS));
   }
}

int
eglutGet(int state)
{
   int val;

   switch (state) {
   case EGLUT_ELAPSED_TIME:
      val = _eglutNow() - _eglut->init_time;
      break;
   default:
      val = -1;
      break;
   }

   return val;
}

void
eglutIdleFunc(EGLUTidleCB func)
{
   _eglut->idle_cb = func;
}

void
eglutPostRedisplay(void)
{
   _eglut->redisplay = 1;
}

void
eglutMainLoop(void)
{
   struct eglut_window *win = _eglut->current;

   if (!win)
      _eglutFatal("no window is created\n");

   if (win->reshape_cb)
      win->reshape_cb(win->native.width, win->native.height);

   _eglutNativeEventLoop();
}

void
eglutFini(void)
{
   eglTerminate(_eglut->dpy);
   _eglutNativeFiniDisplay();
}

void
eglutDestroyWindow(int win)
{
   eglMakeCurrent(_eglut->dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
   _eglutDestroyWindow(_eglut->current);
}

static void
_eglutDefaultKeyboard(unsigned char key)
{
}

int
eglutCreateWindow(const char *title)
{
    struct eglut_window *win;
    int skinh;

    win = _eglutCreateWindow(title, 20, 20,
          _eglut->window_width, _eglut->window_height);

    win->index = _eglut->num_windows++;
    win->reshape_cb = NULL;
    win->display_cb = NULL;
    win->keyboard_cb = _eglutDefaultKeyboard;
    win->special_cb = NULL;

    if (!eglMakeCurrent(_eglut->dpy, win->surface, win->surface, win->context))
        _eglutFatal("failed to make window current");
    _eglut->current = win;

    skinh = get_skin_height();

    _eglut->render = create_render(_eglut->dpy, win->surface, TYPE_3_BORDER_WIDTH, skinh);
    return win->index;
}

int
eglutGetWindowWidth(void)
{
   struct eglut_window *win = _eglut->current;
   return win->native.width;
}

int
eglutGetWindowHeight(void)
{
   struct eglut_window *win = _eglut->current;
   return win->native.height;
}

void
eglutDisplayFunc(EGLUTdisplayCB func)
{
   struct eglut_window *win = _eglut->current;
   win->display_cb = func;

}

void
eglutReshapeFunc(EGLUTreshapeCB func)
{
   struct eglut_window *win = _eglut->current;
   win->reshape_cb = func;
}

void
eglutKeyboardFunc(EGLUTkeyboardCB func)
{
   struct eglut_window *win = _eglut->current;
   win->keyboard_cb = func;
}

void
eglutSpecialFunc(EGLUTspecialCB func)
{
   struct eglut_window *win = _eglut->current;
   win->special_cb = func;
}

int atexit(void (*func)(void))
{
    return 0;
};
