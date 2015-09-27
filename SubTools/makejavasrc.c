#include "C:\Factory\Common\all.h"

static char *GetPackage(char *javaFile)
{
	FILE *fp = fileOpen(javaFile, "rb");
	char *ret;

	cout("*P %s\n", javaFile);

	for(; ; )
	{
		char *line = readLine(fp);

		if(!line)
		{
			ret = strx("_UnknownPackage");
			break;
		}
		ucTrim(line);

		if(startsWith(line, "package "))
		{
			char *p = line + 8;
			char *q;

			q = strchr(p, ';');
			errorCase(!q);
			*q = '\0';

			ret = strx(p);

			memFree(line);
			break;
		}
		memFree(line);
	}
	fileClose(fp);
	return ret;
}
static char *GetJavaClassName(char *javaFile)
{
	FILE *fp = fileOpen(javaFile, "rb");
	int incomment = 0;
	char *ret;

	cout("*C %s\n", javaFile);

	for(; ; )
	{
		char *line = readLine(fp);
		char *p;

		if(!line)
		{
			ret = strx("_UnknownClass");
			break;
		}
		ucTrim(line);

		if(*line == '/') // ? コメント行っぽい。XXX
			*line = '\0';

		if(*line == '*') // ? コメント行っぽい。XXX
			*line = '\0';

		p = strstr(line, "class ");

		if(!p)
			p = strstr(line, "interface ");

		if(!p)
			p = strstr(line, "enum ");

		if(p)
		{
			char *q;

			p = strchr(p, ' ');
			errorCase(!p);
			p++;
			q = strchr(p, ' ');
			if(q) *q = '\0';
			q = strchr(p, '<');
			if(q) *q = '\0';
			q = strchr(p, '{');
			if(q) *q = '\0';

			ret = strx(p);
			trimEdge(ret, ' ');

			memFree(line);
			break;
		}
		memFree(line);
	}
	fileClose(fp);
	return ret;
}
static void ExtractJava(char *javaFile, char *packageRootDir)
{
	char *package;
	char *className;
	char *wFile;

	package = GetPackage(javaFile);
	className = GetJavaClassName(javaFile);

	replaceChar(package, '.', '/');
	wFile = xcout("%s/%s.java", package, className);
	restoreYen(wFile);
	wFile = lineToFairRelPath_x(wFile, strlen(packageRootDir));
	wFile = combine_cx(packageRootDir, wFile);
	wFile = toCreatablePath(wFile, UINTMAX);

	createPath(wFile, 'X');
	copyFile(javaFile, wFile);

	memFree(package);
	memFree(className);
}
static void ExtractClass(char *classFile, char *wDir)
{
	char *file = makeTempPath("java");

	coExecute_x(xcout("C:\\Factory\\SubTools\\jad.exe /DJ \"%s\" \"%s\"", classFile, file));

	ExtractJava(file, wDir);

	removeFile(file);
	memFree(file);
}
static void ExtractAllJava(char *rDir, char *wDir)
{
	autoList_t *files = lssFiles(rDir);
	char *file;
	uint index;

	sortJLinesICase(files);

	foreach(files, file, index)
		if(!_stricmp("java", getExt(file)))
			ExtractJava(file, wDir);

	releaseDim(files, 1);
}
static void ExtractAllClass(char *rDir, char *wDir)
{
	autoList_t *files = lssFiles(rDir);
	char *file;
	uint index;

	sortJLinesICase(files);

	foreach(files, file, index)
		if(!_stricmp("class", getExt(file)) && !strchr(getLocal(file), '$'))
			ExtractClass(file, wDir);

	releaseDim(files, 1);
}
static void ExtractJar(char *jarFile, char *wDir)
{
	char *dir = makeFreeDir();

	coExecute_x(xcout("C:\\Factory\\SubTools\\zip.exe /X \"%s\" \"%s\"", jarFile, dir));

	addCwd(dir);
	coExecute("ATTRIB.EXE -R /S");
	unaddCwd();

	ExtractAllJava(dir, wDir);
	ExtractAllClass(dir, wDir);

	forceRemoveDir(dir);
	memFree(dir);
}
static void ExtractAllJar(char *rDir, char *wDir, char *target_ext)
{
	autoList_t *files = lssFiles(rDir);
	char *file;
	uint index;

	sortJLinesICase(files);

	foreach(files, file, index)
		if(!_stricmp(target_ext, getExt(file)))
			ExtractJar(file, wDir);

	releaseDim(files, 1);
}
static void MakeJavaSrc(char *rDir, char *wDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);

	errorCase(!existDir(rDir));
	createPath(wDir, 'X');
	createDirIfNotExist(wDir);

	ExtractAllJava(rDir, wDir);
	ExtractAllJar(rDir, wDir, "zip");
	ExtractAllClass(rDir, wDir);
	ExtractAllJar(rDir, wDir, "jar");

	memFree(rDir);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	antiSubversion = 1;

	{
		char *rDir;
		char *wDir;

		rDir = nextArg();
		wDir = nextArg();

		MakeJavaSrc(rDir, wDir);
		return;
	}
}
