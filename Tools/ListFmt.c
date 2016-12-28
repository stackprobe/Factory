/*
	ListFmt.exe [/F 入力ファイル] [/-] フォーマット...
*/

#include "C:\Factory\Common\all.h"

#define S_ESCAPE "\1"
#define T_ESCAPE "\2"

static autoList_t *ListFiles;
static char *Format;

static autoList_t *Lists;
static autoList_t *CurrLines;

static char *GetLine(void)
{
	char *ret = replaceLine(strx(Format), "$$", S_ESCAPE, 0);

	ret = replaceLine(ret, "$", T_ESCAPE, 0);

	// ----

	ret = replaceLine(ret, T_ESCAPE "A", "&", 1);
	ret = replaceLine(ret, T_ESCAPE "C", "%", 1);
	ret = replaceLine(ret, T_ESCAPE "D", "\"", 1);
	ret = replaceLine(ret, T_ESCAPE "G", ">", 1);
	ret = replaceLine(ret, T_ESCAPE "I", "^", 1);
	ret = replaceLine(ret, T_ESCAPE "L", "<", 1);
	ret = replaceLine(ret, T_ESCAPE "N", "\n", 1);
	ret = replaceLine(ret, T_ESCAPE "P", "|", 1);
	ret = replaceLine(ret, T_ESCAPE "Q", "'", 1);
	ret = replaceLine(ret, T_ESCAPE "S", " ", 1);
	ret = replaceLine(ret, T_ESCAPE "T", "\t", 1);
	ret = replaceLine(ret, T_ESCAPE "Y", "\\", 1);
	ret = replaceLine(ret, T_ESCAPE "0", refLine(CurrLines, 0), 0);
	ret = replaceLine(ret, T_ESCAPE "1", refLine(CurrLines, 1), 0);
	ret = replaceLine(ret, T_ESCAPE "2", refLine(CurrLines, 2), 0);
	ret = replaceLine(ret, T_ESCAPE "3", refLine(CurrLines, 3), 0);
	ret = replaceLine(ret, T_ESCAPE "4", refLine(CurrLines, 4), 0);
	ret = replaceLine(ret, T_ESCAPE "5", refLine(CurrLines, 5), 0);
	ret = replaceLine(ret, T_ESCAPE "6", refLine(CurrLines, 6), 0);
	ret = replaceLine(ret, T_ESCAPE "7", refLine(CurrLines, 7), 0);
	ret = replaceLine(ret, T_ESCAPE "8", refLine(CurrLines, 8), 0);
	ret = replaceLine(ret, T_ESCAPE "9", refLine(CurrLines, 9), 0);

	// ----

	ret = replaceLine(ret, T_ESCAPE, "$", 0);

	return replaceLine(ret, S_ESCAPE, "$", 0);
}
static void ProcLine(void)
{
	char *line = GetLine();

	cout("%s\n", line);
	memFree(line);
}
static void ListFmt_0(void)
{
	autoList_t *list;
	uint list_index;
	uint index;
	uint maxCount = 0;

	foreach(Lists, list, list_index)
		m_maxim(maxCount, getCount(list));

	for(index = 0; index < maxCount; index++)
	{
		foreach(Lists, list, list_index)
			setElement(CurrLines, list_index, (uint)getLine(list, index % getCount(list)));

		ProcLine();
	}
}
static void ListFmt(void)
{
	autoList_t *list;
	char *file;
	uint index;

	errorCase(!getCount(ListFiles));

	Lists = newList();

	foreach(ListFiles, file, index)
		addElement(Lists, (uint)readLines(file));

	foreach(Lists, list, index)
		errorCase(!getCount(list));

	CurrLines = newList();
	setCount(CurrLines, getCount(Lists));

	ListFmt_0();
}
int main(int argc, char **argv)
{
	ListFiles = newList();
	addElement(ListFiles, (uint)FOUNDLISTFILE);

readArgs:
	if(argIs("/F"))
	{
		addElement(ListFiles, (uint)nextArg());
		goto readArgs;
	}
	argIs("/-");

	Format = untokenize(allArgs(), " "); // gomi
	ListFmt();
}
