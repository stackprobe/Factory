#include "Prime2.h"

#define P13_LEN 15015
#define P13_P_NUM 960960 // == P13_LEN * 64
#define PBIT_LEN 61501440 // == P13_LEN * 4096
#define PBIT_P_NUM 3936092160 // == PBIT_LEN * 64

// ---- PBit ----

static uint *PBits; // bit_0 == prime, bit_1 == not prime
static uint64 BaseNumb;

static int GetPBit(uint prime)
{
	uint bit;
	uint index;

	errorCase(prime % 2 == 0);

	bit = prime / 2;
	index = bit / 32;
	bit = bit % 32;

	errorCase(PBIT_LEN <= index);

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

	errorCase(PBIT_LEN <= index);

	c = PBits[index];

	if(flag)
		c |= 1u << bit;
	else
		c &= ~(1u << bit);

	PBits[index] = c;
}

// ----

static void PutP13(uint prime)
{
	uint64 count;

	for(count = prime; count < PBIT_P_NUM; count += prime * 2)
	{
		SetPBit(count, 1);
	}
}
static void PutPrimeTo13(void)
{
	uint primes[] = { 3, 5, 7, 11, 13 };
	uint index;

	for(index = 0; index < P13_LEN; index++)
		PBits[index] = 0;

	for(index = 0; index < lengthof(primes); index++)
		PutP13(primes[index]);

	for(index = P13_LEN; index < PBIT_LEN; index++)
		PBits[index] = PBits[index % P13_LEN];
}
static void PutPrime(uint prime, uint64 maxNumb)
{
	// TODO
}
static void PutPrimeFrom17(void)
{
	// TODO

	uint64 prime;

	for(prime = 17; prime <= 0xffff; prime += 2)
		if(!GetPBit(prime))
			PutPrime(prime, UINTMAX);
}
static void SetRange(uint64 value)
{
	uint index;

	BaseNumb = (value / PBIT_P_NUM) * PBIT_P_NUM;

	if(!PBits)
		PBits = memAlloc(PBIT_LEN * sizeof(uint));

	LOGPOS();
	PutPrimeTo13();
	LOGPOS();
	PutPrimeFrom17();
	LOGPOS();
}
int IsPrimeEx(uint64 value)
{
	uint index;
	uint bit;

	if(value <= UINTMAX)
		return IsPrime((uint)value);

	if(value % 2 == 0)
		return 0;

	if(BaseNumb != (value / PBIT_P_NUM) * PBIT_P_NUM)
		SetRange(value);

	return !GetPBit(value - BaseNumb);
}
