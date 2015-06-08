#include "C:\Factory\Common\all.h"

static void GetPaths(char *rootDir, autoList_t **p_dirs, autoList_t **p_files)
{
	autoList_t *dirs = lss(rootDir);
	autoList_t *files = newList();

	changeRoots(dirs, rootDir, NULL);

	while(lastDirCount < getCount(dirs))
		addElement(files, (uint)unaddElement(dirs));

	// for mergeLines2
	rapidSortLines(dirs);
	rapidSortLines(files);

	*p_dirs = dirs;
	*p_files = files;
}
static void DelDirs(char *rootDir, autoList_t *dirs)
{
	char *dir;
	uint index;

	// 削除 -> 降順
	sortJLinesICase(dirs);
	reverseElements(dirs);

	foreach(dirs, dir, index)
	{
		dir = combine(rootDir, dir);
		cout("DD: %s\n", dir);

		forceRemoveDir(dir);

		memFree(dir);
	}
}
static void MkDirs(char *rootDir, autoList_t *dirs)
{
	char *dir;
	uint index;

	// 作成 -> 昇順
	sortJLinesICase(dirs);

	foreach(dirs, dir, index)
	{
		dir = combine(rootDir, dir);
		cout("MD: %s\n", dir);

		createDir(dir);

		memFree(dir);
	}
}
static void DelFiles(char *rootDir, autoList_t *files)
{
	char *file;
	uint index;

	// 削除 -> 降順
	sortJLinesICase(files);
	reverseElements(files);

	foreach(files, file, index)
	{
		file = combine(rootDir, file);
		cout("DF: %s\n", file);

		removeFileIfExist(file);

		memFree(file);
	}
}
static void CpFiles(char *rRootDir, char *wRootDir, autoList_t *files)
{
	char *file;
	uint index;

	foreach(files, file, index)
	{
		char *rFile = combine(rRootDir, file);
		char *wFile = combine(wRootDir, file);

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);

		copyFile(rFile, wFile);

		memFree(rFile);
		memFree(wFile);
	}
}
static void EraseNoChangeFiles(char *rRootDir, char *wRootDir, autoList_t *files) // files の中身を「開放せずに」除去すること。
{
	char *file;
	uint index;

	foreach(files, file, index)
	{
		char *rFile = combine(rRootDir, file);
		char *wFile = combine(wRootDir, file);
		int chged = 0;

		cout("Cr: %s\n", rFile);
		cout("Cw: %s\n", wFile);

		{
			uint64 rTimeC;
			uint64 wTimeC;
			uint64 rTimeW;
			uint64 wTimeW;
			uint64 rTime;
			uint64 wTime;

			getFileStamp(rFile, &rTimeC, NULL, &rTimeW);
			getFileStamp(wFile, &wTimeC, NULL, &wTimeW);

			rTime = m_max(rTimeC, rTimeW);
			wTime = m_max(wTimeC, wTimeW);

			if(wTime < rTime)
				chged = 1;
		}

		if(!chged) // ? 変更ナシ
		{
			cout("☆変更ナシ☆\n");
			setElement(files, index, 0);
		}
		else
			cout("★変更アリ★\n");

		memFree(rFile);
		memFree(wFile);
	}
	removeZero(files);
}
static void SyncDir(char *rRootDir, char *wRootDir) // (rRootDir, wRootDir): absDir && exist
{
	autoList_t *rDirs;
	autoList_t *wDirs;
	autoList_t *rFiles;
	autoList_t *wFiles;

	GetPaths(rRootDir, &rDirs, &rFiles);
	GetPaths(wRootDir, &wDirs, &wFiles);

	// dir
	{
		autoList_t *newDirs = newList();
		autoList_t *noChangeDirs = newList();
		autoList_t *oldDirs = newList();

		mergeLines2(rDirs, wDirs, newDirs, noChangeDirs, oldDirs);

		DelDirs(wRootDir, oldDirs);
		MkDirs(wRootDir, newDirs);

		releaseAutoList(newDirs);
		releaseAutoList(noChangeDirs);
		releaseAutoList(oldDirs);
	}

	// file
	{
		autoList_t *newFiles = newList();
		autoList_t *noChangedFiles = newList();
		autoList_t *oldFiles = newList();

		mergeLines2(rFiles, wFiles, newFiles, noChangedFiles, oldFiles);
		EraseNoChangeFiles(rRootDir, wRootDir, noChangedFiles);

		DelFiles(wRootDir, oldFiles);
		CpFiles(rRootDir, wRootDir, noChangedFiles);
		CpFiles(rRootDir, wRootDir, newFiles);

		releaseAutoList(oldFiles);
		releaseAutoList(noChangedFiles);
		releaseAutoList(newFiles);
	}

	releaseDim(rDirs, 1);
	releaseDim(wDirs, 1);
	releaseDim(rFiles, 1);
	releaseDim(wFiles, 1);
}
int main(int argc, char **argv)
{
	char *rDir;
	char *wDir;

	rDir = makeFullPath(nextArg());
	wDir = makeFullPath(nextArg());

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);

	createDirIfNotExist(wDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));

	SyncDir(rDir, wDir);
}
