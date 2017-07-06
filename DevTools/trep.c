/*
	カレントディレクトリ配下の全てのファイルについて、テキストパタンの置き換えを行います。

	- - -

	trep.exe [/LSS] [/-C] [/F] [/I] [/-] src-ptn dest-ptn
*/

#include "C:\Factory\Common\all.h"

static char *SrcPtn;
static char *DestPtn;
static int LssMode;
static int NoCheckMode;
static int ForceMode;
static int IgnoreCase;

static int DTS_Replaced;

static void DoTrepStream(FILE *rfp, FILE *wfp)
{
	// TODO
}
static void DoTrepFile(char *file)
{
	char *midFile = makeTempPath(NULL);
	FILE *rfp;
	FILE *wfp;

	rfp = fileOpen(file, "rb");
	wfp = fileOpen(midFile, "wb");

	DTS_Replaced = FALSE;

	DoTrepStream(rfp, wfp);

	fileClose(rfp);
	fileClose(wfp);

	if(DTS_Replaced)
	{
		removeFile(file);
		moveFile(midFile, file);
	}
	else
		removeFile(midFile);

	memFree(midFile);
}
static void DoTrep(void)
{
	autoList_t *files;
	char *file;
	uint index;

	if(LssMode)
		files = readLines(FOUNDLISTFILE);
	else
		files = lssFiles(".");

	foreach(files, file, index)
	{
		cout("%s\n", file);

		DoTrepFile(file);
	}
	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/LSS"))
	{
		LssMode = 1;
		goto readArgs;
	}
	if(argIs("/-C"))
	{
		NoCheckMode = 1;
		goto readArgs;
	}
	if(argIs("/F"))
	{
		ForceMode = 1;
		goto readArgs;
	}
	if(argIs("/I"))
	{
		IgnoreCase = 1;
		goto readArgs;
	}
	argIs("/-");

	SrcPtn  = nextArg();
	DestPtn = nextArg();

	errorCase(m_isEmpty(SrcPtn));
	errorCase(m_isEmpty(DestPtn));

	if(!NoCheckMode)
	{
		char *prmFile = makeTempPath(NULL);

		error(); // TODO Search.c += "/-"

		writeOneLineNoRet_cx(prmFile, xcout(
			"%s/-\n%s"
			,IgnoreCase ? "/I\n" : ""
			,SrcPtn
			));

		execute("Search.exe //R \"%s\"");
		removeFile(prmFile);
	}
	if(!ForceMode)
	{
		cout("続行？\n");

		if(getKey() == 0x1b)
			termination(0);

		cout("続行します。\n");
	}
	DoTrep();
}
