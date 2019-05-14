#include "C:\Factory\Common\all.h"

static void Run_EE(char *file, int exeFlag, int escapedFlag)
{
	file = makeFullPath(file);

	if(existFile(file))
	{
		char *dir = changeLocal(file, "");
		char *localFile = strx(getLocal(file));
		char *localFileOrig = NULL;
		char *localFileDone = NULL;

		addCwd(dir);
		{
			if(escapedFlag)
			{
				localFileOrig = localFile;
				localFile     = changeExt(localFile, exeFlag ? "exe"      : "bat");
				localFileDone = changeExt(localFile, exeFlag ? "exe_done" : "bat_done");

				moveFile(localFileOrig, localFile);
			}
			execute_x(xcout("\"%s\"", localFile));

			if(escapedFlag)
			{
				moveFile(localFile, localFileDone);
			}
		}
		unaddCwd();

		memFree(dir);
		memFree(localFile);
		memFree(localFileOrig);
		memFree(localFileDone);
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
