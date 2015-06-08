#include "mt19937.h"

void mt19937_initByArray(autoBlock_t *initKey)
{
	errorCase(getSize(initKey) < sizeof(uint));
	mt19937_init_by_array((uint *)directGetBuffer(initKey), getSize(initKey) / sizeof(uint));
}
void mt19937_init32(uint seed)
{
	uint count;

	mt19937_init_genrand(seed);

#if 0 // ‚æ[‚í‚©‚ç‚ñ
//	for(count = 1000000; count; count--) // ƒfƒtƒHƒ‹ƒg‚Ì‰Šú‰»Œã 50 - 100 –œŒ’ö“x“Ç‚ÝŽÌ‚Ä‚½•û‚ª‚¢‚¢‚ç‚µ‚¢B
	for(count = 100000000; count; count--) // ‚à‚Á‚Æ·‘å‚É“Ç‚ÝŽÌ‚Ä‚½•û‚ª‚¢‚¢‚æ‚¤‚¾B
	{
		mt19937_genrand_int32();
	}
#endif
}
void mt19937_init(void)
{
	mt19937_init32((uint)time(NULL));
}

uint mt19937_rnd32(void) // ret: 0 ` UINTMAX
{
	return mt19937_genrand_int32();
}
uint mt19937_rnd(uint modulo) // ret: 0 ` (modulo - 1)
{
	uint r_mod;
	uint r;

	errorCase(!modulo);

	if(modulo == 1)
	{
		return 0;
	}
	r_mod = 0x100000000ui64 % modulo;

	do
	{
		r = mt19937_rnd32();
	}
	while(r < r_mod);

#if 1
	r -= r_mod;
	r /= 0x100000000ui64 / modulo;
#else
	r %= modulo;
#endif

	return r;
}
uint mt19937_range(uint minval, uint maxval) // ret: minval ` maxval
{
	errorCase(maxval < minval);

	if(minval == 0 && maxval == UINTMAX)
	{
		return mt19937_rnd32();
	}
	return minval + mt19937_rnd(maxval - minval + 1);
}

uint64 mt19937_rnd64(void) // ret: 0 ` UINT64MAX
{
	return (uint64)mt19937_rnd32() << 32 | (uint64)mt19937_rnd32();
}
uint64 mt19937_rnd64Mod(uint64 modulo) // ret: 0 ` (modulo - 1)
{
	uint modHi = (uint)(modulo >> 32);
	uint modLow = (uint)modulo;
	uint rHi;
	uint rLow;

	if(!modHi)
		return (uint64)mt19937_rnd((uint)modulo);

	if(!modLow)
		return (uint64)mt19937_rnd((uint)modHi) << 32 | (uint64)mt19937_rnd32();

	for(; ; )
	{
		rHi = modHi == UINTMAX ? mt19937_rnd32() : mt19937_rnd(modHi + 1);
		rLow = mt19937_rnd32();

		if(rHi < modHi || rLow < modLow)
			break;
	}
	return (uint64)rHi << 32 | (uint64)rLow;
}
uint64 mt19937_range64(uint64 minval, uint64 maxval) // ret: minval ` maxval
{
	errorCase(maxval < minval);

	if(minval == 0 && maxval == UINT64MAX)
	{
		return mt19937_rnd64();
	}
	return minval + mt19937_rnd64Mod(maxval - minval + 1);
}
void mt19937_rndBlock(void *block, uint size)
{
	uint index;

	for(index = 0; index < size / sizeof(uint); index++)
	{
		((uint *)block)[index] = mt19937_rnd32();
	}
	if(size & 3)
	{
		uint value = mt19937_rnd32();

		memcpy((uint *)block + index, &value, size & 3);
	}
}
