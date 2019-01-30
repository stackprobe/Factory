#include "uint512.h"

UI512_t ToUI512(uint src[16])
{
	UI512_t ans;

	ans.L = ToUI256(src + 0);
	ans.H = ToUI256(src + 8);

	return ans;
}
UI512_t UI512_x(uint x)
{
	UI512_t ans;

	ans.L = UI256_x(x);
	ans.H = UI256_0();

	return ans;
}
UI512_t UI512_0(void)
{
	UI512_t ans;

	ans.L = UI256_0();
	ans.H = UI256_0();

	return ans;
}
void FromUI512(UI512_t a, uint dest[16])
{
	FromUI256(a.L, dest + 0);
	FromUI256(a.H, dest + 8);
}

UI512_t UI512_Inv(UI512_t a)
{
	UI512_t ans;

	ans.L = UI256_Inv(a.L);
	ans.H = UI256_Inv(a.H);

	return ans;
}
UI512_t UI512_Add(UI512_t a, UI512_t b, UI512_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI512_t UI512_Sub(UI512_t a, UI512_t b)
{
	UI512_t tmp[2];

	b = UI512_Inv(b);
	b = UI512_Add(b, UI512_x(1), tmp);

	return UI512_Add(a, b, tmp);
}
UI512_t UI512_Mul(UI512_t a, UI512_t b, UI512_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI512_t UI512_Div(UI512_t a, UI512_t b, UI512_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI512_t UI512_Mod(UI512_t a, UI512_t b, UI512_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI512_IsZero(UI512_t a)
{
	return UI256_IsZero(a.L) && UI256_IsZero(a.H);
}
int UI512_IsFill(UI512_t a)
{
	return UI256_IsFill(a.L) && UI256_IsFill(a.H);
}
sint UI512_Comp(UI512_t a, UI512_t b)
{
	sint ret = UI256_Comp(a.H, b.H);

	if(!ret)
		ret = UI256_Comp(a.L, b.L);

	return ret;
}
UI512_t UI512_Fill(void)
{
	UI512_t ans;

	ans.L = UI256_Fill();
	ans.H = UI256_Fill();

	return ans;
}
