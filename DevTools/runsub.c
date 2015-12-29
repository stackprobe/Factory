#include "C:\Factory\Common\all.h"
#include "C:\Factory\SubTools\BlueFish\libs\Lock.h"

static uint NonBlockingMode;
static uint TimeWaitSec;

static void Run(char *file)
{
	char *absPath = c_makeFullPath(file);

	cout("runsub: %s\n", absPath);
	execute_x(xcout("TITLE runsub - %s", absPath));

	BlueFish_Lock();
	{
		if(NonBlockingMode)
			execute_x(xcout("START CMD /C \"%s\"", file));
		else
			execute(file);
	}
	BlueFish_Unlock();

	execute("TITLE runsub");
	cout("runsub: %s done\n", absPath);

	if(TimeWaitSec)
		coSleep(TimeWaitSec * 1000);
}
int main(int argc, char **argv)
{
	int intoSubDirMode = 1;
	int skipRootDir = 0;
	char *rootDir = ".";
	char *target;
	char *batch;
	char *execu;
	autoList_t *dirs;
	char *dir;
	uint index;

readArgs:
	if(argIs("/-B"))
	{
		NonBlockingMode = 1;
		goto readArgs;
	}
	if(argIs("/-S"))
	{
		intoSubDirMode = 0;
		goto readArgs;
	}
	if(argIs("/-R"))
	{
		skipRootDir = 1;
		goto readArgs;
	}
	if(argIs("/R"))
	{
		rootDir = nextArg();
		goto readArgs;
	}
	if(argIs("/T"))
	{
		TimeWaitSec = toValue(nextArg());
		goto readArgs;
	}
	target = nextArg();

	batch = addExt(strx(target), "bat");
	execu = addExt(strx(target), "exe");

	if(intoSubDirMode)
	{
		dirs = lssDirs(rootDir);
		rapidSortLines(dirs);

		if(!skipRootDir)
			insertElement(dirs, 0, (uint)strx(rootDir));
	}
	else
		dirs = createOneElement((uint)strx(rootDir));

	foreach(dirs, dir, index)
	{
		if(!existDir(dir)) // Ç±ÇÍÇ‹Ç≈ÇÃ Run() Ç…ÇÊÇ¡ÇƒçÌèúÇ≥ÇÍÇÈÇ±Ç∆Ç‡Ç†ÇÈÅB
			continue;

		addCwd(dir);

		if(target[0] == '*')
		{
			autoList_t *files = lsFiles(".");
			char *file;
			uint file_index;

			eraseParents(files);
			rapidSortLines(files);

			foreach(files, file, file_index)
			{
				if(
					!_stricmp("bat", getExt(file)) ||
					!_stricmp("exe", getExt(file))
					)
				{
					if(target[1] == '\0' || mbs_strstrCase(file, target + 1, 1))
					{
						Run(file);
					}
				}
			}
			releaseDim(files, 1);
		}
		else
		{
			if(existFile(batch))
			{
				Run(batch);
			}
			if(existFile(execu))
			{
				Run(execu);
			}
		}
		unaddCwd();
	}
	releaseDim(dirs, 1);
	memFree(batch);
	memFree(execu);
}
