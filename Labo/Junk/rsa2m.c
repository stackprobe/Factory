#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

// ---- ‚±‚±‚©‚ç

uint ModPow(uint v, uint e, uint m) {
	uint64 r;

	if(!e)
		return 1;

	r = ModPow(v, e / 2, m);
	r *= r;
	r %= m;

	if(e & 1) {
		r *= v;
		r %= m;
	}
	return r;
}

#define CRBUFFSIZE 1024

uint16 CRnd16(void) {
	static uint16 buff[CRBUFFSIZE];
	static uint i = CRBUFFSIZE;

	if(CRBUFFSIZE <= i) {
		HCRYPTPROV hp;

		if(!CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
			exit(1); // fatal

		if(!CryptGenRandom(hp, sizeof(buff), (uchar *)buff))
			exit(1); // fatal

		CryptReleaseContext(hp, 0);
		i = 0;
	}
	return buff[i++];
}

// ---- ‹æØ‚è

TestMain() {
	uchar ops[0x1000];
	uint c, p, q, m, e, d; // 32bit

	memset(ops, 0x00, 0x1000);
	ops[0] = 1;

#define op(p) (ops[(p) / 16] & 1 << (p) / 2 % 8)

	for(c = 3; c < 0x100; c += 2)
		if(!op(c))
			for(d = c * c / 2; d < 0x10000 / 2; d += c)
				ops[d / 8] |= 1 << d % 8;

genPQ:
	do {
		do p = CRnd16() | 1; while(op(p));
		do q = CRnd16() | 1; while(op(q) || p == q);
	}
	while(p * q < 0x10000);

	m = p * q;
	p--;
	q--;

	for(e = p, d = q; e != d; e < d ? (d -= e) : (e -= d));

	p *= q / e;
	p++;

	for(e = 3; op(e) || p % e; e += 2)
		if(p / e < e)
			goto genPQ;

#undef op

	d = p / e;

	printf("m=%u e=%u d=%u\n", m, e, d);

	p = CRnd16();
	c = ModPow(p, e, m);
	q = ModPow(c, d, m);

	printf("p=%u c=%u q=%u\n", p, c, q); // always p == q
}

// ---- ‚±‚±‚Ü‚Å

/*
static uint16 CRnd16(void)
{
	return (uint16)getCryptoRand16();
}
*/
int main(int argc, char **argv)
{
	for(; ; )
	{
		TestMain();
//		cout("%04x\n", CRnd16()); // test
	}
}
