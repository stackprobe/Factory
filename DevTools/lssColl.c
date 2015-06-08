#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	autoList_t *paths = readLines(FOUNDLISTFILE);
	char *path;
	uint index;
	char *outDir = makeFreeDir();

	foreach(paths, path, index)
	{
		char *outPath = toCreatablePath(combine(outDir, getLocal(path)), getCount(paths) + 10);

		cout("< %s\n", path);
		cout("> %s\n", outPath);

		copyPath(path, outPath);
		memFree(outPath);
	}
	execute_x(xcout("START %s", outDir));
	memFree(outDir);
}
