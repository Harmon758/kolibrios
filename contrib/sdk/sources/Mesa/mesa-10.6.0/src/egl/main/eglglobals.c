/**************************************************************************
 *
 * Copyright 2008 VMware, Inc.
 * Copyright 2009-2010 Chia-I Wu <olvaffe@gmail.com>
 * Copyright 2010-2011 LunarG, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/


#include <stdlib.h>
#include <assert.h>
#include "c11/threads.h"

#include "eglglobals.h"
#include "egldisplay.h"
#include "egldriver.h"


static mtx_t _eglGlobalMutex = _MTX_INITIALIZER_NP;

struct _egl_global _eglGlobal =
{
   &_eglGlobalMutex,       /* Mutex */
   NULL,                   /* DisplayList */
   2,                      /* NumAtExitCalls */
   {
      /* default AtExitCalls, called in reverse order */
      _eglUnloadDrivers, /* always called last */
      _eglFiniDisplay
   },

   /* ClientExtensions */
   {
      true, /* EGL_EXT_client_extensions */
      true, /* EGL_EXT_platform_base */
      true, /* EGL_EXT_platform_x11 */
      true, /* EGL_EXT_platform_wayland */
      true, /* EGL_MESA_platform_gbm */
      true, /* EGL_KHR_client_get_all_proc_addresses */
   },

   /* ClientExtensionsString */
   "EGL_EXT_client_extensions"
   " EGL_EXT_platform_base"
   " EGL_EXT_platform_x11"
   " EGL_EXT_platform_wayland"
   " EGL_MESA_platform_gbm"
   " EGL_KHR_client_get_all_proc_addresses"
};


static void
_eglAtExit(void)
{
   EGLint i;
   for (i = _eglGlobal.NumAtExitCalls - 1; i >= 0; i--)
      _eglGlobal.AtExitCalls[i]();
}


void
_eglAddAtExitCall(void (*func)(void))
{
   if (func) {
      static EGLBoolean registered = EGL_FALSE;

      mtx_lock(_eglGlobal.Mutex);

      if (!registered) {
         atexit(_eglAtExit);
         registered = EGL_TRUE;
      }

      assert(_eglGlobal.NumAtExitCalls < ARRAY_SIZE(_eglGlobal.AtExitCalls));
      _eglGlobal.AtExitCalls[_eglGlobal.NumAtExitCalls++] = func;

      mtx_unlock(_eglGlobal.Mutex);
   }
}
