/*
	Calc2.exe [/R <Radix>] [/E <Effect>] /EM <Number>

	Calc2.exe [/R <Radix>] [/S] [/E <Effect>] [/-R] /P <Number> <UInt32>

	Calc2.exe [/R <Radix>] [/S] [/E <Effect>] [/-R] /R <Number> <UInt32>

	Calc2.exe [/R <Radix>] [/B <Basement>] [/-R] <Number.1> <Operator> <Number.2> <Effect>

	Calc2.exe [/R <Radix>] [/B <Basement>] [/S] [/E <Effect>] [/-R] <Number.1> <Operator> <Number.2>

	Calc2.exe [/R <Radix>] [/-R] <Number> <Effect>

	Calc2.exe [/R <Radix>] [/S] [/E <Effect>] [/-R] <Number>

		Radix    ...     2 `   36
		Basement ... -IMAX ` IMAX
		Effect   ...     0 ` IMAX

		Number ... ΐ | ΐΜw\L

		Operator ... + - * /

	----
	α

	Calc2 /S 123.456   ==>   1.23456E+02

	Calc2 /E 10 /EM 123.456   ==>   123.4560000
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc2.h"

int main(int argc, char **argv)
{
readArgs:
	if(argIs("/X"))
	{
		calcRadix = toValue(nextArg()); // 2 ` 36, ΝΝO -> calc()Εerror();
		goto readArgs;
	}
	if(argIs("/B"))
	{
		calcBasement = atoi(nextArg()); // -IMAX ` IMAX, ΝΝO -> calc()Εerror();
		goto readArgs;
	}
	if(argIs("/S"))
	{
		calcScient = 1;
		goto readArgs;
	}
	if(argIs("/E"))
	{
		calcEffect = toValue(nextArg()); // 0 == ³ψ, 1 ` IMAX, ΝΝO -> calc()Εerror();
		goto readArgs;
	}
	if(argIs("/-R"))
	{
		calcRndOff = 0;
		goto readArgs;
	}

	/*
		Calc2 /e 10 /em 123.456 Ζ©
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
		Χ«ζ
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
		Χ«ͺ
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
