#include "uint256.h"

void ToUI256(uint src[8], uint256_t *dest)
{
	ToUI128(src, &dest->L);
	ToUI128(src + 4, &dest->H);
}
void UI256_0(uint256_t *dest)
{
	UI128_0(&dest->L);
	UI128_0(&dest->H);
}
void UI256_x(uint x, uint256_t *dest)
{
	UI128_x(x, &dest->L);
	UI128_0(&dest->H);
}
void UI256_msb1(uint256_t *dest)
{
	UI128_0(&dest->L);
	UI128_msb1(&dest->H);
}
void UnUI256(uint256_t *src, uint dest[8])
{
	UnUI128(&src->L, dest);
	UnUI128(&src->H, dest + 4);
}

uint UI256_Add(uint256_t *a, uint256_t *b, uint256_t *ans) // ret: overflow ? 1 : 0
{
	static uint128_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI128_Add(&a->L, &b->L, &ans->L);
	ofH = UI128_Add(&a->H, &b->H, &ans->H);

	UI128_x(ofL, &tmp);

	return ofH | UI128_Add(&ans->H, &tmp, &ans->H);
}
uint UI256_Sub(uint256_t *a, uint256_t *b, uint256_t *ans) // ret: underflow ? 0 : 1
{
	static uint128_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI128_Sub(&a->L, &b->L, &ans->L);
	ufH = UI128_Sub(&a->H, &b->H, &ans->H);

	UI128_x(ufL ^ 1, &tmp);

	return ufH & UI128_Sub(&ans->H, &tmp, &ans->H);
}
void UI256_Mul(uint256_t *a, uint256_t *b, uint256_t *ans, uint256_t *ans_hi)
{
	static uint256_t tmp1L;
	static uint256_t tmp1H;
	static uint256_t tmp2L;
	static uint256_t tmp2H;
	static uint256_t tmp;

	UI128_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI128_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI128_0(&tmp1L.L);
	UI128_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI128_0(&tmp1H.H);
	UI128_0(&tmp2L.L);
	UI128_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI128_0(&tmp2H.H);

	UI256_x(UI256_Add(ans, &tmp1L, ans), &tmp);
	UI256_Add(ans_hi, &tmp, ans_hi);
	UI256_x(UI256_Add(ans, &tmp2L, ans), &tmp);
	UI256_Add(ans_hi, &tmp, ans_hi);
	UI256_Add(ans_hi, &tmp1H, ans_hi);
	UI256_Add(ans_hi, &tmp2H, ans_hi);
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
void UI256_Div(uint256_t *a, uint256_t *b, uint256_t *ans)
{
	static uint256_t mask;
	static uint256_t t;
	static uint256_t m;
	static uint256_t h;
	static uint256_t dummy;

	UI256_0(ans);

	if(UI256_IsZero(b)) // ? ゼロ除算
		return;

	UI256_msb1(&mask);

	do
	{
		UI256_or(ans, &mask, &t);
		UI256_Mul(b, &t, &m, &h);

		if(UI256_IsZero(&h) && UI256_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI256_rs(&mask, 0));
}

int UI256_IsZero(uint256_t *a)
{
	return UI128_IsZero(&a->L) && UI128_IsZero(&a->H);
}
int UI256_Comp(uint256_t *a, uint256_t *b)
{
	static uint256_t ans;

	if(!UI256_Sub(a, b, &ans))
		return -1;

	if(UI256_IsZero(&ans))
		return 0;

	return 1;
}
uint UI256_rs(uint256_t *a, uint msb)
{
	return UI128_rs(&a->L, UI128_rs(&a->H, msb));
}
void UI256_or(uint256_t *a, uint256_t *b, uint256_t *ans)
{
	UI128_or(&a->L, &b->L, &ans->L);
	UI128_or(&a->H, &b->H, &ans->H);
}
