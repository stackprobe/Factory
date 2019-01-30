#include "uint4096.h"

UI4096_t ToUI4096(uint src[128])
{
	UI4096_t ans;

	ans.L = ToUI2048(src + 0);
	ans.H = ToUI2048(src + 64);

	return ans;
}
UI4096_t UI4096_x(uint x)
{
	UI4096_t ans;

	ans.L = UI2048_x(x);
	ans.H = UI2048_0();

	return ans;
}
UI4096_t UI4096_0(void)
{
	UI4096_t ans;

	ans.L = UI2048_0();
	ans.H = UI2048_0();

	return ans;
}
void FromUI4096(UI4096_t a, uint dest[128])
{
	FromUI2048(a.L, dest + 0);
	FromUI2048(a.H, dest + 64);
}

UI4096_t UI4096_Inv(UI4096_t a)
{
	UI4096_t ans;

	ans.L = UI2048_Inv(a.L);
	ans.H = UI2048_Inv(a.H);

	return ans;
}
UI4096_t UI4096_Add(UI4096_t a, UI4096_t b, UI4096_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI4096_t UI4096_Sub(UI4096_t a, UI4096_t b)
{
	UI4096_t tmp[2];

	b = UI4096_Inv(b);
	b = UI4096_Add(b, UI4096_x(1), tmp);

	return UI4096_Add(a, b, tmp);
}
UI4096_t UI4096_Mul(UI4096_t a, UI4096_t b, UI4096_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI4096_t UI4096_Div(UI4096_t a, UI4096_t b, UI4096_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI4096_t UI4096_Mod(UI4096_t a, UI4096_t b, UI4096_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI4096_IsZero(UI4096_t a)
{
	return UI2048_IsZero(a.L) && UI2048_IsZero(a.H);
}
int UI4096_IsFill(UI4096_t a)
{
	return UI2048_IsFill(a.L) && UI2048_IsFill(a.H);
}
sint UI4096_Comp(UI4096_t a, UI4096_t b)
{
	sint ret = UI2048_Comp(a.H, b.H);

	if(!ret)
		ret = UI2048_Comp(a.L, b.L);

	return ret;
}
UI4096_t UI4096_Fill(void)
{
	UI4096_t ans;

	ans.L = UI2048_Fill();
	ans.H = UI2048_Fill();

	return ans;
}
