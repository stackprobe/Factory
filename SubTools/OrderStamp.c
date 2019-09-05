/*
	ファイル名のソート順になるようにタイムスタンプを変更する。

	OrderStamp.exe [/-I] [対象ディレクトリ]

		/-I ... case sensitive
*/

#include "C:\Factory\Common\all.h"

static int CaseSensitiveFlag;

static void DoOrderStamp(void)
{
	autoList_t *files = lsFiles(".");
	char *file;
	uint index;
	time_t t;

	LOGPOS();

	eraseParents(files);

	if(CaseSensitiveFlag)
		rapidSortLines(files);
	else
		sortJLinesICase(files);

	t = time(NULL) - getCount(files);

	foreach(files, file, index)
	{
		uint64 stamp = getFileStampByTime(t++);

		cout("%I64u -> %s\n", stamp, file);

		setFileStamp(file, stamp, stamp, stamp);
	}
	releaseDim(files, 1);

	LOGPOS();
}
int main(int argc, char **argv)
{
	if(argIs("/-I"))
	{
		CaseSensitiveFlag =1;
	}

	addCwd(hasArgs(1) ? nextArg() : c_dropDir());
	DoOrderStamp();
	unaddCwd();
}
