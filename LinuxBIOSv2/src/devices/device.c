/*
 *      (c) 1999--2000 Martin Mares <mj@suse.cz>
 *      (c) 2003 Eric Biederman <ebiederm@xmission.com>
 *	(c) 2003 Linux Networx
 */
/* lots of mods by ron minnich (rminnich@lanl.gov), with 
 * the final architecture guidance from Tom Merritt (tjm@codegen.com)
 * In particular, we changed from the one-pass original version to 
 * Tom's recommended multiple-pass version. I wasn't sure about doing 
 * it with multiple passes, until I actually started doing it and saw
 * the wisdom of Tom's recommendations ...
 *
 * Lots of cleanups by Eric Biederman to handle bridges, and to
 * handle resource allocation for non-pci devices.
 */

#include <console/console.h>
#include <bitops.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <smp/spinlock.h>

/** Linked list of ALL devices */
struct device *all_devices = &dev_root;
/** Pointer to the last device */
static struct device **last_dev_p = &dev_root.next;

/** The upper limit of MEM resource of the devices.
 * Reserve 20M for the system */
#define DEVICE_MEM_HIGH 0xFEBFFFFFUL
/** The lower limit of IO resource of the devices.
 * Reserve 4k for ISA/Legacy devices */
#define DEVICE_IO_START 0x1000

/**
 * @brief Allocate a new device structure.
 * 
 * Allocte a new device structure and attached it to the device tree as a
 * child of the parent bus.
 *
 * @param parent parent bus the newly created device attached to.
 * @param path path to the device to be created.
 *
 * @return pointer to the newly created device structure.
 *
 * @see device_path
 */
device_t alloc_dev(struct bus *parent, struct device_path *path)
{
	device_t dev, child;
	int link;

	spin_lock(&dev_lock);	
	/* Find the last child of our parent */
	for(child = parent->children; child && child->sibling; ) {
		child = child->sibling;
	}
	dev = malloc(sizeof(*dev));
	if (dev == 0) {
		die("DEV: out of memory.\n");
	}
	memset(dev, 0, sizeof(*dev));
	memcpy(&dev->path, path, sizeof(*path));


	/* Initialize the back pointers in the link fields */
	for(link = 0; link < MAX_LINKS; link++) {
		dev->link[link].dev  = dev;
		dev->link[link].link = link;
	}
	
	/* By default devices are enabled */
	dev->enabled = 1;

	/* Add the new device to the list of children of the bus. */
	dev->bus = parent;
	if (child) {
		child->sibling = dev;
	} else {
		parent->children = dev;
	}

	/* Append a new device to the global device list.
	 * The list is used to find devices once everything is set up.
	 */
	*last_dev_p = dev;
	last_dev_p = &dev->next;

	spin_unlock(&dev_lock);
	return dev;
}

/**
 * @brief round a number up to an alignment. 
 * @param val the starting value
 * @param roundup Alignment as a power of two
 * @returns rounded up number
 */
static unsigned long round(unsigned long val, unsigned long roundup)
{
	/* ROUNDUP MUST BE A POWER OF TWO. */
	unsigned long inverse;
	inverse = ~(roundup - 1);
	val += (roundup - 1);
	val &= inverse;
	return val;
}

static unsigned long round_down(unsigned long val, unsigned long round_down)
{
	/* ROUND_DOWN MUST BE A POWER OF TWO. */
	unsigned long inverse;
	inverse = ~(round_down - 1);
	val &= inverse;
	return val;
}


/** Read the resources on all devices of a given bus.
 * @param bus bus to read the resources on.
 */
static void read_resources(struct bus *bus)
{
	struct device *curdev;

	/* Walk through all of the devices and find which resources they need. */
	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		unsigned links;
		int i;
		if (curdev->resources > 0) {
			continue;
		}
		if (!curdev->ops || !curdev->ops->read_resources) {
			printk_err("%s missing read_resources\n",
				dev_path(curdev));
			continue;
		}
		if (!curdev->enabled) {
			continue;
		}
		curdev->ops->read_resources(curdev);
		/* Read in subtractive resources behind the current device */
		links = 0;
		for(i = 0; i < curdev->resources; i++) {
			struct resource *resource;
			resource = &curdev->resource[i];
			if ((resource->flags & IORESOURCE_SUBTRACTIVE) &&
				(!(links & (1 << resource->index))))
			{
				links |= (1 << resource->index);
				read_resources(&curdev->link[resource->index]);
				
			}
		}
	}
}

struct pick_largest_state {
	struct resource *last;
	struct device   *result_dev;
	struct resource *result;
	int seen_last;
};

