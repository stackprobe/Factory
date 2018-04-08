// smpl
/*
	TextSort.exe [/S PART-SIZE] [/I] (/1 INPUT-TEXT-FILE | INPUT-TEXT-FILE-1 INPUT-TEXT-FILE-2)
*/
#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\MergeSort.h"

int main(int argc, char **argv)
{
	uint partSize = 128 * 1024 * 1024;

	if(argIs("/S"))
	{
		partSize = toValue(nextArg());
	}

	if(argIs("/I"))
	{
		if(argIs("/1"))
		{
			MergeSortTextICase(getArg(0), getArg(0), partSize);
		}
		else
		{
			MergeSortTextICase(getArg(0), getArg(1), partSize);
		}
	}
	else
	{
		if(argIs("/1"))
		{
			MergeSortText(getArg(0), getArg(0), partSize);
		}
		else
		{
			MergeSortText(getArg(0), getArg(1), partSize);
		}
	}
}
