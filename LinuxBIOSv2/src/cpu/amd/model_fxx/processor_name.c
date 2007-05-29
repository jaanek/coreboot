/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 * Copyright (C) 2007 University of Mannheim
 * (Written by Sven Kapferer <skapfere@rumms.uni-mannheim.de> for Uni Ma.)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/*
 * This code sets the Processor Name String for AMD64 CPUs.
 *
 * Written by Stefan Reinauer <stepan@coresystems.de> using
 * the Revision Guide for AMD Athlon™ 64 and AMD Opteron™ Processors
 * Document ID 25759 Rev 3.59, April 2006 (Public Version).
 *
 * Updated by Sven Kapferer <skapfere@rumms.uni-mannheim.de> using
 * the Revision Guide for AMD NPT Family 0Fh Processors
 * Document ID 33610 Rev 3.00, October 2006 (Public Version).
 */

#include <console/console.h>
#include <string.h>
#include <cpu/x86/msr.h>

/* The maximum length of CPU names is 48 bytes, including the final NULL byte.
 * If you change these names your BIOS will _NOT_ pass the AMD validation and
 * your mainboard will not be posted on the AMD Recommended Motherboard Website
 */

#if K8_REV_F_SUPPORT == 0
static char *processor_names[]={
	/* 0x00 */ "AMD Engineering Sample",
	/* 0x01-0x03 */ NULL, NULL, NULL,
	/* 0x04 */ "AMD Athlon(tm) 64 Processor XX00+",
	/* 0x05 */ "AMD Athlon(tm) 64 X2 Dual Core Processor XX00+",
	/* 0x06-0x07 */ NULL, NULL,
	/* 0x08 */ "Mobile AMD Athlon(tm) 64 Processor XX00+",
	/* 0x09 */ "Mobile AMD Athlon(tm) 64 Processor XX00+",
	/* 0x0A */ "AMD Turion(tm) 64 Mobile Technology ML-XX",
	/* 0x0B */ "AMD Turion(tm) 64 Mobile Technology MT-XX",
	/* 0x0C */ "AMD Opteron(tm) Processor 1YY",
	/* 0x0D */ "AMD Opteron(tm) Processor 1YY",
	/* 0x0E */ "AMD Opteron(tm) Processor 1YY HE",
	/* 0x0F */ "AMD Opteron(tm) Processor 1YY EE",
	/* 0x10 */ "AMD Opteron(tm) Processor 2YY",
	/* 0x11 */ "AMD Opteron(tm) Processor 2YY",
	/* 0x12 */ "AMD Opteron(tm) Processor 2YY HE",
	/* 0x13 */ "AMD Opteron(tm) Processor 2YY EE",
	/* 0x14 */ "AMD Opteron(tm) Processor 8YY",
	/* 0x15 */ "AMD Opteron(tm) Processor 8YY",
	/* 0x16 */ "AMD Opteron(tm) Processor 8YY HE",
	/* 0x17 */ "AMD Opteron(tm) Processor 8YY EE",
	/* 0x18 */ "AMD Athlon(tm) 64 Processor VV00+",
	/* 0x19-0x1C */ NULL, NULL, NULL, NULL,
	/* 0x1D */ "Mobile AMD Athlon(tm) XP-M Processor XX00+",
	/* 0x1E */ "Mobile AMD Athlon(tm) XP-M Processor XX00+",
	/* 0x1F */ NULL,
	/* 0x20 */ "AMD Athlon(tm) XP Processor XX00+",
	/* 0x21 */ "Mobile AMD Sempron(tm) Processor TT00+",
	/* 0x22 */ "AMD Sempron(tm) Processor TT00+",
	/* 0x23 */ "Mobile AMD Sempron(tm) Processor TT00+",
	/* 0x24 */ "AMD Athlon(tm) 64 FX-ZZ Processor",
	/* 0x25 */ NULL,
	/* 0x26 */ "AMD Sempron(tm) Processor TT00+",
	/* 0x27-0x28 */ NULL, NULL, 
	/* 0x29 */ "Dual Core AMD Opteron(tm) Processor 1RR SE",
	/* 0x2A */ "Dual Core AMD Opteron(tm) Processor 2RR SE",
	/* 0x2B */ "Dual Core AMD Opteron(tm) Processor 8RR SE",
	/* 0x2C */ "Dual Core AMD Opteron(tm) Processor 1RR",
	/* 0x2D */ "Dual Core AMD Opteron(tm) Processor 1RR",
	/* 0x2E */ "Dual Core AMD Opteron(tm) Processor 1RR HE",
	/* 0x2F */ "Dual Core AMD Opteron(tm) Processor 1RR EE",
	/* 0x30 */ "Dual Core AMD Opteron(tm) Processor 2RR",
	/* 0x31 */ "Dual Core AMD Opteron(tm) Processor 2RR",
	/* 0x32 */ "Dual Core AMD Opteron(tm) Processor 2RR HE",
	/* 0x33 */ "Dual Core AMD Opteron(tm) Processor 2RR EE",
	/* 0x34 */ "Dual Core AMD Opteron(tm) Processor 8RR",
	/* 0x35 */ "Dual Core AMD Opteron(tm) Processor 8RR",
	/* 0x36 */ "Dual Core AMD Opteron(tm) Processor 8RR HE",
	/* 0x37 */ "Dual Core AMD Opteron(tm) Processor 8RR EE",
	/* 0x38 */ "Dual Core AMD Opteron(tm) Processor 1RR",
	/* 0x39 */ "Dual Core AMD Opteron(tm) Processor 2RR",
	/* 0x3A */ "Dual Core AMD Opteron(tm) Processor 8RR"
#define MAX_CPU_NUMBER 0x3A
};
#endif

