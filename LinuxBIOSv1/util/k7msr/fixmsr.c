#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct msrval {
  unsigned long msr, lo, hi;
};

// the pattern is: 
// 1x128 2x128, 0x1x256, 0x2x256, 128+256, 0x256+128
char *fixit[] = {"1x128", "2x128", "1x256", "2x256", 
	"128+256", "256+128"};
struct msrval fixmsr[6][40] = {
  {
0xc0000080, 0x00000000, 0x00000000,
0xc0000081, 0x00000000, 0x00000000,
0xc0010000, 0xff8d7df7, 0x00000000,
0xc0010001, 0xff9fee9f, 0x00000000,
0xc0010002, 0xff1fffbe, 0x00000000,
0xc0010003, 0xff9fff9f, 0x00000000,
0xc0010004, 0xfffeefff, 0x0000ffee,
0xc0010005, 0xffbffdf7, 0x0000ffff,
0xc0010006, 0x7ffffff7, 0x0000ff7d,
0xc0010007, 0xcffbfeff, 0x0000fffb,
0xc0010010, 0x00160604, 0x00000000,
0xc0010015, 0x0e001008, 0x00000000,
0xc0010016, 0x00000000, 0x00000000,
0xc0010017, 0x00000000, 0x00000000,
0xc0010018, 0xd0000018, 0x00000000,
0xc0010019, 0xfc000800, 0x0000000f,
0xc001001a, 0x07800000, 0x00000000,
0xc001001b, 0x29679223, 0x00000000,
0xc001001d, 0x00000000, 0x00000000,
0xc0010030, 0x20444d41, 0x6c687441,
0xc0010031, 0x74286e6f, 0x5020296d,
0xc0010032, 0x65636f72, 0x726f7373,
0xc0010033, 0x00000000, 0x00000000,
0xc0010034, 0x00000000, 0x00000000,
0xc0010035, 0x00000000, 0x00000000,
0xc0010100, 0xfc000000, 0x00000000,
0xc0010101, 0x00000000, 0x00000000,
0xc0010102, 0xf8000000, 0x00000000,
0xc0010103, 0xf8000000, 0x00000000,
0xc0010104, 0x00000011, 0x00000000,
0xc0010105, 0x00050002, 0x00000000,
0xc0010106, 0x00050002, 0x00000000,
0xc0010111, 0x000b0000, 0x00000000,
0xc0010112, 0x00000000, 0x00000000,
0xc0010113, 0x00000001, 0x00000000,
0xc0010114, 0xffffffff, 0x00000008,
0xc0010115, 0xffffffff, 0xffffffff,
0xc0010116, 0x00000027, 0x00000008,
0xc0010117, 0x00000027, 0x00000027,
  0,0,0},
	{
0xc0000080, 0x00000000, 0x00000000,
0xc0000081, 0x00000000, 0x00000000,
0xc0010000, 0x9f8dfdf7, 0x00000000,
0xc0010001, 0xff97fe9f, 0x00000000,
0xc0010002, 0xff1fffbf, 0x00000000,
0xc0010003, 0xff9fff9f, 0x00000000,
0xc0010004, 0xfffeefff, 0x0000ffee,
0xc0010005, 0xffbffdf7, 0x0000ffff,
0xc0010006, 0x7fffffff, 0x0000dfdc,
0xc0010007, 0xeffbfeff, 0x0000ffff,
0xc0010010, 0x00160604, 0x00000000,
0xc0010015, 0x0e001008, 0x00000000,
0xc0010016, 0x00000000, 0x00000000,
0xc0010017, 0x00000000, 0x00000000,
0xc0010018, 0xd0000018, 0x00000000,
0xc0010019, 0xfc000800, 0x0000000f,
0xc001001a, 0x0f800000, 0x00000000,
0xc001001b, 0x29679223, 0x00000000,
0xc001001d, 0x00000000, 0x00000000,
0xc0010030, 0x20444d41, 0x6c687441,
0xc0010031, 0x74286e6f, 0x5020296d,
0xc0010032, 0x65636f72, 0x726f7373,
0xc0010033, 0x00000000, 0x00000000,
0xc0010034, 0x00000000, 0x00000000,
0xc0010035, 0x00000000, 0x00000000,
0xc0010100, 0xfc000000, 0x00000000,
0xc0010101, 0x00000000, 0x00000000,
0xc0010102, 0xf8000000, 0x00000000,
0xc0010103, 0xf8000000, 0x00000000,
0xc0010104, 0x00000011, 0x00000000,
0xc0010105, 0x00050002, 0x00000000,
0xc0010106, 0x00050002, 0x00000000,
0xc0010111, 0x000b0000, 0x00000000,
0xc0010112, 0x00000000, 0x00000000,
0xc0010113, 0x00000001, 0x00000000,
0xc0010114, 0xffffffff, 0x00000008,
0xc0010115, 0xffffffff, 0xffffffff,
0xc0010116, 0x00000027, 0x00000008,
0xc0010117, 0x00000027, 0x00000027,
  0,0,0}, 
{
0xc0000080, 0x00000000, 0x00000000,
0xc0000081, 0x00000000, 0x00000000,
0xc0010000, 0x9f8dfcf7, 0x00000000,
0xc0010001, 0xff97ffdd, 0x00000000,
0xc0010002, 0xff1dffbf, 0x00000000,
0xc0010003, 0xff9fff9f, 0x00000000,
0xc0010004, 0xdffeefff, 0x0000ffee,
0xc0010005, 0xbfbefdf7, 0x0000ffff,
0xc0010006, 0xffffffff, 0x0000dfdc,
0xc0010007, 0xcffbfeff, 0x0000fffb,
0xc0010010, 0x00160604, 0x00000000,
0xc0010015, 0x0e001008, 0x00000000,
0xc0010016, 0x00000000, 0x00000000,
0xc0010017, 0x00000000, 0x00000000,
0xc0010018, 0xd0000018, 0x00000000,
0xc0010019, 0xfc000800, 0x0000000f,
0xc001001a, 0x0f800000, 0x00000000,
0xc001001b, 0x29679223, 0x00000000,
0xc001001d, 0x00000000, 0x00000000,
0xc0010030, 0x20444d41, 0x6c687441,
0xc0010031, 0x74286e6f, 0x5020296d,
0xc0010032, 0x65636f72, 0x726f7373,
0xc0010033, 0x00000000, 0x00000000,
0xc0010034, 0x00000000, 0x00000000,
0xc0010035, 0x00000000, 0x00000000,
0xc0010100, 0xfc000000, 0x00000000,
0xc0010101, 0x00000000, 0x00000000,
0xc0010102, 0xf8000000, 0x00000000,
0xc0010103, 0xf8000000, 0x00000000,
0xc0010104, 0x00000011, 0x00000000,
0xc0010105, 0x00050002, 0x00000000,
0xc0010106, 0x00050002, 0x00000000,
0xc0010111, 0x000b0000, 0x00000000,
0xc0010112, 0x00000000, 0x00000000,
0xc0010113, 0x00000001, 0x00000000,
0xc0010114, 0xffffffff, 0x00000008,
0xc0010115, 0xffffffff, 0xffffffff,
0xc0010116, 0x00000027, 0x00000008,
0xc0010117, 0x00000027, 0x00000027,
  0,0,0},
{
0xc0000080, 0x00000000, 0x00000000,
0xc0000081, 0x00000000, 0x00000000,
0xc0010000, 0xff8ffdf7, 0x00000000,
0xc0010001, 0xff9fee9f, 0x00000000,
0xc0010002, 0xff1fffbf, 0x00000000,
0xc0010003, 0xff9fff9f, 0x00000000,
0xc0010004, 0xfffeefff, 0x0000b7ee,
0xc0010005, 0xffbffdf7, 0x0000ffff,
0xc0010006, 0x7fffffff, 0x0000dfdd,
0xc0010007, 0xcffbffff, 0x0000efff,
0xc0010010, 0x00160604, 0x00000000,
0xc0010015, 0x0e001008, 0x00000000,
0xc0010016, 0x00000000, 0x00000000,
0xc0010017, 0x00000000, 0x00000000,
0xc0010018, 0xd0000018, 0x00000000,
0xc0010019, 0xfc000800, 0x0000000f,
0xc001001a, 0x1f800000, 0x00000000,
0xc001001b, 0x29679223, 0x00000000,
0xc001001d, 0x00000000, 0x00000000,
0xc0010030, 0x20444d41, 0x6c687441,
0xc0010031, 0x74286e6f, 0x5020296d,
0xc0010032, 0x65636f72, 0x726f7373,
0xc0010033, 0x00000000, 0x00000000,
0xc0010034, 0x00000000, 0x00000000,
0xc0010035, 0x00000000, 0x00000000,
0xc0010100, 0xfc000000, 0x00000000,
0xc0010101, 0x00000000, 0x00000000,
0xc0010102, 0xf8000000, 0x00000000,
0xc0010103, 0xf8000000, 0x00000000,
0xc0010104, 0x00000011, 0x00000000,
0xc0010105, 0x00050002, 0x00000000,
0xc0010106, 0x00050002, 0x00000000,
0xc0010111, 0x000b0000, 0x00000000,
0xc0010112, 0x00000000, 0x00000000,
0xc0010113, 0x00000001, 0x00000000,
0xc0010114, 0xffffffff, 0x00000008,
0xc0010115, 0xffffffff, 0xffffffff,
0xc0010116, 0x00000027, 0x00000008,
0xc0010117, 0x00000027, 0x00000027,
  0,0,0},
{
0xc0000080, 0x00000000, 0x00000000,
0xc0000081, 0x00000000, 0x00000000,
0xc0010000, 0x9f8dfdf7, 0x00000000,
0xc0010001, 0xff97fe9f, 0x00000000,
0xc0010002, 0xff1fffbf, 0x00000000,
0xc0010003, 0xff9fffdf, 0x00000000,
0xc0010004, 0xfffeefff, 0x0000ffee,
0xc0010005, 0xbfbffdf7, 0x0000ffff,
0xc0010006, 0x7fffffff, 0x0000dfdd,
0xc0010007, 0xcffbfeff, 0x0000fffb,
0xc0010010, 0x00160604, 0x00000000,
0xc0010015, 0x0e001008, 0x00000000,
0xc0010016, 0x00000000, 0x00000000,
0xc0010017, 0x00000000, 0x00000000,
0xc0010018, 0xd0000018, 0x00000000,
0xc0010019, 0xfc000800, 0x0000000f,
0xc001001a, 0x17800000, 0x00000000,
0xc001001b, 0x29679223, 0x00000000,
0xc001001d, 0x00000000, 0x00000000,
0xc0010030, 0x20444d41, 0x6c687441,
0xc0010031, 0x74286e6f, 0x5020296d,
0xc0010032, 0x65636f72, 0x726f7373,
0xc0010033, 0x00000000, 0x00000000,
0xc0010034, 0x00000000, 0x00000000,
0xc0010035, 0x00000000, 0x00000000,
0xc0010100, 0xfc000000, 0x00000000,
0xc0010101, 0x00000000, 0x00000000,
0xc0010102, 0xf8000000, 0x00000000,
0xc0010103, 0xf8000000, 0x00000000,
0xc0010104, 0x00000011, 0x00000000,
0xc0010105, 0x00050002, 0x00000000,
0xc0010106, 0x00050002, 0x00000000,
0xc0010111, 0x000b0000, 0x00000000,
0xc0010112, 0x00000000, 0x00000000,
0xc0010113, 0x00000001, 0x00000000,
0xc0010114, 0xffffffff, 0x00000008,
0xc0010115, 0xffffffff, 0xffffffff,
0xc0010116, 0x00000027, 0x00000008,
0xc0010117, 0x00000027, 0x00000027,
  0,0,0},
{
0xc0000080, 0x00000000, 0x00000000,
0xc0000081, 0x00000000, 0x00000000,
0xc0010000, 0x9f8dfdf7, 0x00000000,
0xc0010001, 0xff9fefdf, 0x00000000,
0xc0010002, 0xff1dffbe, 0x00000000,
0xc0010003, 0xff9fff9f, 0x00000000,
0xc0010004, 0xdffeefff, 0x0000ffff,
0xc0010005, 0xffbffdf7, 0x0000ffff,
0xc0010006, 0x7fffffff, 0x0000dfdc,
0xc0010007, 0xeffbfeff, 0x0000eff9,
0xc0010010, 0x00160604, 0x00000000,
0xc0010015, 0x0e001008, 0x00000000,
0xc0010016, 0x00000000, 0x00000000,
0xc0010017, 0x00000000, 0x00000000,
0xc0010018, 0xd0000018, 0x00000000,
0xc0010019, 0xfc000800, 0x0000000f,
0xc001001a, 0x17800000, 0x00000000,
0xc001001b, 0x29679223, 0x00000000,
0xc001001d, 0x00000000, 0x00000000,
0xc0010030, 0x20444d41, 0x6c687441,
0xc0010031, 0x74286e6f, 0x5020296d,
0xc0010032, 0x65636f72, 0x726f7373,
0xc0010033, 0x00000000, 0x00000000,
0xc0010034, 0x00000000, 0x00000000,
0xc0010035, 0x00000000, 0x00000000,
0xc0010100, 0xfc000000, 0x00000000,
0xc0010101, 0x00000000, 0x00000000,
0xc0010102, 0xf8000000, 0x00000000,
0xc0010103, 0xf8000000, 0x00000000,
0xc0010104, 0x00000011, 0x00000000,
0xc0010105, 0x00050002, 0x00000000,
0xc0010106, 0x00050002, 0x00000000,
0xc0010111, 0x000b0000, 0x00000000,
0xc0010112, 0x00000000, 0x00000000,
0xc0010113, 0x00000001, 0x00000000,
0xc0010114, 0xffffffff, 0x00000008,
0xc0010115, 0xffffffff, 0xffffffff,
0xc0010116, 0x00000027, 0x00000008,
0xc0010117, 0x00000027, 0x00000027,
0,0,0,
}};
int writemsr(struct msrval *v)
{
	char cpuname[16];
	unsigned long buffer[2];
	int fh;
	int cpu = 0;

	sprintf (cpuname, "/dev/cpu/%d/msr", cpu);

	fh = open (cpuname, O_RDWR);
	if (fh==-1)
		return (0);

	lseek (fh, v->msr, SEEK_CUR);

	buffer[0] = v->lo; buffer[1] = v->hi;
	if (fh != -1) {
	  printf("%#x: lo %#x, hi %#x\n", 	
				v->msr, v->lo, v->hi);
		if (write (fh, buffer, 8) != 8) {
			perror("Write failed");
			fprintf(stderr, "%#x: lo %#x, hi %#x\n", 	
				v->msr, v->lo, v->hi);
			close (fh);
			return (0);
		}

	}
	close(fh);
	fh = open (cpuname, O_RDWR);
	if (fh==-1)
		return (0);

	lseek (fh, v->msr, SEEK_CUR);
		if (read (fh, buffer, 8) != 8) {
			perror("Read failed");
			close (fh);
			return (0);
		}
	if (buffer[0] != v->lo)
		printf("set lo fails, is %#x want %#x\n", 
			buffer[0], v->lo);
	if (buffer[1] != v->hi)
		printf("set hi fails, is %#x want %#x\n", 
			buffer[1], v->hi);
	close (fh);
	return (1);
}

main(int argc, char *argv[])
{
  int i = 0, j;

  if (argc > 1)
	i = strtol(argv[1], 0, 0);
  printf("Fixing MSRs with offset %d(%s)\n", i, fixit[i]);

  for(j = 0; fixmsr[i][j].msr; j++)
	writemsr(&fixmsr[i][j]);
}
