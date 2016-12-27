#include "C:\Factory\Common\all.h"

#define S_ESCAPE "\1"

static char *MakeLine(char *format, char *path)
{
	char *ret = strx(format);

	ret = replaceLine(ret, "$$", S_ESCAPE, 0);

	// ----

	ret = replaceLine(ret, "$A", "&", 1);
	ret = replaceLine(ret, "$D", "\"", 1);
	ret = replaceLine(ret, "$P", "%", 1);
	ret = replaceLine(ret, "$Q", "'", 1);
	ret = replaceLine(ret, "$S", " ", 1);
	ret = replaceLine(ret, "$T", "^", 1);
	ret = replaceLine(ret, "$Y", "\\", 1);
	ret = replaceLine(ret, "$*", path, 1); // ç≈å„Ç…ÅI

	// ----

	ret = replaceLine(ret, S_ESCAPE, "$", 0);

	return ret;
}
static void MkLnCout(char *format)
{
	autoList_t *paths = readLines(FOUNDLISTFILE);
	char *path;
	uint index;

	foreach(paths, path, index)
	{
		char *line = MakeLine(format, path);

		cout("%s\n", line);
		memFree(line);
	}
	releaseDim(paths, 1);
}
static void Main2(void)
{
	MkLnCout(untokenize(allArgs(), " ")); // gomi, allArgs() ÇÃóvëfÇÕ malloc ÇµÇƒÇ¢Ç»Ç¢ÅB
}
int main(int argc, char **argv)
{
	Main2();
	termination(0);
}
