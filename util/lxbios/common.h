/*****************************************************************************\
 * common.h
 * $Id: common.h,v 1.2 2005/12/16 22:45:49 dsp_llnl Exp $
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by Dave Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
 *  UCRL-CODE-2003-012
 *  All rights reserved.
 *
 *  This file is part of lxbios, a utility for reading/writing LinuxBIOS
 *  parameters and displaying information from the LinuxBIOS table.
 *  For details, see <http://www.llnl.gov/linux/lxbios/>.
 *
 *  Please also read the file DISCLAIMER which is included in this software
 *  distribution.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License (as published by the
 *  Free Software Foundation) version 2, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the terms and
 *  conditions of the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
\*****************************************************************************/

#ifndef LXBIOS_COMMON_H
#define LXBIOS_COMMON_H

#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1

#define BUG() assert(0)

#define COMMON_RESULT_START 0x10000
#define LAYOUT_RESULT_START 0x20000
#define CMOS_RESULT_START 0x30000
#define CMOS_OP_RESULT_START 0x40000

#define OK 0  /* 0 is used universally to indicate success. */

#define LINE_EOF (COMMON_RESULT_START + 0)
#define LINE_TOO_LONG (COMMON_RESULT_START + 1)

/* basename of this program, as reported by argv[0] */
extern const char prog_name[];

/* version of this program */
extern const char prog_version[];

int get_line_from_file (FILE *f, char line[], int line_buf_size);
void out_of_memory (void);
void usage (FILE *outfile);

#endif  /* LXBIOS_COMMON_H */