static void pick_largest_resource(struct pick_largest_state *state, 
	struct device *dev, struct resource *resource)
{
	struct resource *last;
	last = state->last;
	/* Be certain to pick the successor to last */
	if (resource == last) {
		state->seen_last = 1;
		return;
	}
	if (last && (
		    (last->align < resource->align) ||
		    ((last->align == resource->align) &&
			    (last->size < resource->size)) ||
		    ((last->align == resource->align) &&
			    (last->size == resource->size) &&
			    (!state->seen_last)))) {
		return;
	}
	if (!state->result || 
		(state->result->align < resource->align) ||
		((state->result->align == resource->align) &&
			(state->result->size < resource->size))) {
		state->result_dev = dev;
		state->result = resource;
	}    
}

static void find_largest_resource(struct pick_largest_state *state, 
	struct bus *bus, unsigned long type_mask, unsigned long type)
{
	struct device *curdev;
	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		int i;
		for(i = 0; i < curdev->resources; i++) {
			struct resource *resource = &curdev->resource[i];
			/* If it isn't the right kind of resource ignore it */
			if ((resource->flags & type_mask) != type) {
				continue;
			}
			/* If it is a subtractive resource recurse */
			if (resource->flags & IORESOURCE_SUBTRACTIVE) {
				struct bus *subbus;
				subbus = &curdev->link[resource->index];
				find_largest_resource(state, subbus, type_mask, type);
				continue;
			}
			/* See if this is the largest resource */
			pick_largest_resource(state, curdev, resource);
		}
	}
}

static struct device *largest_resource(struct bus *bus,
				       struct resource **result_res,
				       unsigned long type_mask,
				       unsigned long type)
{
	struct pick_largest_state state;

	state.last = *result_res;
	state.result_dev = 0;
	state.result = 0;
	state.seen_last = 0;

	find_largest_resource(&state, bus, type_mask, type);

	*result_res = state.result;
	return state.result_dev;
}

/* Compute allocate resources is the guts of the resource allocator.
 * 
 * The problem.
 *  - Allocate resources locations for every device.
 *  - Don't overlap, and follow the rules of bridges.
 *  - Don't overlap with resources in fixed locations.
 *  - Be efficient so we don't have ugly strategies.
 *
 * The strategy.
 * - Devices that have fixed addresses are the minority so don't
 *   worry about them too much.  Instead only use part of the address
 *   space for devices with programmable addresses.  This easily handles
 *   everything except bridges.
 *
 * - PCI devices are required to have thier sizes and their alignments
 *   equal.  In this case an optimal solution to the packing problem
 *   exists.  Allocate all devices from highest alignment to least
 *   alignment or vice versa.  Use this.
 *
 * - So we can handle more than PCI run two allocation passes on
 *   bridges.  The first to see how large the resources are behind
 *   the bridge, and what their alignment requirements are.  The
 *   second to assign a safe address to the devices behind the
 *   bridge.  This allows me to treat a bridge as just a device with 
 *   a couple of resources, and not need to special case it in the
 *   allocator.  Also this allows handling of other types of bridges.
 *
 */

