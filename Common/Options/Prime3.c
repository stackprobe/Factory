#include "Prime3.h"

/*
	Keisan 1 / 4 p 60 !/ 2 L 2 ---> 121
*/
//#define M_TEST_CNT 60
#define M_TEST_CNT 20

static uint64 ModMul(uint64 a, uint64 b, uint64 modulo)
{
	uint a4[4];
	uint b4[4];
	uint c4[4];
	uint m4[4];
	UI128_t aa;
	UI128_t bb;
	UI128_t cc;
	UI128_t mm;

	a4[0] = (uint)((a >>  0) & 0xffffffffui64);
	a4[1] = (uint)((a >> 32) & 0xffffffffui64);
	a4[2] = 0;
	a4[3] = 0;

	b4[0] = (uint)((b >>  0) & 0xffffffffui64);
	b4[1] = (uint)((b >> 32) & 0xffffffffui64);
	b4[2] = 0;
	b4[3] = 0;

	m4[0] = (uint)((modulo >>  0) & 0xffffffffui64);
	m4[1] = (uint)((modulo >> 32) & 0xffffffffui64);
	m4[2] = 0;
	m4[3] = 0;

	aa = ToUI128(a4);
	bb = ToUI128(b4);
	mm = ToUI128(m4);

	cc = UI128_Mul(aa, bb, NULL);
	cc = UI128_Mod(cc, mm, NULL);

	FromUI128(cc, c4);

	return (uint64)c4[1] << 32 | c4[0];
}
static uint64 ModPow(uint64 value, uint64 exponent, uint64 modulo)
{
	uint64 ret = 1;

	while(1 <= exponent)
	{
		if(exponent % 2 == 1)
			ret = ModMul(ret, value, modulo);

		value = ModMul(value, value, modulo);
		exponent /= 2;
	}
	return ret;
}
/*
	ミラー・ラビン素数判定法による素数判定
*/
int IsPrime_M(uint64 value)
{
	uint64 d;
	uint64 a;
	uint r;
	uint c;
	uint testCnt;

	if(value <= 1)
		return 0;

	if(value <= 3)
		return 1;

	if(value % 2 == 0)
		return 0;

	d = (value - 1) / 2;

	for(r = 0; d % 2 == 0; r++)
		d /= 2;

	for(testCnt = M_TEST_CNT; testCnt; testCnt--)
	{
		a = ModPow(getCryptoRand64Range(2, value - 2), d, value);

		if(a != 1 && a != value - 1)
		{
			for(c = r; ; c--)
			{
				if(!c)
					return 0;

				a = ModPow(a, 2, value);

				if(a == value - 1)
					break;
			}
		}
	}
	return 1;
}
