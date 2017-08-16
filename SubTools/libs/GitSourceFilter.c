#include "GitSourceFilter.h"

#define COMMAND_PREFIX "$""_git:" // Ç±Ç±ÇåüèoÇµÇ»Ç¢ÇÊÇ§Ç…ÅAä‘Ç… ""
#define COMMAND_SECRET "secret"
#define COMMAND_SECRET_BEGIN "secretBegin"
#define COMMAND_SECRET_END "secretEnd"
#define CHR_SECRET '/'

static void MaskLine(char *p, char *q)
{
#if 1
	for(; p < q; p++)
		if(' ' < *p)
			*p = CHR_SECRET;
#else // old
	while(p < q && *p <= ' ')
		p++;

	while(p < q && q[-1] <= ' ')
		q--;

	while(p < q)
		*p++ = CHR_SECRET;
#endif
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
		int onSecretLater = 0;

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
				MaskLine(line, p);
			}
			else if(!strcmp(command, COMMAND_SECRET_BEGIN))
			{
				onSecretLater = 1;
			}
			else if(!strcmp(command, COMMAND_SECRET_END))
			{
				onSecret = 0;
			}
			// Ç±Ç±Ç÷í«â¡...
			else
				error();

			memFree(command);
		}
		if(onSecret)
		{
			MaskLine(line, strchr(line, '\0'));
		}
		onSecret |= onSecretLater;
	}
	writeLines_cx(file, lines);

	errorCase_m(onSecret, "secret is not closed !");
}
void GitSourceFilter(char *rootDir)
{
	autoList_t *files = lssFiles(rootDir);
	char *file;
	uint index;

	LOGPOS();

	RemoveGitPaths(files);

	foreach(files, file, index)
	{
		char *ext = getExt(file);

		if(
			!_stricmp(ext, "c") ||
			!_stricmp(ext, "h") ||
			!_stricmp(ext, "cs") ||
			!_stricmp(ext, "cpp") ||
			!_stricmp(ext, "java")
			)
			FilterSourceFile(file);
	}
	releaseDim(files, 1);

	LOGPOS();
}
