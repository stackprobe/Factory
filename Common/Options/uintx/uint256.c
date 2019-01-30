#include "uint256.h"

UI256_t ToUI256(uint src[8])
{
	UI256_t ans;

	ans.L = ToUI128(src + 0);
	ans.H = ToUI128(src + 4);

	return ans;
}
UI256_t UI256_x(uint x)
{
	UI256_t ans;

	ans.L = UI128_x(x);
	ans.H = UI128_0();

	return ans;
}
UI256_t UI256_0(void)
{
	UI256_t ans;

	ans.L = UI128_0();
	ans.H = UI128_0();

	return ans;
}
void FromUI256(UI256_t a, uint dest[8])
{
	FromUI128(a.L, dest + 0);
	FromUI128(a.H, dest + 4);
}

UI256_t UI256_Inv(UI256_t a)
{
	UI256_t ans;

	ans.L = UI128_Inv(a.L);
	ans.H = UI128_Inv(a.H);

	return ans;
}
UI256_t UI256_Add(UI256_t a, UI256_t b, UI256_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI256_t UI256_Sub(UI256_t a, UI256_t b)
{
	UI256_t tmp[2];

	b = UI256_Inv(b);
	b = UI256_Add(b, UI256_x(1), tmp);

	return UI256_Add(a, b, tmp);
}
UI256_t UI256_Mul(UI256_t a, UI256_t b, UI256_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI256_t UI256_Div(UI256_t a, UI256_t b, UI256_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI256_t UI256_Mod(UI256_t a, UI256_t b, UI256_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI256_IsZero(UI256_t a)
{
	return UI128_IsZero(a.L) && UI128_IsZero(a.H);
}
int UI256_IsFill(UI256_t a)
{
	return UI128_IsFill(a.L) && UI128_IsFill(a.H);
}
sint UI256_Comp(UI256_t a, UI256_t b)
{
	sint ret = UI128_Comp(a.H, b.H);

	if(!ret)
		ret = UI128_Comp(a.L, b.L);

	return ret;
}
UI256_t UI256_Fill(void)
{
	UI256_t ans;

	ans.L = UI128_Fill();
	ans.H = UI128_Fill();

	return ans;
}
