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

uint16 CRnd16(void);

static uint ModPow(uint v, uint e, uint m)
{
	uint64 r;

	if(!e)
		return 1;

	if(e == 1)
		return v;

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
	static char tp[0x8000];
	uint c, p, q, m, e, d; // 32bit

memset(tp, 0x00, 0x8000); // not needed

	tp[0] = 1;

	for(c = 3; c < 0x100; c += 2)
		if(!tp[c / 2])
			for(d = c * c; d < 0x10000; d += c * 2)
				tp[d / 2] = 1;

genPQ:
	do {
		do p = CRnd16() | 1; while(tp[p / 2]);
		do q = CRnd16() | 1; while(tp[q / 2]);
	}
	while(p == q || p * q < 0x10000);

	m = p * q; // max are 65519, 65521
	p--;
	q--;

	for(c = 3; c < 0x100; c += 2)
		if(!tp[c / 2] && !(p % c) && !(q % c))
			q /= c;

	p *= q / 2; // LCD -> p
	p++;

	for(e = 3; tp[e / 2] || p % e; e += 2)
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
