#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\MergeSort.h"
#include "C:\Factory\DevTools\libs\RandData.h"

static void SortTextFile(char *file)
{
	autoList_t *lines = readLines(file);

	rapidSortLines(lines);

	writeLines(file, lines);
	releaseDim(lines, 1);
}
static void DoTest(uint partSize, uint rowcnt, uint minlen, uint maxlen)
{
	char *file1;
	char *file2;

	cout("partSize: %u\n", partSize);
	cout("rowcnt: %u\n", rowcnt);
	cout("minlen: %u\n", minlen);
	cout("maxlen: %u\n", maxlen);

	file1 = makeTempPath("001");
	file2 = makeTempPath("002");

	MakeRandTextFileRange(file1, (uint64)rowcnt, minlen, maxlen);
	copyFile(file1, file2);

	MergeSortText(file1, file1, partSize);
	SortTextFile(file2);

	errorCase(!isSameFile(file1, file2));

	removeFile(file1);
	removeFile(file2);

	memFree(file1);
	memFree(file2);

	cout("OK!\n");
}
int main(int argc, char **argv)
{
	uint c;

	DoTest(0, 0, 0, 0);
	DoTest(1, 0, 0, 0);

	for(c = 0; c < 100; c++)
	{
		DoTest(mt19937_rnd(100), 1, 0, 100);
	}
	for(c = 0; c < 100; c++)
	{
		uint partSize = mt19937_rnd(500) + 1;

		DoTest(0,        mt19937_rnd(100), 0, 100);
		DoTest(partSize, mt19937_rnd(100), 0, 100);
		DoTest(10000,    mt19937_rnd(100), 0, 100);

		DoTest(0,        mt19937_rnd(100), 0, 2);
		DoTest(partSize, mt19937_rnd(100), 0, 2);
		DoTest(10000,    mt19937_rnd(100), 0, 2);

		DoTest(0,        mt19937_rnd(100), 1, 1);
		DoTest(partSize, mt19937_rnd(100), 1, 1);
		DoTest(10000,    mt19937_rnd(100), 1, 1);
	}

	for(c = 0; c < 100; c++)
	{
		DoTest(mt19937_rnd(128 * 1024), mt19937_rnd(1024), 0, 1024);
	}
}
