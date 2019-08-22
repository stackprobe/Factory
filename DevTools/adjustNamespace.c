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
	char *dir = getParent(file);
	char *relDir;
	char *namespace;

	relDir = changeRoot(dir, rootDir, NULL);
	replaceChar(relDir, '\\', '.');
	namespace = xcout("%s.%s", rootNamespace, relDir);
	memFree(relDir);
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

	targetDir = makeFullPath(targetDir);

	cout("T %s\n", targetDir);

	projFile = GetProjFile(targetDir);
	rootDir = getParent(projFile);
	rootNamespace = GetProjRootNamespace(projFile);

	cout("P %s\n", projFile);
	cout("D %s\n", rootDir);
	cout("N %s\n", rootNamespace);

	errorCase(strlen(targetDir) <= strlen(rootDir)); // targetDir は rootDir の配下であること。

	if(!BatchMode)
	{
		cout("続行？\n");

		if(clearGetKey() == 0x1b)
			termination(0);

		cout("続行します。\n");
	}
	files = lssFiles(targetDir);

	foreach(files, file, index)
	{
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
	releaseDim(files, 1);
	memFree(targetDir);
	memFree(projFile);
	memFree(rootDir);
	memFree(rootNamespace);
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
