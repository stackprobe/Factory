#include "C:\Factory\Common\all.h"

#define DAT_FILE "P10T.dat"

static IsPrime(uint64 value)
{
	uint denom;
	uint maxDenom = iSqrt64(value);

	cout("IsPrime_value: %I64u\n", value);

	if(value < 2)
		return 0;

	if(value == 2)
		return 1;

	if(value % 2 == 0)
		return 0;

	for(denom = 3; denom <= maxDenom; denom += 2)
		if(value % denom == 0)
			return 0;

	return 1;
}

static FILE *Fp;

static uint64 NextPrime(void)
{
	uint ret;
	uint64 prime;

	ret = fread(&prime, 1, 6, Fp);

	if(ret == 0)
		return UINT64MAX;

	errorCase(ret != 6);
	return prime & 0x0000ffffffffffffui64;
}
static void DoTest(void)
{
	uint64 chaser = 1;

	for(; ; )
	{
		uint64 prime = NextPrime();

		cout("prime: %I64u\n", prime);

		if(prime == UINT64MAX)
			break;

		errorCase(prime <= chaser);

		for(; ; )
		{
			chaser++;

			if(prime <= chaser)
				break;

			errorCase(IsPrime(chaser));
		}
		errorCase(!IsPrime(chaser));
	}
}
int main(int argc, char **argv)
{
	Fp = fileOpen(DAT_FILE, "rb");

	DoTest();

	fileClose(Fp);
	Fp = NULL;
}
