/*
	autoJavaDoc.exe [/S] [/F Javadoc�t�@�C��] [/LSS | �Ώۃf�B���N�g��]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static int IntoSubDir;
static autoList_t *JavaDoc;

static void DoAutoJavaDoc_File(char *file)
{
	autoList_t *lines = readLines(file);
	char *line;
	uint index;

	cout("%s\n", file);

	foreach(lines, line, index)
	{
		if(
			lineExp("<0,100,\t\t>public <>", line) ||
			lineExp("<0,100,\t\t>private <>", line) ||
			lineExp("<0,100,\t\t>protected <>", line)
			)
		{
			char *indentPtn = strx(line);
			int hasAnnotation = 0;

			*ne_strchr(indentPtn, 'p') = '\0';

			if(index && lineExp("<0,100,\t\t>@<>", getLine(lines, index - 1)))
			{
				hasAnnotation = 1;
				index--;
			}
			if(!index || !lineExp("<0,100,\t\t> *//", getLine(lines, index - 1))) // �G�X�P�[�v���� / -> //
			{
				char *javaDocLine;
				uint javaDocLineIndex;

				if(index && *getLine(lines, index - 1))
					insertElement(lines, index++, (uint)strx(""));

				foreach(JavaDoc, javaDocLine, javaDocLineIndex)
					insertElement(lines, index++, (uint)xcout("%s%s", indentPtn, javaDocLine));
			}
			if(hasAnnotation)
				index++;

			memFree(indentPtn);
		}
	}
	writeLines_cx(file, lines);
	cout("done\n");
}
static void Confirm(void)
{
	LOGPOS();
	cout("���s�H\n");

	if(clearGetKey() == 0x1b)
		termination(0);

	cout("���s���܂��B\n");
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
	char *javaDocFile = changeExt(getSelfFile(), "txt");

readArgs:
	if(argIs("/S"))
	{
		IntoSubDir = 1;
		goto readArgs;
	}
	if(argIs("/F"))
	{
		javaDocFile = nextArg();
		goto readArgs;
	}

	JavaDoc = readLines(javaDocFile);

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
