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
static void Test_MergeSort(void)
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
static void MergeFileText_TestVer(char *file1, char *file2, char *destFile1, char *destFile2, char *destFileBoth)
{
	autoList_t *lines1 = readLines(file1);
	autoList_t *lines2 = readLines(file2);
	autoList_t *destLines1 = newList();
	autoList_t *destLines2 = newList();
	autoList_t *destLinesBoth = newList();

	rapidSortLines(lines1);
	rapidSortLines(lines2);

	mergeLines2(lines1, lines2, destLines1, destLinesBoth, destLines2);

	writeLines(destFile1, destLines1);
	writeLines(destFile2, destLines2);
	writeLines(destFileBoth, destLinesBoth);
	releaseDim(lines1, 1);
	releaseDim(lines2, 1);
	releaseAutoList(destLines1);
	releaseAutoList(destLines2);
	releaseAutoList(destLinesBoth);
}
static void DoTest2(int d1, int d2, int db, int s1, int s2, uint rowcnt1, uint rowcnt2, uint minlen, uint maxlen)
{
	char *file101 = makeTempPath("101");
	char *file102 = makeTempPath("102");
	char *file201 = makeTempPath("201");
	char *file202 = makeTempPath("202");
	char *file111 = makeTempPath("111");
	char *file112 = makeTempPath("112");
	char *file11B = makeTempPath("11B");
	char *file211 = makeTempPath("211");
	char *file212 = makeTempPath("212");
	char *file21B = makeTempPath("21B");

	cout("d1: %d\n", d1);
	cout("d2: %d\n", d2);
	cout("db: %d\n", db);
	cout("s1: %d\n", s1);
	cout("s2: %d\n", s2);
	cout("rowcnt1: %u\n", rowcnt1);
	cout("rowcnt2: %u\n", rowcnt2);
	cout("minlen: %u\n", minlen);
	cout("maxlen: %u\n", maxlen);

	MakeRandTextFileRange(file101, (uint64)rowcnt1, minlen, maxlen);
	MakeRandTextFileRange(file102, (uint64)rowcnt2, minlen, maxlen);
	copyFile(file101, file201);
	copyFile(file102, file202);

	if(s1)
		SortTextFile(file101);

	if(s2)
		SortTextFile(file102);

	MergeFileText(
			file101,
			s1,
			file102,
			s2,
			d1 ? file111 : NULL,
			d2 ? file112 : NULL,
			db ? file11B : NULL,
			128000000
			);
LOGPOS();
	MergeFileText_TestVer(
			file201,
			file202,
			file211,
			file212,
			file21B
			);
LOGPOS();

	errorCase(d1 && !isSameFile(file111, file211));
LOGPOS();
	errorCase(d2 && !isSameFile(file112, file212));
LOGPOS();
	errorCase(db && !isSameFile(file11B, file21B));
LOGPOS();

	removeFile(file101);
LOGPOS();
	removeFile(file102);
LOGPOS();
	removeFile(file201);
LOGPOS();
	removeFile(file202);
LOGPOS();
	if(d1) removeFile(file111);
LOGPOS();
	if(d2) removeFile(file112);
LOGPOS();
	if(db) removeFile(file11B);
LOGPOS();
	removeFile(file211);
LOGPOS();
	removeFile(file212);
LOGPOS();
	removeFile(file21B);
LOGPOS();

	memFree(file101);
LOGPOS();
	memFree(file102);
LOGPOS();
	memFree(file201);
LOGPOS();
	memFree(file202);
LOGPOS();
	memFree(file111);
LOGPOS();
	memFree(file112);
LOGPOS();
	memFree(file11B);
LOGPOS();
	memFree(file211);
LOGPOS();
	memFree(file212);
LOGPOS();
	memFree(file21B);
LOGPOS();
}
static void Test_MergeFile(void)
{
	uint d1;
	uint d2;
	uint db;
	uint s1;
	uint s2;
	uint c;

	for(d1 = 0; d1 <= 1; d1++)
	for(d2 = 0; d2 <= 1; d2++)
	for(db = 0; db <= 1; db++)
	for(s1 = 0; s1 <= 1; s1++)
	for(s2 = 0; s2 <= 1; s2++)
	for(c = 0; c < 100; c++)
	{
		DoTest2(d1, d2, db, s1, s2, 100, 100, 0, 100);
	}
	for(c = 0; c < 100; c++)
	{
		DoTest2(1, 1, 1, 0, 0, mt19937_rnd(100), mt19937_rnd(100), 0, 100);
		DoTest2(1, 1, 1, 0, 0, mt19937_rnd(100), mt19937_rnd(100), 0, 2);
		DoTest2(1, 1, 1, 0, 0, mt19937_rnd(100), mt19937_rnd(100), 1, 1);
	}
}
int main(int argc, char **argv)
{
	mt19937_init();

//	Test_MergeSort();
	Test_MergeFile();
}
