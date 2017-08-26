/*
	qrumall [/D] [ROOT-DIR]

		/D ... Daily-Release mode
*/

#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	int dailymode = argIs("/D");
	autoList_t *dirs;
	char *dir;
	uint index;

	dirs = lssDirs(hasArgs(1) ? nextArg() : c_dropDir());

	if(dailymode)
	{
		coExecute("rum /c Daily");

		coExecute("frum -qa");
	}
	else
	{
		coExecute("rum /c");

		// confirm
		{
			cout("続行？\n");

			if(clearGetKey() == 0x1b)
				termination(0);

			cout("続行します。\n");
		}
	}

	foreach(dirs, dir, index)
	{
		char *rumDir = addExt(strx(dir), "rum");

		if(existDir(rumDir))
		{
			cout("* %s\n", dir);

			addCwd(dir);
			{
				coExecute("qrum -- -qa");
			}
			unaddCwd();
		}
		memFree(rumDir);
	}
	releaseDim(dirs, 1);

	coExecute("rum /c-");
}
