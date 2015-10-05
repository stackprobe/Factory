#include "C:\Factory\Common\all.h"

static autoList_t *FindPtns;
static autoList_t *DestPtns;
static int IgnoreCaseFlag;
static int KeepTimeStamp;

static void RepText(char *rFile, char *wFile)
{
	char *text = readText_b(rFile);
	char *findPtn;
	uint index;

	foreach(FindPtns, findPtn, index)
	{
		char *destPtn = getLine(DestPtns, index);

		text = replaceLine(text, findPtn, destPtn, IgnoreCaseFlag);
	}
	if(KeepTimeStamp && existFile(wFile))
	{
		uint64 cTm;
		uint64 aTm;
		uint64 uTm;

		getFileStamp(wFile, &cTm, &aTm, &uTm);
		writeOneLineNoRet_b(wFile, text);
		setFileStamp(wFile, cTm, aTm, uTm);
	}
	else
		writeOneLineNoRet_b(wFile, text);

	memFree(text);
}
int main(int argc, char **argv)
{
	FindPtns = newList();
	DestPtns = newList();

readArgs:
	if(argIs("/I")) // Ignore case
	{
		IgnoreCaseFlag = 1;
		goto readArgs;
	}
	if(argIs("/K")) // Keep time stamp
	{
		KeepTimeStamp = 1;
		goto readArgs;
	}
	if(argIs("/P")) // Pair
	{
		addElement(FindPtns, (uint)nextArg());
		addElement(DestPtns, (uint)nextArg());

		goto readArgs;
	}

	{
		char *rFile;
		char *wFile;

		rFile = nextArg();
		wFile = nextArg();

		errorCase(hasArgs(1)); // 不明なオプション

		RepText(rFile, wFile);
	}
}
