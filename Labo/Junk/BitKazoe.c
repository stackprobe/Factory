#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Random.h"

static uint BitKazoe_Test(uint64 value)
{
	uint64 bit;
	uint count = 0;

	for(bit = 1ui64 << 63; bit != 0; bit /= 2)
		if((value & bit) != 0)
			count++;

	return count;
}
static uint BitKazoe(uint64 value)
{
	uint count = 0;

	while(value != 0)
	{
		value ^= value & (~value + 1);
		count++;
	}
	return count;
}
static uint BitKazoe_2(uint64 value)
{
	value = ((value & 0xaaaaaaaaaaaaaaaaui64) >>  1) + (value & 0x5555555555555555ui64);
	value = ((value & 0xccccccccccccccccui64) >>  2) + (value & 0x3333333333333333ui64);
	value = ((value & 0xf0f0f0f0f0f0f0f0ui64) >>  4) + (value & 0x0f0f0f0f0f0f0f0fui64);
	value = ((value & 0xff00ff00ff00ff00ui64) >>  8) + (value & 0x00ff00ff00ff00ffui64);
	value = ((value & 0xffff0000ffff0000ui64) >> 16) + (value & 0x0000ffff0000ffffui64);
	value = ((value & 0xffffffff00000000ui64) >> 32) + (value & 0x00000000ffffffffui64);

	return (uint)value;
}

// ---- map ----

static uchar *Map;

static void MakeMap(void)
{
	uint index;

	Map = memAlloc(256);

	for(index = 0; index < 256; index++)
	{
		uint count = 0;
		uint bit;

		for(bit = 1 << 7; bit; bit /= 2)
			if(index & bit)
				count++;

		Map[index] = count;
	}
}
static uint BitKazoe_Map(uint64 value)
{
	return
		Map[((uchar *)&value)[0]] +
		Map[((uchar *)&value)[1]] +
		Map[((uchar *)&value)[2]] +
		Map[((uchar *)&value)[3]] +
		Map[((uchar *)&value)[4]] +
		Map[((uchar *)&value)[5]] +
		Map[((uchar *)&value)[6]] +
		Map[((uchar *)&value)[7]];
}

// ---- map16 ----

static uchar *Map16;

static void MakeMap16(void)
{
	uint index;

	Map16 = memAlloc(65536);

	for(index = 0; index < 65536; index++)
	{
		uint count = 0;
		uint bit;

		for(bit = 1 << 15; bit; bit /= 2)
			if(index & bit)
				count++;

		Map16[index] = count;
	}
}
static uint BitKazoe_Map16(uint64 value)
{
	return
		Map16[((uint16 *)&value)[0]] +
		Map16[((uint16 *)&value)[1]] +
		Map16[((uint16 *)&value)[2]] +
		Map16[((uint16 *)&value)[3]];
}

// ----

static void Test01(void)
{
	while(!waitKey(0))
	{
		uint64 value = mt19937_rnd64();
		uint n1;
		uint n2;
		uint n3;
		uint n4;
		uint n5;

		n1 = BitKazoe_Test(value);
		n2 = BitKazoe(value);
		n3 = BitKazoe_2(value);
		n4 = BitKazoe_Map(value);
		n5 = BitKazoe_Map16(value);

		cout("%016I64x %u %u %u %u %u\n", value, n1, n2, n3, n4, n5);

		errorCase(n1 != n2);
		errorCase(n1 != n3);
		errorCase(n1 != n4);
		errorCase(n1 != n5);
	}
}
static void Test02(void)
{
	uint c;

	for(c = 100000000; c; c--)
	{
		uint value = mt19937_rnd64();

//		BitKazoe_Test(value);
//		BitKazoe(value);
		BitKazoe_2(value);
//		BitKazoe_Map(value);
//		BitKazoe_Map16(value);
	}
}
int main(int argc, char **argv)
{
	MakeMap();
	MakeMap16();

	mt19937_initRnd((uint)time(NULL));

//	Test01();
	Test02();
}
