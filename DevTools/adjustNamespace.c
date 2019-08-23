/*
	adjustNamespace.exe [変換を行うソースのディレクトリ]

		変換を行うソースのディレクトリ は プロジェクトのルートディレクトリ と同じか配下であること。

		プロジェクトのルートディレクトリ == .csproj があるディレクトリ
*/

#include "C:\Factory\Common\all.h"

static int BatchMode;

static char *GetProjFile_Dir(char *dir)
{
	autoList_t *files = lsFiles(dir);
	char *file;
	uint index;
	char *projFile;

	foreach(files, file, index)
		if(!_stricmp("csproj", getExt(file)))
			break;

	if(file)
		projFile = strx(file);
	else
		projFile = NULL;

	releaseDim(files, 1);
	return projFile;
}
static char *GetProjFile(char *dir)
{
	char *projFile;

	dir = strx(dir);

	while(!(projFile = GetProjFile_Dir(dir)))
	{
		errorCase(isRootDirAbs(dir));

		dir = getParent_x(dir);
	}
	memFree(dir);
	return projFile;
}
static char *GetProjRootNamespace(char *projFile)
{
	autoList_t *lines = readLines(projFile);
	char *line;
	uint index;
	char *ret = NULL;

	foreach(lines, line, index)
	{
		ucTrim(line);

		if(startsWith(line, "<RootNamespace>") && endsWith(line, "</RootNamespace>"))
		{
			ret = strx(ne_strchr(line, '>') + 1);
			*ne_strchr(ret, '<') = '\0';
			goto endFunc;
		}
	}
	errorCase(!ret);

endFunc:
	releaseDim(lines, 1);
	return ret;
}
static char *GetNamespaceFromLine(char *line)
{
	return strx(line + 10);
}
static char *GetNamespaceFromPath(char *rootNamespace, char *rootDir, char *file)
{
	char *namespace;

	file = changeRoot(strx(file), rootDir, NULL);
	namespace = xcout("%s\\%s", rootNamespace, file);
	namespace = changeLocal_xc(namespace, "");
	replaceChar(namespace, '\\', '.');

	memFree(file);
	return namespace;
}
static void AdjustNamespace(char *targetDir)
{
	char *projFile;
	char *rootDir;
	char *rootNamespace;
	autoList_t *files;
	char *file;
	uint index;
	char *ignPathPfx_1;
	char *ignPathPfx_2;
	char *ignPathPfx_3;

	targetDir = makeFullPath(targetDir);

	cout("T %s\n", targetDir);

	projFile = GetProjFile(targetDir);
	rootDir = getParent(projFile);
	rootNamespace = GetProjRootNamespace(projFile);

	cout("P %s\n", projFile);
	cout("D %s\n", rootDir);
	cout("N %s\n", rootNamespace);

	if(!BatchMode)
	{
		cout("続行？\n");

		if(clearGetKey() == 0x1b)
			termination(0);

		cout("続行します。\n");
	}
	files = lssFiles(targetDir);

	ignPathPfx_1 = combine(rootDir, "bin");
	ignPathPfx_2 = combine(rootDir, "obj");
	ignPathPfx_3 = combine(rootDir, "Properties");

	ignPathPfx_1 = addChar(ignPathPfx_1, '\\');
	ignPathPfx_2 = addChar(ignPathPfx_2, '\\');
	ignPathPfx_3 = addChar(ignPathPfx_3, '\\');

	foreach(files, file, index)
	{
		if(
			startsWithICase(file, ignPathPfx_1) ||
			startsWithICase(file, ignPathPfx_2) ||
			startsWithICase(file, ignPathPfx_3)
			)
		{
			LOGPOS();
			continue;
		}

		if(!_stricmp("cs", getExt(file)))
		{
			autoList_t *lines = readLines(file);
			char *line;
			uint index;
			int modified = 0;

			foreach(lines, line, index)
			{
				if(startsWith(line, "namespace "))
				{
					char *oldNamespace = GetNamespaceFromLine(line);
					char *newNamespace = GetNamespaceFromPath(rootNamespace, rootDir, file);

					cout("< %s\n", oldNamespace);
					cout("> %s\n", newNamespace);

					if(strcmp(oldNamespace, newNamespace))
					{
						LOGPOS();
						memFree(line);
						line = xcout("namespace %s", newNamespace);
						setElement(lines, index, (uint)line);
						modified = 1;
					}
					memFree(oldNamespace);
					memFree(newNamespace);
				}
			}
			if(modified)
			{
				LOGPOS();
				semiRemovePath(file);
				writeLines(file, lines);
			}
			releaseDim(lines, 1);
		}
	}
	LOGPOS();
	releaseDim(files, 1);
	memFree(targetDir);
	memFree(projFile);
	memFree(rootDir);
	memFree(rootNamespace);
	memFree(ignPathPfx_1);
	memFree(ignPathPfx_2);
	memFree(ignPathPfx_3);
	LOGPOS();
}
int main(int argc, char **argv)
{
	if(argIs("/B"))
	{
		BatchMode = 1;
	}

	if(hasArgs(1))
	{
		AdjustNamespace(nextArg());
		return;
	}

	{
		char *targetDir;

		cout("[C#]変換を行うソースのディレクトリ：\n");
		targetDir = dropDir();

		AdjustNamespace(targetDir);

		memFree(targetDir);
	}
}
