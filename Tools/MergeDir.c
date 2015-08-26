/*
	MergeDir.exe マスターDIR スレーブDIR

	----

	以下をスレーブからマスターに上書きコピーする。

		1. マスターとスレーブで内容が異なるファイル
		2. スレーブにしか無いファイル
*/

#include "C:\Factory\Common\all.h"

static char *RDir;
static char *WDir;
static autoList_t *RWFiles;

static void DoSearch(void)
{
	autoList_t *files = lssFiles(RDir);
	char *rFile;
	uint index;

	sortJLinesICase(files);

	foreach(files, rFile, index)
	{
		char *wFile = changeRoot(strx(rFile), RDir, WDir);

		if(!existFile(wFile) || !isSameFile(rFile, wFile)) // ? マージすべきファイル
		{
			cout("< %s\n", rFile);
			cout("> %s\n", wFile);

			addElement(RWFiles, (uint)strx(eraseRoot(rFile, RDir)));
		}
		memFree(wFile);
	}
	releaseDim(files, 1);
}
static void DoConfirm(void)
{
restart:
	if(!getCount(RWFiles))
	{
		cout("上書きコピーするファイルはありません。\n");
		termination(0);
	}

	{
		char *dir1 = getOutFile("s");
		char *dir2 = getOutFile("m");
		char *file;
		uint index;

		// 2周目用
		forceRemoveDirIfExist(dir1);
		forceRemoveDirIfExist(dir2);

		foreach(RWFiles, file, index)
		{
			char *rFile = combine(RDir, file);
			char *wFile = combine(WDir, file);
			char *file1 = combine(dir1, file);
			char *file2 = combine(dir2, file);

			createPath(file1, 'X');
			createPath(file2, 'X');

			copyFile(rFile, file1);

			if(existFile(wFile))
				copyFile(wFile, file2);

			memFree(rFile);
			memFree(wFile);
			memFree(file1);
			memFree(file2);
		}
		openOutDir();

		memFree(dir1);
		memFree(dir2);
	}

	{
		char *file;
		uint index;

		foreach(RWFiles, file, index)
			cout("* %s\n", file);
	}

	cout("上書きコピーするファイル数: %u\n", getCount(RWFiles));
	cout("------------\n");
	cout("ENTER = 続行\n");
	cout("SPACE = 絞り込み\n");
	cout("OTHER = 中止\n");
	cout("------------\n");

	switch(getKey())
	{
	case 0x20:
		RWFiles = selectLines_x(RWFiles);
		goto restart;

	case 0x0d:
		break;

	default:
		termination(0);
	}
}
static void DoMerge(void)
{
	char *file;
	uint index;

	foreach(RWFiles, file, index)
	{
		char *rFile = combine(RDir, file);
		char *wFile = combine(WDir, file);

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);

		if(existFile(wFile))
			semiRemovePath(wFile);
		else
			createPath(wFile, 'X');

		copyFile(rFile, wFile);
	}
}

static void MergeDir(char *masterDir, char *slaveDir)
{
	errorCase(m_isEmpty(masterDir));
	errorCase(m_isEmpty(slaveDir));

	masterDir = makeFullPath(masterDir);
	slaveDir = makeFullPath(slaveDir);

	errorCase(!existDir(masterDir));
	errorCase(!existDir(slaveDir));
	errorCase(!mbs_stricmp(masterDir, slaveDir)); // マスターとスレーブが同じDIR

	cout("[マスター] > %s\n", masterDir);
	cout("[スレーブ] < %s\n", slaveDir);
	cout("続行？\n");

	if(getKey() == 0x1b)
		termination(0);

	cout("続行します。\n");

	RDir = slaveDir;
	WDir = masterDir;
	RWFiles = newList();

	DoSearch();
	DoConfirm();
	DoMerge();
}
int main(int argc, char **argv)
{
	MergeDir(getArg(0), getArg(1));
}
