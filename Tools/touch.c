#include "C:\Factory\Common\all.h"

static void Touch_File(char *file)
{
	stampData_t *sd = getStampDataTime(0);
	uint64 stamp;

	cout("%s\n", file);

	stamp = sd->year;
	stamp *= 100; stamp += sd->month;
	stamp *= 100; stamp += sd->day;
	stamp *= 100; stamp += sd->hour;
	stamp *= 100; stamp += sd->minute;
	stamp *= 100; stamp += sd->second;
	stamp *= 1000;

	setFileStamp(file, 0ui64, stamp, stamp);
}
static void Touch(char *path)
{
	if(existDir(path))
	{
		autoList_t *files = lssFiles(path);
		char *file;
		uint index;

		foreach(files, file, index)
			Touch_File(file);

		releaseDim(files, 1);
	}
	else
		Touch_File(path);
}
int main(int argc, char **argv)
{
	if(hasArgs(1))
	{
		do
		{
			Touch(nextArg());
		}
		while(hasArgs(1));
	}
	else
	{
		for(; ; )
		{
			Touch(c_dropDirFile());
			cout("\n");
		}
	}
}
