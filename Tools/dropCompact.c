#include "C:\Factory\Common\all.h"

#define COMPACT_FILE_SIZE_MAX 5000000000ui64 // 5GB

static void CompactDirHasHugeFile(char *dir)
{
	autoList_t *paths = lss(dir);
	char *path;
	uint index;

	foreach(paths, path, index)
	{
		if(existFile(path) && COMPACT_FILE_SIZE_MAX < getFileSize(path))
		{
			coExecute_x(xcout("Compact.exe /U \"%s\"", path));
		}
		else
		{
			coExecute_x(xcout("Compact.exe /C \"%s\"", path));
		}
	}
	releaseDim(paths, 1);
}
static int HasHugeFile(char *dir)
{
	autoList_t *files = lssFiles(dir);
	char *file;
	uint index;
	int ret = 0;

	foreach(files, file, index)
	{
		cout("* %s\n", file);

		if(COMPACT_FILE_SIZE_MAX < getFileSize(file))
		{
			cout("‚±‚ê‚Å‚©‚¢I\n");
			ret = 1;
			break;
		}
	}
	releaseDim(files, 1);
	return ret;
}
int main(int argc, char **argv)
{
	for(; ; )
	{
		char *path = dropDirFile();

		if(existFile(path))
		{
			if(HasHugeFile(path))
			{
				CompactDirHasHugeFile(path);
			}
			else
			{
				coExecute_x(xcout("Compact.exe /C /S:\"%s\"", path));
			}
		}
		else // file
		{
			coExecute_x(xcout("Compact.exe /C \"%s\"", path));
		}
	}
}
