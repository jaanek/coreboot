/*****************************************************************************\
 * input_file.h
 * $Id: input_file.h,v 1.2 2005/12/16 22:45:49 dsp_llnl Exp $
 *****************************************************************************
 *  Copyright (C) 2002-2005 The Regents of the University of California.
 *  Produced at the Lawrence Livermore National Laboratory.
 *  Written by David S. Peterson <dsp@llnl.gov> <dave_peterson@pobox.com>.
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

#ifndef LXBIOS_INPUT_FILE_H
#define LXBIOS_INPUT_FILE_H

#include "common.h"

typedef struct cmos_write_t cmos_write_t;

/* This represents a pending CMOS write operation.  When changing multiple
 * CMOS parameter values, we first represent the changes as a list of pending
 * write operations.  This allows us to sanity check all write operations
 * before any of them are performed.
 */
struct cmos_write_t
 { unsigned bit;
   unsigned length;
   unsigned long long value;
   cmos_write_t *next;
 };

cmos_write_t * process_input_file (FILE *f);
void do_cmos_writes (cmos_write_t *list);

extern const char assignment_regex[];

#endif  /* LXBIOS_INPUT_FILE_H */