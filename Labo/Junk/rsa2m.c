#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

// ---- ここから

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
TestMain(uint16 (*crand16)(void))
{
	uchar ops[0x1000];
	uint c, p, q, m, e, d; // 32bit

#define op(p) (ops[(p) / 16] & 1 << (p) / 2 % 8)

	memset(ops, 0x00, 0x1000);
	ops[0] = 1;

	for(c = 3; c < 0x100; c += 2)
		if(!op(c))
			for(d = c * c / 2; d < 0x10000 / 2; d += c)
				ops[d / 8] |= 1 << d % 8;

genPQ:
	do {
		do p = crand16() | 1; while(op(p)); // odd prime -> p
		do q = crand16() | 1; while(op(q)); // odd prime -> q
	}
	while(p == q || p * q < 0x10000);

	m = p * q;
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

	do p = crand16(); while(p <= 1);
	c = ModPow(p, e, m);
	q = ModPow(c, d, m);

	printf("p=%u c=%u q=%u\n", p, c, q); // always p == q

#undef op
}

// ---- ここまで

static uint16 CRnd16(void)
{
	return (uint16)getCryptoRand16();
}
int main(int argc, char **argv)
{
	for(; ; )
	{
		TestMain(CRnd16);
	}
}