void compute_allocate_resource(
	struct bus *bus,
	struct resource *bridge,
	unsigned long type_mask,
	unsigned long type)
{
	struct device *dev;
	struct resource *resource;
	resource_t base;
	unsigned long align, min_align;
	min_align = 0;
	base = bridge->base;

	printk_spew("%s compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d\n", 
		dev_path(bus->dev),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);


	/* We want different minimum alignments for different kinds of
	 * resources.  These minimums are not device type specific
	 * but resource type specific.
	 */
	if (bridge->flags & IORESOURCE_IO) {
		min_align = log2(DEVICE_IO_ALIGN);
	}
	if (bridge->flags & IORESOURCE_MEM) {
		min_align = log2(DEVICE_MEM_ALIGN);
	}

	/* Make certain I have read in all of the resources */
	read_resources(bus);

	/* Remember I haven't found anything yet. */
	resource = 0;

	/* Walk through all the devices on the current bus and 
	 * compute the addresses.
	 */
	while((dev = largest_resource(bus, &resource, type_mask, type))) {
		resource_t size;
		/* Do NOT I repeat do not ignore resources which have zero size.
		 * If they need to be ignored dev->read_resources should not even
		 * return them.   Some resources must be set even when they have
		 * no size.  PCI bridge resources are a good example of this.
		 */

		/* Propogate the resource alignment to the bridge register  */
		if (resource->align > bridge->align) {
			bridge->align = resource->align;
		}

		/* Propogate the resource limit to the bridge register */
		if (bridge->limit > resource->limit) {
			bridge->limit = resource->limit;
		}
		/* Artificially deny limits between DEVICE_MEM_HIGH and 0xffffffff */
		if ((bridge->limit > DEVICE_MEM_HIGH) && (bridge->limit <= 0xffffffff)) {
			bridge->limit = DEVICE_MEM_HIGH;
		}

		/* Make certain we are dealing with a good minimum size */
		size = resource->size;
		align = resource->align;
		if (align < min_align) {
			align = min_align;
		}
		if (resource->flags & IORESOURCE_FIXED) {
			continue;
		}
		if (resource->flags & IORESOURCE_IO) {
			/* Don't allow potential aliases over the
			 * legacy pci expansion card addresses.
			 * The legacy pci decodes only 10 bits,
			 * uses 100h - 3ffh. Therefor, only 0 - ff
			 * can be used out of each 400h block of io
			 * space.
			 */
			if ((base & 0x300) != 0) {
				base = (base & ~0x3ff) + 0x400;
			}
			/* Don't allow allocations in the VGA IO range.
			 * PCI has special cases for that.
			 */
			else if ((base >= 0x3b0) && (base <= 0x3df)) {
				base = 0x3e0;
			}
		}
		if (((round(base, align) + size) -1) <= resource->limit) {
			/* base must be aligned to size */
			base = round(base, align);
			resource->base = base;
			resource->flags |= IORESOURCE_ASSIGNED;
			resource->flags &= ~IORESOURCE_STORED;
			base += size;
			
			printk_spew(
				"%s %02x *  [0x%08Lx - 0x%08Lx] %s\n",
				dev_path(dev),
				resource->index, 
				resource->base, 
				resource->base + resource->size - 1,
				(resource->flags & IORESOURCE_IO)? "io":
				(resource->flags & IORESOURCE_PREFETCH)? "prefmem": "mem");
		}
	}
	/* A pci bridge resource does not need to be a power
	 * of two size, but it does have a minimum granularity.
	 * Round the size up to that minimum granularity so we
	 * know not to place something else at an address postitively
	 * decoded by the bridge.
	 */
	bridge->size = round(base, bridge->gran) - bridge->base;

	printk_spew("%s compute_allocate_%s: base: %08lx size: %08lx align: %d gran: %d done\n", 
		dev_path(bus->dev),
		(bridge->flags & IORESOURCE_IO)? "io":
		(bridge->flags & IORESOURCE_PREFETCH)? "prefmem" : "mem",
		base, bridge->size, bridge->align, bridge->gran);


}

