#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static void RareErrorCase(int state, int *p_w)
{
	if(state)
		*p_w += 10;
	else
		*p_w && --*p_w;

	errorCase(100 < *p_w);
}

// ---- kokokara

uint16 CRnd16(void); // cryptographically random number generator

static uint GetLCD(uint a, uint b)
{
}
static uint ModPow(uint v, uint e, uint m)
{
	uint64 r;

	if(!e)
		return 1;

	r = ModPow(v, e / 2, m);
	r *= r;
	r %= m;

	if(e & 1)
	{
		r *= v;
		r %= m;
	}
	return r;
}
static void TestMain(void)
{
	static uchar ops[0x1000];
	uint c, p, q, m, e, d; // 32bit

memset(ops, 0x00, 0x1000); // not needed

#define op(p) (ops[(p) / 16] & 1 << (p) / 2 % 8)

	ops[0] = 1;

	for(c = 3; c < 0x100; c += 2)
		if(!op(c))
			for(d = c * c; d < 0x10000; d += c * 2)
				ops[d / 16] |= 1 << d / 2 % 8;

genPQ:
	do {
		do p = CRnd16() | 1; while(op(p));
		do q = CRnd16() | 1; while(op(q));
	}
	while(p == q || p * q < 0x10000);

	m = p * q; // max are 65519, 65521
	p--;
	q--;

	for(e = p, d = q; e != d; e < d ? (d -= e) : (e -= d));

	p *= q / e;
	p++;

	for(e = 3; op(e) || p % e; e += 2)
		if(p / e < e)
			goto genPQ;

	d = p / e;

	printf("m=%u e=%u d=%u\n", m, e, d);

	p = CRnd16();
	c = ModPow(p, e, m);
	q = ModPow(c, d, m);

	printf("p=%u c=%u q=%u\n", p, c, q); // always p == q

	// ----

	{ static int w; RareErrorCase(p == c, &w); }
	errorCase(p != q);
}

// ---- kokomade

static uint16 CRnd16(void)
{
	return (uint16)getCryptoRand16();
}
int main(int argc, char **argv)
{
	for(; ; )
	{
		TestMain();
	}
}
