#include "uint1024.h"

void ToUI1024(uint src[32], uint1024_t *dest)
{
	ToUI512(src, &dest->L);
	ToUI512(src + 16, &dest->H);
}
void UI1024_0(uint1024_t *dest)
{
	UI512_0(&dest->L);
	UI512_0(&dest->H);
}
void UI1024_x(uint x, uint1024_t *dest)
{
	UI512_x(x, &dest->L);
	UI512_0(&dest->H);
}
void UI1024_msb1(uint1024_t *dest)
{
	UI512_0(&dest->L);
	UI512_msb1(&dest->H);
}
void UnUI1024(uint1024_t *src, uint dest[32])
{
	UnUI512(&src->L, dest);
	UnUI512(&src->H, dest + 16);
}

uint UI1024_Add(uint1024_t *a, uint1024_t *b, uint1024_t *ans) // ret: overflow ? 1 : 0
{
	static uint512_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI512_Add(&a->L, &b->L, &ans->L);
	ofH = UI512_Add(&a->H, &b->H, &ans->H);

	UI512_x(ofL, &tmp);

	return ofH | UI512_Add(&ans->H, &tmp, &ans->H);
}
uint UI1024_Sub(uint1024_t *a, uint1024_t *b, uint1024_t *ans) // ret: underflow ? 0 : 1
{
	static uint512_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI512_Sub(&a->L, &b->L, &ans->L);
	ufH = UI512_Sub(&a->H, &b->H, &ans->H);

	UI512_x(ufL ^ 1, &tmp);

	return ufH & UI512_Sub(&ans->H, &tmp, &ans->H);
}
void UI1024_Mul(uint1024_t *a, uint1024_t *b, uint1024_t *ans, uint1024_t *ans_hi)
{
	static uint1024_t tmp1L;
	static uint1024_t tmp1H;
	static uint1024_t tmp2L;
	static uint1024_t tmp2H;
	static uint1024_t tmp;

	UI512_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI512_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI512_0(&tmp1L.L);
	UI512_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI512_0(&tmp1H.H);
	UI512_0(&tmp2L.L);
	UI512_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI512_0(&tmp2H.H);

	UI1024_x(UI1024_Add(ans, &tmp1L, ans), &tmp);
	UI1024_Add(ans_hi, &tmp, ans_hi);
	UI1024_x(UI1024_Add(ans, &tmp2L, ans), &tmp);
	UI1024_Add(ans_hi, &tmp, ans_hi);
	UI1024_Add(ans_hi, &tmp1H, ans_hi);
	UI1024_Add(ans_hi, &tmp2H, ans_hi);
}
/*
	---- H, L ‚É 0 ‚ğ‚Âê‡ ----

	0 0 / 0 0 = 0 div
	A 0 / 0 0 = 0 div
	0 a / 0 0 = 0 div
	0 0 / B 0 = 0
	0 0 / 0 b = 0
	A a / 0 0 = 0 div
	A 0 / B 0 = A / B
	A 0 / 0 b = x
	0 a / B 0 = 0
	0 a / 0 b = a / b
	0 0 / B b = 0
	A a / B 0 = A / B
	A a / 0 b = x
	A 0 / B b = z
	0 a / B b = 0
	A a / B b = z

	---- case x ----

	A < b
		ans += A * (fill / b) [Ä‹A]
	else
		ans += (A / b) << Hi [Ä‹A]

	---- case z ----

	A < B ... 0
	A = B ... a < b ? 0 : 1
	A > B ...

		ans += A / (B + 1) + (A / B - A / (B + 1)) / (fill / b) [Ä‹A] <-- TODO ‚±‚ê‚Å‚¢‚¢‚Ì‚©H
*/
void UI1024_Div(uint1024_t *a, uint1024_t *b, uint1024_t *ans)
{
	// TODO ’x‚¢I

	static uint1024_t mask;
	static uint1024_t t;
	static uint1024_t m;
	static uint1024_t h;
	static uint1024_t dummy;

	UI1024_0(ans);

	if(UI1024_IsZero(b)) // ? ƒ[ƒœZ
		return;

	UI1024_msb1(&mask);

	do
	{
		UI1024_or(ans, &mask, &t);
		UI1024_Mul(b, &t, &m, &h);

		if(UI1024_IsZero(&h) && UI1024_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI1024_rs(&mask, 0));
}

int UI1024_IsZero(uint1024_t *a)
{
	return UI512_IsZero(&a->L) && UI512_IsZero(&a->H);
}
int UI1024_Comp(uint1024_t *a, uint1024_t *b)
{
	static uint1024_t ans;

	if(!UI1024_Sub(a, b, &ans))
		return -1;

	if(UI1024_IsZero(&ans))
		return 0;

	return 1;
}
uint UI1024_rs(uint1024_t *a, uint msb)
{
	return UI512_rs(&a->L, UI512_rs(&a->H, msb));
}
void UI1024_or(uint1024_t *a, uint1024_t *b, uint1024_t *ans)
{
	UI512_or(&a->L, &b->L, &ans->L);
	UI512_or(&a->H, &b->H, &ans->H);
}
