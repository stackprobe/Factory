/*
	.7z ‚Ì“WŠJ‚Ì‚Ý

	- - -

	7z.exe [7z-FILE | ZIP-FILE]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"

static char *Get7zExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\7z1602-extra\\7za.exe");

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

	errorCase(!existFile(file7z));

	dir = makeFreeDir();
	addCwd(dir);
	coExecute_x(xcout("%s x \"%s\"", Get7zExeFile(), file7z));
	unaddCwd();
	dir = Unlittering(dir, file7z);
	execute_x(xcout("START %s", dir));
	memFree(dir);
}
int main(int argc, char **argv)
{
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
