#include "uint256.h"

uint UI256_Overflow;
uint UI256_Ununderflow;
uint256_t UI256_Hi;

uint256_t ToUI256(uint src[8])
{
	uint256_t ans;

	ans.L = ToUI128(src);
	ans.H = ToUI128(src + 4);

	return ans;
}
uint256_t UI256_0(void)
{
	uint256_t ans;

	ans.L = UI128_0();
	ans.H = UI128_0();

	return ans;
}
uint256_t UI256_x(uint x)
{
	uint256_t ans;

	ans.L = UI128_x(x);
	ans.H = UI128_0();

	return ans;
}
uint256_t UI256_msb1(void)
{
	uint256_t ans;

	ans.L = UI128_0();
	ans.H = UI128_msb1();

	return ans;
}
void UnUI256(uint256_t src, uint dest[8])
{
	UnUI128(src.L, dest);
	UnUI128(src.H, dest + 4);
}

uint256_t UI256_Add(uint256_t a, uint256_t b)
{
	uint256_t ans;
	uint ofL;
	uint ofH;

	ans.L = UI128_Add(a.L, b.L);
	ofL = UI128_Overflow;
	ans.H = UI128_Add(a.H, b.H);
	ofH = UI128_Overflow;
	ans.H = UI128_Add(ans.H, UI128_x(ofL));

	UI256_Overflow = ofH | UI128_Overflow;

	return ans;
}
uint256_t UI256_Sub(uint256_t a, uint256_t b)
{
	uint256_t ans;
	uint ufL;
	uint ufH;

	ans.L = UI128_Sub(a.L, b.L);
	ufL = UI128_Ununderflow;
	ans.H = UI128_Sub(a.H, b.H);
	ufH = UI128_Ununderflow;
	ans.H = UI128_Sub(ans.H, UI128_x(ufL ^ 1));

	UI256_Ununderflow = ufH & UI128_Ununderflow;

	return ans;
}
uint256_t UI256_Mul(uint256_t a, uint256_t b)
{
	uint256_t ans;
	uint256_t tmp1L;
	uint256_t tmp1H;
	uint256_t tmp2L;
	uint256_t tmp2H;

	ans.L = UI128_Mul(a.L, b.L);
	ans.H = UI128_Hi;

	UI256_Hi.L = UI128_Mul(a.H, b.H);
	UI256_Hi.H = UI128_Hi;

	tmp1L.L = UI128_0();
	tmp1L.H = UI128_Mul(a.L, b.H);
	tmp1H.L = UI128_Hi;
	tmp1H.H = UI128_0();
	tmp2L.L = UI128_0();
	tmp2L.H = UI128_Mul(a.H, b.L);
	tmp2H.L = UI128_Hi;
	tmp2H.H = UI128_0();

	ans = UI256_Add(ans, tmp1L);
	UI256_Hi = UI256_Add(UI256_Hi, UI256_x(UI128_Overflow));
	ans = UI256_Add(ans, tmp2L);
	UI256_Hi = UI256_Add(UI256_Hi, UI256_x(UI128_Overflow));
	UI256_Hi = UI256_Add(UI256_Hi, tmp1H);
	UI256_Hi = UI256_Add(UI256_Hi, tmp2H);

	return ans;
}
uint256_t UI256_Div(uint256_t a, uint256_t b)
{
	uint256_t ans = UI256_0();
	uint256_t mask = UI256_msb1();
	uint256_t t;
	uint256_t m;

	if(!UI256_IsZero(b)) // ? ! ƒ[ƒœZ
	{
		do
		{
			t = UI256_or(ans, mask);
			m = UI256_Mul(b, t);

			if(UI256_IsZero(UI256_Hi))
			{
				UI256_Comp(a, m);

				if(UI256_Ununderflow)
					ans = t;
			}
		}
		while(!UI256_rs(&mask, 0));
	}
	return ans;
}

int UI256_IsZero(uint256_t a)
{
	return UI128_IsZero(a.L) && UI128_IsZero(a.H);
}
int UI256_Comp(uint256_t a, uint256_t b)
{
	uint256_t ans = UI256_Sub(a, b);

	if(!UI256_Ununderflow)
		return -1;

	if(UI256_IsZero(ans))
		return 0;

	return 1;
}
uint UI256_rs(uint256_t *a, uint msb)
{
	return UI128_rs(&a->L, UI128_rs(&a->H, msb));
}
uint256_t UI256_or(uint256_t a, uint256_t b)
{
	uint256_t ans;

	ans.L = UI128_or(a.L, b.L);
	ans.H = UI128_or(a.H, b.H);

	return ans;
}
