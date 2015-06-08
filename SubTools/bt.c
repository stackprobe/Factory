/*
	bt == batch tool
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRRandom.h"

int main(int argc, char **argv)
{
	uint retval = 0;

	mt19937_initCRnd();

	if(argIs("CMP"))
	{
		char *file1;
		char *file2;

		file1 = nextArg();
		file2 = nextArg();

		cout("1: %s\n", file1);
		cout("2: %s\n", file2);

		if(isSameFile(file1, file2))
		{
			cout("ˆê’v\n");
			retval = 0;
		}
		else
		{
			cout("•sˆê’v\n");
			retval = 1;
		}
		goto endMain;
	}
	if(argIs("RND"))
	{
		uint minval;
		uint maxval;

		minval = toValue(nextArg());
		maxval = toValue(nextArg());

		cout("minval: %u\n", minval);
		cout("maxval: %u\n", maxval);

		retval = mt19937_range(minval, maxval);
		goto endMain;
	}
endMain:
	cout("retval: %u\n", retval);
	termination(retval);
}
