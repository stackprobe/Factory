#include "uint1024.h"

uint UI1024_Overflow;
uint UI1024_Ununderflow;
uint1024_t UI1024_Hi;

uint1024_t ToUI1024(uint src[32])
{
	uint1024_t ans;

	ans.L = ToUI512(src);
	ans.H = ToUI512(src + 16);

	return ans;
}
uint1024_t UI1024_0(void)
{
	uint1024_t ans;

	ans.L = UI512_0();
	ans.H = UI512_0();

	return ans;
}
uint1024_t UI1024_x(uint x)
{
	uint1024_t ans;

	ans.L = UI512_x(x);
	ans.H = UI512_0();

	return ans;
}
uint1024_t UI1024_msb1(void)
{
	uint1024_t ans;

	ans.L = UI512_0();
	ans.H = UI512_msb1();

	return ans;
}
void UnUI1024(uint1024_t src, uint dest[32])
{
	UnUI512(src.L, dest);
	UnUI512(src.H, dest + 16);
}

uint1024_t UI1024_Add(uint1024_t a, uint1024_t b)
{
	uint1024_t ans;
	uint ofL;
	uint ofH;

	ans.L = UI512_Add(a.L, b.L);
	ofL = UI512_Overflow;
	ans.H = UI512_Add(a.H, b.H);
	ofH = UI512_Overflow;
	ans.H = UI512_Add(ans.H, UI512_x(ofL));

	UI1024_Overflow = ofH | UI512_Overflow;

	return ans;
}
uint1024_t UI1024_Sub(uint1024_t a, uint1024_t b)
{
	uint1024_t ans;
	uint ufL;
	uint ufH;

	ans.L = UI512_Sub(a.L, b.L);
	ufL = UI512_Ununderflow;
	ans.H = UI512_Sub(a.H, b.H);
	ufH = UI512_Ununderflow;
	ans.H = UI512_Sub(ans.H, UI512_x(ufL ^ 1));

	UI1024_Ununderflow = ufH & UI512_Ununderflow;

	return ans;
}
uint1024_t UI1024_Mul(uint1024_t a, uint1024_t b)
{
	uint1024_t ans;
	uint1024_t tmp1L;
	uint1024_t tmp1H;
	uint1024_t tmp2L;
	uint1024_t tmp2H;

	ans.L = UI512_Mul(a.L, b.L);
	ans.H = UI512_Hi;

	UI1024_Hi.L = UI512_Mul(a.H, b.H);
	UI1024_Hi.H = UI512_Hi;

	tmp1L.L = UI512_0();
	tmp1L.H = UI512_Mul(a.L, b.H);
	tmp1H.L = UI512_Hi;
	tmp1H.H = UI512_0();
	tmp2L.L = UI512_0();
	tmp2L.H = UI512_Mul(a.H, b.L);
	tmp2H.L = UI512_Hi;
	tmp2H.H = UI512_0();

	ans = UI1024_Add(ans, tmp1L);
	UI1024_Hi = UI1024_Add(UI1024_Hi, UI1024_x(UI512_Overflow));
	ans = UI1024_Add(ans, tmp2L);
	UI1024_Hi = UI1024_Add(UI1024_Hi, UI1024_x(UI512_Overflow));
	UI1024_Hi = UI1024_Add(UI1024_Hi, tmp1H);
	UI1024_Hi = UI1024_Add(UI1024_Hi, tmp2H);

	return ans;
}
uint1024_t UI1024_Div(uint1024_t a, uint1024_t b)
{
	uint1024_t ans = UI1024_0();
	uint1024_t mask = UI1024_msb1();
	uint1024_t t;
	uint1024_t m;

	if(!UI1024_IsZero(b)) // ? ! ƒ[ƒœZ
	{
		do
		{
			t = UI1024_or(ans, mask);
			m = UI1024_Mul(b, t);

			if(UI1024_IsZero(UI1024_Hi))
			{
				UI1024_Comp(a, m);

				if(UI1024_Ununderflow)
					ans = t;
			}
		}
		while(!UI1024_rs(&mask, 0));
	}
	return ans;
}

int UI1024_IsZero(uint1024_t a)
{
	return UI512_IsZero(a.L) && UI512_IsZero(a.H);
}
int UI1024_Comp(uint1024_t a, uint1024_t b)
{
	uint1024_t ans = UI1024_Sub(a, b);

	if(!UI1024_Ununderflow)
		return -1;

	if(UI1024_IsZero(ans))
		return 0;

	return 1;
}
uint UI1024_rs(uint1024_t *a, uint msb)
{
	return UI512_rs(&a->L, UI512_rs(&a->H, msb));
}
uint1024_t UI1024_or(uint1024_t a, uint1024_t b)
{
	uint1024_t ans;

	ans.L = UI512_or(a.L, b.L);
	ans.H = UI512_or(a.H, b.H);

	return ans;
}
