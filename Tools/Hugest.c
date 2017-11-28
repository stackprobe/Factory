/*
	Hugest.exe [/S 最小の下限] [入力ディレクトリ]
*/

#include "C:\Factory\Common\all.h"

static uint64 SmallestMin = 0;

static void DispHugest(char *dir)
{
	autoList_t *files = lssFiles(dir);
	char *file;
	uint index;
	uint64 minsz;
	uint64 maxsz;

	minsz = UINT64MAX;
	maxsz = 0;

	foreach(files, file, index)
	{
		uint64 csz = getFileSize(file);

		if(csz < minsz && SmallestMin <= csz)
		{
			minsz = csz;
			cout("< %s:%s\n", c_thousandComma(xcout("%I64u", minsz)), file);
		}
		if(maxsz < csz)
		{
			maxsz = csz;
			cout("> %s:%s\n", c_thousandComma(xcout("%I64u", maxsz)), file);
		}
	}
	releaseDim(files, 1);

	cout("====\n");
	cout("%s\n", c_thousandComma(xcout("%I64u", minsz)));
	cout("%s\n", c_thousandComma(xcout("%I64u", maxsz)));
}
int main(int argc, char **argv)
{
	if(argIs("/S"))
	{
		SmallestMin = toValue64(nextArg());
		cout("Smallest-Min: %I64u\n", SmallestMin);
	}

	if(hasArgs(1))
	{
		DispHugest(nextArg());
		return;
	}
	for(; ; )
	{
		DispHugest(c_dropDir());
		cout("\n");
	}
}
