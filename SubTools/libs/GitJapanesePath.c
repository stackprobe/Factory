#include "GitJapanesePath.h"

#define ESC_LPATH "[Japanese-path-name]_0001"
//#define ESC_LPATH "JP_0001"

static autoList_t *RBLines; // restore batch lines

static int HasJpn(char *str)
{
	char *p;

	for(p = str; *p; p++)
		if(_ismbblead(*p))
			return 1;

	return 0;
}
static char *PutDq(char *path)
{
	if(strchr(path, ' '))
	{
		path = insertChar(path, 0, '"');
		path = addChar(path, '"');
	}
	return path;
}
static void SolveJpnPath(char *rootDir, char *realRootDir)
{
	autoList_t *paths = ls(rootDir);
	char *path;
	uint index;

	RemoveGitPaths(paths);

	sortJLinesICase(paths);

	foreach(paths, path, index)
	{
		char *lPath = getLocal(path);

		if(HasJpn(lPath))
		{
			char *dest = changeExt_xc(changeLocal(path, ESC_LPATH), getExt(path));
			char *relDest;

			dest = toCreatablePath(dest, IMAX);
			relDest = changeRoot(strx(dest), realRootDir, NULL);

			movePath(path, dest);

			lPath = strx(lPath);
			lPath = PutDq(lPath);
			relDest = PutDq(relDest);

			addElement(RBLines, (uint)xcout("REN %s %s", relDest, lPath));

			memFree(lPath);
			memFree(dest);
			memFree(relDest);
		}
	}
	releaseDim(paths, 1);
	paths = lsDirs(rootDir);

	RemoveGitPaths(paths);

	sortJLinesICase(paths);

	foreach(paths, path, index)
	{
		SolveJpnPath(path, realRootDir);
	}
	releaseDim(paths, 1);
}
void SolveJapanesePath(char *rootDir)
{
	char *batFile = combine(rootDir, "_run_me_for_restore_japanese_path_name.bat_");

	LOGPOS();

	RBLines = newList();
	rootDir = makeFullPath(rootDir);

	SolveJpnPath(rootDir, rootDir);

	if(getCount(RBLines))
	{
		LOGPOS();
		reverseElements(RBLines);
		writeLines(batFile, RBLines);
	}
	releaseDim(RBLines, 1);
	memFree(rootDir);
	memFree(batFile);

	LOGPOS();
}
