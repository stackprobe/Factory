/*
	CopyLib.exe 入力DIR 出力DIR

		入力DIR, 出力DIR ... 存在するディレクトリであること。

	環境変数

		@CopyLib_NoPause に 非0 ... 終了時ポーズしない。
*/

#include "C:\Factory\Common\all.h"

static int IsNoPause(void)
{
	return toValue(getEnvLine("@CopyLib_NoPause")) != 0;
}
static int IsCSharpFile(char *file)
{
	return !_stricmp("cs", getExt(file));
}

// ---- AutoComment-SyncEntered ----

static int ACSE_SyncEntered;

static void ACSE_Reset(void)
{
	ACSE_SyncEntered = 0;
}
static void ACSE_SetLine(char *line)
{
	int enter;
	int leave;

	{
		char *tmp = strx(line);

		ucTrim(tmp);

		enter = startsWith(tmp, "// sync >");
		leave = startsWith(tmp, "// < sync");

		memFree(tmp);
	}

	if(enter)
	{
		errorCase(ACSE_SyncEntered);
		ACSE_SyncEntered = 1;
	}
	else if(leave)
	{
		errorCase(!ACSE_SyncEntered);
		ACSE_SyncEntered = 0;
	}
}
static int ACSE_IsOutSync(void)
{
	return !ACSE_SyncEntered;
}

// ----

static autoList_t *ResAutoComment;
static autoList_t *ResAutoComment_CS;

