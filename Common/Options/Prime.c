#include "Prime.h"

// ---- PBit ----

#define PBIT_LEN 0x04000000

static uint *PBits; // bit_0 == prime, bit_1 == not prime

static int GetPBit(uint prime)
{
	uint bit;
	uint index;

	if(prime == 2)
		return 0;

	if(prime % 2 == 0)
		return 1;

	bit = prime / 2;
	index = bit / 32;
	bit = bit % 32;

//	errorCase(PBIT_LEN <= index);

	return PBits[index] >> bit & 1;
}
static void SetPBit(uint prime, int flag)
{
	uint bit;
	uint index;
	uint c;

	errorCase(prime % 2 == 0);

	bit = prime / 2;
	index = bit / 32;
	bit = bit % 32;

//	errorCase(PBIT_LEN <= index);

	c = PBits[index];

	if(flag)
		c |= 1u << bit;
	else
		c &= ~(1u << bit);

	PBits[index] = c;
}

// ---- Save Load ----

#define DAT_FILE "C:\\Factory\\tmp\\Prime.dat"

static void SavePBits(void)
{
	autoBlock_t gab;

	if(isFactoryDirDisabled())
		return;

	writeBinary(DAT_FILE, gndBlockVar(PBits, PBIT_LEN * sizeof(uint), gab));
}
static int LoadPBits(void)
{
	FILE *fp;
	autoBlock_t gab;

	if(isFactoryDirDisabled() || !existFile(DAT_FILE))
		return 0;

	errorCase(getFileSize(DAT_FILE) != PBIT_LEN * sizeof(uint));

	fp = fileOpen(DAT_FILE, "rb");
	fileRead(fp, gndBlockVar(PBits, PBIT_LEN * sizeof(uint), gab));
	fileClose(fp);

	return 1;
}

// ---- INIT ----

static void PutPrime(uint prime, uint maxNumb)
{
	uint64 count;

	for(count = prime * prime; count <= maxNumb; count += prime * 2)
	{
		SetPBit(count, 1);
	}
}
static void PutPrimeTo13(void)
{
	uint primes[] = { 3, 5, 7, 11, 13 };
	const uint PB_BND = 15015;
	uint index;

	for(index = 0; index < lengthof(primes); index++)
	{
		PutPrime(primes[index], PB_BND * 64 - 1);
		SetPBit(primes[index], 1);
	}
	for(index = PB_BND; index < PBIT_LEN; index++)
		PBits[index] = PBits[index % PB_BND];

	for(index = 0; index < lengthof(primes); index++)
		SetPBit(primes[index], 0);

	SetPBit(1, 1);
}
static void PutPrimeFrom17(void)
{
	uint prime;

	for(prime = 17; prime <= 0xffff; prime += 2)
		if(!GetPBit(prime))
			PutPrime(prime, UINTMAX);
}
static void DoINIT(void)
{
	PBits = (uint *)memCalloc(PBIT_LEN * sizeof(uint));

	if(LoadPBits())
		return;

	LOGPOS();
	PutPrimeTo13();
	LOGPOS();
	PutPrimeFrom17();
	LOGPOS();

	SavePBits();
}
static void INIT(void)
{
	static int inited;

	if(!inited)
	{
		DoINIT();
		inited = 1;
	}
}

// ----

static int IsPrime_32(uint value)
{
	INIT();
	return !GetPBit(value);
}
int IsPrime(uint64 value)
{
	uint64 denom; // maxDenom is max UINTMAX
	uint maxDenom;

	if(value <= UINTMAX)
		return IsPrime_32((uint)value);

	if(value % 2 == 0)
		return 0;

	maxDenom = iSqrt64(value);

	for(denom = 3; denom <= maxDenom; denom += 2)
		if(IsPrime_32(denom) && value % denom == 0)
			return 0;

	return 1;
}
void Factorization(uint64 value, uint64 dest[64]) // dest: Å‘å 63 ŒÂ, ÅŒã‚Ì—v‘f‚Í 0 ‚Å•Â‚¶‚éB1 ‚Ì‚Æ‚«‚Í { 1 }, 0 ‚Ì‚Æ‚«‚Í { } ‚ð•Ô‚·B
{
	uint wPos = 0;

	if(value == 0)
	{
		// noop
	}
	else if(value == 1)
	{
		dest[wPos++] = 1;
	}
	else if(value <= UINTMAX && IsPrime_32((uint)value))
	{
		dest[wPos++] = value;
	}
	else
	{
		uint64 denom; // maxDenom is max UINTMAX
		uint maxDenom = iSqrt64(value);

		while(value % 2 == 0)
		{
			dest[wPos++] = 2;
			value /= 2;

			if(value < 2)
				goto value_one;
		}
		for(denom = 3; denom <= maxDenom; denom += 2)
		{
			if(IsPrime_32(denom))
			{
				while(value % denom == 0)
				{
					dest[wPos++] = denom;
					value /= denom;

					if(value < 2)
						goto value_one;
				}
			}
		}
		dest[wPos++] = value;

	value_one:
		errorCase(value < 1); // 2bs
	}
	dest[wPos] = 0;
}
