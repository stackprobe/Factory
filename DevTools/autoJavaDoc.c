/*
	autoJavaDoc.exe [/S] [/LSS | 対象ディレクトリ]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static int IntoSubDir;

static char *c_GetRandomCode(void)
{
	static char *ret;

	memFree(ret);

	ret = MakeRandID();

	return ret;
}
static void DoAutoJavaDoc_File(char *file)
{
	autoList_t *lines = readLines(file);
	char *line;
	uint index;

	foreach(lines, line, index)
	{
		if(
			lineExp("<0,100,\t\t>public <>", line) ||
			lineExp("<0,100,\t\t>private <>", line) ||
			lineExp("<0,100,\t\t>protected <>", line)
			)
		{
			char *atmarkLine = NULL;
			char *indent = strx(line);

			*ne_strchr(indent, 'p') = '\0';

			if(index && lineExp("<0,100,\t\t> *//", getLine(lines, index - 1))) // エスケープ注意 / -> //
				goto ignoreTheLine;

			if(index && lineExp("<0,100,\t\t>@<>", getLine(lines, index - 1)))
				atmarkLine = (char *)desertElement(lines, --index);

			if(index && *getLine(lines, index - 1))
				insertElement(lines, index++, (uint)strx(""));

			insertElement(lines, index++, (uint)xcout("%s/**", indent));
//			insertElement(lines, index++, (uint)xcout("%s * random code: %s", indent, c_GetRandomCode()));
			insertElement(lines, index++, (uint)xcout("%s * Javadoc", indent));
			insertElement(lines, index++, (uint)xcout("%s * @version 1.0", indent));
			insertElement(lines, index++, (uint)xcout("%s *", indent));
			insertElement(lines, index++, (uint)xcout("%s */", indent));

			if(atmarkLine)
				insertElement(lines, index++, (uint)atmarkLine);

		ignoreTheLine:
			memFree(indent);
		}
	}
	writeLines_cx(file, lines);
}
static void Confirm(void)
{
	cout("続行？\n");

	if(clearGetKey() == 0x1b)
		termination(0);

	cout("続行します。\n");
}
static void DoAutoJavaDoc_List(autoList_t *files)
{
	char *file;
	uint index;

	foreach(files, file, index)
		cout("file: %s\n", file);

	cout("files_count: %u\n", getCount(files));

	Confirm();

	foreach(files, file, index)
		DoAutoJavaDoc_File(file);

	releaseDim(files, 1);
}
static void DoAutoJavaDoc(char *dir)
{
	autoList_t *files = (IntoSubDir ? lssFiles : lsFiles)(dir);
	char *file;
	uint index;

	cout("dir: %s\n", dir);
	cout("IntoSubDir: %d\n", IntoSubDir);

	Confirm();

	foreach(files, file, index)
		if(!_stricmp("java", getExt(file)))
			DoAutoJavaDoc_File(file);

	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/S"))
	{
		IntoSubDir = 1;
		goto readArgs;
	}

	if(argIs("/LSS"))
	{
		DoAutoJavaDoc_List(readLines(FOUNDLISTFILE)); // g
		return;
	}
	if(hasArgs(1))
	{
		DoAutoJavaDoc(nextArg());
		return;
	}

	for(; ; )
	{
		DoAutoJavaDoc(c_dropDirFile());
		cout("\n");
	}
}
