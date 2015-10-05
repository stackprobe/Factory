#include "uint2048.h"

uint UI2048_Overflow;
uint UI2048_Ununderflow;
uint2048_t UI2048_Hi;

uint2048_t ToUI2048(uint src[64])
{
	uint2048_t ans;

	ans.L = ToUI1024(src);
	ans.H = ToUI1024(src + 32);

	return ans;
}
uint2048_t UI2048_0(void)
{
	uint2048_t ans;

	ans.L = UI1024_0();
	ans.H = UI1024_0();

	return ans;
}
uint2048_t UI2048_x(uint x)
{
	uint2048_t ans;

	ans.L = UI1024_x(x);
	ans.H = UI1024_0();

	return ans;
}
uint2048_t UI2048_msb1(void)
{
	uint2048_t ans;

	ans.L = UI1024_0();
	ans.H = UI1024_msb1();

	return ans;
}
void UnUI2048(uint2048_t src, uint dest[64])
{
	UnUI1024(src.L, dest);
	UnUI1024(src.H, dest + 32);
}

uint2048_t UI2048_Add(uint2048_t a, uint2048_t b)
{
	uint2048_t ans;
	uint ofL;
	uint ofH;

	ans.L = UI1024_Add(a.L, b.L);
	ofL = UI1024_Overflow;
	ans.H = UI1024_Add(a.H, b.H);
	ofH = UI1024_Overflow;
	ans.H = UI1024_Add(ans.H, UI1024_x(ofL));

	UI2048_Overflow = ofH | UI1024_Overflow;

	return ans;
}
uint2048_t UI2048_Sub(uint2048_t a, uint2048_t b)
{
	uint2048_t ans;
	uint ufL;
	uint ufH;

	ans.L = UI1024_Sub(a.L, b.L);
	ufL = UI1024_Ununderflow;
	ans.H = UI1024_Sub(a.H, b.H);
	ufH = UI1024_Ununderflow;
	ans.H = UI1024_Sub(ans.H, UI1024_x(ufL ^ 1));

	UI2048_Ununderflow = ufH & UI1024_Ununderflow;

	return ans;
}
uint2048_t UI2048_Mul(uint2048_t a, uint2048_t b)
{
	uint2048_t ans;
	uint2048_t tmp1L;
	uint2048_t tmp1H;
	uint2048_t tmp2L;
	uint2048_t tmp2H;

	ans.L = UI1024_Mul(a.L, b.L);
	ans.H = UI1024_Hi;

	UI2048_Hi.L = UI1024_Mul(a.H, b.H);
	UI2048_Hi.H = UI1024_Hi;

	tmp1L.L = UI1024_0();
	tmp1L.H = UI1024_Mul(a.L, b.H);
	tmp1H.L = UI1024_Hi;
	tmp1H.H = UI1024_0();
	tmp2L.L = UI1024_0();
	tmp2L.H = UI1024_Mul(a.H, b.L);
	tmp2H.L = UI1024_Hi;
	tmp2H.H = UI1024_0();

	ans = UI2048_Add(ans, tmp1L);
	UI2048_Hi = UI2048_Add(UI2048_Hi, UI2048_x(UI1024_Overflow));
	ans = UI2048_Add(ans, tmp2L);
	UI2048_Hi = UI2048_Add(UI2048_Hi, UI2048_x(UI1024_Overflow));
	UI2048_Hi = UI2048_Add(UI2048_Hi, tmp1H);
	UI2048_Hi = UI2048_Add(UI2048_Hi, tmp2H);

	return ans;
}
uint2048_t UI2048_Div(uint2048_t a, uint2048_t b)
{
	uint2048_t ans = UI2048_0();
	uint2048_t mask = UI2048_msb1();
	uint2048_t t;
	uint2048_t m;

	if(!UI2048_IsZero(b)) // ? ! ƒ[ƒœZ
	{
		do
		{
			t = UI2048_or(ans, mask);
			m = UI2048_Mul(b, t);

			if(UI2048_IsZero(UI2048_Hi))
			{
				UI2048_Comp(a, m);

				if(UI2048_Ununderflow)
					ans = t;
			}
		}
		while(!UI2048_rs(&mask, 0));
	}
	return ans;
}

int UI2048_IsZero(uint2048_t a)
{
	return UI1024_IsZero(a.L) && UI1024_IsZero(a.H);
}
int UI2048_Comp(uint2048_t a, uint2048_t b)
{
	uint2048_t ans = UI2048_Sub(a, b);

	if(!UI2048_Ununderflow)
		return -1;

	if(UI2048_IsZero(ans))
		return 0;

	return 1;
}
uint UI2048_rs(uint2048_t *a, uint msb)
{
	return UI1024_rs(&a->L, UI1024_rs(&a->H, msb));
}
uint2048_t UI2048_or(uint2048_t a, uint2048_t b)
{
	uint2048_t ans;

	ans.L = UI1024_or(a.L, b.L);
	ans.H = UI1024_or(a.H, b.H);

	return ans;
}
