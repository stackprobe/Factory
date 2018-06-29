#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc2.h"

int main(int argc, char **argv)
{
readArgs:
	if(argIs("/X"))
	{
		calcRadix = toValue(nextArg()); // 2 �` 36, �͈͊O -> calc()��error();
		goto readArgs;
	}
	if(argIs("/B"))
	{
		calcBasement = atoi(nextArg()); // -IMAX �` IMAX, �͈͊O -> calc()��error();
		goto readArgs;
	}
	if(argIs("/S"))
	{
		calcScient = 1;
		goto readArgs;
	}
	if(argIs("/E"))
	{
		calcEffect = toValue(nextArg()); // 0 == ����, 1 �` IMAX, �͈͊O -> calc()��error();
		goto readArgs;
	}
	if(argIs("/-R"))
	{
		calcRndOff = 0;
		goto readArgs;
	}

	/*
		Calc2 /e 10 /em 123.456 �Ƃ�
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
		�ׂ���
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
		�ׂ���
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
