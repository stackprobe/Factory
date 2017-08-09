#include "GitSourceFilter.h"

#define COMMAND_PREFIX "$""_git:" // ここを検出しないように、間に ""
#define COMMAND_SECRET "secret"
#define COMMAND_SECRET_BEGIN "secretBegin"
#define COMMAND_SECRET_END "secretEnd"
#define CHR_SECRET '*'

static char *FindUnspc(char *p)
{
	while(*p && *p <= ' ')
		p++;

	return p;
}
static FilterSourceFile(char *file)
{
	autoList_t *lines = readLines(file);
	char *line;
	uint index;
	int onSecret = 0;

	cout("* %s\n", file);

	foreach(lines, line, index)
	{
		char *p = strstr(line, COMMAND_PREFIX);

		if(p)
		{
			char *command = strx(p + strlen(COMMAND_PREFIX));
			char *q;

			ucTrim(command);
			q = strchr(command, ' ');

			if(q)
				*q = '\0';

			if(!strcmp(command, COMMAND_SECRET))
			{
				for(q = FindUnspc(line); q < p; q++)
					*q = CHR_SECRET;
			}
			else if(!strcmp(command, COMMAND_SECRET_BEGIN))
			{
				onSecret = 1;
			}
			else if(!strcmp(command, COMMAND_SECRET_END))
			{
				onSecret = 0;
			}
			// ここへ追加...
			else
				error();

			memFree(command);
		}
		if(onSecret)
		{
			for(p = FindUnspc(line); *p; p++)
				*p = CHR_SECRET;
		}
	}
	writeLines_cx(file, lines);
}
void GitSourceFilter(char *rootDir)
{
	autoList_t *files = lssFiles(rootDir);
	char *file;
	uint index;

	LOGPOS();

	foreach(files, file, index)
	{
		char *ext = getExt(file);

		if(
			!_stricmp(ext, "c") ||
			!_stricmp(ext, "h") ||
			!_stricmp(ext, "cs") ||
			!_stricmp(ext, "java")
			)
			FilterSourceFile(file);
	}
	releaseDim(files, 1);

	LOGPOS();
}
