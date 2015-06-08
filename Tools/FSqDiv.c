/*
	FSqDiv.exe ディレクトリ 分割数
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Progress.h"

static void FSqDiv(char *srcdir, uint divnum)
{
	char *destRootDir = makeFreeDir();
	autoList_t *destDirs = newList();
	autoList_t *files = lsFiles(srcdir);
	char *file;
	uint index;
	uint progCyc;

	for(index = 0; index < divnum; index++)
	{
		char *wDir = combine_cx(destRootDir, xcout("%05u", index + 1));

		createDir(wDir);
		addElement(destDirs, (uint)wDir);
	}

	ProgressBegin();
	progCyc = getCount(files) / 100;
	m_maxim(progCyc, 1);

	foreach(files ,file, index)
	{
		char *wDir = getLine(destDirs, index % getCount(destDirs));
		char *wFile;

		if(index % progCyc == 0)
			Progress();

		wFile = combine(wDir, getLocal(file));
		copyFile(file, wFile);
		memFree(wFile);
	}
	ProgressEnd(0);

	coExecute_x(xcout("START %s", destRootDir));

	memFree(destRootDir);
	releaseDim(destDirs, 1);
	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
	char *dir;
	uint divnum;

	dir = makeFullPath(nextArg());
	divnum = toValue(nextArg());

	errorCase(!existDir(dir));
	errorCase(!m_isRange(divnum, 2, 99999));

	FSqDiv(dir, divnum);
}
