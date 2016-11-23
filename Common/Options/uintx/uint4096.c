#include "uint4096.h"

void ToUI4096(uint src[128], uint4096_t *dest)
{
	ToUI2048(src, &dest->L);
	ToUI2048(src + 64, &dest->H);
}
void UI4096_0(uint4096_t *dest)
{
	UI2048_0(&dest->L);
	UI2048_0(&dest->H);
}
void UI4096_x(uint x, uint4096_t *dest)
{
	UI2048_x(x, &dest->L);
	UI2048_0(&dest->H);
}
void UI4096_msb1(uint4096_t *dest)
{
	UI2048_0(&dest->L);
	UI2048_msb1(&dest->H);
}
void UnUI4096(uint4096_t *src, uint dest[128])
{
	UnUI2048(&src->L, dest);
	UnUI2048(&src->H, dest + 64);
}

uint UI4096_Add(uint4096_t *a, uint4096_t *b, uint4096_t *ans) // ret: overflow ? 1 : 0
{
	static uint2048_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI2048_Add(&a->L, &b->L, &ans->L);
	ofH = UI2048_Add(&a->H, &b->H, &ans->H);

	UI2048_x(ofL, &tmp);

	return ofH | UI2048_Add(&ans->H, &tmp, &ans->H);
}
uint UI4096_Sub(uint4096_t *a, uint4096_t *b, uint4096_t *ans) // ret: underflow ? 0 : 1
{
	static uint2048_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI2048_Sub(&a->L, &b->L, &ans->L);
	ufH = UI2048_Sub(&a->H, &b->H, &ans->H);

	UI2048_x(ufL ^ 1, &tmp);

	return ufH & UI2048_Sub(&ans->H, &tmp, &ans->H);
}
void UI4096_Mul(uint4096_t *a, uint4096_t *b, uint4096_t *ans, uint4096_t *ans_hi)
{
	static uint4096_t tmp1L;
	static uint4096_t tmp1H;
	static uint4096_t tmp2L;
	static uint4096_t tmp2H;
	static uint4096_t tmp;

	UI2048_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI2048_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI2048_0(&tmp1L.L);
	UI2048_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI2048_0(&tmp1H.H);
	UI2048_0(&tmp2L.L);
	UI2048_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI2048_0(&tmp2H.H);

	UI4096_x(UI4096_Add(ans, &tmp1L, ans), &tmp);
	UI4096_Add(ans_hi, &tmp, ans_hi);
	UI4096_x(UI4096_Add(ans, &tmp2L, ans), &tmp);
	UI4096_Add(ans_hi, &tmp, ans_hi);
	UI4096_Add(ans_hi, &tmp1H, ans_hi);
	UI4096_Add(ans_hi, &tmp2H, ans_hi);
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
void UI4096_Div(uint4096_t *a, uint4096_t *b, uint4096_t *ans)
{
	static uint4096_t mask;
	static uint4096_t t;
	static uint4096_t m;
	static uint4096_t h;
	static uint4096_t dummy;

	UI4096_0(ans);

	if(UI4096_IsZero(b)) // ? ゼロ除算
		return;

	UI4096_msb1(&mask);

	do
	{
		UI4096_or(ans, &mask, &t);
		UI4096_Mul(b, &t, &m, &h);

		if(UI4096_IsZero(&h) && UI4096_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI4096_rs(&mask, 0));
}

int UI4096_IsZero(uint4096_t *a)
{
	return UI2048_IsZero(&a->L) && UI2048_IsZero(&a->H);
}
int UI4096_Comp(uint4096_t *a, uint4096_t *b)
{
	static uint4096_t ans;

	if(!UI4096_Sub(a, b, &ans))
		return -1;

	if(UI4096_IsZero(&ans))
		return 0;

	return 1;
}
uint UI4096_rs(uint4096_t *a, uint msb)
{
	return UI2048_rs(&a->L, UI2048_rs(&a->H, msb));
}
void UI4096_or(uint4096_t *a, uint4096_t *b, uint4096_t *ans)
{
	UI2048_or(&a->L, &b->L, &ans->L);
	UI2048_or(&a->H, &b->H, &ans->H);
}
