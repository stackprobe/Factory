#include "C:\Factory\Common\all.h"
#include "Define.h"

static uint StartedTime;

static void ShowElapsed(void)
{
	static uint lastTime = UINTMAX;
	uint currTime = now();

	if(lastTime == UINTMAX)
		lastTime = currTime;

	cout("ELAPSED: %u (%u)\n", now() - StartedTime, currTime - lastTime);
	lastTime = currTime;
}
static uint Root(uint64 value)
{
	uint ret = 0;
	uint tBit;

	for(tBit = 1u << 31; tBit; tBit >>= 1)
	{
		uint t = ret | tBit;

		if((uint64)t * t <= value)
		{
			ret = t;
		}
	}
	return ret;
}

#define PRIME_LEN 25878772920ui64 // == (2 * 3 * 5 ... 29) * 4
#define MAP_BIT_LEN 12939386460ui64 // == PRIME_LEN / 2
#define MAP_LEN 404355827 // == Floor(MAP_BIT_LEN / 32)

static uint *Map; // bit_0 == prime, bit_1 == not prime

// ---- Map accessor ----

static uint GetBit(uint64 bit)
{
	return Map[(uint)(bit >> 5)] >> (uint)(bit & 31) & 1;
}
static void SetBit(uint64 bit, uint value)
{
	uint c = Map[(uint)(bit >> 5)];

	if(value)
		c |= 1 << (uint)(bit & 31);
	else
		c &= ~(1 << (uint)(bit & 31));

	Map[(uint)(bit >> 5)] = c;
}

// ---- Prime 3-29 ----

#define MINI_MAP_LEN 105
#define MINI_MAP_BIT_LEN 3360

static void PutMiniPrime(uint prime)
{
	uint64 bit;

	for(bit = prime / 2; bit < MINI_MAP_BIT_LEN; bit += prime)
	{
		SetBit(bit, 1);
	}
}
static void PutPrime3_5_7(void)
{
	uint index;
	uint i;

	LOGPOS();

	for(index = 0; index < MINI_MAP_LEN; index++)
	{
		Map[index] = 0;
	}
	PutMiniPrime(3);
	PutMiniPrime(5);
	PutMiniPrime(7);

	for(index = MINI_MAP_LEN; index < MAP_LEN; index++)
	{
		Map[index] = Map[index % MINI_MAP_LEN];
	}
	LOGPOS();
}
static void PutPrime(uint prime)
{
	uint64 bit;

	for(bit = prime / 2; bit < MAP_BIT_LEN; bit += prime)
	{
		SetBit(bit, 1);
	}
}
static void PutPrime3To29(void)
{
	uint primes[] = { 11, 13, 17, 19, 23, 29 };
	uint index;

	LOGPOS();
	PutPrime3_5_7();

	for(index = 0; index < lengthof(primes); index++)
	{
		PutPrime(primes[index]);
	}
	LOGPOS();
}

// ---- File ----

static FILE *Fp;

static void WritePrime(uint64 prime)
{
	errorCase(fwrite(&prime, 1, F_PRIME_SIZE, Fp) != F_PRIME_SIZE); // !!!! Little endian !!!!
}
static void WritePrime2To29(void)
{
	uint primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
	uint index;

	for(index = 0; index < lengthof(primes); index++)
	{
		WritePrime(primes[index]);
	}
}
static void WriteMap(uint64 baseNumb, uint64 maxNumb)
{
	uint64 bit;

	LOGPOS();

	for(bit = 0; bit < MAP_BIT_LEN; bit++)
	{
		if(!GetBit(bit))
		{
			uint64 prime = baseNumb + bit * 2;

			if(maxNumb < prime)
				break;

			WritePrime(prime);
		}
	}
	LOGPOS();
}

// ---- Prime 31-X ----

#define PRIME_MAX 10000000000000ui64

#define PUT_PRIME_MIN 31
#define PUT_PRIME_MAX 3162277 // == Root(PRIME_MAX)
#define PUT_PRIME_NUM 227637

static uint *P_Primes;

static void PutPrime_FM(uint prime)
{
	uint64 bit;

	for(bit = (prime * 3) / 2; bit < MAP_BIT_LEN; bit += prime)
	{
		SetBit(bit, 1);
	}
}
static void ProcFirstMap(void)
{
	uint bit;
	uint bitMax = Root(PRIME_LEN - 1) / 2;
	uint ppndx = 0;

	LOGPOS();
	ShowElapsed();

	PutPrime3To29();
	SetBit(0, 1); // 1 is not prime

	ShowElapsed();

	for(bit = 15; bit <= bitMax; bit++)
	{
		if(!GetBit(bit))
		{
			uint prime = bit * 2 + 1;

			if(pulseSec(10, NULL))
				cout("ProcFirstMap_bit: %u, %f\n", bit, (double)bit / bitMax);

			PutPrime_FM(prime);

			if(prime <= PUT_PRIME_MAX)
			{
				errorCase(PUT_PRIME_NUM <= ppndx);

				P_Primes[ppndx] = prime;
				ppndx++;
			}
		}
	}
	for(; ; bit++)
	{
		uint prime = bit * 2 + 1;

		if(PUT_PRIME_MAX < prime)
			break;

		if(!GetBit(bit))
		{
			errorCase(PUT_PRIME_NUM <= ppndx);

			P_Primes[ppndx] = prime;
			ppndx++;
		}
	}
	cout("ppndx: %u\n", ppndx);
	errorCase(ppndx != PUT_PRIME_NUM);

	ShowElapsed();

	WritePrime2To29();
	WriteMap(1, UINT64MAX);

	LOGPOS();
	ShowElapsed();
}
static void PutPrimeB(uint64 baseNumb, uint prime)
{
	uint d = baseNumb % prime;
	uint64 bit;

	d = prime - d;

	if(d & 1)
		d += prime;

	for(bit = d / 2; bit < MAP_BIT_LEN; bit += prime)
	{
		SetBit(bit, 1);
	}
}
static void ProcMap(uint64 baseNumb)
{
	uint index;

	LOGPOS();
	cout("baseNumb: %I64u\n", baseNumb);
	ShowElapsed();

	PutPrime3To29();

	ShowElapsed();

	for(index = 0; index < PUT_PRIME_NUM; index++)
	{
		if(pulseSec(10, NULL))
			cout("ProcMap_index: %u, %f\n", index, (double)index / PUT_PRIME_NUM);

		PutPrimeB(baseNumb, P_Primes[index]);
	}
	ShowElapsed();

	WriteMap(baseNumb, PRIME_MAX);

	LOGPOS();
	ShowElapsed();
}

// ----

static int TestMode;

static void ProcPrimeMain(void)
{
	uint64 baseNumb;
	uint lpcnt = 1;

	ProcFirstMap();

	for(baseNumb = PRIME_LEN + 1; baseNumb <= PRIME_MAX; baseNumb += PRIME_LEN)
	{
		ProcMap(baseNumb);

		cout("lpcnt: %u\n", lpcnt);

		if(TestMode && 3 <= lpcnt)
		{
			cout("TEST_BREAK\n");
			break;
		}
		lpcnt++;
	}
}
int main(int argc, char **argv)
{
	if(argIs("/T"))
	{
		TestMode = 1;
	}

	StartedTime = now();

	Map = (uint *)memAlloc(MAP_LEN * sizeof(uint));
	P_Primes = (uint *)memAlloc(PUT_PRIME_NUM * sizeof(uint));
	Fp = fileOpen(OUTPUT_FILE, "wb");

	ProcPrimeMain();

	fileClose(Fp);
	memFree(Map);
	memFree(P_Primes);
}
