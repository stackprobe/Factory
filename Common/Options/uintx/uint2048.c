#include "uint2048.h"

void ToUI2048(uint src[64], uint2048_t *dest)
{
	ToUI1024(src, &dest->L);
	ToUI1024(src + 32, &dest->H);
}
void UI2048_0(uint2048_t *dest)
{
	UI1024_0(&dest->L);
	UI1024_0(&dest->H);
}
void UI2048_x(uint x, uint2048_t *dest)
{
	UI1024_x(x, &dest->L);
	UI1024_0(&dest->H);
}
void UI2048_msb1(uint2048_t *dest)
{
	UI1024_0(&dest->L);
	UI1024_msb1(&dest->H);
}
void UnUI2048(uint2048_t *src, uint dest[64])
{
	UnUI1024(&src->L, dest);
	UnUI1024(&src->H, dest + 32);
}

uint UI2048_Add(uint2048_t *a, uint2048_t *b, uint2048_t *ans) // ret: overflow ? 1 : 0
{
	static uint1024_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI1024_Add(&a->L, &b->L, &ans->L);
	ofH = UI1024_Add(&a->H, &b->H, &ans->H);

	UI1024_x(ofL, &tmp);

	return ofH | UI1024_Add(&ans->H, &tmp, &ans->H);
}
uint UI2048_Sub(uint2048_t *a, uint2048_t *b, uint2048_t *ans) // ret: underflow ? 0 : 1
{
	static uint1024_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI1024_Sub(&a->L, &b->L, &ans->L);
	ufH = UI1024_Sub(&a->H, &b->H, &ans->H);

	UI1024_x(ufL ^ 1, &tmp);

	return ufH & UI1024_Sub(&ans->H, &tmp, &ans->H);
}
void UI2048_Mul(uint2048_t *a, uint2048_t *b, uint2048_t *ans, uint2048_t *ans_hi)
{
	static uint2048_t tmp1L;
	static uint2048_t tmp1H;
	static uint2048_t tmp2L;
	static uint2048_t tmp2H;
	static uint2048_t tmp;

	UI1024_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI1024_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI1024_0(&tmp1L.L);
	UI1024_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI1024_0(&tmp1H.H);
	UI1024_0(&tmp2L.L);
	UI1024_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI1024_0(&tmp2H.H);

	UI2048_x(UI2048_Add(ans, &tmp1L, ans), &tmp);
	UI2048_Add(ans_hi, &tmp, ans_hi);
	UI2048_x(UI2048_Add(ans, &tmp2L, ans), &tmp);
	UI2048_Add(ans_hi, &tmp, ans_hi);
	UI2048_Add(ans_hi, &tmp1H, ans_hi);
	UI2048_Add(ans_hi, &tmp2H, ans_hi);
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
void UI2048_Div(uint2048_t *a, uint2048_t *b, uint2048_t *ans)
{
	static uint2048_t mask;
	static uint2048_t t;
	static uint2048_t m;
	static uint2048_t h;
	static uint2048_t dummy;

	UI2048_0(ans);

	if(UI2048_IsZero(b)) // ? ゼロ除算
		return;

	UI2048_msb1(&mask);

	do
	{
		UI2048_or(ans, &mask, &t);
		UI2048_Mul(b, &t, &m, &h);

		if(UI2048_IsZero(&h) && UI2048_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI2048_rs(&mask, 0));
}

int UI2048_IsZero(uint2048_t *a)
{
	return UI1024_IsZero(&a->L) && UI1024_IsZero(&a->H);
}
int UI2048_Comp(uint2048_t *a, uint2048_t *b)
{
	static uint2048_t ans;

	if(!UI2048_Sub(a, b, &ans))
		return -1;

	if(UI2048_IsZero(&ans))
		return 0;

	return 1;
}
uint UI2048_rs(uint2048_t *a, uint msb)
{
	return UI1024_rs(&a->L, UI1024_rs(&a->H, msb));
}
void UI2048_or(uint2048_t *a, uint2048_t *b, uint2048_t *ans)
{
	UI1024_or(&a->L, &b->L, &ans->L);
	UI1024_or(&a->H, &b->H, &ans->H);
}
