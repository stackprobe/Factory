#include "uint128.h"

void ToUI128(uint src[4], uint128_t *dest)
{
	ToUI64(src, &dest->L);
	ToUI64(src + 2, &dest->H);
}
void UI128_0(uint128_t *dest)
{
	UI64_0(&dest->L);
	UI64_0(&dest->H);
}
void UI128_x(uint x, uint128_t *dest)
{
	UI64_x(x, &dest->L);
	UI64_0(&dest->H);
}
void UI128_msb1(uint128_t *dest)
{
	UI64_0(&dest->L);
	UI64_msb1(&dest->H);
}
void UnUI128(uint128_t *src, uint dest[4])
{
	UnUI64(&src->L, dest);
	UnUI64(&src->H, dest + 2);
}

uint UI128_Add(uint128_t *a, uint128_t *b, uint128_t *ans) // ret: overflow ? 1 : 0
{
	static uint64_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI64_Add(&a->L, &b->L, &ans->L);
	ofH = UI64_Add(&a->H, &b->H, &ans->H);

	UI64_x(ofL, &tmp);

	return ofH | UI64_Add(&ans->H, &tmp, &ans->H);
}
uint UI128_Sub(uint128_t *a, uint128_t *b, uint128_t *ans) // ret: underflow ? 0 : 1
{
	static uint64_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI64_Sub(&a->L, &b->L, &ans->L);
	ufH = UI64_Sub(&a->H, &b->H, &ans->H);

	UI64_x(ufL ^ 1, &tmp);

	return ufH & UI64_Sub(&ans->H, &tmp, &ans->H);
}
void UI128_Mul(uint128_t *a, uint128_t *b, uint128_t *ans, uint128_t *ans_hi)
{
	static uint128_t tmp1L;
	static uint128_t tmp1H;
	static uint128_t tmp2L;
	static uint128_t tmp2H;
	static uint128_t tmp;

	UI64_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI64_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI64_0(&tmp1L.L);
	UI64_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI64_0(&tmp1H.H);
	UI64_0(&tmp2L.L);
	UI64_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI64_0(&tmp2H.H);

	UI128_x(UI128_Add(ans, &tmp1L, ans), &tmp);
	UI128_Add(ans_hi, &tmp, ans_hi);
	UI128_x(UI128_Add(ans, &tmp2L, ans), &tmp);
	UI128_Add(ans_hi, &tmp, ans_hi);
	UI128_Add(ans_hi, &tmp1H, ans_hi);
	UI128_Add(ans_hi, &tmp2H, ans_hi);
}
/*
	---- H, L に 0 を持つ場合 ----

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
		ans += A * (fill / b) [再帰]
	else
		ans += (A / b) << Hi [再帰]

	---- case z ----

	A < B ... 0
	A = B ... a < b ? 0 : 1
	A > B ...

		ans += A / (B + 1) + (A / B - A / (B + 1)) / (fill / b) [再帰] <-- todo これでいいのか？
*/
void UI128_Div(uint128_t *a, uint128_t *b, uint128_t *ans)
{
	static uint128_t mask;
	static uint128_t t;
	static uint128_t m;
	static uint128_t h;
	static uint128_t dummy;

	UI128_0(ans);

	if(UI128_IsZero(b)) // ? ゼロ除算
		return;

	UI128_msb1(&mask);

	do
	{
		UI128_or(ans, &mask, &t);
		UI128_Mul(b, &t, &m, &h);

		if(UI128_IsZero(&h) && UI128_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI128_rs(&mask, 0));
}

int UI128_IsZero(uint128_t *a)
{
	return UI64_IsZero(&a->L) && UI64_IsZero(&a->H);
}
int UI128_Comp(uint128_t *a, uint128_t *b)
{
	static uint128_t ans;

	if(!UI128_Sub(a, b, &ans))
		return -1;

	if(UI128_IsZero(&ans))
		return 0;

	return 1;
}
uint UI128_rs(uint128_t *a, uint msb)
{
	return UI64_rs(&a->L, UI64_rs(&a->H, msb));
}
void UI128_or(uint128_t *a, uint128_t *b, uint128_t *ans)
{
	UI64_or(&a->L, &b->L, &ans->L);
	UI64_or(&a->H, &b->H, &ans->H);
}
