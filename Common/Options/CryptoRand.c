#include "CryptoRand.h"

#define POOLSIZE_BGN 8
#define POOLSIZE_MAX (16 * 1024 * 1024) // == POOLSIZE_BGN * (2^n)

uint getCryptoByte(void)
{
	static uchar *pool;
	static uint poolSize = POOLSIZE_BGN;
	static uint index    = POOLSIZE_BGN;

	if(index == poolSize)
	{
		if(poolSize < POOLSIZE_MAX)
		{
			poolSize *= 2;
			pool = memRealloc(pool, poolSize);
		}
		getCryptoBlock_MS(pool, poolSize);
		index = 0;
	}
	return pool[index++];
}
uint getCryptoRand16(void)
{
	return getCryptoByte() | getCryptoByte() << 8;
}
uint getCryptoRand24(void)
{
	return getCryptoByte() | getCryptoByte() << 8 | getCryptoByte() << 16;
}
uint getCryptoRand(void)
{
	return getCryptoByte() | getCryptoByte() << 8 | getCryptoByte() << 16 | getCryptoByte() << 24;
}
uint64 getCryptoRand64(void)
{
	return
		(uint64)getCryptoByte() <<  0 |
		(uint64)getCryptoByte() <<  8 |
		(uint64)getCryptoByte() << 16 |
		(uint64)getCryptoByte() << 24 |
		(uint64)getCryptoByte() << 32 |
		(uint64)getCryptoByte() << 40 |
		(uint64)getCryptoByte() << 48 |
		(uint64)getCryptoByte() << 56;
}
autoBlock_t *makeCryptoRandBlock(uint count)
{
	autoBlock_t *block = createBlock(count);

	while(count)
	{
		addByte(block, getCryptoByte());
		count--;
	}
	return block;
}
