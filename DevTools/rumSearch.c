/*
	デフォルトの検索対象 == カレントの配下

	Search.exe [/E SPEC-EXTS] [/I] [/T] [/R RUM-DIR]... FIND-PATTERN

		SPEC-EXTS ... 検索対象の拡張子を '.' 区切りで記述する。

			例) c.h.txt
*/

#include "C:\Factory\Common\all.h"

#define RUM_DIR_EXT "rum"
#define DIR_FILES "files"
#define DIR_REVISIONS "revisions"
#define FILE_FILES "files.txt"

static char *FindPattern;
static autoList_t *RumDirs;
static autoList_t *SpecExts;
static int IgnoreCase;
static int TokenOnlyMode;

static void SearchEntFile(char *entFile, char *file, char *revision, char *rumDir)
{
	FILE *fp;

	if(SpecExts)
	{
		char *fileExt = getExt(file);
		char *ext;
		uint index;

		foreach(SpecExts, ext, index)
			if(!_stricmp(ext, fileExt))
				break;

		if(!ext) // ? SpecExts に一致する拡張子が無い -> 検索対象外
			return;
	}
	fp = fileOpen(entFile, "rb");

	// TODO

	fileClose(fp);
}
static void SearchRumDir(char *rumDir)
{
	autoList_t *dirs;
	char *dir;
	uint index;

	addCwd(rumDir);

	dirs = lsDirs(DIR_REVISIONS);
	rapidSortLines(dirs);

	foreach(dirs, dir, index)
	{
		char *revision = getLocal(dir);
		autoList_t *lines;
		char *line;
		uint line_index;

		addCwd(dir);
		lines = readLines(FILE_FILES);
		unaddCwd();

		foreach(lines, line, line_index)
		{
			char *hash;
			char *file;
			char *entFile;

			errorCase(!lineExp("<32,09AFaf> <>", line));

			line[32] = '\0';
			hash = line;
			file = line + 33;
			entFile = combine(DIR_FILES, hash);

			SearchEntFile(entFile, file, revision, rumDir);

			memFree(entFile);
		}
		releaseDim(lines, 1);
	}
	releaseDim(dirs, 1);

	unaddCwd();
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/E"))
	{
		SpecExts = tokenize(nextArg(), '.');
		goto readArgs;
	}
	if(argIs("/I"))
	{
		IgnoreCase = 1;
		goto readArgs;
	}
	if(argIs("/T"))
	{
		TokenOnlyMode = 1;
		goto readArgs;
	}
	if(argIs("/R"))
	{
		char *dir = nextArg();

		if(!RumDirs)
			RumDirs = newList();

		dir = makeFullPath(dir);

		errorCase(!existDir(dir));
		errorCase(_stricmp(RUM_DIR_EXT, getExt(dir)));

		addElement(RumDirs, (uint)dir);
		goto readArgs;
	}

	FindPattern = nextArg();
	errorCase(!*FindPattern);

	if(!RumDirs)
	{
		autoList_t *dirs = lssDirs(".");
		char *dir;
		uint index;

		RumDirs = newList();

		foreach(dirs, dir, index)
			if(!_stricmp(RUM_DIR_EXT, getExt(dir)))
				addElement(dirs, (uint)strx(dir));

		releaseDim(dirs, 1);
	}

	sortJLinesICase(RumDirs);

	{
		char *dir;
		uint index;

		foreach(RumDirs, dir, index)
			SearchRumDir(dir);
	}
}
