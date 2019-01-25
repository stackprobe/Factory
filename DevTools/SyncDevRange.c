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

	errorCase_m(!getCount(Ranges), "// sync > 〜 // sync < が１つも無い。");

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

		foreach(Ranges, range, idx)
		if(!strcmp(range->Name, name))
		{
			if(findLine(md5s, range->TextMD5) == getCount(md5s))
				addElement(md5s, (uint)range->TextMD5);
		}

		cout("%s %u %s\n", getCount(md5s) == 1 ? "同期済" : "要同期", getCount(md5s), name);

		if(getCount(md5s) != 1)
			addElement(NeedSyncRangeNames, (uint)name);

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
		SRG_SyncFile(masterRange, targetRange);
	}
}

// ---- ProcAllRange ----

static sint Comp_RangeStampDesc(uint v1, uint v2)
{
	Range_t *a = (Range_t *)v1;
	Range_t *b = (Range_t *)v2;

	return m_simpleComp(a->Stamp, b->Stamp) * -1;
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
		cout("%s %I64u %u\n", range->TextMD5, range->Stamp, strlen(range->Text));
	}
	cout("====\n");
}
static int IsRangeGroupExpectedCond(autoList_t *rangeGroup)
{
	uint index;

	for(index = 1; index < getCount(rangeGroup); index++)
	{
		Range_t *r1 = (Range_t *)getElement(rangeGroup, index - 1);
		Range_t *r2 = (Range_t *)getElement(rangeGroup, index - 0);

		if(index == 1)
		{
			if(!strcmp(r1->TextMD5, r2->TextMD5)) // 先頭と2番目は異なるはず。
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
