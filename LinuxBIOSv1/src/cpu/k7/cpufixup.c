// Needed so the AMD K7 runs correctly. 
#include <printk.h>
#include <cpu/p6/msr.h>
#include <cpu/cpufixup.h>


#ifndef WE_KNOW_K7_IS_BROKEN
#SORRY but AMD doesnt want this released.
#endif
void
k7_cpufixup(unsigned long ram_kilobytes)
{
	printk_err("WARNING: you are running the busted version of linuxbios\n");
	printk_err("for K7. It will really run slow\n");
}

// see the explanation in freebios/util/k7msr for how this code was
// created. Short form: NO NDA material was used to create this code.
// Go sue somebody else.


struct msrval {
	unsigned long msr, lo, hi;
};

// the pattern is: 
// 1x128 2x128, 0x1x256, 0x2x256, 128+256, 0x256+128
char *fixit[] = {"1x128", "2x128", "1x256", "2x256", 
		 "128+256", "256+128"};
struct msrval fixmsr[6][40] = {
	{
		{0xc0000080, 0x00000000, 0x00000000},
		{0xc0000081, 0x00000000, 0x00000000},
		{0xc0010000, 0xff8d7df7, 0x00000000},
		{0xc0010001, 0xff9fee9f, 0x00000000},
		{0xc0010002, 0xff1fffbe, 0x00000000},
		{0xc0010003, 0xff9fff9f, 0x00000000},
		{0xc0010004, 0xfffeefff, 0x0000ffee},
		{0xc0010005, 0xffbffdf7, 0x0000ffff},
		{0xc0010006, 0x7ffffff7, 0x0000ff7d},
		{0xc0010007, 0xcffbfeff, 0x0000fffb},
		{0xc0010010, 0x00160604, 0x00000000},
		{0xc0010015, 0x0e001008, 0x00000000},
		{0xc0010016, 0x00000000, 0x00000000},
		{0xc0010017, 0x00000000, 0x00000000},
		{0xc0010018, 0xd0000018, 0x00000000},
		{0xc0010019, 0xfc000800, 0x0000000f},
		{0xc001001a, 0x07800000, 0x00000000},
		{0xc001001b, 0x29679223, 0x00000000},
		{0xc001001d, 0x00000000, 0x00000000},
		{0xc0010030, 0x20444d41, 0x6c687441},
		{0xc0010031, 0x74286e6f, 0x5020296d},
		{0xc0010032, 0x65636f72, 0x726f7373},
		{0xc0010033, 0x00000000, 0x00000000},
		{0xc0010034, 0x00000000, 0x00000000},
		{0xc0010035, 0x00000000, 0x00000000},
		{0xc0010100, 0xfc000000, 0x00000000},
		{0xc0010101, 0x00000000, 0x00000000},
		{0xc0010102, 0xf8000000, 0x00000000},
		{0xc0010103, 0xf8000000, 0x00000000},
		{0xc0010104, 0x00000011, 0x00000000},
		{0xc0010105, 0x00050002, 0x00000000},
		{0xc0010106, 0x00050002, 0x00000000},
		{0xc0010111, 0x000b0000, 0x00000000},
		{0xc0010112, 0x00000000, 0x00000000},
		{0xc0010113, 0x00000001, 0x00000000},
		{0xc0010114, 0xffffffff, 0x00000008},
		{0xc0010115, 0xffffffff, 0xffffffff},
		{0xc0010116, 0x00000027, 0x00000008},
		{0xc0010117, 0x00000027, 0x00000027},
		{0,0,0}},
	{
		{0xc0000080, 0x00000000, 0x00000000},
		{0xc0000081, 0x00000000, 0x00000000},
		{0xc0010000, 0x9f8dfdf7, 0x00000000},
		{0xc0010001, 0xff97fe9f, 0x00000000},
		{0xc0010002, 0xff1fffbf, 0x00000000},
		{0xc0010003, 0xff9fff9f, 0x00000000},
		{0xc0010004, 0xfffeefff, 0x0000ffee},
		{0xc0010005, 0xffbffdf7, 0x0000ffff},
		{0xc0010006, 0x7fffffff, 0x0000dfdc},
		{0xc0010007, 0xeffbfeff, 0x0000ffff},
		{0xc0010010, 0x00160604, 0x00000000},
		{0xc0010015, 0x0e001008, 0x00000000},
		{0xc0010016, 0x00000000, 0x00000000},
		{0xc0010017, 0x00000000, 0x00000000},
		{0xc0010018, 0xd0000018, 0x00000000},
		{0xc0010019, 0xfc000800, 0x0000000f},
		{0xc001001a, 0x0f800000, 0x00000000},
		{0xc001001b, 0x29679223, 0x00000000},
		{0xc001001d, 0x00000000, 0x00000000},
		{0xc0010030, 0x20444d41, 0x6c687441},
		{0xc0010031, 0x74286e6f, 0x5020296d},
		{0xc0010032, 0x65636f72, 0x726f7373},
		{0xc0010033, 0x00000000, 0x00000000},
		{0xc0010034, 0x00000000, 0x00000000},
		{0xc0010035, 0x00000000, 0x00000000},
		{0xc0010100, 0xfc000000, 0x00000000},
		{0xc0010101, 0x00000000, 0x00000000},
		{0xc0010102, 0xf8000000, 0x00000000},
		{0xc0010103, 0xf8000000, 0x00000000},
		{0xc0010104, 0x00000011, 0x00000000},
		{0xc0010105, 0x00050002, 0x00000000},
		{0xc0010106, 0x00050002, 0x00000000},
		{0xc0010111, 0x000b0000, 0x00000000},
		{0xc0010112, 0x00000000, 0x00000000},
		{0xc0010113, 0x00000001, 0x00000000},
		{0xc0010114, 0xffffffff, 0x00000008},
		{0xc0010115, 0xffffffff, 0xffffffff},
		{0xc0010116, 0x00000027, 0x00000008},
		{0xc0010117, 0x00000027, 0x00000027},
		{0,0,0}}, 
	{
		{0xc0000080, 0x00000000, 0x00000000},
		{0xc0000081, 0x00000000, 0x00000000},
		{0xc0010000, 0x9f8dfcf7, 0x00000000},
		{0xc0010001, 0xff97ffdd, 0x00000000},
		{0xc0010002, 0xff1dffbf, 0x00000000},
		{0xc0010003, 0xff9fff9f, 0x00000000},
		{0xc0010004, 0xdffeefff, 0x0000ffee},
		{0xc0010005, 0xbfbefdf7, 0x0000ffff},
		{0xc0010006, 0xffffffff, 0x0000dfdc},
		{0xc0010007, 0xcffbfeff, 0x0000fffb},
		{0xc0010010, 0x00160604, 0x00000000},
		{0xc0010015, 0x0e001008, 0x00000000},
		{0xc0010016, 0x00000000, 0x00000000},
		{0xc0010017, 0x00000000, 0x00000000},
		{0xc0010018, 0xd0000018, 0x00000000},
		{0xc0010019, 0xfc000800, 0x0000000f},
		{0xc001001a, 0x0f800000, 0x00000000},
		{0xc001001b, 0x29679223, 0x00000000},
		{0xc001001d, 0x00000000, 0x00000000},
		{0xc0010030, 0x20444d41, 0x6c687441},
		{0xc0010031, 0x74286e6f, 0x5020296d},
		{0xc0010032, 0x65636f72, 0x726f7373},
		{0xc0010033, 0x00000000, 0x00000000},
		{0xc0010034, 0x00000000, 0x00000000},
		{0xc0010035, 0x00000000, 0x00000000},
		{0xc0010100, 0xfc000000, 0x00000000},
		{0xc0010101, 0x00000000, 0x00000000},
		{0xc0010102, 0xf8000000, 0x00000000},
		{0xc0010103, 0xf8000000, 0x00000000},
		{0xc0010104, 0x00000011, 0x00000000},
		{0xc0010105, 0x00050002, 0x00000000},
		{0xc0010106, 0x00050002, 0x00000000},
		{0xc0010111, 0x000b0000, 0x00000000},
		{0xc0010112, 0x00000000, 0x00000000},
		{0xc0010113, 0x00000001, 0x00000000},
		{0xc0010114, 0xffffffff, 0x00000008},
		{0xc0010115, 0xffffffff, 0xffffffff},
		{0xc0010116, 0x00000027, 0x00000008},
		{0xc0010117, 0x00000027, 0x00000027},
		{0,0,0}},
	{
		{0xc0000080, 0x00000000, 0x00000000},
		{0xc0000081, 0x00000000, 0x00000000},
		{0xc0010000, 0xff8ffdf7, 0x00000000},
		{0xc0010001, 0xff9fee9f, 0x00000000},
		{0xc0010002, 0xff1fffbf, 0x00000000},
		{0xc0010003, 0xff9fff9f, 0x00000000},
		{0xc0010004, 0xfffeefff, 0x0000b7ee},
		{0xc0010005, 0xffbffdf7, 0x0000ffff},
		{0xc0010006, 0x7fffffff, 0x0000dfdd},
		{0xc0010007, 0xcffbffff, 0x0000efff},
		{0xc0010010, 0x00160604, 0x00000000},
		{0xc0010015, 0x0e001008, 0x00000000},
		{0xc0010016, 0x00000000, 0x00000000},
		{0xc0010017, 0x00000000, 0x00000000},
		{0xc0010018, 0xd0000018, 0x00000000},
		{0xc0010019, 0xfc000800, 0x0000000f},
		{0xc001001a, 0x1f800000, 0x00000000},
		{0xc001001b, 0x29679223, 0x00000000},
		{0xc001001d, 0x00000000, 0x00000000},
		{0xc0010030, 0x20444d41, 0x6c687441},
		{0xc0010031, 0x74286e6f, 0x5020296d},
		{0xc0010032, 0x65636f72, 0x726f7373},
		{0xc0010033, 0x00000000, 0x00000000},
		{0xc0010034, 0x00000000, 0x00000000},
		{0xc0010035, 0x00000000, 0x00000000},
		{0xc0010100, 0xfc000000, 0x00000000},
		{0xc0010101, 0x00000000, 0x00000000},
		{0xc0010102, 0xf8000000, 0x00000000},
		{0xc0010103, 0xf8000000, 0x00000000},
		{0xc0010104, 0x00000011, 0x00000000},
		{0xc0010105, 0x00050002, 0x00000000},
		{0xc0010106, 0x00050002, 0x00000000},
		{0xc0010111, 0x000b0000, 0x00000000},
		{0xc0010112, 0x00000000, 0x00000000},
		{0xc0010113, 0x00000001, 0x00000000},
		{0xc0010114, 0xffffffff, 0x00000008},
		{0xc0010115, 0xffffffff, 0xffffffff},
		{0xc0010116, 0x00000027, 0x00000008},
		{0xc0010117, 0x00000027, 0x00000027},
		{0,0,0}},
	{
		{0xc0000080, 0x00000000, 0x00000000},
		{0xc0000081, 0x00000000, 0x00000000},
		{0xc0010000, 0x9f8dfdf7, 0x00000000},
		{0xc0010001, 0xff97fe9f, 0x00000000},
		{0xc0010002, 0xff1fffbf, 0x00000000},
		{0xc0010003, 0xff9fffdf, 0x00000000},
		{0xc0010004, 0xfffeefff, 0x0000ffee},
		{0xc0010005, 0xbfbffdf7, 0x0000ffff},
		{0xc0010006, 0x7fffffff, 0x0000dfdd},
		{0xc0010007, 0xcffbfeff, 0x0000fffb},
		{0xc0010010, 0x00160604, 0x00000000},
		{0xc0010015, 0x0e001008, 0x00000000},
		{0xc0010016, 0x00000000, 0x00000000},
		{0xc0010017, 0x00000000, 0x00000000},
		{0xc0010018, 0xd0000018, 0x00000000},
		{0xc0010019, 0xfc000800, 0x0000000f},
		{0xc001001a, 0x17800000, 0x00000000},
		{0xc001001b, 0x29679223, 0x00000000},
		{0xc001001d, 0x00000000, 0x00000000},
		{0xc0010030, 0x20444d41, 0x6c687441},
		{0xc0010031, 0x74286e6f, 0x5020296d},
		{0xc0010032, 0x65636f72, 0x726f7373},
		{0xc0010033, 0x00000000, 0x00000000},
		{0xc0010034, 0x00000000, 0x00000000},
		{0xc0010035, 0x00000000, 0x00000000},
		{0xc0010100, 0xfc000000, 0x00000000},
		{0xc0010101, 0x00000000, 0x00000000},
		{0xc0010102, 0xf8000000, 0x00000000},
		{0xc0010103, 0xf8000000, 0x00000000},
		{0xc0010104, 0x00000011, 0x00000000},
		{0xc0010105, 0x00050002, 0x00000000},
		{0xc0010106, 0x00050002, 0x00000000},
		{0xc0010111, 0x000b0000, 0x00000000},
		{0xc0010112, 0x00000000, 0x00000000},
		{0xc0010113, 0x00000001, 0x00000000},
		{0xc0010114, 0xffffffff, 0x00000008},
		{0xc0010115, 0xffffffff, 0xffffffff},
		{0xc0010116, 0x00000027, 0x00000008},
		{0xc0010117, 0x00000027, 0x00000027},
		{0,0,0}},
	{
		{0xc0000080, 0x00000000, 0x00000000},
		{0xc0000081, 0x00000000, 0x00000000},
		{0xc0010000, 0x9f8dfdf7, 0x00000000},
		{0xc0010001, 0xff9fefdf, 0x00000000},
		{0xc0010002, 0xff1dffbe, 0x00000000},
		{0xc0010003, 0xff9fff9f, 0x00000000},
		{0xc0010004, 0xdffeefff, 0x0000ffff},
		{0xc0010005, 0xffbffdf7, 0x0000ffff},
		{0xc0010006, 0x7fffffff, 0x0000dfdc},
		{0xc0010007, 0xeffbfeff, 0x0000eff9},
		{0xc0010010, 0x00160604, 0x00000000},
		{0xc0010015, 0x0e001008, 0x00000000},
		{0xc0010016, 0x00000000, 0x00000000},
		{0xc0010017, 0x00000000, 0x00000000},
		{0xc0010018, 0xd0000018, 0x00000000},
		{0xc0010019, 0xfc000800, 0x0000000f},
		{0xc001001a, 0x17800000, 0x00000000},
		{0xc001001b, 0x29679223, 0x00000000},
		{0xc001001d, 0x00000000, 0x00000000},
		{0xc0010030, 0x20444d41, 0x6c687441},
		{0xc0010031, 0x74286e6f, 0x5020296d},
		{0xc0010032, 0x65636f72, 0x726f7373},
		{0xc0010033, 0x00000000, 0x00000000},
		{0xc0010034, 0x00000000, 0x00000000},
		{0xc0010035, 0x00000000, 0x00000000},
		{0xc0010100, 0xfc000000, 0x00000000},
		{0xc0010101, 0x00000000, 0x00000000},
		{0xc0010102, 0xf8000000, 0x00000000},
		{0xc0010103, 0xf8000000, 0x00000000},
		{0xc0010104, 0x00000011, 0x00000000},
		{0xc0010105, 0x00050002, 0x00000000},
		{0xc0010106, 0x00050002, 0x00000000},
		{0xc0010111, 0x000b0000, 0x00000000},
		{0xc0010112, 0x00000000, 0x00000000},
		{0xc0010113, 0x00000001, 0x00000000},
		{0xc0010114, 0xffffffff, 0x00000008},
		{0xc0010115, 0xffffffff, 0xffffffff},
		{0xc0010116, 0x00000027, 0x00000008},
		{0xc0010117, 0x00000027, 0x00000027},
		{0,0,0},
	}};
