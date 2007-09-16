/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "superiotool.h"

unsigned char regval(unsigned short port, unsigned char reg)
{
	outb(reg, port);
	return inb(port + 1);
}

void regwrite(unsigned short port, unsigned char reg, unsigned char val)
{
	outb(reg, port);
	outb(val, port + 1);
}

void probe_superio(unsigned short port)
{
	probe_idregs_simple(port);
	probe_idregs_fintek(port);
	probe_idregs_ite(port);
}

int main(int argc, char *argv[])
{
	if (iopl(3) < 0) {
		perror("iopl");
		exit(1);
	}

	probe_superio(0x2e);	/* Try 0x2e. */
	probe_superio(0x4e);	/* Try 0x4e. */

	return 0;
}
