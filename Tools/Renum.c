/*
	Renum.exe [/N] 開始番号 ステップ [対象DIR]
*/

#include "C:\Factory\Common\all.h"

static int ToNumOnly;

static uint StartNum;
static uint NumStep;

static char *IN_NamePtr;

static int IsNumbered(char *file)
{
	char *p;

	for(p = file; *p; p++)
	{
		if(*p == '.' || *p == '_')
			break;

		if(!m_isdecimal(*p))
			return 0;
	}
	IN_NamePtr = p;
	return 1;
}
static void DoFRenum(void)
{
	autoList_t *files = lsFiles(".");
	autoList_t *midFiles = newList();
	autoList_t *destFiles = newList();
	char *file;
	uint index;
	uint no = StartNum;

	eraseParents(files);
	sortJLinesICase(files);

	foreach(files, file, index)
	{
		char *dest;

		if(ToNumOnly)
			dest = xcout("%04u%s", no, getExtWithDot(file));
		else if(IsNumbered(file))
			dest = xcout("%04u%s", no, IN_NamePtr);
		else
			dest = xcout("%04u_%s", no, file);

		if(strcmp(file, dest)) // ? file != dest
		{
			cout("< %s\n", file);
			cout("> %s\n", dest);
		}
		addElement(midFiles, (uint)xcout("_$$$_%s", dest));
		addElement(destFiles, (uint)dest);

		no += NumStep;
	}
	cout("Press R to renumber\n");

	if(getKey() != 'R')
		termination(0);

	LOGPOS();

	for(index = 0; index < getCount(files); index++)
		moveFile(
			getLine(files, index),
			getLine(midFiles, index)
			);

	for(index = 0; index < getCount(files); index++)
		moveFile(
			getLine(midFiles, index),
			getLine(destFiles, index)
			);

	cout("Renumbered!\n");

	releaseDim(files, 1);
	releaseDim(midFiles, 1);
	releaseDim(destFiles, 1);
}
int main(int argc, char **argv)
{
	ToNumOnly = argIs("/N");

	StartNum = toValue(nextArg());
	NumStep  = toValue(nextArg());

	errorCase(!NumStep);

	addCwd(hasArgs(1) ? nextArg() : c_dropDir());
	DoFRenum();
	unaddCwd();
}
