#include "C:\Factory\Common\all.h"

#define RECORD_SIZE 16

static void ReadRecord(FILE *fp, uchar dest[RECORD_SIZE])
{
	errorCase(fread(dest, 1, RECORD_SIZE, fp) != RECORD_SIZE);
}
static BinMD5MergeCounter(char *file1, char *file2, char *outFile)
{
	FILE *fp1 = fileOpen(file1, "rb");
	FILE *fp2 = fileOpen(file2, "rb");
	uint64 size1;
	uint64 size2;
	uint64 countBoth = 0;
	uint64 countOnly1 = 0;
	uint64 countOnly2 = 0;

	size1 = getFileSizeFPSS(fp1);
	size2 = getFileSizeFPSS(fp2);

	errorCase(size1 % RECORD_SIZE != 0);
	errorCase(size2 % RECORD_SIZE != 0);

	size1 /= RECORD_SIZE;
	size2 /= RECORD_SIZE;

	if(size1 == 0)
	{
		countOnly2 = size2 / RECORD_SIZE;
	}
	else if(size2 == 0)
	{
		countOnly1 = size1 / RECORD_SIZE;
	}
	else if(size1 != 0 && size2 != 0)
	{
		uchar buff1[RECORD_SIZE];
		uchar buff2[RECORD_SIZE];
		uint64 currIdx1 = 0;
		uint64 currIdx2 = 0;

		ReadRecord(fp1, buff1);
		ReadRecord(fp2, buff2);

		for(; ; )
		{
			int ret = memcmp(buff1, buff2, RECORD_SIZE);

			if(ret < 0)
			{
				countOnly1++;
				currIdx1++;

				if(size1 <= currIdx1)
					break;

				ReadRecord(fp1, buff1);
			}
			else if(0 < ret)
			{
				countOnly2++;
				currIdx2++;

				if(size2 <= currIdx2)
					break;

				ReadRecord(fp2, buff2);
			}
			else
			{
				countBoth++;
				currIdx1++;
				currIdx2++;

				if(size1 <= currIdx1 || size2 <= currIdx2)
					break;

				ReadRecord(fp1, buff1);
				ReadRecord(fp2, buff2);
			}
		}
		countOnly1 += size1 - currIdx1;
		countOnly2 += size2 - currIdx2;
	}
	fileClose(fp1);
	fileClose(fp2);

	writeOneLineNoRet_b_cx(outFile, xcout("%I64u,%I64u,%I64u", countOnly1, countBoth, countOnly2));
}
int main(int argc, char **argv)
{
	BinMD5MergeCounter(getArg(0), getArg(1), getArg(2));
}
