#include "C:\Factory\Common\all.h"

#define GIT_ROOTDIR "C:\\huge\\GitHub"
#define MEM_ROOTDIR "C:\\huge\\GitMem"

static uint EndCode = 0;

static int IsGitPath(char *path)
{
	char *localPath = getLocal(path);

	return localPath[0] == '.'; // '.' で始まっているファイルは全てgitのファイルと見なす。
}
static void MemoryMain(void)
{
	autoList_t *repoDirs = lsDirs(GIT_ROOTDIR);
	char *repoDir;
	uint repoDir_index;
	char *memDir = combine_cx(MEM_ROOTDIR, addLine(makeCompactStamp(NULL), "00"));

	sortJLinesICase(repoDirs);

	memDir = toCreatablePath(memDir, 99); // 秒間 100 件超えは想定しない。
	cout("memDir: %s\n", memDir);
	createDir(memDir);

	foreach(repoDirs, repoDir, repoDir_index)
	{
		char *memRepoDir = combine(memDir, getLocal(repoDir));
		autoList_t *paths;
		char *path;
		uint index;

		cout("< %s\n", repoDir);
		cout("> %s\n", memRepoDir);
		createDir(memRepoDir);

		paths = ls(repoDir);
		sortJLinesICase(paths);

		foreach(paths, path, index)
		{
			char *wPath = combine(memRepoDir, getLocal(path));

			cout("<# %s\n", path);
			cout("># %s\n", wPath);

			if(IsGitPath(path))
			{
				cout("ignore git file!\n");
			}
			else if(existDir(path))
			{
				createDir(wPath);
				copyDir(path, wPath);
			}
			else
			{
				copyFile(path, wPath);
			}
			LOGPOS();
			memFree(wPath);
			LOGPOS();
		}
		LOGPOS();
		releaseDim(paths, 1);
		memFree(memRepoDir);
		LOGPOS();
	}
	LOGPOS();
	releaseDim(repoDirs, 1);
	memFree(memDir);
	LOGPOS();
}
static void FlushMain(void)
{
	autoList_t *memDirs = lsDirs(MEM_ROOTDIR);

	if(getCount(memDirs) == 0)
	{
		cout("no more memory!\n");
		EndCode = 1;
	}
	else
	{
		char *memDir;
		autoList_t *memRepoDirs;
		char *memRepoDir;
		uint memRepoDir_index;

		sortJLinesICase(memDirs);
		memDir = getLine(memDirs, 0); // 辞書順で最初のフォルダを処理する。
		cout("memDir: %s\n", memDir);

		memRepoDirs = lsDirs(memDir);
		sortJLinesICase(memRepoDirs);

		foreach(memRepoDirs, memRepoDir, memRepoDir_index)
		{
			char *repoDir = combine(GIT_ROOTDIR, getLocal(memRepoDir));
			autoList_t *paths;
			char *path;
			uint index;

			cout("< %s\n", memRepoDir);
			cout("> %s\n", repoDir);

			errorCase(!existDir(repoDir)); // fixme: リポジトリを削除することは無いという前提、、、

			paths = ls(repoDir);
			sortJLinesICase(paths);

			foreach(paths, path, index)
			{
				cout("! %s\n", path);

				if(IsGitPath(path))
				{
					cout("ignore git file!\n");
				}
				else
				{
					recurRemovePath(path);
				}
				LOGPOS();
			}
			LOGPOS();
			releaseDim(paths, 1);
			LOGPOS();

			paths = ls(memRepoDir);
			sortJLinesICase(paths);

			foreach(paths, path, index)
			{
				char *wPath = combine(repoDir, getLocal(path));

				cout("<# %s\n", path);
				cout("># %s\n", wPath);

				errorCase(IsGitPath(path)); // あるはずない。

				if(existDir(path))
				{
					createDir(wPath);
					copyDir(path, wPath);
				}
				else
				{
					copyFile(path, wPath);
				}
				LOGPOS();
				memFree(wPath);
				LOGPOS();
			}
			LOGPOS();
			releaseDim(paths, 1);
			memFree(repoDir);
			LOGPOS();
		}
		LOGPOS();
		releaseDim(memRepoDirs, 1);
		LOGPOS();

		semiRemovePath(memDir);
		LOGPOS();
	}
	LOGPOS();
	releaseDim(memDirs, 1);
	LOGPOS();
}
int main(int argc, char **argv)
{
	errorCase(!existDir(GIT_ROOTDIR));
	errorCase(!existDir(MEM_ROOTDIR));

	if(argIs("MEMORY"))
	{
		MemoryMain();
	}
	else if(argIs("FLUSH"))
	{
		FlushMain();
	}
	else
		error_m("不明なコマンド引数");

	cout("EndCode: %u\n", EndCode);
	termination(EndCode);
}
