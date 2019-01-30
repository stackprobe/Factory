#include "uint1024.h"

UI1024_t ToUI1024(uint src[32])
{
	UI1024_t ans;

	ans.L = ToUI512(src + 0);
	ans.H = ToUI512(src + 16);

	return ans;
}
UI1024_t UI1024_x(uint x)
{
	UI1024_t ans;

	ans.L = UI512_x(x);
	ans.H = UI512_0();

	return ans;
}
UI1024_t UI1024_0(void)
{
	UI1024_t ans;

	ans.L = UI512_0();
	ans.H = UI512_0();

	return ans;
}
void FromUI1024(UI1024_t a, uint dest[32])
{
	FromUI512(a.L, dest + 0);
	FromUI512(a.H, dest + 16);
}

UI1024_t UI1024_Inv(UI1024_t a)
{
	UI1024_t ans;

	ans.L = UI512_Inv(a.L);
	ans.H = UI512_Inv(a.H);

	return ans;
}
UI1024_t UI1024_Add(UI1024_t a, UI1024_t b, UI1024_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI1024_t UI1024_Sub(UI1024_t a, UI1024_t b)
{
	UI1024_t tmp[2];

	b = UI1024_Inv(b);
	b = UI1024_Add(b, UI1024_x(1), tmp);

	return UI1024_Add(a, b, tmp);
}
UI1024_t UI1024_Mul(UI1024_t a, UI1024_t b, UI1024_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI1024_t UI1024_Div(UI1024_t a, UI1024_t b, UI1024_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI1024_t UI1024_Mod(UI1024_t a, UI1024_t b, UI1024_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI1024_IsZero(UI1024_t a)
{
	return UI512_IsZero(a.L) && UI512_IsZero(a.H);
}
int UI1024_IsFill(UI1024_t a)
{
	return UI512_IsFill(a.L) && UI512_IsFill(a.H);
}
sint UI1024_Comp(UI1024_t a, UI1024_t b)
{
	sint ret = UI512_Comp(a.H, b.H);

	if(!ret)
		ret = UI512_Comp(a.L, b.L);

	return ret;
}
UI1024_t UI1024_Fill(void)
{
	UI1024_t ans;

	ans.L = UI512_Fill();
	ans.H = UI512_Fill();

	return ans;
}
