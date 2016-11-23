#include "uint%HBIT%.h"

void ToUI%HBIT%(uint src[%HSZ%], uint%HBIT%_t *dest)
{
	ToUI%LBIT%(src, &dest->L);
	ToUI%LBIT%(src + %LSZ%, &dest->H);
}
void UI%HBIT%_0(uint%HBIT%_t *dest)
{
	UI%LBIT%_0(&dest->L);
	UI%LBIT%_0(&dest->H);
}
void UI%HBIT%_x(uint x, uint%HBIT%_t *dest)
{
	UI%LBIT%_x(x, &dest->L);
	UI%LBIT%_0(&dest->H);
}
void UI%HBIT%_msb1(uint%HBIT%_t *dest)
{
	UI%LBIT%_0(&dest->L);
	UI%LBIT%_msb1(&dest->H);
}
void UnUI%HBIT%(uint%HBIT%_t *src, uint dest[%HSZ%])
{
	UnUI%LBIT%(&src->L, dest);
	UnUI%LBIT%(&src->H, dest + %LSZ%);
}

uint UI%HBIT%_Add(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans) // ret: overflow ? 1 : 0
{
	static uint%LBIT%_t tmp;
	uint ofL;
	uint ofH;

	ofL = UI%LBIT%_Add(&a->L, &b->L, &ans->L);
	ofH = UI%LBIT%_Add(&a->H, &b->H, &ans->H);

	UI%LBIT%_x(ofL, &tmp);

	return ofH | UI%LBIT%_Add(&ans->H, &tmp, &ans->H);
}
uint UI%HBIT%_Sub(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans) // ret: underflow ? 0 : 1
{
	static uint%LBIT%_t tmp;
	uint ufL;
	uint ufH;

	ufL = UI%LBIT%_Sub(&a->L, &b->L, &ans->L);
	ufH = UI%LBIT%_Sub(&a->H, &b->H, &ans->H);

	UI%LBIT%_x(ufL ^ 1, &tmp);

	return ufH & UI%LBIT%_Sub(&ans->H, &tmp, &ans->H);
}
void UI%HBIT%_Mul(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans, uint%HBIT%_t *ans_hi)
{
	static uint%HBIT%_t tmp1L;
	static uint%HBIT%_t tmp1H;
	static uint%HBIT%_t tmp2L;
	static uint%HBIT%_t tmp2H;
	static uint%HBIT%_t tmp;

	UI%LBIT%_Mul(&a->L, &b->L, &ans->L, &ans->H);
	UI%LBIT%_Mul(&a->H, &b->H, &ans_hi->L, &ans_hi->H);

	UI%LBIT%_0(&tmp1L.L);
	UI%LBIT%_Mul(&a->L, &b->H, &tmp1L.H, &tmp1H.L);
	UI%LBIT%_0(&tmp1H.H);
	UI%LBIT%_0(&tmp2L.L);
	UI%LBIT%_Mul(&a->H, &b->L, &tmp2L.H, &tmp2H.L);
	UI%LBIT%_0(&tmp2H.H);

	UI%HBIT%_x(UI%HBIT%_Add(ans, &tmp1L, ans), &tmp);
	UI%HBIT%_Add(ans_hi, &tmp, ans_hi);
	UI%HBIT%_x(UI%HBIT%_Add(ans, &tmp2L, ans), &tmp);
	UI%HBIT%_Add(ans_hi, &tmp, ans_hi);
	UI%HBIT%_Add(ans_hi, &tmp1H, ans_hi);
	UI%HBIT%_Add(ans_hi, &tmp2H, ans_hi);
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
void UI%HBIT%_Div(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans)
{
	static uint%HBIT%_t mask;
	static uint%HBIT%_t t;
	static uint%HBIT%_t m;
	static uint%HBIT%_t h;
	static uint%HBIT%_t dummy;

	UI%HBIT%_0(ans);

	if(UI%HBIT%_IsZero(b)) // ? ゼロ除算
		return;

	UI%HBIT%_msb1(&mask);

	do
	{
		UI%HBIT%_or(ans, &mask, &t);
		UI%HBIT%_Mul(b, &t, &m, &h);

		if(UI%HBIT%_IsZero(&h) && UI%HBIT%_Sub(a, &m, &dummy))
			*ans = t;
	}
	while(!UI%HBIT%_rs(&mask, 0));
}

int UI%HBIT%_IsZero(uint%HBIT%_t *a)
{
	return UI%LBIT%_IsZero(&a->L) && UI%LBIT%_IsZero(&a->H);
}
int UI%HBIT%_Comp(uint%HBIT%_t *a, uint%HBIT%_t *b)
{
	static uint%HBIT%_t ans;

	if(!UI%HBIT%_Sub(a, b, &ans))
		return -1;

	if(UI%HBIT%_IsZero(&ans))
		return 0;

	return 1;
}
uint UI%HBIT%_rs(uint%HBIT%_t *a, uint msb)
{
	return UI%LBIT%_rs(&a->L, UI%LBIT%_rs(&a->H, msb));
}
void UI%HBIT%_or(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans)
{
	UI%LBIT%_or(&a->L, &b->L, &ans->L);
	UI%LBIT%_or(&a->H, &b->H, &ans->H);
}
