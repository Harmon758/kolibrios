/*
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NETSURF_DESKTOP_NETSURF_H_
#define _NETSURF_DESKTOP_NETSURF_H_

#include <stdbool.h>
#include "utils/errors.h"

extern bool netsurf_quit;
extern bool verbose_log;
extern const char * const netsurf_version;
extern const int netsurf_version_major;
extern const int netsurf_version_minor;

nserror netsurf_init(int *argc, char ***argv, const char *options, const char *messages);
extern void netsurf_exit(void);
extern int netsurf_main_loop(void);

#endif
