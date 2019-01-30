#include "uint128.h"

UI128_t ToUI128(uint src[4])
{
	UI128_t ans;

	ans.L = ToUI64(src + 0);
	ans.H = ToUI64(src + 2);

	return ans;
}
UI128_t UI128_x(uint x)
{
	UI128_t ans;

	ans.L = UI64_x(x);
	ans.H = UI64_0();

	return ans;
}
UI128_t UI128_0(void)
{
	UI128_t ans;

	ans.L = UI64_0();
	ans.H = UI64_0();

	return ans;
}
void FromUI128(UI128_t a, uint dest[4])
{
	FromUI64(a.L, dest + 0);
	FromUI64(a.H, dest + 2);
}

UI128_t UI128_Inv(UI128_t a)
{
	UI128_t ans;

	ans.L = UI64_Inv(a.L);
	ans.H = UI64_Inv(a.H);

	return ans;
}
UI128_t UI128_Add(UI128_t a, UI128_t b, UI128_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI128_t UI128_Sub(UI128_t a, UI128_t b)
{
	UI128_t tmp[2];

	b = UI128_Inv(b);
	b = UI128_Add(b, UI128_x(1), tmp);

	return UI128_Add(a, b, tmp);
}
UI128_t UI128_Mul(UI128_t a, UI128_t b, UI128_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI128_t UI128_Div(UI128_t a, UI128_t b, UI128_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI128_t UI128_Mod(UI128_t a, UI128_t b, UI128_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI128_IsZero(UI128_t a)
{
	return UI64_IsZero(a.L) && UI64_IsZero(a.H);
}
int UI128_IsFill(UI128_t a)
{
	return UI64_IsFill(a.L) && UI64_IsFill(a.H);
}
sint UI128_Comp(UI128_t a, UI128_t b)
{
	sint ret = UI64_Comp(a.H, b.H);

	if(!ret)
		ret = UI64_Comp(a.L, b.L);

	return ret;
}
UI128_t UI128_Fill(void)
{
	UI128_t ans;

	ans.L = UI64_Fill();
	ans.H = UI64_Fill();

	return ans;
}
