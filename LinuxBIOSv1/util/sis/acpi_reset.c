/*
 * acpi_reset.c: Reboot your LinuxBIOS system with ACPI software watchdo
 *
 *
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *
 * $Id$
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>

main()
{
	unsigned char b;
	unsigned short w;
	unsigned short acpi_base;

	/* get io privilege access PCI configuration space */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}

	/* Enable ACPI by set B7 on Reg 0x40, LPC */
	outl(0x80000840, 0x0cf8);
	b = inb(0x0cfc) | 0x80;
	outb(b, 0xcfc);

	/* get the ACPI base address for register 0x74,0x75 of LPC */
	outl(0x80000874, 0x0cf8);
	w = inw(0x0cfc);
	acpi_base = w;

	printf ("acpi base: %x\n", acpi_base);

	/* Set software watchdog timer init value */	
	outb(0x03, 0x4a + acpi_base);
	
	/* Software watchdog enable, issue PCIRST# when time expire */
	outb(0x8f, 0x4b + acpi_base);
}