int writemsr(struct msrval *v)
{
	unsigned int lo, hi;
	printk_info("%#x: lo %#x, hi %#x\n", 	
		v->msr, v->lo, v->hi);
	wrmsr(v->msr, v->lo, v->hi);
	rdmsr(v->msr, lo, hi);

	if (lo != v->lo)
		printk_err("set lo fails, is %#x want %#x\n", 
			lo, v->lo);
	if (hi != v->hi)
		printk_err("set hi fails, is %#x want %#x\n", 
			hi, v->hi);
	return (1);
}


// take the 2 dimm sizes in Kbytes, and figure out which MSR
// values to patch.
void
fixk7msr_2dimms(unsigned long dimm1sizeM, unsigned long dimm2sizeM)
{
	int i, j;
	int offset;

// the pattern is:
// 1x128 2x128, 1x256, 0x2x256, 128+256, 0x256+128
	struct dimmindex {int dimm1sizeM, dimm2sizeM;};
	struct dimmindex dimms[] = {
		{128, 0},
		{128, 128},
		{256, 0},
		{256, 256},
		{128, 256},
		{256, 128}
	};

	for(i = 0, offset = -1; (offset < 0) && 
		    (i < sizeof(dimms)/sizeof(dimms[0])); i++)
		if ((dimm1sizeM == dimms[i].dimm1sizeM) && 
			(dimm2sizeM == dimms[i].dimm2sizeM))
			offset = i;
	
	if (offset == -1) {
		printk_err("%dM in slot 1 and %dM in slot 2:"
			" This is an unsupported setup\n",
			dimm1sizeM, dimm2sizeM);
		return;
	}

	printk_err("Fixing DIMMS with %s\n", fixit[offset]);
	for(j = 0; fixmsr[offset][j].msr; j++)
		writemsr(&fixmsr[offset][j]);
}





