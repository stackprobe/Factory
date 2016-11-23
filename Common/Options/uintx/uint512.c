#include "uint512.h"

void ToUI512(uint src[16], uint512_t *dest)
{
	ToUI256(src, &dest->L);
	ToUI256(src + 8, &dest->H);
}
void UI512_0(uint512_t *dest)
{
	UI256_0(&dest->L);
	UI256_0(&dest->H);
}
void UI512_x(uint x, uint512_t *dest)
{
	UI256_x(x, &dest->L);
	UI256_0(&dest->H);
}
void UI512_msb1(uint512_t *dest)
{
	UI256_0(&dest->L);
	UI256_msb1(&dest->H);
}
void UnUI512(uint512_t *src, uint dest[16])
{
	UnUI256(&src->L, dest);
	UnUI256(&src->H, dest + 8);
}

uint UI512_Add(uint512_t *a, uint512_t *b, uint512_t *ans) // ret: overflow ? 1 : 0
{
	static uint256_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI256_Add(&a->L, &b->L, &ans->L);
	ofH = UI256_Add(&a->H, &b->H, &ans->H);

	UI256_x(ofL, &tmp);

	return ofH | UI256_Add(&ans->H, &tmp, &ans->H);
}
uint UI512_Sub(uint512_t *a, uint512_t *b, uint512_t *ans) // ret: underflow ? 0 : 1
{
	static uint256_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI256_Sub(&a->L, &b->L, &ans->L);
	ufH = UI256_Sub(&a->H, &b->H, &ans->H);

	UI256_x(ufL ^ 1, &tmp);

	return ufH & UI256_Sub(&ans->H, &tmp, &ans->H);
}
void UI512_Mul(uint512_t *a, uint512_t *b, uint512_t *ans, uint512_t *ans_hi)
{
	static uint512_t tmp1L;
	static uint512_t tmp1H;
	static uint512_t tmp2L;
	static uint512_t tmp2H;
	static uint512_t tmp;

	UI256_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI256_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI256_0(&tmp1L.L);
	UI256_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI256_0(&tmp1H.H);
	UI256_0(&tmp2L.L);
	UI256_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI256_0(&tmp2H.H);

	UI512_x(UI512_Add(ans, &tmp1L, ans), &tmp);
	UI512_Add(ans_hi, &tmp, ans_hi);
	UI512_x(UI512_Add(ans, &tmp2L, ans), &tmp);
	UI512_Add(ans_hi, &tmp, ans_hi);
	UI512_Add(ans_hi, &tmp1H, ans_hi);
	UI512_Add(ans_hi, &tmp2H, ans_hi);
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
void UI512_Div(uint512_t *a, uint512_t *b, uint512_t *ans)
{
	static uint512_t mask;
	static uint512_t t;
	static uint512_t m;
	static uint512_t h;
	static uint512_t dummy;

	UI512_0(ans);

	if(UI512_IsZero(b)) // ? ゼロ除算
		return;

	UI512_msb1(&mask);

	do
	{
		UI512_or(ans, &mask, &t);
		UI512_Mul(b, &t, &m, &h);

		if(UI512_IsZero(&h) && UI512_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI512_rs(&mask, 0));
}

int UI512_IsZero(uint512_t *a)
{
	return UI256_IsZero(&a->L) && UI256_IsZero(&a->H);
}
int UI512_Comp(uint512_t *a, uint512_t *b)
{
	static uint512_t ans;

	if(!UI512_Sub(a, b, &ans))
		return -1;

	if(UI512_IsZero(&ans))
		return 0;

	return 1;
}
uint UI512_rs(uint512_t *a, uint msb)
{
	return UI256_rs(&a->L, UI256_rs(&a->H, msb));
}
void UI512_or(uint512_t *a, uint512_t *b, uint512_t *ans)
{
	UI256_or(&a->L, &b->L, &ans->L);
	UI256_or(&a->H, &b->H, &ans->H);
}
