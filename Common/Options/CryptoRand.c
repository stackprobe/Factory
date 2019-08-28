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
void getCryptoBytes(uchar *buff, uint size)
{
	uint index;

	for(index = 0; index < size; index++)
	{
		buff[index] = getCryptoByte();
	}
}
uint getCryptoRand16(void)
{
	uchar r[2];

	getCryptoBytes(r, 2);

	return
		((uint)r[0] << 0) |
		((uint)r[1] << 8);
}
uint getCryptoRand24(void)
{
	uchar r[3];

	getCryptoBytes(r, 3);

	return
		((uint)r[0] << 0) |
		((uint)r[1] << 8) |
		((uint)r[2] << 16);
}
uint getCryptoRand(void)
{
	uchar r[4];

	getCryptoBytes(r, 4);

	return
		((uint)r[0] << 0) |
		((uint)r[1] << 8) |
		((uint)r[2] << 16) |
		((uint)r[3] << 24);
}
uint64 getCryptoRand64(void)
{
	uchar r[8];

	getCryptoBytes(r, 8);

	return
		((uint64)r[0] << 0) |
		((uint64)r[1] << 8) |
		((uint64)r[2] << 16) |
		((uint64)r[3] << 24) |
		((uint64)r[4] << 32) |
		((uint64)r[5] << 40) |
		((uint64)r[6] << 48) |
		((uint64)r[7] << 56);
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