static void AutoComment(autoList_t *ranges)
{
	autoList_t *range;
	uint range_index;
	char *line;
	uint index;
	int commentEntered = 0;
	int classEntered = 0;

	ACSE_Reset();

	foreach(ranges, range, range_index)
	foreach(range, line, index)
	{
		int insCmt;

		ACSE_SetLine(line);

		line = strx(line);
		nn_strstr(line, "//")[0] = '\0'; // インラインコメントの除去
		trimTrail(line, ' ');

		if(startsWith(line, "class "))
			classEntered = 1;

		if(!strcmp(line, "};"))
			classEntered = 0;

		if(!strcmp(line, "}"))
			classEntered = 0;

		// set insCmt >

		if(classEntered)
		{
			insCmt = line[0] == '\t' && (m_isalpha(line[1]) || line[1] == '_' || line[1] == '~');

			if(startsWith(line, "#define "))
				insCmt = 1;

			if(index && !strcmp(getLine(range, index - 1), "\t*/"))
				insCmt = 0;

			if(index && startsWith(getLine(range, index - 1), "#define "))
				insCmt = 0;

			if(!strcmp(line, "\t/*"))
				insCmt = 1;
		}
		else
		{
			insCmt = m_isalpha(line[0]) || line[0] == '_';

			if(startsWith(line, "#define "))
				insCmt = 1;

			if(index && !strcmp(getLine(range, index - 1), "}") && endsWith(line, ";")) // typedef など
				insCmt = 0;

			if(index && !strcmp(getLine(range, index - 1), "*/"))
				insCmt = 0;

			if(index && startsWith(getLine(range, index - 1), "template <"))
				insCmt = 0;

			if(!strcmp(line, "/*"))
				insCmt = 1;
		}

		if(endsWith(line, ":"))
			insCmt = 0;

		if(commentEntered)
			insCmt = 0;

		if(!range_index && !index)
			insCmt = 1;

		// < set insCmt

		if(insCmt && ACSE_IsOutSync())
		{
			char *comment;
			uint comment_index;

			foreach(ResAutoComment, comment, comment_index)
			{
				insertElement(range, index++, (uint)xcout("%s%s", classEntered ? "\t" : "", comment));
			}
		}

		{
			char *tmp = strx(getLine(range, index));
//			char *tmp = strx(line); // インラインコメントが削除されているのでng

			ucTrim(tmp);

			if(commentEntered)
			{
				if(!strcmp(tmp, "*/"))
					commentEntered = 0;

				if(!strcmp(tmp, "//*/")) // LOG_ENABLED んとことかの /* ～ /*/ ～ //*/
					commentEntered = 0;
			}
			else
			{
				if(!strcmp(tmp, "/*"))
					commentEntered = 1;
			}
			memFree(tmp);
		}

		memFree(line);
	}
	errorCase(commentEntered);
	errorCase(classEntered);
}
static void AutoComment_CS(autoList_t *ranges)
{
	autoList_t *range;
	uint range_index;
	char *line;
	uint index;

	ACSE_Reset();

	foreach(ranges, range, range_index)
	foreach(range, line, index)
	{
		char *prevLine = index ? getLine(range, index - 1) : "";
		char *insCmtIndent = NULL;

		ACSE_SetLine(line);

		if(!startsWith(prevLine, "\t/// "))
		if(
			startsWith(line, "\t/// ") ||
			startsWith(line, "\tpublic class ") ||
			startsWith(line, "\tpublic static class ") ||
			startsWith(line, "\tpublic interface ")
			)
			insCmtIndent = "\t";

		if(!startsWith(prevLine, "\t\t/// "))
		if(!startsWith(prevLine, "\t\t[DllImport"))
		if(!startsWith(prevLine, "\t\t[StructLayout"))
		if(
			startsWith(line, "\t\t/// ") ||
			startsWith(line, "\t\t[DllImport") ||
			startsWith(line, "\t\t[StructLayout") ||
			startsWith(line, "\t\tpublic ") ||
			startsWith(line, "\t\tprivate ") ||
			startsWith(line, "\t\tprotected ")
			)
			insCmtIndent = "\t\t";

		if(insCmtIndent && ACSE_IsOutSync())
		{
			char *comment;
			uint comment_index;

			foreach(ResAutoComment_CS, comment, comment_index)
			{
				insertElement(range, index++, (uint)xcout("%s%s", insCmtIndent, comment));
			}
		}

		// TODO @" など、いずれ対応が必要になると思う。
	}
}
static autoList_t *ReadCommonAndAppSpecRanges(char *file)
{
	autoList_t *ranges = newList();
	autoList_t *lines = newList();
	FILE *fp = fileOpen(file, "rt");
	char *line;
	int appSpecEntered = 0;

	while(line = readLine(fp))
	{
		int enter;
		int leave;

		{
			char *tmp = strx(line);

			ucTrim(tmp);

			enter = startsWith(tmp, "// app >");
			leave = startsWith(tmp, "// < app");

			memFree(tmp);
		}

		errorCase(enter && leave);

		if(leave)
		{
			errorCase(!appSpecEntered);
			appSpecEntered = 0;

			addElement(ranges, (uint)lines);
			lines = newList();
		}
		addElement(lines, (uint)line);

		if(enter)
		{
			errorCase(appSpecEntered);
			appSpecEntered = 1;

			addElement(ranges, (uint)lines);
			lines = newList();
		}
	}
	errorCase(appSpecEntered);

	addElement(ranges, (uint)lines);

	fileClose(fp);
	return ranges;
}
static void CheckAppSpecRangesPair(autoList_t *rRanges, autoList_t *wRanges)
{
	uint index;

	errorCase_m(getCount(rRanges) != getCount(wRanges), "アプリ固有コードの数が合わないため上書き出来ません。");

	errorCase(getCount(rRanges) % 2 != 1); // 2bs
	errorCase(getCount(wRanges) % 2 != 1); // 2bs

	for(index = 2; index < getCount(rRanges); index += 2)
	{
		{
			char *r = (char *)getLastElement(getList(rRanges, index - 2));
			char *w = (char *)getLastElement(getList(wRanges, index - 2));

coutJLine_x(xcout("r: %s", r)); // test
coutJLine_x(xcout("w: %s", w)); // test

			errorCase_m(strcmp(r, w), "アプリ固有コード開始行不一致");
		}

		{
			char *r = getLine(getList(rRanges, index), 0);
			char *w = getLine(getList(wRanges, index), 0);

coutJLine_x(xcout("r: %s", r)); // test
coutJLine_x(xcout("w: %s", w)); // test

			errorCase_m(strcmp(r, w), "アプリ固有コード終了行不一致");
		}
	}
}

static int DCL_ExistNewFile;

