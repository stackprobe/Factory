#include "uint64.h"

uint UI64_Overflow;
uint UI64_Ununderflow;
uint64_t UI64_Hi;

uint64_t ToUI64(uint src[2])
{
	uint64_t ans;

	ans.L = src[0];
	ans.H = src[1];

	return ans;
}
uint64_t UI64_0(void)
{
	uint64_t ans;

	ans.L = 0;
	ans.H = 0;

	return ans;
}
uint64_t UI64_x(uint x)
{
	uint64_t ans;

	ans.L = x;
	ans.H = 0;

	return ans;
}
uint64_t UI64_msb1(void)
{
	uint64_t ans;

	ans.L = 0;
	ans.H = 0x80000000;

	return ans;
}
void UnUI64(uint64_t src, uint dest[2])
{
	dest[0] = src.L;
	dest[1] = src.H;
}

uint64_t UI64_Add(uint64_t a, uint64_t b)
{
	uint64_t ans;
	uint64 tmp;

	tmp = (uint64)a.L + b.L;

	ans.L = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += (uint64)a.H + b.H;

	ans.H = tmp & 0xffffffff;

	UI64_Overflow = tmp >> 32;

	return ans;
}
uint64_t UI64_Sub(uint64_t a, uint64_t b)
{
	uint64_t ans;
	uint64 tmp;

	tmp = ((uint64)a.L + 0x100000000) - b.L;

	ans.L = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += ((uint64)a.H + 0xffffffff) - b.H;

	ans.H = tmp & 0xffffffff;

	UI64_Ununderflow = tmp >> 32;

	return ans;
}
uint64_t UI64_Mul(uint64_t a, uint64_t b)
{
	uint64_t ans;
	uint64 tmp;

	tmp = (uint64)a.L * b.L;

	ans.L = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += (uint64)a.L * b.H;
	tmp += (uint64)a.H * b.L;

	ans.H = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += (uint64)a.H * b.H;

	UI64_Hi.L = tmp & 0xffffffff;
	UI64_Hi.H = tmp >> 32;

	return ans;
}
uint64_t UI64_Div(uint64_t a, uint64_t b)
{
	uint64_t ans;
	uint64 tmp1 = ((uint64)a.H << 32) | a.L;
	uint64 tmp2 = ((uint64)b.H << 32) | b.L;
	uint64 tmp3;

	if(tmp2 == 0) // ? ƒ[ƒœZ
		tmp3 = 0;
	else
		tmp3 = tmp1 / tmp2;

	ans.L = tmp3 & 0xffffffff;
	ans.H = tmp3 >> 32;

	return ans;
}

int UI64_IsZero(uint64_t a)
{
	return a.L == 0 && a.H == 0;
}
int UI64_Comp(uint64_t a, uint64_t b)
{
	uint64_t ans = UI64_Sub(a, b);

	if(!UI64_Ununderflow)
		return -1;

	if(UI64_IsZero(ans))
		return 0;

	return 1;
}
static uint Rs(uint *a, uint msb)
{
	uint ret = *a & 1;

	*a >>= 1;
	*a |= msb << 31;

	return ret;
}
uint UI64_rs(uint64_t *a, uint msb)
{
	return Rs(&a->L, Rs(&a->H, msb));
}
uint64_t UI64_or(uint64_t a, uint64_t b)
{
	uint64_t ans;

	ans.L = a.L | b.L;
	ans.H = a.H | b.H;

	return ans;
}
