/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#define REALMODE_BASE		0x600
#define RELOCATED(x)	(x - __realmode_code + REALMODE_BASE)

/* CR0 bits */
#define PE		(1 << 0)

/* This is the intXX interrupt handler stub code. It gets copied
 * to the IDT and to some fixed addresses in the F segment. Before
 * the code can used, it gets patched up by the C function copying
 * it: byte 3 (the $0 in movb $0, %al) is overwritten with the int#.
 */

	.code16
	.globl __idt_handler
__idt_handler:
	pushal
	movb 	$0, %al /* This instruction gets modified */
	ljmp 	$0, $__interrupt_handler_16bit
	.globl __idt_handler_size
__idt_handler_size = ( . - __idt_handler)


/* In order to be independent of coreboot's position in RAM
 * we relocate a part of the code to the low megabyte, so the
 * CPU can use it in real-mode. This code lives at __realmode_code.
 */
	.globl __realmode_code
__realmode_code:

/* Realmode IDT pointer structure. */
	.globl __realmode_idt
__realmode_idt = RELOCATED(.)
	.word 1023	/* 16 bit limit */
	.long 0		/* 24 bit base */
	.word 0

/* Preserve old stack */
__stack = RELOCATED(.)
	.long 0

/* Register store for realmode_call and realmode_interrupt */
__registers = RELOCATED(.)
	.long 0 /*  0 - EAX */
	.long 0 /*  4 - EBX */
	.long 0 /*  8 - ECX */
	.long 0 /* 12 - EDX */
	.long 0 /* 16 - ESI */
	.long 0 /* 20 - EDI */

	.code32
	.globl __realmode_call
__realmode_call = RELOCATED(.)
	/* save all registers to the stack */
	pushal

	/* Move the protected mode stack to a safe place */
	movl	%esp, __stack
	movl    %esp, %ebp

	/* This function is called with regparm=0 and we have
	 * to skip the 32 byte from pushal. Hence start at 36.
	 */

	/* entry point */
	movl    36(%ebp), %eax
	mov	%ax, __lcall_instr + 1
	andl	$0xffff0000, %eax
	shrl	$4, %eax
	mov	%ax, __lcall_instr + 3

	/* initial register values */
	movl	40(%ebp), %eax
	movl	%eax, __registers +  0 /* eax */
	movl	44(%ebp), %eax
	movl	%eax, __registers +  4 /* ebx */
	movl	48(%ebp), %eax
	movl	%eax, __registers +  8 /* ecx */
	movl	52(%ebp), %eax
	movl	%eax, __registers + 12 /* edx */
	movl	56(%ebp), %eax
	movl	%eax, __registers + 16 /* esi */
	movl	60(%ebp), %eax
	movl	%eax, __registers + 20 /* edi */

	/* Activate the right segment descriptor real mode. */
	ljmp	$0x28, $RELOCATED(1f)
1:
.code16
	/* 16 bit code from here on... */

	/* Load the segment registers w/ properly configured
	 * segment descriptors. They will retain these
	 * configurations (limits, writability, etc.) once
	 * protected mode is turned off.
	 */
	mov	$0x30, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* Turn off protection */
	movl	%cr0, %eax
	andl	$~PE, %eax
	movl	%eax, %cr0

	/* Now really going into real mode */
	ljmp	$0, $RELOCATED(1f)
1:
	/* Setup a stack: Put the stack at the end of page zero.
	 * That way we can easily share it between real and
	 * protected, since the 16 bit ESP at segment 0 will
	 * work for any case. */
	mov	$0x0, %ax
	mov	%ax, %ss
	movl	$0x1000, %eax
	movl	%eax, %esp

	/* Load 16 bit IDT */
	xor	%ax, %ax
	mov	%ax, %ds
	lidt	__realmode_idt

	/* initialize registers for option rom lcall */
	movl	__registers +  0, %eax
	movl	__registers +  4, %ebx
	movl	__registers +  8, %ecx
	movl	__registers + 12, %edx
	movl	__registers + 16, %esi
	movl	__registers + 20, %edi

	/* Set all segments to 0x0000, ds to 0x0040 */
	push	%ax
	xor	%ax, %ax
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	$0x40, %ax
	mov	%ax, %ds
	pop	%ax

	/* ************************************ */
__lcall_instr = RELOCATED(.)
	.byte 0x9a
	.word 0x0000, 0x0000
	/* ************************************ */

	/* If we got here, we are just about done.
	 * Need to get back to protected mode.
	 */
	movl	%cr0, %eax
	orl	$PE, %eax
	movl	%eax, %cr0

	/* Now that we are in protected mode
	 * jump to a 32 bit code segment.
	 */
	data32	ljmp	$0x10, $RELOCATED(1f)
1:
	.code32
	mov	$0x18, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* restore proper idt */
	lidt	idtarg

	/* and exit */
	movl	__stack, %esp
	popal

	// TODO return AX from OPROM call
	ret

	.globl __realmode_interrupt
