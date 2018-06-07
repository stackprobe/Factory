/*
	trimCsProjCs.exe [/D | ソリューションがあるディレクトリ]
*/

#include "C:\Factory\Common\all.h"

#define Clean() \
	(coExecute("qq -f"))

#define Build() \
	(coExecute("cx *"))

static char *SlnFile;
static char *ProjFile;
static char *ProjDir;
static char *OutFile;

static void ProcProj(int checkOnly)
{
	cout("checkOnly: %d\n", checkOnly);
	cout("ProjFile: %s\n", ProjFile);

	ProjDir = getParent(ProjFile);

	cout("ProjDir: %s\n", ProjDir);

	Clean();
	Build();

	{
		autoList_t *files = lssFiles(ProjDir);
		char *file;
		uint index;

		foreach(files, file, index)
			if(!mbs_stristr(file, "bin\\Release") || _stricmp(getExt(file), "exe")) // DLL は対象外
				file[0] = '\0';

		trimLines(files);

		errorCase_m(getCount(files) != 1, "出力ファイルが１つに絞れません。");

		OutFile = getLine(files, 0);
		releaseAutoList(files);
	}

	cout("OutFile: %s\n", OutFile);

	Clean();

	errorCase_m(existFile(OutFile), "クリーンアップしても出力ファイルが削除されません。");

	if(checkOnly)
		goto checkOnlyEnd;

	LOGPOS();



	LOGPOS();

checkOnlyEnd:
	memFree(ProjDir);
	memFree(OutFile);
	ProjDir = NULL;
	OutFile = NULL;

	LOGPOS();
}
static void TrimCsProjCs(void)
{
	// set SlnFile
	{
		autoList_t *files = lsFiles(".");
		char *file;
		uint index;

		foreach(files, file, index)
			if(_stricmp(getExt(file), "sln"))
				file[0] = '\0';

		trimLines(files);

		errorCase_m(getCount(files) != 1, "ソリューションを１つに絞れません。");

		SlnFile = getLine(files, 0);
		releaseAutoList(files);
	}

	// set ProjFile + call (CheckProj, ProcProj)
	{
		autoList_t *files = lssFiles(".");
		char *file;
		uint index;

		foreach(files, file, index)
			if(_stricmp(getExt(file), "csproj"))
				file[0] = '\0';

		trimLines(files);

		errorCase_m(!getCount(files), "プロジェクトが１つも有りません。");

		foreach(files, file, index)
		{
			ProjFile = file;
			ProcProj(1);
			ProjFile = NULL;
		}
		foreach(files, file, index)
		{
			ProjFile = file;
			ProcProj(0);
			ProjFile = NULL;
		}
		releaseDim(files, 1);
	}

	memFree(SlnFile);
	SlnFile = NULL;
}
int main(int argc, char **argv)
{
	if(argIs("/D"))
	{
		char *slnDir = c_dropDir();

		addCwd(slnDir);
		{
			TrimCsProjCs();
		}
		unaddCwd();
	}
	else if(hasArgs(1))
	{
		char *slnDir = nextArg();

		addCwd(slnDir);
		{
			TrimCsProjCs();
		}
		unaddCwd();
	}
	else
	{
		TrimCsProjCs();
	}
}
