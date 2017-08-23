#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\mt19937.h"

static uint PrimeMax = 0xffff;
static uint CheckMax = 0;

static uint MakePrime(void)
{
	uint p;
	uint d;

retry:
	p = mt19937_range(2, PrimeMax);

	for(d = 2; d * d <= p; d = (d + 1) | 1)
	{
		if(p % d == 0)
		{
			goto retry;
		}
	}
	return p;
}
static uint GetLCD(uint x, uint y)
{
	uint c = 1;
	uint d;

	for(d = 2; d <= m_min(x, y); d = (d + 1) | 1)
	{
		while(x % d == 0 && y % d == 0)
		{
			c *= d;
			x /= d;
			y /= d;
		}
	}
	return c * x * y;
}
static uint Calc_xmr(uint x, uint m, uint r) // ret: (x ^ m) % r
{
#define MUL_R(a, b, r) ((uint)(((uint64)(a) * (uint64)(b)) % (uint64)(r)))
#if 1
	if(m == 0)
		return 1;

	if(m == 1)
		return x;

	if(m % 2 == 0)
	{
		uint mx = Calc_xmr(x, m / 2, r);
		return MUL_R(mx, mx, r);
	}
	return MUL_R(x, Calc_xmr(x, m - 1, r), r);
#else
	uint mx = 1;
	uint c;

	for(c = 0; c < m; c++)
	{
		mx = MUL_R(mx, x, r)
	}
	return mx;
#endif
#undef MUL_R
}
static void Check_xmr(uint x, uint m, uint r)
{
	uint mx = Calc_xmr(x, m + 1, r);

	if(mx != x)
	{
		cout("x, mx = %u, %u NG\n", x, mx);
		error();
	}
}
static void DoTest(void)
{
	uint p;
	uint q;
	uint m;
	uint r;
	uint x;

	do
	{
		p = MakePrime();
		q = MakePrime();
	}
	while(p == q);

	cout("P = %u\n", p);
	cout("Q = %u\n", q);

	m = GetLCD(p - 1, q - 1);
	r = p * q;

	cout("M = %u\n", m);
	cout("R = %u\n", r);

	if(CheckMax) // CheckMax 件だけチェック
	{
		uint c;

		for(c = 0; c < CheckMax; c++)
		{
			Check_xmr(mt19937_range(0, r - 1), m, r);
		}
	}
	else // 全件チェック
	{
		for(x = 0; x < r; x++)
		{
			Check_xmr(x, m, r);
		}
	}
	cout("OK!\n");
}
int main(int argc, char **argv)
{
	uint c;

	mt19937_init();

	// ----

	PrimeMax = 13;

	for(c = 0; c < 100; c++)
	{
		DoTest();
	}

	// ----

//	PrimeMax = 101;
//	PrimeMax = 307;
	PrimeMax = 1009;

	for(c = 0; c < 100; c++)
	{
		DoTest();
	}

	// ----

	PrimeMax = 0xffff;
	CheckMax = 100000;

	while(waitKey(0) == 0)
	{
		DoTest();
	}
}
