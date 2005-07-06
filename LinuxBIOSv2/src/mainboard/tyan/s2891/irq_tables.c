/* This file was generated by getpir.c, do not modify! 
   (but if you do, please run checkpir on it to verify)
   Contains the IRQ Routing Table dumped directly from your memory , wich BIOS sets up

   Documentation at : http://www.microsoft.com/hwdev/busbios/PCIIRQ.HTM
*/

#include <arch/pirq_routing.h>

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE, /* u32 signature */
	PIRQ_VERSION,   /* u16 version   */
	32+16*11,        /* there can be total 11 devices on the bus */
	1,           /* Where the interrupt router lies (bus) */
	((CK804_DEVN_BASE+9)<<3)|0,           /* Where the interrupt router lies (dev) */
	0,         /* IRQs devoted exclusively to PCI usage */
	0x10de,         /* Vendor */
	0x005c,         /* Device */
	0,         /* Crap (miniport) */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x5a,         /*  u8 checksum , this hase to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		{1,((CK804_DEVN_BASE+9)<<3)|0, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0, 0},
		{0x5,(1<<3)|0, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0, 0},
		{0x5,(4<<3)|0, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0x0, 0},
		{0x5,(3<<3)|0, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0x1, 0},
		{0x5,(6<<3)|0, {{0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}, {0x1, 0xdef8}}, 0x2, 0},
		{0x4,(8<<3)|0, {{0x4, 0xdef8}, {0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}}, 0x3, 0},
		{0x4,(7<<3)|0, {{0x3, 0xdef8}, {0x4, 0xdef8}, {0x1, 0xdef8}, {0x2, 0xdef8}}, 0x4, 0},
		{0x6,(0x0a<<3)|0, {{0x1, 0xdef8}, {0x2, 0xdef8}, {0x3, 0xdef8}, {0x4, 0xdef8}}, 0x5, 0},
		{0x4,(9<<3)|0, {{0x1, 0xdef8}, {2, 0xdef8}, {0, 0}, {0, 0}}, 0, 0},
		{0x6,(0x0b<<3)|0, {{0x2, 0xdef8}, {0, 0}, {0, 0}, {0, 0}}, 0, 0},
		{0x6,(0x0c<<3)|0, {{0x4, 0xdef8}, {0, 0}, {0, 0}, {0, 0}}, 0, 0},
	}
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
        return copy_pirq_routing_table(addr);
}