/* wrmsr_amd() is from yhlu's changes to model_fxx_init.c */

static inline void wrmsr_amd(unsigned index, msr_t msr)
{
        __asm__ __volatile__ (
                "wrmsr"
                : /* No outputs */
                : "c" (index), "a" (msr.lo), "d" (msr.hi), "D" (0x9c5a203a)
                );
}

static inline unsigned int cpuid_eax(unsigned int op)
{
        unsigned int eax;

        __asm__("cpuid"
                : "=a" (eax)
                : "0" (op)
                : "ebx", "ecx", "edx");
        return eax;
}

static inline unsigned int cpuid_ebx(unsigned int op)
{
        unsigned int eax, ebx;

        __asm__("cpuid"
                : "=a" (eax), "=b" (ebx)
                : "0" (op)
                : "ecx", "edx" );
        return ebx;
}

static inline unsigned int cpuid_ecx(unsigned int op)
{
        unsigned int eax, ecx;

        __asm__("cpuid"
                : "=a" (eax), "=c" (ecx)
                : "0" (op)
                : "ebx", "edx" );
        return ecx;
}

static inline void strcpy(char *dst, char *src) 
{
	while (*src) *dst++ = *src++;
}


int init_processor_name(void)
{
	u32 EightBitBrandId;
	u32 BrandId;
	u32 BrandTableIndex;
	u32 NN;
	u32 ModelNumber=0;
	msr_t progmsr;
	int i;

	char *processor_name_string=NULL;
	char program_string[48];
	unsigned int *program_values = (unsigned int *)program_string;

#if K8_REV_F_SUPPORT == 0
	/* Find out which CPU brand it is */
	EightBitBrandId = cpuid_ebx(0x00000001) & 0xff;
	BrandId = cpuid_ebx(0x80000001) & 0xffff;

	if(!EightBitBrandId && !BrandId) {
		BrandTableIndex = 0;
		NN = 0xffffff;
	} else if(!EightBitBrandId) {
		BrandTableIndex = (BrandId >> 6) & 0x3f; // BrandId[11:6]
		NN = BrandId & 0x3f; // // BrandId[6:0]
	} else {
		BrandTableIndex = EightBitBrandId >> (5-2) & 0xfc; // { 0b, 8BitBrandId[7:5], 00b }
		NN = EightBitBrandId & 0x1f; // 8BitBrandId[4:0]
	}

	/* Look up the CPU brand in above table */
	if (BrandTableIndex <= MAX_CPU_NUMBER)
		processor_name_string = processor_names[BrandTableIndex];

	if (!processor_name_string)
		processor_name_string = "AMD Processor model unknown";
#endif

#if K8_REV_F_SUPPORT == 1
	u32 Socket;
	u32 CmpCap;
	u32 PwrLmt;

	BrandId = cpuid_ebx(0x80000001) & 0xffff;
	Socket = (cpuid_eax(0x80000001) & 0x00000030) >> 4; // 00b = S1g1, 01b = F (1207), 11b = AM2
	CmpCap = cpuid_ecx(0x80000008) & 0x03; // Number of CPU cores

	PwrLmt = ((BrandId >> 14) & 0x01) | ((BrandId >> 5) & 0x0e); // BrandId[8:6,14]
	BrandTableIndex = (BrandId >> 9) & 0x1f; // BrandId[13:9]
	NN = ((BrandId >> 15) & 0x01) | (BrandId & 0x3f); // BrandId[15,5:0]

	if (((BrandTableIndex == 0) && (PwrLmt == 0)) || (NN == 0)) {
		processor_name_string = "AMD Engineering Sample";
	} else {
		/* Use all fields to identify CPU */
		switch ((Socket << 16) | (CmpCap << 12) | (BrandTableIndex << 4)
			| PwrLmt) {
		/* Socket F */
		case 0x11016:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 22RR HE";
			break;
		case 0x1101a:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 22RR";
			break;
		case 0x1101c:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 22RR SE";
			break;
		case 0x11046:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 82RR HE";
			break;
		case 0x1104a:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 82RR";
			break;
		case 0x1104c:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 82RR SE";
			break;
		/* Socket AM2 */
		case 0x30044:
			processor_name_string =
			    "AMD Athlon(tm) 64 Processor TT00+";
			break;
		case 0x30048:
			processor_name_string =
			    "AMD Athlon(tm) 64 Processor TT00+";
			break;
		case 0x30064:
			processor_name_string =
			    "AMD Sempron(tm) Processor TT00+";
			break;
		case 0x30068:
			processor_name_string =
			    "AMD Sempron(tm) Processor TT00+";
			break;
		case 0x3101a:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 12RR";
			break;
		case 0x3101c:
			processor_name_string =
			    "Dual-Core AMD Opteron(tm) Processor 12RR SE";
			break;
		case 0x31042:
			processor_name_string =
			    "AMD Athlon(tm) 64 X2 Dual Core Processor TT00+";
			break;
		case 0x31046:
			processor_name_string =
			    "AMD Athlon(tm) 64 X2 Dual Core Processor TT00+";
			break;
		case 0x31048:
			processor_name_string =
			    "AMD Athlon(tm) 64 X2 Dual Core Processor TT00+";
			break;
		case 0x3105c:
			processor_name_string =
			    "AMD Athlon(tm) 64 FX-ZZ Dual Core Processor";
			break;
		/* Socket S1g1 */
		case 0x0102c:
			processor_name_string =
			    "AMD Turion(tm) 64 X2 Mobile Technology TL-YY";
			break;
		default:
			processor_name_string = "AMD Processor model unknown";
		}
	}
#endif

	memset(program_string, 0, 48);
	strcpy(program_string, processor_name_string);
	
	/* Now create a model number - See Table 4. Model Number Calculation
	 * in the Revision Guide. NOTE: #6, EE was changed to VV because 
	 * otherwise it clashes with the brand names.
	 */
	
	for (i=0; i<47; i++) { // 48 -1 
		if(program_string[i] == program_string[i+1]) {
			switch (program_string[i]) {
#if K8_REV_F_SUPPORT == 0
			case 'X': ModelNumber = 22+ NN; break;
			case 'Y': ModelNumber = 38 + (2*NN); break;
			case 'Z':
			case 'T': ModelNumber = 24 + NN; break;
			case 'R': ModelNumber = 45 + (5*NN); break;
			case 'V': ModelNumber =  9 + NN; break;
#endif

#if K8_REV_F_SUPPORT == 1
			case 'R': ModelNumber = NN - 1; break;
			case 'P': ModelNumber = 26 + NN; break;
			case 'T': ModelNumber = 15 + (CmpCap * 10) + NN; break;
			case 'Z': ModelNumber = 57 + NN; break;
			case 'Y': ModelNumber = 29 + NN; break;
#endif
			}
			
			if(ModelNumber && ModelNumber < 100) {
				// No idea what to do with RR=100. According
				// to the revision guide this is possible.
				// 
				// --> "AMD Opteron(tm) Processor 8100"?
				program_string[i]=(ModelNumber/10)+'0';
				program_string[i+1]=(ModelNumber%10)+'0';
				break;
			}
		}
	}
	
	printk_debug("CPU model %s\n", program_string);

	for (i=0; i<6; i++) {
		progmsr.lo = program_values[(2*i)+0];
		progmsr.hi = program_values[(2*i)+1];
		wrmsr_amd(0xc0010030+i, progmsr);
	}

	return 0;
}

