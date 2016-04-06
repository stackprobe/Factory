/*
	.7z ‚Ì“WŠJ‚Ì‚Ý

	- - -

	7z.exe [7z-FILE]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"

static char *Get7zExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\7za920\\7za.exe");

	return file;
}
static void Extract7z(char *file7z)
{
	char *dir;

	errorCase(!existFile(file7z));

	dir = makeFreeDir();
	addCwd(dir);
	coExecute_x(xcout("%s x \"%s\"", Get7zExeFile(), file7z));
	unaddCwd();
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
