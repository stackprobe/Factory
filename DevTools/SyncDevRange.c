/*
	SyncDevRange.exe [/D ルートDIR] [/E 拡張子リスト]

		拡張子リスト ... 拡張子を '.' 区切りで指定する。例 "js", "js.jsp", "js.jsp.java"

	----

	以下のように中身が空の range は、そうでない最新の range によって上書きされる。

		// sync > @ xxx

		// < sync
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5.h"

static char *RootDir = "C:\\Dev";
static char *S_TargetExts = "c.h.cpp.cs";
static autoList_t *TargetExts;

typedef struct Range_st
{
	char *File;
	uint64 Stamp;
	char *Name;
	autoList_t *Lines;
	char *Text; // { 行1 + CRLF + 行2 + CRLF + ... + 行(n-1) + CRLF + 行n + CRLF }
	char *TextMD5;
	uint StartSymLineIndex;
	uint EndSymLineIndex;
}
Range_t;

static autoList_t *Ranges;

// ---- Search ----

static char *LastRangeName;

static int GetLineKind(char *line) // ret: "SE-"
{
	if(lineExpICase("<\1 >////<\1 >sync<\1 >><>", line)) // ? StartSymLine らしい
	{
		errorCase(!lineExp("<\t\t>//// sync > @ <1,,__09AZaz>", line)); // ? StartSymLine ではない。

		memFree(LastRangeName);
		LastRangeName = strx(strchr(line, '@') + 2);

		return 'S';
	}
	if(lineExpICase("<\1 >////<\1 >/<<\1 >sync<>", line)) // ? EndSymLine らしい
	{
		errorCase(!lineExp("<\t\t>//// /< sync", line)); // ? EndSymLine ではない。

		return 'E';
	}
	return '-';
}
static char *LinesToText(autoList_t *lines)
{
	char *text = strx("");
	char *line;
	uint index;

	foreach(lines, line, index)
	{
		text = addLine(text, line);
		text = addChar(text, '\n');
	}
	return text;
}
static void Search_File(char *file)
{
	autoList_t *lines = readLines(file);
	char *line;
	uint index;
	Range_t *range = NULL;

	foreach(lines, line, index)
	{
		int kind = GetLineKind(line);

		if(kind == 'S')
		{
			errorCase(range);

			range = nb(Range_t);
			range->Lines = newList();
			range->StartSymLineIndex = index;
		}
		else if(kind == 'E')
		{
			errorCase(!range);

			range->File = strx(file);
			getFileStamp(file, NULL, NULL, &range->Stamp);
			range->Name = strx(LastRangeName);
			range->Text = LinesToText(range->Lines);
			range->TextMD5 = md5_makeHexHashLine(range->Text);
			range->EndSymLineIndex = index;

			addElement(Ranges, (uint)range);

			range = NULL;
		}
		else if(range)
		{
			addElement(range->Lines, (uint)strx(line));
		}
	}
	errorCase(range);

	releaseDim(lines, 1);
}
static void Search(void)
{
	autoList_t *files = lssFiles(RootDir);
	char *file;
	uint index;

	sortJLinesICase(files);

	foreach(files, file, index)
	if(findLineCase(TargetExts, getExt(file), 1) < getCount(TargetExts))
	{
		cout("file: %s\n", file);

		Search_File(file);
	}
	releaseDim(files, 1);
}

// ---- Confirm ----

static autoList_t *NeedSyncRangeNames;

static void DispAllRange_GroupByName(void)
{
	autoList_t *names = newList();
	Range_t *range;
	char *name;
	uint index;
	uint idx;

	errorCase_m(!getCount(Ranges), "// sync > ～ // sync < が１つも無い。");

	foreach(Ranges, range, index)
		if(findLine(names, range->Name) == getCount(names))
			addElement(names, (uint)range->Name);

	rapidSortLines(names);

	cout("====\n");

	foreach(names, name, index)
	{
		uint count = 0;

		foreach(Ranges, range, idx)
		if(!strcmp(range->Name, name))
		{
			count++;
		}

		cout("%08u %s\n", count, name);
	}
	cout("====\n");

	foreach(names, name, index)
	{
		autoList_t *md5s = newList();
		int needSync;

		foreach(Ranges, range, idx)
		if(!strcmp(range->Name, name))
		{
			if(findLine(md5s, range->TextMD5) == getCount(md5s))
				addElement(md5s, (uint)range->TextMD5);
		}

		needSync = getCount(md5s) != 1;

		cout("%s %u %s\n", needSync ? "要同期" : "同期済", getCount(md5s), name);

		if(needSync)
			addElement(NeedSyncRangeNames, (uint)name);

#if 0 // test
		if(needSync)
		{
			addCwd("C:\\temp");
			{
				createDir(name);

				addCwd(name);
				{
					foreach(Ranges, range, idx)
					if(!strcmp(range->Name, name))
					{
						writeOneLineNoRet(range->TextMD5, range->Text);
					}
				}
				unaddCwd();
			}
			unaddCwd();
		}
#endif

		releaseAutoList(md5s);
	}
	cout("====\n");

	releaseAutoList(names);
}
static void Confirm(void)
{
	Range_t *range;
	uint index;

	foreach(Ranges, range, index)
	{
		cout("----\n");

		cout("File : %s\n", range->File);
		cout("Stamp: %I64u\n", range->Stamp);
		cout("Name : %s\n", range->Name);
		cout("T-MD5: %s\n", range->TextMD5);
		cout("Start: %u\n", range->StartSymLineIndex);
		cout("End  : %u\n", range->EndSymLineIndex);

		cout("----\n");
	}

	foreach(Ranges, range, index)
		cout("%s %s\n", range->TextMD5, range->Name);

	DispAllRange_GroupByName();

	cout("続行？\n");

	if(clearGetKey() == 0x1b)
		termination(0);

	cout("続行します。\n");
}

// ---- SyncRangeGroup ----

static void SRG_SyncFile(Range_t *masterRange, Range_t *targetRange)
{
	autoList_t *lines = readLines(targetRange->File);
	uint index;
	FILE *fp;

	LOGPOS();

	semiRemovePath(targetRange->File);

	fp = fileOpen(targetRange->File, "wt");

	for(index = 0; index <= targetRange->StartSymLineIndex; index++)
		writeLine(fp, getLine(lines, index));

	writeToken(fp, masterRange->Text);

	for(index = targetRange->EndSymLineIndex; index < getCount(lines); index++)
		writeLine(fp, getLine(lines, index));

	fileClose(fp);

	LOGPOS();
}
static sint Comp_RangeSSLIDesc(uint v1, uint v2)
{
	Range_t *a = (Range_t *)v1;
	Range_t *b = (Range_t *)v2;
	sint ret;

	ret = m_simpleComp(a->StartSymLineIndex, b->StartSymLineIndex) * -1;

	if(!ret)
		ret = strcmp(a->File, b->File);

	return ret;
}
static void SyncRangeGroup(autoList_t *rangeGroup)
{
	Range_t *masterRange = (Range_t *)desertElement(rangeGroup, 0);
	Range_t *targetRange;
	uint index;

	rapidSort(rangeGroup, Comp_RangeSSLIDesc);

	// test 更新順確認
	{
		Range_t *range;
		uint range_index;

		LOGPOS();

		foreach(rangeGroup, range, range_index)
		{
			cout("%s (%u)\n", range->File, range->StartSymLineIndex);
		}
		cout("test 更新順確認 -- PRESS KEY !!!\n");
		clearGetKey();
		LOGPOS();
	}

	foreach(rangeGroup, targetRange, index)
	{
		if(!strcmp(masterRange->TextMD5, targetRange->TextMD5)) // ? 同じ -> 更新不要
		{
			LOGPOS();
		}
		else
		{
			SRG_SyncFile(masterRange, targetRange);
		}
	}
}

// ---- ProcAllRange ----

static int IsRangeEmptyText(Range_t *range)
{
	char *text = strx(range->Text);
	int ret;

	ucTrim(text);

	ret = text[0] == '\0';

	memFree(text);
	return ret;
}
static void DispRangeGroup(autoList_t *rangeGroup)
{
	Range_t *range;
	uint index;

	foreach(rangeGroup, range, index)
	{
		cout("----\n");

		cout("File : %s\n", range->File);
		cout("T-MD5: %s\n", range->TextMD5);
		cout("Start: %u\n", range->StartSymLineIndex);

		cout("----\n");
	}

	foreach(rangeGroup, range, index)
	{
		cout("%s (%u)\n", range->File, range->StartSymLineIndex);
	}
	cout("====\n");

	foreach(rangeGroup, range, index)
	{
		cout("%s %c %I64u %u\n", range->TextMD5, IsRangeEmptyText(range) ? '-' : '+', range->Stamp, strlen(range->Text));
	}
	cout("====\n");

	// ついでに FOUNDLISTFILE へ出力
	{
		FILE *fp = fileOpen(FOUNDLISTFILE, "wt");

		foreach(rangeGroup, range, index)
			writeLine(fp, range->File);

		fileClose(fp);
	}
}
static sint Comp_RangeStampDesc(uint v1, uint v2)
{
	Range_t *a = (Range_t *)v1;
	Range_t *b = (Range_t *)v2;
	sint ret;

	// 中身あり -> 中身空っぽ　の順
	{
		sint et1 = IsRangeEmptyText(a) ? 1 : 0;
		sint et2 = IsRangeEmptyText(b) ? 1 : 0;

		ret = et1 - et2;
		if(ret)
			return ret;
	}

	ret = m_simpleComp(a->Stamp, b->Stamp) * -1;
	if(ret)
		return ret;

	ret = strcmp(a->File, b->File);
	if(ret)
		return ret;

	ret = m_simpleComp(a->StartSymLineIndex, b->StartSymLineIndex);
	return ret;
}
static int IsRangeGroupExpectedCond_Case1(autoList_t *rangeGroup) // 1ヶ所だけ修正した場合
{
	uint index;

	for(index = 1; index < getCount(rangeGroup); index++)
	{
		Range_t *r1 = (Range_t *)getElement(rangeGroup, index - 1);
		Range_t *r2 = (Range_t *)getElement(rangeGroup, index - 0);

		if(index == 1)
		{
			if(!strcmp(r1->TextMD5, r2->TextMD5)) // 最初と2番目は異なるはず。
				return 0;
		}
		else
		{
			if(strcmp(r1->TextMD5, r2->TextMD5)) // それ以降は同じはず。
				return 0;
		}
	}
	return 1;
}
static int IsRangeGroupExpectedCond_Case2(autoList_t *rangeGroup) // 1ヶ所以上、空の range を追加した場合
{
	uint index;

	for(index = 1; index < getCount(rangeGroup); index++)
	{
		Range_t *r1 = (Range_t *)getElement(rangeGroup, index - 1);
		Range_t *r2 = (Range_t *)getElement(rangeGroup, index - 0);

		if(strcmp(r1->TextMD5, r2->TextMD5)) // 基本的に同じはず。
		{
			if(!IsRangeEmptyText(r2)) // 異なる場合は、少なくとも後側が空っぽのはず。
			{
				return 0;
			}
		}
	}
	return 1;
}
static int IsRangeGroupExpectedCond(autoList_t *rangeGroup)
{
	return
		IsRangeGroupExpectedCond_Case1(rangeGroup) ||
		IsRangeGroupExpectedCond_Case2(rangeGroup);
}
static void ProcRangeGroup(autoList_t *rangeGroup)
{
	rapidSort(rangeGroup, Comp_RangeStampDesc);

	DispRangeGroup(rangeGroup);

	if(!IsRangeGroupExpectedCond(rangeGroup))
	{
		cout("★★★想定外の修正状態です。続行？\n");

		if(clearGetKey() == 0x1b)
			termination(0);

		cout("続行します。\n");
	}
	SyncRangeGroup(rangeGroup);
}
static void ProcAllRange(void)
{
	autoList_t *rangeGroup = newList();
	char *name;
	uint index;

	foreach(NeedSyncRangeNames, name, index)
	{
		Range_t *range;
		uint range_index;

		setCount(rangeGroup, 0);

		foreach(Ranges, range, range_index)
			if(!strcmp(range->Name, name))
				addElement(rangeGroup, (uint)range);

		ProcRangeGroup(rangeGroup);
	}
	releaseAutoList(rangeGroup);
}

// ----

int main(int argc, char **argv)
{
	Ranges = newList();
	NeedSyncRangeNames = newList();

readArgs:
	if(argIs("/D"))
	{
		RootDir = nextArg();
		goto readArgs;
	}
	if(argIs("/E"))
	{
		S_TargetExts = nextArg();
		goto readArgs;
	}

	errorCase(!existDir(RootDir));

	TargetExts = tokenize(S_TargetExts, '.');
	trimLines(TargetExts);

	errorCase(!getCount(TargetExts));

	Search();
	Confirm();
	ProcAllRange();

	cout("\\e\n");
}
