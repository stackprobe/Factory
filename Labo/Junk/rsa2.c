#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static void RareErrorCase(int state)
{
	static uint w;

	if(state)
		w += 10;
	else if(w)
		w--;

	errorCase(100 < w);
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
	static char ps[0x10000];
	uint c, p, q, m, e, d; // 32bit

memset(ps, 0x00, 0x10000); // not needed
	ps[0] = 1;
	ps[1] = 1;

	for(c = 2; c < 0x100; c++)
		if(!ps[c])
			for(d = c * 2; d < 0x10000; d += c)
				ps[d] = 1;

genPQ:
	do {
		do p = CRnd16(); while(ps[p]);
		do q = CRnd16(); while(ps[q]);
	}
	while(p == q || p * q < 0x10000);

	m = p * q; // max are 65519, 65521
	p--;
	q--;

	for(c = 2; c < 0x100; c++)
		if(!ps[c] && !(p % c) && !(q % c))
			q /= c;

	p *= q; // LCD -> p
	p++;

	for(e = 2; ps[e] || p % e; e++)
		if(p / e < e)
			goto genPQ;

	d = p / e;

	printf("m=%u e=%u d=%u\n", m, e, d);

	p = CRnd16();
	c = ModPow(p, e, m);
	q = ModPow(c, d, m);

	printf("p=%u c=%u q=%u\n", p, c, q); // always p == q

	// ----

	RareErrorCase(p == c);
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
