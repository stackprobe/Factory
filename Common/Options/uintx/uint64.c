#include "uint64.h"

UI64_t ToUI64(uint src[2])
{
	UI64_t ans;

	ans.L = ToUI32(src + 0);
	ans.H = ToUI32(src + 1);

	return ans;
}
UI64_t UI64_x(uint x)
{
	UI64_t ans;

	ans.L = UI32_x(x);
	ans.H = UI32_0();

	return ans;
}
UI64_t UI64_0(void)
{
	UI64_t ans;

	ans.L = UI32_0();
	ans.H = UI32_0();

	return ans;
}
void FromUI64(UI64_t a, uint dest[2])
{
	FromUI32(a.L, dest + 0);
	FromUI32(a.H, dest + 1);
}

UI64_t UI64_Inv(UI64_t a)
{
	UI64_t ans;

	ans.L = UI32_Inv(a.L);
	ans.H = UI32_Inv(a.H);

	return ans;
}
UI64_t UI64_Add(UI64_t a, UI64_t b, UI64_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI64_t UI64_Sub(UI64_t a, UI64_t b)
{
	UI64_t tmp[2];

	b = UI64_Inv(b);
	b = UI64_Add(b, UI64_x(1), tmp);

	return UI64_Add(a, b, tmp);
}
UI64_t UI64_Mul(UI64_t a, UI64_t b, UI64_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI64_t UI64_Div(UI64_t a, UI64_t b, UI64_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI64_t UI64_Mod(UI64_t a, UI64_t b, UI64_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI64_IsZero(UI64_t a)
{
	return UI32_IsZero(a.L) && UI32_IsZero(a.H);
}
int UI64_IsFill(UI64_t a)
{
	return UI32_IsFill(a.L) && UI32_IsFill(a.H);
}
sint UI64_Comp(UI64_t a, UI64_t b)
{
	sint ret = UI32_Comp(a.H, b.H);

	if(!ret)
		ret = UI32_Comp(a.L, b.L);

	return ret;
}
UI64_t UI64_Fill(void)
{
	UI64_t ans;

	ans.L = UI32_Fill();
	ans.H = UI32_Fill();

	return ans;
}
