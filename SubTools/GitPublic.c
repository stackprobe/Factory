#include "C:\Factory\Common\all.h"

#define MEM_ROOTDIR "C:\\huge\\GitMem"
#define PUB_ROOTDIR "C:\\huge\\GitPub"

static void NormalizeMemRepoDir(char *memRepoDir)
{
	autoList_t *paths = ls(memRepoDir);
	char *path;
	uint index;

	LOGPOS();

	foreach(paths, path, index)
	{
		char *wPath = strx(path);
		char *wLocalPath;

		wLocalPath = getLocal(wPath);

		if(*wLocalPath == '.') // GitMemory の IsGitPath() 対策 -- 通常は無いはずだけど、念の為
		{
			*wLocalPath = '_';
			wPath = toCreatableTildaPath(wPath, IMAX);

			cout("< %s\n", path);
			cout("> %s\n", wPath);

			movePath(path, wPath);
		}
		memFree(wPath);
	}
	releaseDim(paths, 1);

	LOGPOS();
}
static void MemoryToPublic(void)
{
	autoList_t *memRepoDirs = lsDirs(MEM_ROOTDIR);
	char *memRepoDir;
	uint memRepoDir_index;

	sortJLinesICase(memRepoDirs);

	foreach(memRepoDirs, memRepoDir, memRepoDir_index)
	{
		char *pubFile = combine(PUB_ROOTDIR, getLocal(memRepoDir));

		pubFile = toCreatablePath(pubFile, 99); // -> GitMemory.c

		cout("< %s\n", memRepoDir);
		cout("> %s\n", pubFile);

		coExecute_x(xcout("C:\\Factory\\Tools\\Cluster.exe /OAD /M \"%s\" \"%s\"", pubFile, memRepoDir));

		LOGPOS();
		memFree(pubFile);
		LOGPOS();
	}
	LOGPOS();
	releaseDim(memRepoDirs, 1);
	LOGPOS();
}
static void PublicToMemory(void)
{
	autoList_t *pubFiles = lsFiles(PUB_ROOTDIR);
	char *pubFile;
	uint pubFile_index;

	sortJLinesICase(pubFiles);

	foreach(pubFiles, pubFile, pubFile_index)
	{
		char *memRepoDir = combine(MEM_ROOTDIR, getLocal(pubFile));

		memRepoDir = toCreatablePath(memRepoDir, 99); // -> GitMemory.c

		cout("< %s\n", pubFile);
		cout("> %s\n", memRepoDir);

		coExecute_x(xcout("C:\\Factory\\Tools\\Cluster.exe /OAD /R \"%s\" \"%s\"", pubFile, memRepoDir));

		NormalizeMemRepoDir(memRepoDir);

		LOGPOS();
		memFree(memRepoDir);
		LOGPOS();
	}
	LOGPOS();
	releaseDim(pubFiles, 1);
	LOGPOS();
}
int main(int argc, char **argv)
{
	errorCase(!existDir(MEM_ROOTDIR));
	errorCase(!existDir(PUB_ROOTDIR));

	if(argIs("PUSH"))
	{
		MemoryToPublic();
	}
	else if(argIs("PULL"))
	{
		PublicToMemory();
	}
	else
		error_m("不明なコマンド引数");

	LOGPOS();
}
