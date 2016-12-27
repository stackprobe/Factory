#include "C:\Factory\Common\all.h"

#define S_ESCAPE "\1"

static void LssRun_Path(char *commandLine, char *path)
{
	commandLine = strx(commandLine);

	{
		char *tmp = replaceLine(commandLine, "$$", S_ESCAPE, 0);

		tmp = replaceLine(tmp, "$A", "&", 1);
		tmp = replaceLine(tmp, "$D", "\"", 1);
		tmp = replaceLine(tmp, "$P", "%", 1);
		tmp = replaceLine(tmp, "$Q", "'", 1);
		tmp = replaceLine(tmp, "$T", "^", 1);
		tmp = replaceLine(tmp, "$Y", "\\", 1);
		tmp = replaceLine(tmp, "$*", path, 1); // ç≈å„Ç…ÅI

		commandLine = replaceLine(tmp, S_ESCAPE, "$", 0);
	}

	coExecute_x(commandLine);
}
static void LssRun(char *commandLine)
{
	autoList_t *paths = readLines(FOUNDLISTFILE);
	char *path;
	uint index;

	foreach(paths, path, index)
	{
		LssRun_Path(commandLine, path);
	}
	releaseDim(paths, 1);
}
int main(int argc, char **argv)
{
	LssRun(untokenize(allArgs(), " ")); // gomi, allArgs() ÇÃóvëfÇÕ malloc ÇµÇƒÇ¢Ç»Ç¢ÅB
}
