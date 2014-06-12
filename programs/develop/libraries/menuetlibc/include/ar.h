/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef _AR_H_
#define _AR_H_

#define ARMAG "!<arch>\n"
#define SARMAG 8

#define ARFMAG "`\n"

struct ar_hdr {
  char ar_name[16];
  char ar_date[12];
  char ar_uid[6];
  char ar_gid[6];
  char ar_mode[8];
  char ar_size[10];
  char ar_fmag[2];
};

#endif
