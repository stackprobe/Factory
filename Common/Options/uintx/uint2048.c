#include "uint2048.h"

UI2048_t ToUI2048(uint src[64])
{
	UI2048_t ans;

	ans.L = ToUI1024(src + 0);
	ans.H = ToUI1024(src + 32);

	return ans;
}
UI2048_t UI2048_x(uint x)
{
	UI2048_t ans;

	ans.L = UI1024_x(x);
	ans.H = UI1024_0();

	return ans;
}
UI2048_t UI2048_0(void)
{
	UI2048_t ans;

	ans.L = UI1024_0();
	ans.H = UI1024_0();

	return ans;
}
void FromUI2048(UI2048_t a, uint dest[64])
{
	FromUI1024(a.L, dest + 0);
	FromUI1024(a.H, dest + 32);
}

UI2048_t UI2048_Inv(UI2048_t a)
{
	UI2048_t ans;

	ans.L = UI1024_Inv(a.L);
	ans.H = UI1024_Inv(a.H);

	return ans;
}
UI2048_t UI2048_Add(UI2048_t a, UI2048_t b, UI2048_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI2048_t UI2048_Sub(UI2048_t a, UI2048_t b)
{
	UI2048_t tmp[2];

	b = UI2048_Inv(b);
	b = UI2048_Add(b, UI2048_x(1), tmp);

	return UI2048_Add(a, b, tmp);
}
UI2048_t UI2048_Mul(UI2048_t a, UI2048_t b, UI2048_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI2048_t UI2048_Div(UI2048_t a, UI2048_t b, UI2048_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI2048_t UI2048_Mod(UI2048_t a, UI2048_t b, UI2048_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI2048_IsZero(UI2048_t a)
{
	return UI1024_IsZero(a.L) && UI1024_IsZero(a.H);
}
int UI2048_IsFill(UI2048_t a)
{
	return UI1024_IsFill(a.L) && UI1024_IsFill(a.H);
}
sint UI2048_Comp(UI2048_t a, UI2048_t b)
{
	sint ret = UI1024_Comp(a.H, b.H);

	if(!ret)
		ret = UI1024_Comp(a.L, b.L);

	return ret;
}
UI2048_t UI2048_Fill(void)
{
	UI2048_t ans;

	ans.L = UI1024_Fill();
	ans.H = UI1024_Fill();

	return ans;
}
