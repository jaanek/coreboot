/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801gx.h"

static void usb_ehci_init(struct device *dev)
{
	u32 reg32;

	printk_debug("EHCI: Setting up controller.. ");
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	reg32 = pci_read_config32(dev, 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_write_config32(dev, 0xdc, reg32);

	reg32 = pci_read_config32(dev, 0xfc);
	reg32 &= ~(3 << 2);
	reg32 |= (2 << 2) | (1 << 29) | (1 << 17);
	pci_write_config32(dev, 0xfc, reg32);

	printk_debug("done.\n");
}

static void usb_ehci_set_subsystem(device_t dev, unsigned vendor,
				   unsigned device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	/* Write the subsystem vendor and device ID. */
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
	.ops_pci		= &lops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gx_usb_ehci __pci_driver = {
	.ops	= &usb_ehci_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27cc,
};
