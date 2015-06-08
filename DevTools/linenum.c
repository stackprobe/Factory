#include "C:\Factory\Common\all.h"

static void LineNumMain(char *file)
{
	FILE *fp = fileOpen(file, "rb");
	uint64 linenum = 0;

	for(; ; )
	{
		char *line = readLine(fp);

		if(!line)
			break;

		linenum++;
		memFree(line);
	}
	cout("%I64u\n", linenum);
	fileClose(fp);
}
int main(int argc, char **argv)
{
	LineNumMain(nextArg());
}
