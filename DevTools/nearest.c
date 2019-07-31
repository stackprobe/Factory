#include "C:\Factory\Common\all.h"

static char *TargetExt;

static int IsMatchTargetExt(char *file)
{
	return !_stricmp(getExt(file), TargetExt);
}
static void FoundOpen(char *file)
{
	coExecute(getLocal(file));
}
static void FoundOpenNB(char *file)
{
	coExecute_x(xcout("START \"\" \"%s\"", getLocal(file)));
}

static int (*FuncIsMatch)(char *);
static void (*FoundAction)(char *);

static void Search(void)
{
	char *dir = getCwd();
	int found = 0;

	while(3 < strlen(dir)) // ? ! reached root-dirc
	{
		addCwd(dir);
		{
			autoList_t *files = lsFiles(".");
			char *file;
			uint index;

			foreach(files, file, index)
			{
				if(FuncIsMatch(file))
				{
					FoundAction(file);
					found = 1;
					break;
				}
			}
			releaseDim(files, 1);
		}
		unaddCwd();

		if(found)
			break;

		eraseLocal(dir); // ÅŒã‚Í "C:" ‚È‚Ç‚Q•¶Žš‚É‚È‚éB
	}
	memFree(dir);

	if(!found)
		cout("not found\n");
}
int main(int argc, char **argv)
{
	FuncIsMatch = (int (*)(char *))getZero_u;
	FoundAction = (void (*)(char *))noop_u;

readArgs:
	if(argIs("/E"))
	{
		FuncIsMatch = IsMatchTargetExt;
		TargetExt = nextArg();
		goto readArgs;
	}
	if(argIs("/O"))
	{
		FoundAction = FoundOpenNB;
		goto readArgs;
	}
	if(argIs("/OB"))
	{
		FoundAction = FoundOpen;
		goto readArgs;
	}

	Search();
}
