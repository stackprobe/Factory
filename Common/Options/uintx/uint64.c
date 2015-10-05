#include "uint64.h"

void ToUI64(uint src[2], uint64_t *dest)
{
	dest->L = src[0];
	dest->H = src[1];
}
void UI64_0(uint64_t *dest)
{
	dest->L = 0;
	dest->H = 0;
}
void UI64_x(uint x, uint64_t *dest)
{
	dest->L = x;
	dest->H = 0;
}
void UI64_msb1(uint64_t *dest)
{
	dest->L = 0;
	dest->H = 0x80000000;
}
void UnUI64(uint64_t *src, uint dest[2])
{
	dest[0] = src->L;
	dest[1] = src->H;
}

uint UI64_Add(uint64_t *a, uint64_t *b, uint64_t *ans) // ret: overflow ? 1 : 0
{
	uint64 tmp;

	tmp = (uint64)a->L + b->L;

	ans->L = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += (uint64)a->H + b->H;

	ans->H = tmp & 0xffffffff;

	return tmp >> 32;
}
uint UI64_Sub(uint64_t *a, uint64_t *b, uint64_t *ans) // ret: underflow ? 0 : 1
{
	uint64 tmp;

	tmp = ((uint64)a->L + 0x100000000) - b->L;

	ans->L = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += ((uint64)a->H + 0xffffffff) - b->H;

	ans->H = tmp & 0xffffffff;

	return tmp >> 32;
}
void UI64_Mul(uint64_t *a, uint64_t *b, uint64_t *ans, uint64_t *ans_hi)
{
	uint64 tmp;

	tmp = (uint64)a->L * b->L;

	ans->L = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += (uint64)a->L * b->H;
	tmp += (uint64)a->H * b->L;

	ans->H = tmp & 0xffffffff;

	tmp >>= 32;
	tmp += (uint64)a->H * b->H;

	ans_hi->L = tmp & 0xffffffff;
	ans_hi->H = tmp >> 32;
}
void UI64_Div(uint64_t *a, uint64_t *b, uint64_t *ans)
{
	uint64 tmp1 = ((uint64)a->H << 32) | a->L;
	uint64 tmp2 = ((uint64)b->H << 32) | b->L;
	uint64 tmp3;

	if(tmp2 == 0) // ? ƒ[ƒœŽZ
		tmp3 = 0;
	else
		tmp3 = tmp1 / tmp2;

	ans->L = tmp3 & 0xffffffff;
	ans->H = tmp3 >> 32;
}

int UI64_IsZero(uint64_t *a)
{
	return a->L == 0 && a->H == 0;
}
int UI64_Comp(uint64_t *a, uint64_t *b)
{
	uint64_t ans;

	if(!UI64_Sub(a, b, &ans))
		return -1;

	if(UI64_IsZero(&ans))
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
void UI64_or(uint64_t *a, uint64_t *b, uint64_t *ans)
{
	ans->L = a->L | b->L;
	ans->H = a->H | b->H;
}
