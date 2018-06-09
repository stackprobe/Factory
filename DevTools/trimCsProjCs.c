/*
	trimCsProjCs.exe [/D | ソリューションがあるディレクトリ]
*/

#include "C:\Factory\Common\all.h"

#define Clean() \
	(coExecute("solclean ."))
//	(coExecute("qq -f")) // 1.txt にリダイレクトしたときマズい。

#define Build() \
	(coExecute("cx *"))

#define WriteProjFile(file, lines) \
	(writeOneLineNoRet_cx((file), untokenize((lines), "\n")))

static char *SlnFile;
static char *ProjFile;
static char *ProjName;
static char *ProjDir;
static char *OutFile;
static char *ProjBackupFile;
static autoList_t *ProjLines;
static autoList_t *DeletableCsFiles;

static int TryBuild(void)
{
	WriteProjFile(ProjFile, ProjLines);

	Clean();
	Build();

	removeFile(ProjFile);

	return existFile(OutFile);
}
static int TrimProjLines(void)
{
	char *line;
	uint index;
	int trimmed = 0;

	foreach(ProjLines, line, index)
	{
		if(startsWith(line, "    <Compile Include=\"") && endsWith(line, "\" />"))
		{
			char *csFile = ne_strchr(line, '"') + 1;
			char *p;

			p = ne_strchr(csFile, '"');
			*p = '\0';
			csFile = strx(csFile);
			*p = '"';

			cout("csFile.1: %s\n", csFile);

			if(
				isFairRelPath(csFile, strlen(ProjDir)) && // ? SJISファイル名 && ProjDirの配下
				!_stricmp(getExt(csFile), "cs") && // ? .csファイル
				!startsWithICase(csFile, "Properties\\") // ? ! 除外
				)
			{
				csFile = combine_cx(ProjDir, csFile);

				cout("csFile.2: %s\n", csFile);

				if(existFile(csFile))
				{
					desertElement(ProjLines, index);

					if(TryBuild())
					{
						memFree(line);
						addElement(DeletableCsFiles, (uint)strx(csFile));
					}
					else
						insertElement(ProjLines, index, (uint)line); // 元に戻す。
				}
			}
			memFree(csFile);
		}
	}
	return trimmed;
}
static int ConfirmDeleteCsFiles(void)
{
	char *file;
	uint index;
	char *line;
	int ret;

	cout("【確認】\n");

	foreach(DeletableCsFiles, file, index)
		cout("削除対象：%s\n", file);

	cout("削除するには”DELCS”と入力してね。\n");
	line = coInputLine();
	ret = !strcmp(line, "DELCS");
	memFree(line);
	cout("ret: %d\n", ret);
	return ret;
}
static void ProcProj(int checkOnly)
{
	cout("checkOnly: %d\n", checkOnly);
	cout("ProjFile: %s\n", ProjFile);

	ProjName = changeExt(getLocal(ProjFile), "");
	ProjDir = getParent(ProjFile);

	cout("ProjName: %s\n", ProjName);
	cout("ProjDir: %s\n", ProjDir);

	Clean();
	Build();

	{
		autoList_t *files = lssFiles(ProjDir);
		char *file;
		uint index;

		foreach(files, file, index)
		{
			if(
				!mbs_stristr(file, "\\bin\\Release\\") ||
				!mbs_stristr(getLocal(file), ProjName) ||
				_stricmp(getExt(file), "exe") && _stricmp(getExt(file), "dll")
				)
				file[0] = '\0';
		}
		trimLines(files);

		errorCase_m(getCount(files) != 1, "出力ファイルが１つに絞れません。");

		OutFile = getLine(files, 0);
		releaseAutoList(files);
	}

	cout("OutFile: %s\n", OutFile);

	Clean();

	errorCase_m(existFile(OutFile), "クリーンアップしても出力ファイルが削除されません。");

	ProjBackupFile = addExt(strx(ProjFile), "bak");

	cout("ProjBackupFile: %s\n", ProjBackupFile);

	errorCase_m(existPath(ProjBackupFile), "バックアップファイルが残っています。");

	ProjLines = readLines(ProjFile);

	// check ProjLines -- 2bs
	{
		char *wkFile = makeTempPath(NULL);

		WriteProjFile(wkFile, ProjLines);

		errorCase(!isSameFile(wkFile, ProjFile));

		removeFile(wkFile);
		memFree(wkFile);
	}

	if(checkOnly)
		goto checkOnlyEnd;

	LOGPOS();

	moveFile(ProjFile, ProjBackupFile);

	DeletableCsFiles = newList();

	while(TrimProjLines());

	if(ConfirmDeleteCsFiles())
	{
		char *file;
		uint index;

		WriteProjFile(ProjFile, ProjLines);

		foreach(DeletableCsFiles, file, index)
			semiRemovePath(file);

		semiRemovePath(ProjBackupFile);
	}
	else
		moveFile(ProjBackupFile, ProjFile);

	releaseDim(DeletableCsFiles, 1);
	DeletableCsFiles = NULL;

	LOGPOS();

checkOnlyEnd:
	memFree(ProjName);
	memFree(ProjDir);
	memFree(OutFile);
	memFree(ProjBackupFile);
	releaseDim(ProjLines, 1);
	ProjName = NULL;
	ProjDir = NULL;
	OutFile = NULL;
	ProjBackupFile = NULL;
	ProjLines = NULL;

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

		errorCase_m(getCount(files) != 1, "ソリューションを１つに絞れません。ソリューションファイルと同じ場所から実行して下さい。");

		SlnFile = getLine(files, 0);
		releaseAutoList(files);
	}

	cout("SlnFile: %s\n", SlnFile);

	// check vcs 2010 express
	{
		FILE *fp = fileOpen(SlnFile, "rt");

		c_neReadLine(fp);
		errorCase_m(strcmp(c_neReadLine(fp), "Microsoft Visual Studio Solution File, Format Version 11.00"), "vs2010じゃないですよ。");
		errorCase_m(strcmp(c_neReadLine(fp), "# Visual C# Express 2010"), "C#じゃないですよ。");

		fileClose(fp);
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
