#include "C:\Factory\Common\all.h"
#include "C:\Factory\DevTools\libs\RandData.h"

static void DoTest_copyBlock(char *beforeData, uint destPos, uint srcPos, uint size, char *afterData)
{
	char *line = strx(beforeData);

	cout("line_1: %s\n", line);

	copyBlock(line + destPos, line + srcPos, size);

	cout("line_2: %s\n", line);
	cout("answer: %s\n", afterData);

	errorCase(strcmp(line, afterData));

	memFree(line);
}
static void Test_copyBlock(void)
{
	DoTest_copyBlock("AAABBBCCC", 0, 3, 6, "BBBCCCCCC");
	DoTest_copyBlock("AAABBBCCC", 3, 0, 6, "AAAAAABBB");
	DoTest_copyBlock("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbcccccccccccccccccccc", 0, 20, 40, "bbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccc");
	DoTest_copyBlock("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbcccccccccccccccccccc", 20, 0, 40, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbb");

	cout("OK\n");
}
static char *TestFunc_trimSequ(char *str)
{
	error(); // TODO
	return NULL;
}
static void DoTest_trimSequ(char *str, char *ext)
{
	uint strsz = strlen(str) + 1;
	uint extsz = strlen(ext) + 1;
	uint size;
	char *buff1;
	char *buff2;

	size = strsz + extsz;

	buff1 = memAlloc(size);
	buff2 = memAlloc(size);

	memcpy(buff1,         str, strsz);
	memcpy(buff1 + strsz, ext, extsz);

	str = TestFunc_trimSequ(str);

	memcpy(buff2,         str, strsz);
	memcpy(buff2 + strsz, ext, extsz);

	trimSequ(buff1, ' ');

	errorCase(strcmp(buff1, buff2));
	errorCase(strcmp(buff1 + strsz, buff2 + strsz));

	memFree(buff1);
	memFree(buff2);
}
static void Test_trimSequ(void)
{
	uint c;

	mt19937_init32((uint)time(NULL));

	MRLChrLow = 0x20;
	MRLChrLow = 0x21;

	for(c = 0; c < 1000; c++)
	{
		char *str = MakeRandLineRange(0, 1000);
		char *ext = MakeRandLineRange(0, 1000);

		DoTest_trimSequ(str, ext);

		memFree(str);
		memFree(ext);
	}
}
int main(int argc, char **argv)
{
	Test_copyBlock();
	Test_trimSequ();
}
