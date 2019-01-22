#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5.h"

static char *RootDir = "C:\\Dev";
static char *S_TargetExts = "c.cs";
static autoList_t *TargetExts;

typedef struct Range_st
{
	char *File;
	char *Name;
	autoList_t *Lines;
	char *Text; // { 行1 + CRLF + 行2 + CRLF + ... + 行(n-1) + CRLF + 行n + CRLF }
	char *TextMD5;
	uint StartSymLineIndex;
	uint EndSymLineIndex;
}
Range_t;

static autoList_t *Ranges;

static char *LastRangeName;

static int GetLineKind(char *line) // ret: "SE-"
{
	if(lineExp("<\1 >////<\1 >sync<\1 >><>", line)) // ? StartSymLine らしい
	{
		errorCase(!lineExp("<\t\t>//// sync > @ <1,,__09AZaz>", line)); // ? StartSymLine ではない。

		memFree(LastRangeName);
		LastRangeName = strx(strchr(line, '@') + 2);

		return 'S';
	}
	if(lineExp("<\1 >////<\1 >/<<\1 >sync<>", line)) // ? EndSymLine らしい
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
			range->Name = strx(LastRangeName);
			range->Text = LinesToText(range->Lines);
			range->TextMD5 = md5_makeHexHashLine(range->Text);
			range->EndSymLineIndex = index;

			addElement(Ranges, (uint)range);
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
		Search_File(file);
	}
	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
	Ranges = newList();

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

	// TODO
}
