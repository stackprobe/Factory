/*
	.7z の展開のみ

	- - -

	7z.exe [/C] [/T] [/OAD] [7z-FILE | ZIP-FILE]

		/C   ... 入力ファイルと同じ場所に展開する。
		/T   ... 不要な上位階層を除去する。
		/OAD ... 元ファイル自動削除
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"
#include "C:\Factory\Meteor\7z.h"

static int ExtractSameDir;
static int TrimOneDir;
static int OutputAndDelete;

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
static void DoTrimOneDir(char *wDir)
{
	char *rDir = strx(wDir);
	int dove = 0;

	while(lsCount(rDir) == 1)
	{
		autoList_t *paths = ls(rDir);
		char *path;

		path = getLine(paths, 0);

		if(!existDir(path))
		{
			releaseDim(paths, 1);
			break;
		}
		memFree(rDir);
		rDir = path;
		releaseAutoList(paths);
		dove = 1;
	}
	if(dove)
	{
		char *midDir = strx(wDir);

		midDir = toCreatablePath(midDir, IMAX);

		cout("< %s\n", rDir);
		cout("> %s\n", wDir);
		cout("M %s\n", midDir);

		createDir(midDir);
		moveDir(rDir, midDir);
		recurRemoveDir(wDir);
		moveFile(midDir, wDir);
	}
	memFree(rDir);
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
		dir = toCreatableTildaPath(dir, 10000);

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
	if(TrimOneDir)
		DoTrimOneDir(dir);

	if(OutputAndDelete)
	{
		LOGPOS();
		removeFile(file7z);
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
	if(argIs("/T"))
	{
		cout("TRIM-ONE-DIR\n");

		TrimOneDir = 1;
		goto readArgs;
	}
	if(argIs("/OAD"))
	{
		cout("+-------------------+\n");
		cout("| OUTPUT AND DELETE |\n");
		cout("+-------------------+\n");

		OutputAndDelete = 1;
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
