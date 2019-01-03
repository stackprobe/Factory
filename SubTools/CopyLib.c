/*
	CopyLib.exe 入力DIR 出力DIR

		入力DIR, 出力DIR ... 存在するディレクトリであること。
*/

#include "C:\Factory\Common\all.h"

static autoList_t *ReadCommonAndAppSpecRanges(char *file)
{
	autoList_t *ranges = newList();

	error(); // TODO

	return ranges;
}
static void DoCopyLib(char *rDir, char *wDir, int testMode)
{
	autoList_t *rSubDirs = lssDirs(rDir);
	autoList_t *wSubDirs = lssDirs(wDir);
	autoList_t *rFiles = lssFiles(rDir);
	autoList_t *wFiles = lssFiles(wDir);
	autoList_t *owFiles;
	char *dir;
	char *file;
	uint index;

	changeRoots(rSubDirs, rDir, NULL);
	changeRoots(wSubDirs, wDir, NULL);
	changeRoots(rFiles, rDir, NULL);
	changeRoots(wFiles, wDir, NULL);

	releaseDim(mergeLinesICase(rSubDirs, wSubDirs), 1);
	reverseElements(wSubDirs); // ★★★ 削除は後ろから行うため、ここで逆転しておく。★★★

	owFiles = mergeLinesICase(rFiles, wFiles);

	foreach(rSubDirs, dir, index)
		if(!testMode)
			createPath_x(combine(rDir, dir), 'X');

	foreach(rFiles, file, index)
	{
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);

		cout("R %s\n", rFile);
		cout("> %s\n", wFile);

		if(!testMode)
			copyFile(rFile, wFile);

		memFree(rFile);
		memFree(wFile);
	}
	foreach(wFiles, file, index)
	{
		char *wFile = combine(wDir, file);

		cout("D %s\n", wFile);

		{
			autoList_t *ranges = ReadCommonAndAppSpecRanges(wFile);

			cout("Dr %u\n", getCount(ranges));

			errorCase_m(getCount(ranges), "アプリ固有コードを含むため削除出来ません。手動で削除してね。");

			releaseDim(ranges, 2);
		}

		if(!testMode)
			removeFile(wFile);

		memFree(wFile);
	}
	foreach(owFiles, file, index)
	{
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);
		autoList_t *rRanges;
		autoList_t *wRanges;

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);

		rRanges = ReadCommonAndAppSpecRanges(rFile);
		wRanges = ReadCommonAndAppSpecRanges(wFile);

		cout("<r %u\n", getCount(rRanges));
		cout(">r %u\n", getCount(wRanges));

		errorCase_m(getCount(rRanges) != getCount(wRanges), "アプリ固有コードの数が合わないため上書き出来ません。");

		{
			autoList_t *lines = newList();
			uint index;

			for(index = 0; index < getCount(rRanges); index++)
				addElements(lines, getList(index % 2 ? wRanges : rRanges, index));

			if(!testMode)
				writeLines(wFile, lines);

			releaseAutoList(lines);
		}

		memFree(rFile);
		memFree(wFile);
		releaseDim(rRanges, 2);
		releaseDim(wRanges, 2);
	}
	foreach(wSubDirs, dir, index)
		if(!testMode)
			removeDir(combine(wDir, dir));

	releaseDim(rSubDirs, 1);
	releaseDim(wSubDirs, 1);
	releaseDim(rFiles, 1);
	releaseDim(wFiles, 1);
	releaseDim(owFiles, 1);
}
static void CopyLib(char *rDir, char *wDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));
	errorCase(!_stricmp(rDir, wDir)); // ? 同じディレクトリ

	DoCopyLib(rDir, wDir, 1);
//	DoCopyLib(rDir, wDir, 0);

	memFree(rDir);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	CopyLib(getArg(0), getArg(1));
}
