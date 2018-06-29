#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc2.h"

int main(int argc, char **argv)
{
readArgs:
	if(argIs("/X"))
	{
		calcRadix = toValue(nextArg()); // 2 Å` 36, îÕàÕäO -> calc()Ç≈error();
		goto readArgs;
	}
	if(argIs("/B"))
	{
		calcBasement = atoi(nextArg()); // -IMAX Å` IMAX, îÕàÕäO -> calc()Ç≈error();
		goto readArgs;
	}
	if(argIs("/S"))
	{
		calcScient = 1;
		goto readArgs;
	}
	if(argIs("/E"))
	{
		calcEffect = toValue(nextArg()); // 0 == ñ≥å¯, 1 Å` IMAX, îÕàÕäO -> calc()Ç≈error();
		goto readArgs;
	}
	if(argIs("/-R"))
	{
		calcRndOff = 0;
		goto readArgs;
	}

	/*
		Calc2 /e 10 /em 123.456 Ç∆Ç©
	*/
	if(argIs("/EM"))
	{
		calcOperand_t *co = calcFromString(nextArg());
		char *ret;

		ret = calcGetSmplString_EM(co, calcEffect);
		calcRelease(co);
		cout("%s\n", ret);
		memFree(ret);
		return;
	}

	/*
		Ç◊Ç´èÊ
	*/
	if(argIs("/P"))
	{
		calcOperand_t *co = calcFromString(getArg(0));
		uint exponent = toValue(getArg(1));

		cout("%s\n", c_calcGetString_x(calcPower(co, exponent)));

		calcRelease(co);
		return;
	}

	/*
		Ç◊Ç´ç™
	*/
	if(argIs("/R"))
	{
		calcOperand_t *co = calcFromString(getArg(0));
		uint exponent = toValue(getArg(1));

		cout("%s\n", c_calcGetString_x(calcRoot(co, exponent)));

		calcRelease(co);
		return;
	}

	if(hasArgs(4))
	{
		calcScient = 1;
		calcEffect = toValue(getArg(3));
		cout("%s\n", c_calc(getArg(0), getArg(1)[0], getArg(2)));
		return;
	}
	if(hasArgs(3))
	{
		cout("%s\n", c_calc(getArg(0), getArg(1)[0], getArg(2)));
		return;
	}
	if(hasArgs(2))
	{
		calcScient = 1;
		calcEffect = toValue(getArg(1));
		cout("%s\n", c_calcGetString_x(calcFromString(getArg(0))));
		return;
	}
	if(hasArgs(1))
	{
		cout("%s\n", c_calcGetString_x(calcFromString(getArg(0))));
		return;
	}
	error();
}
