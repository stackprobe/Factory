#include "C:\Factory\Common\all.h"

static void DirSize(char *dir)
{
	autoList_t *paths = ls(dir);
	char *path;
	uint index;
	uint dircnt;
	uint64 size;
	uint64 totalSize = 0;
	autoList_t *lines = newList();
	char *line;
	char *lsize;
	char *lfile;

	dircnt = lastDirCount;

	foreach(paths, path, index)
	{
		cmdTitle_x(xcout("ds - %s (%u / %u)", path, index, getCount(paths)));

		size = ( index < dircnt ? getDirSize : getFileSize )(path);
		totalSize += size;

		addElement(lines, (uint)xcout("%020I64u%s", size, getLocal(path)));
	}
	cmdTitle("ds");
	rapidSortLines(lines);

	foreach(lines, line, index)
	{
		lsize = strxl(line, 20);
		lfile = strx(line + 20);

		while(lsize[0] == '0' && lsize[1])
			eraseChar(lsize);

		lsize = thousandComma(lsize);

		memFree(line);
		line = xcout("%s*%s", lfile, lsize);
		setElement(lines, index, (uint)line);

		memFree(lsize);
		memFree(lfile);
	}
	spacingStarLines(lines, 79);

	foreach(lines, line, index)
		cout("%s\n", line);

	line = xcout("%I64u", totalSize);
	line = thousandComma(line);

	cout("\n");
	cout("合計サイズ %68s\n", line);
	cout("\n");

	memFree(line);

	releaseDim(paths, 1);
	releaseDim(lines, 1);
}
int main(int argc, char **argv)
{
	if(argIs("/S")) // Simple mode
	{
		char *lsize = xcout("%I64u", getDirSize(nextArg()));

		lsize = thousandComma(lsize);
		cout("%s\n", lsize);
		memFree(lsize);
		return;
	}
	if(hasArgs(1))
	{
		DirSize(nextArg());
		cout("\\e\n");
		return;
	}
	for(; ; )
	{
		char *dir = dropDir();

		DirSize(dir);
		memFree(dir);
	}
}
