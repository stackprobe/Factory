#include "uint4096.h"

uint UI4096_Overflow;
uint UI4096_Ununderflow;
uint4096_t UI4096_Hi;

uint4096_t ToUI4096(uint src[128])
{
	uint4096_t ans;

	ans.L = ToUI2048(src);
	ans.H = ToUI2048(src + 64);

	return ans;
}
uint4096_t UI4096_0(void)
{
	uint4096_t ans;

	ans.L = UI2048_0();
	ans.H = UI2048_0();

	return ans;
}
uint4096_t UI4096_x(uint x)
{
	uint4096_t ans;

	ans.L = UI2048_x(x);
	ans.H = UI2048_0();

	return ans;
}
uint4096_t UI4096_msb1(void)
{
	uint4096_t ans;

	ans.L = UI2048_0();
	ans.H = UI2048_msb1();

	return ans;
}
void UnUI4096(uint4096_t src, uint dest[128])
{
	UnUI2048(src.L, dest);
	UnUI2048(src.H, dest + 64);
}

uint4096_t UI4096_Add(uint4096_t a, uint4096_t b)
{
	uint4096_t ans;
	uint ofL;
	uint ofH;

	ans.L = UI2048_Add(a.L, b.L);
	ofL = UI2048_Overflow;
	ans.H = UI2048_Add(a.H, b.H);
	ofH = UI2048_Overflow;
	ans.H = UI2048_Add(ans.H, UI2048_x(ofL));

	UI4096_Overflow = ofH | UI2048_Overflow;

	return ans;
}
uint4096_t UI4096_Sub(uint4096_t a, uint4096_t b)
{
	uint4096_t ans;
	uint ufL;
	uint ufH;

	ans.L = UI2048_Sub(a.L, b.L);
	ufL = UI2048_Ununderflow;
	ans.H = UI2048_Sub(a.H, b.H);
	ufH = UI2048_Ununderflow;
	ans.H = UI2048_Sub(ans.H, UI2048_x(ufL ^ 1));

	UI4096_Ununderflow = ufH & UI2048_Ununderflow;

	return ans;
}
uint4096_t UI4096_Mul(uint4096_t a, uint4096_t b)
{
	uint4096_t ans;
	uint4096_t tmp1L;
	uint4096_t tmp1H;
	uint4096_t tmp2L;
	uint4096_t tmp2H;

	ans.L = UI2048_Mul(a.L, b.L);
	ans.H = UI2048_Hi;

	UI4096_Hi.L = UI2048_Mul(a.H, b.H);
	UI4096_Hi.H = UI2048_Hi;

	tmp1L.L = UI2048_0();
	tmp1L.H = UI2048_Mul(a.L, b.H);
	tmp1H.L = UI2048_Hi;
	tmp1H.H = UI2048_0();
	tmp2L.L = UI2048_0();
	tmp2L.H = UI2048_Mul(a.H, b.L);
	tmp2H.L = UI2048_Hi;
	tmp2H.H = UI2048_0();

	ans = UI4096_Add(ans, tmp1L);
	UI4096_Hi = UI4096_Add(UI4096_Hi, UI4096_x(UI2048_Overflow));
	ans = UI4096_Add(ans, tmp2L);
	UI4096_Hi = UI4096_Add(UI4096_Hi, UI4096_x(UI2048_Overflow));
	UI4096_Hi = UI4096_Add(UI4096_Hi, tmp1H);
	UI4096_Hi = UI4096_Add(UI4096_Hi, tmp2H);

	return ans;
}
uint4096_t UI4096_Div(uint4096_t a, uint4096_t b)
{
	uint4096_t ans = UI4096_0();
	uint4096_t mask = UI4096_msb1();
	uint4096_t t;
	uint4096_t m;

	if(!UI4096_IsZero(b)) // ? ! ƒ[ƒœZ
	{
		do
		{
			t = UI4096_or(ans, mask);
			m = UI4096_Mul(b, t);

			if(UI4096_IsZero(UI4096_Hi))
			{
				UI4096_Comp(a, m);

				if(UI4096_Ununderflow)
					ans = t;
			}
		}
		while(!UI4096_rs(&mask, 0));
	}
	return ans;
}

int UI4096_IsZero(uint4096_t a)
{
	return UI2048_IsZero(a.L) && UI2048_IsZero(a.H);
}
int UI4096_Comp(uint4096_t a, uint4096_t b)
{
	uint4096_t ans = UI4096_Sub(a, b);

	if(!UI4096_Ununderflow)
		return -1;

	if(UI4096_IsZero(ans))
		return 0;

	return 1;
}
uint UI4096_rs(uint4096_t *a, uint msb)
{
	return UI2048_rs(&a->L, UI2048_rs(&a->H, msb));
}
uint4096_t UI4096_or(uint4096_t a, uint4096_t b)
{
	uint4096_t ans;

	ans.L = UI2048_or(a.L, b.L);
	ans.H = UI2048_or(a.H, b.H);

	return ans;
}
