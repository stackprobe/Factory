#include "C:\Factory\Common\all.h"

static void Run_EE(char *file, int exeFlag, int escapedFlag)
{
	file = makeFullPath(file);

	if(existFile(file))
	{
		char *dir = changeLocal(file, "");
		char *lFile = strx(getLocal(file));
		char *lFileOrig = NULL;
		char *lFileDone = NULL;

		addCwd(dir);
		{
			if(escapedFlag)
			{
				lFileOrig = lFile;
				lFile     = changeExt(lFile, exeFlag ? "exe"      : "bat");
				lFileDone = changeExt(lFile, exeFlag ? "exe_done" : "bat_done");

				moveFile(lFileOrig, lFile);
			}
			execute_x(xcout("\"%s\"", lFile));

			if(escapedFlag)
			{
				moveFile(lFile, lFileDone);
			}
		}
		unaddCwd();

		memFree(dir);
		memFree(lFile);
		memFree(lFileOrig);
		memFree(lFileDone);
	}
	memFree(file);
}
static void Run(char *file)
{
	if(!_stricmp("bat", getExt(file)))
	{
		Run_EE(file, 0, 0);
	}
	else if(!_stricmp("bat_", getExt(file)))
	{
		Run_EE(file, 0, 1);
	}
	else if(!_stricmp("exe", getExt(file)))
	{
		Run_EE(file, 1, 0);
	}
	else if(!_stricmp("exe_", getExt(file)))
	{
		Run_EE(file, 1, 1);
	}
	else
	{
		error();
	}
}
int main(int argc, char **argv)
{
	Run(nextArg());
}
