#include "C:\Factory\Common\all.h"

static void CollectKeepTree(void)
{
	autoList_t *paths = readLines(FOUNDLISTFILE);
	char *path;
	uint index;
	char *outDir = makeFreeDir();

	foreach(paths, path, index)
	{
		char *outPath = combine_cx(outDir, lineToFairRelPath(path, strlen(outDir)));

		cout("< %s\n", path);
		cout("> %s\n", outPath);

		createPath(outPath, 'X');
		copyPath(path, outPath);
		memFree(outPath);
	}
	execute_x(xcout("START %s", outDir));
	memFree(outDir);
}
static void Collect(void)
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
int main(int argc, char **argv)
{
	if(argIs("/K"))
	{
		CollectKeepTree();
	}
	else
	{
		Collect();
	}
}
