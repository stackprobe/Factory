#include "C:\Factory\Common\all.h"
#include "libs\GitResourceMask.h"

#define IGNORE_FILE "_gitignore"

static void CleanupGitDir(char *dir)
{
	autoList_t *paths;
	char *path;
	uint index;

	addCwd(dir);
	paths = ls(".");
	eraseParents(paths);

	foreach(paths, path, index)
	{
		if(
			!_stricmp(path, ".git") ||
			!_stricmp(path, ".gitattributes") ||
			!_stricmp(path, ".gitignore")
			)
			continue;

		errorCase(path[0] == '.'); // ? 新しい git のファイルかしら？

		recurRemovePath(path);
	}
	releaseDim(paths, 1);
	unaddCwd();
}

static void FoundIgnoreFile(autoList_t *files, char *ignFile)
{
	char *ignPtn;
	char *ignPtnYen;
	char *file;
	uint index;

	ignPtn = changeLocal(ignFile, "");
	ignPtnYen = addChar(strx(ignPtn), '\\');

	foreach(files, file, index)
	{
		if(
			!_stricmp(file, ignPtn) ||
			startsWithICase(file, ignPtnYen)
			)
		{
			cout("IGNORE: %s\n", file);

			memFree(file);
			setElement(files, index, 0);
		}
	}
	removeZero(files);

	memFree(ignPtn);
	memFree(ignPtnYen);
}
static void RemoveIgnoreFiles(autoList_t *files)
{
	char *file;
	uint index;

restart:
	foreach(files, file, index)
	{
		if(!_stricmp(getLocal(file), IGNORE_FILE))
		{
			FoundIgnoreFile(files, file);
			goto restart;
		}
	}
}

static void SolveEmptyDir(char *rootDir)
{
	autoList_t *dirs = lssDirs(rootDir);
	char *dir;
	uint index;

	foreach(dirs, dir, index)
	{
		if(!lsCount(dir)) // ? 空のDIR
		{
			char *dmyfile = combine(dir, "_this-folder-is-empty");

			cout("SED_dmyfile: %s\n", dmyfile);

			createFile(dmyfile);
			memFree(dmyfile);
		}
	}
	releaseDim(dirs, 1);
}

static void GitFactory(char *rDir, char *wDir, int allowOverwrite)
{
	autoList_t *files;
	char *file;
	uint index;

	errorCase(m_isEmpty(rDir));
	errorCase(m_isEmpty(wDir));

	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	errorCase(!existDir(rDir));

	if(existPath(wDir))
	{
		errorCase(!allowOverwrite);
		CleanupGitDir(wDir);
	}
	else
		createPath(wDir, 'D');

	antiSubversion = 1;
	files = lss(rDir);
	changeRoots(files, rDir, NULL);

	foreach(files, file, index)
		errorCase_m(startsWithICase(file, ".git"), ".gitで始まる何かがあります。");

	RemoveIgnoreFiles(files);
	sortJLinesICase(files);

	foreach(files, file, index)
	{
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);

		if(existDir(rFile))
		{
			createDir(wFile);
		}
		else
		{
			if(startsWithICase(file, "tmp\\"))
				goto skipfile;

			if(!_stricmp("obj", getExt(file)))
				goto skipfile;

			if(!_stricmp("exe", getExt(file)))
				goto skipfile;

			copyFile(rFile, wFile);
		}

	skipfile:
		memFree(rFile);
		memFree(wFile);
	}

	{
		char *dir = combine(wDir, "tmp");

		if(existDir(dir))
		{
			file = combine(dir, "1.tmp");
			createFile(file);
			memFree(file);
		}
		memFree(dir);
	}

	GitResourceMask(wDir);
	SolveEmptyDir(wDir);

	memFree(rDir);
	memFree(wDir);
	releaseDim(files, 1);
}

int main(int argc, char **argv)
{
	int allowOverwrite = 0;

readArgs:
	if(argIs("/OW"))
	{
		allowOverwrite = 1;
		goto readArgs;
	}

	if(hasArgs(2))
	{
		GitFactory(getArg(0), getArg(1), allowOverwrite);
		return;
	}
	if(hasArgs(1))
	{
		GitFactory("C:\\Factory", nextArg(), allowOverwrite);
		return;
	}
	GitFactory("C:\\Factory", "C:\\home\\GitHub\\Factory", 1);
}
