#include "C:\Factory\Common\all.h"

static char *RootDir = "C:\\home\\GitHub";
static uint SearchDeep = 1;
static char *User = "user01";
static char *Pass = "9999";
static char *InsertPtn;

#define BEFORE_TARGET "\turl = https://"
#define POST_TARGET_A "@github.com/"

static void FoundCfgFile(char *cfgFile)
{
	autoList_t *lines = readLines(cfgFile);
	char *line;
	uint index;
	int mod = 0;

	cout("C %s\n", cfgFile);

	foreach(lines, line, index)
	{
		if(startsWith(line, BEFORE_TARGET))
		{
			char *p = line + strlen(BEFORE_TARGET);
			char *q;

			q = strstr(p, POST_TARGET_A);

			if(q)
				q++;
			else
				q = strstr(p, POST_TARGET_A + 1);

			if(q)
			{
				if(!memcmp(p, InsertPtn, strlen(InsertPtn)) && p + strlen(InsertPtn) == q)
				{
					cout("no-mod\n");
				}
				else
				{
					q = strx(q);
					*p = '\0';
					p = xcout("%s%s%s", line, InsertPtn, q);

					memFree(line);
					setElement(lines, index, (uint)p);
					mod = 1;

					cout("* %s\n", getElement(lines, index));
				}
			}
		}
	}
	cout("mod: %d\n", mod);

	if(mod)
		writeLines_cx(cfgFile, lines);
	else
		releaseDim(lines, 1);
}
static void Search(char *currDir, uint remDeep)
{
	char *cfgFile = combine(currDir, ".git\\config");

	if(existFile(cfgFile))
	{
		FoundCfgFile(cfgFile);
	}
	else if(remDeep)
	{
		autoList_t *dirs = lsDirs(currDir);
		char *dir;
		uint index;

		foreach(dirs, dir, index)
			Search(dir, remDeep - 1);

		releaseDim(dirs, 1);
	}
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/R"))
	{
		RootDir = nextArg();
		goto readArgs;
	}
	if(argIs("/D"))
	{
		SearchDeep = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/U"))
	{
		User = nextArg();
		goto readArgs;
	}
	if(argIs("/P"))
	{
		Pass = nextArg();
		goto readArgs;
	}
	if(argIs("/C"))
	{
		User = "";
		Pass = "";
		goto readArgs;
	}

	errorCase(!existDir(RootDir));
	errorCase(IMAX < SearchDeep);
	errorCase((*User || *Pass) && (!*User || !*Pass));

	if(*User)
		InsertPtn = xcout("%s:%s@", User, Pass);
	else
		InsertPtn = "";

	Search(RootDir, SearchDeep);
}
