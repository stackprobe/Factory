/*
	.7z の展開のみ

	- - -

	7z.exe [/C] [7z-FILE | ZIP-FILE]

		/C ... 入力ファイルと同じ場所に展開する。
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"
#include "C:\Factory\Meteor\7z.h"

static int ExtractSameDir;

static char *Get7zExeFile(void) // ret: 空白を含まないパスであること。
{
	static char *file;

	if(!file)
		file = GetCollaboFile(FILE_7Z_EXE);

	return file;
}
static char *Unlittering(char *dir, char *file7z)
{
	if(2 <= lsCount(dir))
	{
		char *wRtDir = makeFreeDir();
		char *wDir;
		char *name = changeExt(getLocal(file7z), "");

		wDir = combine(wRtDir, name);
		createDir(wDir);

		LOGPOS();
		cout("< %s\n", dir);
		cout("> %s\n", wDir);

		moveDir(dir, wDir);

		memFree(dir);
		dir = wRtDir;
		memFree(wDir);
		memFree(name);
	}
	return dir;
}
static void Extract7z(char *file7z)
{
	char *dir;
	char *dir2;

	file7z = makeFullPath(file7z);

	cout("< %s\n", file7z);

	errorCase(!existFile(file7z));
	errorCase(!*getExt(file7z)); // ? 拡張子ナシ

	if(ExtractSameDir)
	{
		dir = changeExt(file7z, "");
		dir = toCreatablePath(dir, 100);

		cout("> %s\n", dir);

		createDir(dir);
		addCwd(dir);
		coExecute_x(xcout("%s x \"%s\"", Get7zExeFile(), file7z));
		unaddCwd();
	}
	else
	{
		dir = makeFreeDir();

		cout("> %s\n", dir);

		addCwd(dir);
		coExecute_x(xcout("%s x \"%s\"", Get7zExeFile(), file7z));
		unaddCwd();
		dir = Unlittering(dir, file7z);
		execute_x(xcout("START %s", dir));
	}
	memFree(dir);
	memFree(file7z);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/C"))
	{
		cout("+----------------+\n");
		cout("| 同じ場所に展開 |\n");
		cout("+----------------+\n");

		ExtractSameDir = 1;
		goto readArgs;
	}

	if(hasArgs(1))
	{
		Extract7z(nextArg());
		return;
	}

	for(; ; )
	{
		Extract7z(c_dropFile());
		cout("\n");
	}
}
