#include "uint$BIT.h"

UI$BIT_t ToUI$BIT(uint src[$SZ])
{
	UI$BIT_t ans;

	ans.L = ToUI$hBIT(src + 0);
	ans.H = ToUI$hBIT(src + $hSZ);

	return ans;
}
UI$BIT_t UI$BIT_x(uint x)
{
	UI$BIT_t ans;

	ans.L = UI$hBIT_x(x);
	ans.H = UI$hBIT_0();

	return ans;
}
UI$BIT_t UI$BIT_0(void)
{
	UI$BIT_t ans;

	ans.L = UI$hBIT_0();
	ans.H = UI$hBIT_0();

	return ans;
}
void FromUI$BIT(UI$BIT_t a, uint dest[$SZ])
{
	FromUI$hBIT(a.L, dest + 0);
	FromUI$hBIT(a.H, dest + $hSZ);
}

UI$BIT_t UI$BIT_Inv(UI$BIT_t a)
{
	UI$BIT_t ans;

	ans.L = UI$hBIT_Inv(a.L);
	ans.H = UI$hBIT_Inv(a.H);

	return ans;
}
UI$BIT_t UI$BIT_Add(UI$BIT_t a, UI$BIT_t b, UI$BIT_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI$BIT_t UI$BIT_Sub(UI$BIT_t a, UI$BIT_t b)
{
	UI$BIT_t tmp[2];

	b = UI$BIT_Inv(b);
	b = UI$BIT_Add(b, UI$BIT_x(1), tmp);

	return UI$BIT_Add(a, b, tmp);
}
UI$BIT_t UI$BIT_Mul(UI$BIT_t a, UI$BIT_t b, UI$BIT_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI$BIT_t UI$BIT_Div(UI$BIT_t a, UI$BIT_t b, UI$BIT_t ans[2])
{
	error(); // TODO

	return ans[0];
}
UI$BIT_t UI$BIT_Mod(UI$BIT_t a, UI$BIT_t b, UI$BIT_t ans[2])
{
	error(); // TODO

	return ans[1];
}

int UI$BIT_IsZero(UI$BIT_t a)
{
	return UI$hBIT_IsZero(a.L) && UI$hBIT_IsZero(a.H);
}
int UI$BIT_IsFill(UI$BIT_t a)
{
	return UI$hBIT_IsFill(a.L) && UI$hBIT_IsFill(a.H);
}
sint UI$BIT_Comp(UI$BIT_t a, UI$BIT_t b)
{
	sint ret = UI$hBIT_Comp(a.H, b.H);

	if(!ret)
		ret = UI$hBIT_Comp(a.L, b.L);

	return ret;
}
UI$BIT_t UI$BIT_Fill(void)
{
	UI$BIT_t ans;

	ans.L = UI$hBIT_Fill();
	ans.H = UI$hBIT_Fill();

	return ans;
}