static void allocate_vga_resource(void)
{
#warning "FIXME modify allocate_vga_resource so it is less pci centric!"
#warning "This function knows to much about PCI stuff, it should be just a ietrator/visitor."

	/* FIXME handle the VGA pallette snooping */
	struct device *dev, *vga;
	struct bus *bus;
	bus = 0;
	vga = 0;
	for(dev = all_devices; dev; dev = dev->next) {
		if (((dev->class >> 16) == PCI_BASE_CLASS_DISPLAY) &&
			((dev->class >> 8) != PCI_CLASS_DISPLAY_OTHER)) 
		{
			if (!vga) {
				printk_debug("Allocating VGA resource %s\n",
					dev_path(dev));
				vga = dev;
			}
			if (vga == dev) {
				/* All legacy VGA cards have MEM & I/O space registers */
				dev->command |= PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
			} else {
				/* It isn't safe to enable other VGA cards */
				dev->command &= ~(PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
			}
		}
	}
	if (vga) {
		bus = vga->bus;
	}
	/* Now walk up the bridges setting the VGA enable */
	while(bus) {
		bus->bridge_ctrl |= PCI_BRIDGE_CTL_VGA;
		bus = (bus == bus->dev->bus)? 0 : bus->dev->bus;
	} 
}


/** Assign the computed resources to the bridges and devices on the bus.
 * Recurse to any bridges found on this bus first. Then do the devices
 * on this bus.
 * 
 * @param bus Pointer to the structure for this bus
 */ 
void assign_resources(struct bus *bus)
{
	struct device *curdev;

	printk_spew("%s assign_resources, bus %d link: %d\n", 
		dev_path(bus->dev), bus->secondary, bus->link);

	for(curdev = bus->children; curdev; curdev = curdev->sibling) {
		if (!curdev->enabled || !curdev->resources) {
			continue;
		}
		if (!curdev->ops || !curdev->ops->set_resources) {
			printk_err("%s missing set_resources\n",
				dev_path(curdev));
			continue;
		}
		curdev->ops->set_resources(curdev);
	}
	printk_spew("%s assign_resources, bus %d link: %d\n", 
		dev_path(bus->dev), bus->secondary, bus->link);
}

/**
 * @brief Enable the resources for a specific device
 *
 * @param dev the device whose resources are to be enabled
 *
 * Enable resources of the device by calling the device specific
 * enable_resources() method.
 *
 * The parent's resources should be enabled first to avoid having enabling
 * order problem. This is done by calling the parent's enable_resources()
 * method and let that method to call it's children's enable_resoruces() via
 * enable_childrens_resources().
 *
 * Indirect mutual recursion:
 */
void enable_resources(struct device *dev)
{
	if (!dev->enabled) {
		return;
	}
	if (!dev->ops || !dev->ops->enable_resources) {
		printk_err("%s missing enable_resources\n", dev_path(dev));
		return;
	}
	dev->ops->enable_resources(dev);
}

/**
 * @brief Determine the existence of dynamic devices and construct dynamic
 * device tree.
 *
 * Start from the root device 'dev_root', scan the buses in the system
 * recursively, build the dynamic device tree according to the result
 * of the probe.
 *
 * This function has no idea how to scan and probe buses and devices at all.
 * It depends on the bus/device specific scan_bus() method to do it. The
 * scan_bus() function also has to create the device structure and attach
 * it to the device tree. 
 */
void dev_enumerate(void)
{
	struct device *root;
	unsigned subordinate;
	printk_info("Enumerating buses...\n");
	root = &dev_root;
	if (!root->ops || !root->ops->scan_bus) {
		printk_err("dev_root missing scan_bus operation");
		return;
	}
	subordinate = root->ops->scan_bus(root, 0);
	printk_info("done\n");
}


/**
 * @brief Configure devices on the devices tree.
 * 
 * Starting at the root of the dynamic device tree, travel recursively,
 * and compute resources needed by each device and allocate them.
 *
 * I/O resources start at DEVICE_IO_START and grow upward. MEM resources start
 * at DEVICE_MEM_START and grow downward.
 *
 * Since the assignment is hierarchical we set the values into the dev_root
 * struct. 
 */
void dev_configure(void)
{
	struct resource *io, *mem;
	struct device *root;

	printk_info("Allocating resources...\n");

	root = &dev_root;
	if (!root->ops || !root->ops->read_resources) {
		printk_err("dev_root missing read_resources\n");
		return;
	}
	if (!root->ops || !root->ops->set_resources) {
		printk_err("dev_root missing set_resources\n");
		return;
	}
	root->ops->read_resources(root);

	/* Get the resources */
	io  = &root->resource[0];
	mem = &root->resource[1];
	/* Make certain the io devices are allocated somewhere safe. */
	io->base = DEVICE_IO_START;
	io->flags |= IORESOURCE_ASSIGNED;
	io->flags &= ~IORESOURCE_STORED;
	/* Now reallocate the pci resources memory with the
	 * highest addresses I can manage.
	 */
	mem->base = resource_max(&root->resource[1]);
	mem->flags |= IORESOURCE_ASSIGNED;
	mem->flags &= ~IORESOURCE_STORED;

	/* Allocate the VGA I/O resource.. */
	allocate_vga_resource(); 

	/* Store the computed resource allocations into device registers ... */
	root->ops->set_resources(root);

#if 0
	mem->flags |= IORESOURCE_STORED;
	report_resource_stored(root, mem, "");
#endif

	printk_info("done.\n");
}

/**
 * @brief Enable devices on the device tree.
 *
 * Starting at the root, walk the tree and enable all devices/bridges by
 * calling the device's enable_resources() method.
 */
void dev_enable(void)
{
	printk_info("Enabling resourcess...\n");

	/* now enable everything. */
	enable_resources(&dev_root);

	printk_info("done.\n");
}

/**
 * @brief Initialize all devices in the global device list.
 *
 * Starting at the first device on the global device link list,
 * walk the list and call a driver to do device specific setup.
 */
void dev_initialize(void)
{
	struct device *dev;

	printk_info("Initializing devices...\n");
	for(dev = all_devices; dev; dev = dev->next) {
		if (dev->enabled && !dev->initialized && 
			dev->ops && dev->ops->init) 
		{
			printk_debug("%s init\n", dev_path(dev));
			dev->initialized = 1;
			dev->ops->init(dev);
		}
	}
	printk_info("Devices initialized\n");
}
