/* This file was generated by getpir.c, do not modify! 
   (but if you do, please run checkpir on it to verify)
   Contains the IRQ Routing Table dumped directly from your memory , wich BIOS sets up

   Documentation at : http://www.microsoft.com/hwdev/busbios/PCIIRQ.HTM
*/

#include <arch/pirq_routing.h>

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE, /* u32 signature */
	PIRQ_VERSION,   /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT,        /* there can be total CONFIG_IRQ_SLOT_COUNT devices on the bus */
	1,           /* Where the interrupt router lies (bus) */
	(4<<3)|3,           /* Where the interrupt router lies (dev) */
	0,         /* IRQs devoted exclusively to PCI usage */
	0x1022,         /* Vendor */
	0x7400,         /* Device */
	0,         /* Crap (miniport) */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x9a,         /*  u8 checksum , this hase to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		{0,0xc0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{1,(3<<3)|0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x4,0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0x4, 0xdef8}}, 0, 0},
		{0x4,0x8, {{0x1, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x4,0x20, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0x1, 0},
		{0x4,0x18, {{0x2, 0xdef8}, {0x1, 0xdef8}, {0x3, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x4,0x28, {{0x4, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x4,0x30, {{0x3, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{1,(4<<3)|0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{1,(1<<3)|0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x2,0x18, {{0x4, 0xdef8}, {0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}}, 0x2, 0},
		{0x2,0x10, {{0x3, 0xdef8}, {0x4, 0xdef8}, {0x1, 0xdef8}, {0x2, 0xdef8}}, 0x3, 0},
		{0x2,0x48, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x2,0x20, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{1,(2<<3)|0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0x3,0x18, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0x4, 0},
		{0x3,0x8, {{0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}, {0x1, 0xdef8}}, 0x5, 0},
		{0x3,0x20, {{0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}, {0x1, 0xdef8}}, 0x6, 0},
		{0x3,0x28, {{0x3, 0xdef8}, {0x4, 0xdef8}, {0x1, 0xdef8}, {0x2, 0xdef8}}, 0x7, 0},
		{0,0xc8, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0,0xd0, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
		{0,0xd8, {{0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}, {0, 0xdef8}}, 0, 0},
	}
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
        return copy_pirq_routing_table(addr);
}
