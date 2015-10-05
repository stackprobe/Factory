#include "uint128.h"

uint UI128_Overflow;
uint UI128_Ununderflow;
uint128_t UI128_Hi;

uint128_t ToUI128(uint src[4])
{
	uint128_t ans;

	ans.L = ToUI64(src);
	ans.H = ToUI64(src + 2);

	return ans;
}
uint128_t UI128_0(void)
{
	uint128_t ans;

	ans.L = UI64_0();
	ans.H = UI64_0();

	return ans;
}
uint128_t UI128_x(uint x)
{
	uint128_t ans;

	ans.L = UI64_x(x);
	ans.H = UI64_0();

	return ans;
}
uint128_t UI128_msb1(void)
{
	uint128_t ans;

	ans.L = UI64_0();
	ans.H = UI64_msb1();

	return ans;
}
void UnUI128(uint128_t src, uint dest[4])
{
	UnUI64(src.L, dest);
	UnUI64(src.H, dest + 2);
}

uint128_t UI128_Add(uint128_t a, uint128_t b)
{
	uint128_t ans;
	uint ofL;
	uint ofH;

	ans.L = UI64_Add(a.L, b.L);
	ofL = UI64_Overflow;
	ans.H = UI64_Add(a.H, b.H);
	ofH = UI64_Overflow;
	ans.H = UI64_Add(ans.H, UI64_x(ofL));

	UI128_Overflow = ofH | UI64_Overflow;

	return ans;
}
uint128_t UI128_Sub(uint128_t a, uint128_t b)
{
	uint128_t ans;
	uint ufL;
	uint ufH;

	ans.L = UI64_Sub(a.L, b.L);
	ufL = UI64_Ununderflow;
	ans.H = UI64_Sub(a.H, b.H);
	ufH = UI64_Ununderflow;
	ans.H = UI64_Sub(ans.H, UI64_x(ufL ^ 1));

	UI128_Ununderflow = ufH & UI64_Ununderflow;

	return ans;
}
uint128_t UI128_Mul(uint128_t a, uint128_t b)
{
	uint128_t ans;
	uint128_t tmp1L;
	uint128_t tmp1H;
	uint128_t tmp2L;
	uint128_t tmp2H;

	ans.L = UI64_Mul(a.L, b.L);
	ans.H = UI64_Hi;

	UI128_Hi.L = UI64_Mul(a.H, b.H);
	UI128_Hi.H = UI64_Hi;

	tmp1L.L = UI64_0();
	tmp1L.H = UI64_Mul(a.L, b.H);
	tmp1H.L = UI64_Hi;
	tmp1H.H = UI64_0();
	tmp2L.L = UI64_0();
	tmp2L.H = UI64_Mul(a.H, b.L);
	tmp2H.L = UI64_Hi;
	tmp2H.H = UI64_0();

	ans = UI128_Add(ans, tmp1L);
	UI128_Hi = UI128_Add(UI128_Hi, UI128_x(UI64_Overflow));
	ans = UI128_Add(ans, tmp2L);
	UI128_Hi = UI128_Add(UI128_Hi, UI128_x(UI64_Overflow));
	UI128_Hi = UI128_Add(UI128_Hi, tmp1H);
	UI128_Hi = UI128_Add(UI128_Hi, tmp2H);

	return ans;
}
uint128_t UI128_Div(uint128_t a, uint128_t b)
{
	uint128_t ans = UI128_0();
	uint128_t mask = UI128_msb1();
	uint128_t t;
	uint128_t m;

	if(!UI128_IsZero(b)) // ? ! ƒ[ƒœZ
	{
		do
		{
			t = UI128_or(ans, mask);
			m = UI128_Mul(b, t);

			if(UI128_IsZero(UI128_Hi))
			{
				UI128_Comp(a, m);

				if(UI128_Ununderflow)
					ans = t;
			}
		}
		while(!UI128_rs(&mask, 0));
	}
	return ans;
}

int UI128_IsZero(uint128_t a)
{
	return UI64_IsZero(a.L) && UI64_IsZero(a.H);
}
int UI128_Comp(uint128_t a, uint128_t b)
{
	uint128_t ans = UI128_Sub(a, b);

	if(!UI128_Ununderflow)
		return -1;

	if(UI128_IsZero(ans))
		return 0;

	return 1;
}
uint UI128_rs(uint128_t *a, uint msb)
{
	return UI64_rs(&a->L, UI64_rs(&a->H, msb));
}
uint128_t UI128_or(uint128_t a, uint128_t b)
{
	uint128_t ans;

	ans.L = UI64_or(a.L, b.L);
	ans.H = UI64_or(a.H, b.H);

	return ans;
}
