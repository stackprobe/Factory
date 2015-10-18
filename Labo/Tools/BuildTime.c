#include "C:\Factory\Common\all.h"

static void ShowBuildTime_File(char *file, char *file4Prn)
{
	autoBlock_t *fileData = readBinary(file);
	uint index;
	time_t buildTime;

	for(index = 0; ; index++)
	{
		if(
			getByte(fileData, index + 0) == 'P' &&
			getByte(fileData, index + 1) == 'E' &&
			getByte(fileData, index + 2) == '\0' &&
			getByte(fileData, index + 3) == '\0'
			)
			break;
	}
	index += 8; // ƒwƒbƒ_‚Æ‚©

	buildTime =
		getByte(fileData, index + 0) * 0x1 +
		getByte(fileData, index + 1) * 0x100 +
		getByte(fileData, index + 2) * 0x10000 +
		getByte(fileData, index + 3) * 0x1000000;

	cout("%s %s\n", makeJStamp(getStampDataTime(buildTime), 0), file4Prn);

	releaseAutoBlock(fileData);
}
static void ShowBuildTime(char *path)
{
	if(existDir(path))
	{
		autoList_t *files = lsFiles(path);
		char *file;
		uint index;

//		cout("%s\n", path);

		sortJLinesICase(files);

		foreach(files, file, index)
		{
			if(
				!_stricmp("exe", getExt(file)) ||
				!_stricmp("dll", getExt(file)) ||
				!_stricmp("exe_", getExt(file)) ||
				!_stricmp("dll_", getExt(file))
				)
			{
				ShowBuildTime_File(file, getLocal(file));
			}
			else
			{
				cout("------------------------ %s\n", getLocal(file));
			}
		}
		releaseDim(files, 1);
	}
	else
		ShowBuildTime_File(path, path);
}
int main(int argc, char **argv)
{
	if(hasArgs(1))
	{
		ShowBuildTime(nextArg());
		return;
	}

	for(; ; )
	{
		ShowBuildTime(c_dropDirFile());
		cout("\n");
	}
}
