#include "C:\Factory\Common\all.h"
#include "Define.h"

static autoList_t *KnownPrimes;

static IsPrime(uint64 value)
{
	uint index;
	uint denom;
	uint maxDenom = iSqrt64(value);

//	cout("IsPrime_value: %I64u\n", value);

	if(value < 2)
		return 0;

	if(value == 2)
		return 1;

	foreach(KnownPrimes, denom, index)
	{
		if(maxDenom < denom)
			break;

		if(value % denom == 0)
			return 0;
	}
	return 1;
}
static void CheckPrime(uint64 value, int assume)
{
	if(IsPrime(value) ? !assume : assume)
	{
		cout("CheckPrime_value: %I64u %d\n", value, assume);
		error();
	}
}

static FILE *Fp;

static uint64 NextPrime(void)
{
	uint ret;
	uint64 prime;

	ret = fread(&prime, 1, F_PRIME_SIZE, Fp);

	if(ret == 0)
		return UINT64MAX;

	errorCase(ret != F_PRIME_SIZE);
	prime &= 0x0000ffffffffffffui64;
//	cout("prime: %I64u\n", prime);

	if(prime <= UINTMAX)
		addElement(KnownPrimes, (uint)prime);

	return prime;
}
static void DoTest(void)
{
	uint64 chaser = 1;

	for(; ; )
	{
		uint64 prime = NextPrime();

		if(pulseSec(2, NULL))
			cout("prime: %I64u\n", prime);

		if(prime == UINT64MAX)
			break;

		errorCase(prime <= chaser);

		for(; ; )
		{
			chaser++;

			if(prime <= chaser)
				break;

			CheckPrime(chaser, 0);
		}
		CheckPrime(chaser, 1);
	}
}
int main(int argc, char **argv)
{
	KnownPrimes = newList();
	Fp = fileOpen(OUTPUT_FILE, "rb");

	DoTest();

	fileClose(Fp);
	releaseAutoList(KnownPrimes);
}
