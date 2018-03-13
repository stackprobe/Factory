#include "C:\Factory\Common\all.h"

static char *ToPrms(autoList_t *args)
{
	char *buff = strx("");
	char *arg;
	uint index;

	foreach(args, arg, index)
	{
		buff = addChar(buff, ' ');

		if(!*arg || strchr(arg, '\t') || strchr(arg, ' '))
		{
			buff = addChar(buff, '"');
			buff = addLine(buff, arg);
			buff = addChar(buff, '"');
		}
		else
			buff = addLine(buff, arg);
	}
	return buff;
}

static char *FP_Program;
static autoList_t *FP_Founds;

static void FindProgram_Collect(char *trgDir)
{
	char *batFile = xcout("%s\\%s.bat", trgDir, FP_Program);
	char *exeFile = xcout("%s\\%s.exe", trgDir, FP_Program);

	if(existFile(batFile))
	{
		addElement(FP_Founds, (uint)strx(batFile));
	}
	if(existFile(exeFile))
	{
		addElement(FP_Founds, (uint)strx(exeFile));
	}
	memFree(batFile);
	memFree(exeFile);
}
static void FindProgram_Main(char *trgDir)
{
	autoList_t *dirs = lsDirs(trgDir);
	char *dir;
	uint dir_index;

	FindProgram_Collect(trgDir);

	foreach(dirs, dir, dir_index)
	{
		char *lDir = getLocal(dir);

		if(lineExp("<1,,__09AZaz>", lDir))
		{
			char *subDir = xcout("%s\\%s", trgDir, lDir);

			FindProgram_Main(subDir);

			memFree(subDir);
		}
	}
	releaseDim(dirs, 1);
}
static uint GetProgramWeight(char *program)
{
	sint ret = 0;

	if(mbs_stristr(program, "Release"))
		ret += 100;

	if(mbs_stristr(program, "Debug"))
		ret -= 100;

	if(mbs_stristr(program, "bin"))
		ret += 100;

	if(mbs_stristr(program, "obj"))
		ret -= 100;

	return ret;
}
static sint FP_Comp(uint v1, uint v2)
{
	char *a = (char *)v1;
	char *b = (char *)v2;
	sint wa;
	sint wb;

	wa = GetProgramWeight(a);
	wb = GetProgramWeight(b);

	if(wa != wb)
		return wb - wa; // çÇÇ¢ï˚Ç™ëO

	return strcmp(a, b);
}
static char *FindProgram(char *program)
{
	FP_Program = program;
	FP_Founds = newList();

	FindProgram_Main(".");

	rapidSort(FP_Founds, FP_Comp);

	// debug print
	{
		char *found;
		uint found_index;

		foreach(FP_Founds, found, found_index)
		{
			cout("found: %s\n", found);
		}
	}

	errorCase(!getCount(FP_Founds));

	program = getLine(FP_Founds, 0);

	FP_Program = NULL;
	releaseAutoList(FP_Founds);
	FP_Founds = NULL;

	return program;
}
int main(int argc, char **argv)
{
	char *program = nextArg();
	char *prms;

	errorCase(!lineExp("<1,,__09AZaz>", program));

	{
		autoList_t *args = allArgs();

		prms = ToPrms(args);

		releaseAutoList(args);
	}

	program = FindProgram(program);

	LOGPOS();
	coExecute_x(xcout("%s%s", program, prms));
	LOGPOS();

	memFree(program);
	memFree(prms);
}
