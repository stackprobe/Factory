#include "uint512.h"

uint UI512_Overflow;
uint UI512_Ununderflow;
uint512_t UI512_Hi;

uint512_t ToUI512(uint src[16])
{
	uint512_t ans;

	ans.L = ToUI256(src);
	ans.H = ToUI256(src + 8);

	return ans;
}
uint512_t UI512_0(void)
{
	uint512_t ans;

	ans.L = UI256_0();
	ans.H = UI256_0();

	return ans;
}
uint512_t UI512_x(uint x)
{
	uint512_t ans;

	ans.L = UI256_x(x);
	ans.H = UI256_0();

	return ans;
}
uint512_t UI512_msb1(void)
{
	uint512_t ans;

	ans.L = UI256_0();
	ans.H = UI256_msb1();

	return ans;
}
void UnUI512(uint512_t src, uint dest[16])
{
	UnUI256(src.L, dest);
	UnUI256(src.H, dest + 8);
}

uint512_t UI512_Add(uint512_t a, uint512_t b)
{
	uint512_t ans;
	uint ofL;
	uint ofH;

	ans.L = UI256_Add(a.L, b.L);
	ofL = UI256_Overflow;
	ans.H = UI256_Add(a.H, b.H);
	ofH = UI256_Overflow;
	ans.H = UI256_Add(ans.H, UI256_x(ofL));

	UI512_Overflow = ofH | UI256_Overflow;

	return ans;
}
uint512_t UI512_Sub(uint512_t a, uint512_t b)
{
	uint512_t ans;
	uint ufL;
	uint ufH;

	ans.L = UI256_Sub(a.L, b.L);
	ufL = UI256_Ununderflow;
	ans.H = UI256_Sub(a.H, b.H);
	ufH = UI256_Ununderflow;
	ans.H = UI256_Sub(ans.H, UI256_x(ufL ^ 1));

	UI512_Ununderflow = ufH & UI256_Ununderflow;

	return ans;
}
uint512_t UI512_Mul(uint512_t a, uint512_t b)
{
	uint512_t ans;
	uint512_t tmp1L;
	uint512_t tmp1H;
	uint512_t tmp2L;
	uint512_t tmp2H;

	ans.L = UI256_Mul(a.L, b.L);
	ans.H = UI256_Hi;

	UI512_Hi.L = UI256_Mul(a.H, b.H);
	UI512_Hi.H = UI256_Hi;

	tmp1L.L = UI256_0();
	tmp1L.H = UI256_Mul(a.L, b.H);
	tmp1H.L = UI256_Hi;
	tmp1H.H = UI256_0();
	tmp2L.L = UI256_0();
	tmp2L.H = UI256_Mul(a.H, b.L);
	tmp2H.L = UI256_Hi;
	tmp2H.H = UI256_0();

	ans = UI512_Add(ans, tmp1L);
	UI512_Hi = UI512_Add(UI512_Hi, UI512_x(UI256_Overflow));
	ans = UI512_Add(ans, tmp2L);
	UI512_Hi = UI512_Add(UI512_Hi, UI512_x(UI256_Overflow));
	UI512_Hi = UI512_Add(UI512_Hi, tmp1H);
	UI512_Hi = UI512_Add(UI512_Hi, tmp2H);

	return ans;
}
uint512_t UI512_Div(uint512_t a, uint512_t b)
{
	uint512_t ans = UI512_0();
	uint512_t mask = UI512_msb1();
	uint512_t t;
	uint512_t m;

	if(!UI512_IsZero(b)) // ? ! ƒ[ƒœZ
	{
		do
		{
			t = UI512_or(ans, mask);
			m = UI512_Mul(b, t);

			if(UI512_IsZero(UI512_Hi))
			{
				UI512_Comp(a, m);

				if(UI512_Ununderflow)
					ans = t;
			}
		}
		while(!UI512_rs(&mask, 0));
	}
	return ans;
}

int UI512_IsZero(uint512_t a)
{
	return UI256_IsZero(a.L) && UI256_IsZero(a.H);
}
int UI512_Comp(uint512_t a, uint512_t b)
{
	uint512_t ans = UI512_Sub(a, b);

	if(!UI512_Ununderflow)
		return -1;

	if(UI512_IsZero(ans))
		return 0;

	return 1;
}
uint UI512_rs(uint512_t *a, uint msb)
{
	return UI256_rs(&a->L, UI256_rs(&a->H, msb));
}
uint512_t UI512_or(uint512_t a, uint512_t b)
{
	uint512_t ans;

	ans.L = UI256_or(a.L, b.L);
	ans.H = UI256_or(a.H, b.H);

	return ans;
}