static void DoCopyLib(char *rDir, char *wDir, int testMode)
{
	autoList_t *rSubDirs = lssDirs(rDir);
	autoList_t *wSubDirs = lssDirs(wDir);
	autoList_t *rFiles = lssFiles(rDir);
	autoList_t *wFiles = lssFiles(wDir);
	autoList_t *owFiles;
	char *dir;
	char *file;
	uint index;

	changeRoots(rSubDirs, rDir, NULL);
	changeRoots(wSubDirs, wDir, NULL);
	changeRoots(rFiles, rDir, NULL);
	changeRoots(wFiles, wDir, NULL);

	releaseDim(mergeLinesICase(rSubDirs, wSubDirs), 1);
	reverseElements(wSubDirs); // ★★★ 削除は後ろから行うため、ここで逆転しておく。★★★

	owFiles = mergeLinesICase(rFiles, wFiles);

	foreach(rSubDirs, dir, index)
		if(!testMode)
			createPath_x(combine(rDir, dir), 'X');

	DCL_ExistNewFile = 1 <= getCount(rFiles);

	foreach(rFiles, file, index)
	{
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);

		cout("R %s\n", rFile);
		cout("> %s\n", wFile);

		if(!testMode)
		{
			createPath(wFile, 'X');
			copyFile(rFile, wFile);
		}
		memFree(rFile);
		memFree(wFile);
	}
	foreach(wFiles, file, index)
	{
		int csMode = IsCSharpFile(file);
		char *wFile = combine(wDir, file);

		cout("D %s\n", wFile);

		{
			autoList_t *ranges = ReadCommonAndAppSpecRanges(wFile);

			cout("Dr %u\n", getCount(ranges));

			errorCase_m(1 < getCount(ranges), "アプリ固有コードを含むため削除出来ません。手動で削除してね。");

			releaseDim(ranges, 2);
		}

		if(!testMode)
		{
			if(csMode)
				removeFile(wFile);
			else
				writeOneLine(wFile, "// deleted"); // 削除すると .vcxproj のエントリーを消せなくなるので、削除NG
		}
		memFree(wFile);
	}
	foreach(owFiles, file, index)
	{
		int csMode = IsCSharpFile(file);
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);
		autoList_t *rRanges;
		autoList_t *wRanges;

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);

		rRanges = ReadCommonAndAppSpecRanges(rFile);
		wRanges = ReadCommonAndAppSpecRanges(wFile);

		cout("<r %u\n", getCount(rRanges));
		cout(">r %u\n", getCount(wRanges));

		CheckAppSpecRangesPair(rRanges, wRanges);

		(csMode ? AutoComment_CS : AutoComment)(rRanges);

		{
			autoList_t *lines = newList();
			uint index;

			for(index = 0; index < getCount(rRanges); index++)
				addElements(lines, getList(index % 2 ? wRanges : rRanges, index));

			if(!testMode)
			{
				removeFile(wFile); // 2bs?
//				semiRemovePath(wFile);

				writeLines(wFile, lines);
			}
			releaseAutoList(lines);
		}

		memFree(rFile);
		memFree(wFile);
		releaseDim(rRanges, 2);
		releaseDim(wRanges, 2);
	}
	foreach(wSubDirs, dir, index)
		if(!testMode)
			LOGPOS();
//			removeDir_x(combine(wDir, dir)); // ファイルを削除しないのでDIRも削除しない。

	releaseDim(rSubDirs, 1);
	releaseDim(wSubDirs, 1);
	releaseDim(rFiles, 1);
	releaseDim(wFiles, 1);
	releaseDim(owFiles, 1);
}
static void CopyLib(char *rDir, char *wDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));
	errorCase(!_stricmp(rDir, wDir)); // ? 同じディレクトリ

	LOGPOS();
	DoCopyLib(rDir, wDir, 1);
	LOGPOS();
	DoCopyLib(rDir, wDir, 0);
	LOGPOS();

	if(DCL_ExistNewFile) // 新規追加ファイルがあった場合は２回行う必要がある。新規追加ファイルには AutoComment が適用されない。
	{
		LOGPOS();
		DoCopyLib(rDir, wDir, 0);
		LOGPOS();
	}
	memFree(rDir);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	{
		char *cmtFile = changeExt(getSelfFile(), "txt");

		ResAutoComment = readLines(cmtFile);

		memFree(cmtFile);
	}

	{
		char *cmtFile = addLine(changeExt(getSelfFile(), ""), "_CS.txt");

		ResAutoComment_CS = readLines(cmtFile);

		memFree(cmtFile);
	}

	CopyLib(getArg(0), getArg(1));

	if(!IsNoPause())
	{
		cout("Press any key to continue ...");
		getKey();
		cout("\n");
	}
}