__realmode_interrupt = RELOCATED(.)
	/* save all registers to the stack */
	pushal
	/* save the stack */
	movl	%esp, __stack
	movl    %esp, %ebp

	/* This function is called with regparm=0 and we have
	 * to skip the 32 byte from pushal. Hence start at 36.
	 */

	/* prepare interrupt calling code */
	movl    36(%ebp), %eax
	movb	%al, __intXX_instr + 1 /* intno */

	/* initial register values */
	movl	40(%ebp), %eax
	movl	%eax, __registers +  0 /* eax */
	movl	44(%ebp), %eax
	movl	%eax, __registers +  4 /* ebx */
	movl	48(%ebp), %eax
	movl	%eax, __registers +  8 /* ecx */
	movl	52(%ebp), %eax
	movl	%eax, __registers + 12 /* edx */
	movl	56(%ebp), %eax
	movl	%eax, __registers + 16 /* esi */
	movl	60(%ebp), %eax
	movl	%eax, __registers + 20 /* edi */

	/*  This configures CS properly for real mode. */
	ljmp 	$0x28, $RELOCATED(1f)
1:
	.code16 /* 16 bit code from here on... */

	/* Load the segment registers w/ properly configured segment
	 * descriptors.  They will retain these configurations (limits,
	 * writability, etc.) once protected mode is turned off.
	 */
	mov	$0x30, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* Turn off protected mode */
	movl	%cr0, %eax
	andl	$~PE, %eax
	movl	%eax, %cr0

	/* Now really going into real mode */
	data32 ljmp	$0, $RELOCATED(1f)
1:

	/* put the stack at the end of page zero.  That way we can easily 
	 * share it between real mode and protected mode, because %esp and
	 * %ss:%sp point to the same memory.
	 */
	/* setup a stack */
	mov	$0x0, %ax
	mov	%ax, %ss
	movl	$0x1000, %eax
	movl	%eax, %esp

	/* Load 16-bit intXX IDT */
	xor	%ax, %ax
	mov	%ax, %ds
	lidt	__realmode_idt

	/* initialize registers for intXX call */
	movl	__registers +  0, %eax
	movl	__registers +  4, %ebx
	movl	__registers +  8, %ecx
	movl	__registers + 12, %edx
	movl	__registers + 16, %esi
	movl	__registers + 20, %edi

	/* Set all segments to 0x0000 */
	push	%ax
	xor	%ax, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	pop	%ax

__intXX_instr = RELOCATED(.)
	.byte 0xcd, 0x00 /* This becomes intXX */

	/* Ok, the job is done, now go back to protected mode coreboot */
	movl	%cr0, %eax
	orl	$PE, %eax
	movl	%eax, %cr0

	/* Now that we are in protected mode jump to a 32-bit code segment. */
	data32	ljmp	$0x10, $RELOCATED(1f)
1:
	.code32
	mov	$0x18, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* restore coreboot's 32-bit IDT */
	lidt	idtarg

	/* Exit */
	movl	__stack, %esp
	popal
	ret

/* This is the 16-bit interrupt entry point called by the IDT stub code.
 *
 * Before this code code is called, %eax is pushed to the stack, and the
 * interrupt number is loaded into %al. On return this function cleans up
 * for its caller.
 */
	.code16
__interrupt_handler_16bit = RELOCATED(.)
	push	%ds
	push	%es
	push	%fs
	push	%gs

	/* Clean up the interrupt number. We could have done this in the stub,
	 * but it would have cost 2 more bytes per stub entry.
	 */
	andl	$0xff, %eax
	pushl	%eax		/* ... and make it the first parameter */

	/* Switch to protected mode */
	movl    %cr0, %eax
	orl	$PE, %eax
	movl	%eax, %cr0

	/* ... and jump to a 32 bit code segment. */
	data32 ljmp    $0x10, $RELOCATED(1f)
1:
	.code32
	mov	$0x18, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	lidt	idtarg

	/* Call the C interrupt handler */
	movl	$interrupt_handler, %eax
	call	*%eax

	/* Now return to real mode ... */
	ljmp	$0x28, $RELOCATED(1f)
1:
	.code16
	/* Load the segment registers with properly configured segment
	 * descriptors.  They will retain these configurations (limits,
	 * writability, etc.) once protected mode is turned off.
	 */
	mov	$0x30, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* Disable Protected Mode */
	movl	%cr0, %eax
	andl	$~PE, %eax
	movl	%eax, %cr0

	/* Now really going into real mode */
	ljmp $0,  $RELOCATED(1f)
1:
	/* Restore real-mode stack segment */
	mov	$0x0, %ax
	mov	%ax, %ss

	/* Restore 16 bit IDT */
	xor	%ax, %ax
	mov	%ax, %ds
	lidt	__realmode_idt

	/* Restore all registers, including those
	 * manipulated by the C handler
	 */
	popl	%eax
	pop	%gs
	pop	%fs
	pop	%es
	pop	%ds
	popal
	iret

	.globl __realmode_code_size
__realmode_code_size = (. - __realmode_code)

	.code32
