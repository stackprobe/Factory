#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc.h"

static char *GetString(void *pVal, uint valSz, uint expBits, uint expBase, uint basement)
{
	char *str = strx("");
	uint index;
	uint bit;
	char *sign;
	char *exp;
	char *fract;

	for(index = 0; index < valSz; index++)
	for(bit = 1; bit < 0x100; bit <<= 1)
	{
		str = addChar(str, ((uchar *)pVal)[index] & bit ? '1' : '0');
	}
	reverseLine(str);

	cout("BINA = %s\n", str);

	sign = strxl(str, 1);
	exp = strxl(str + 1, expBits);
	fract = strx(str + 1 + expBits);

	memFree(str);

	cout("SIGN = %s\n", sign);
	cout("EXP  = %s\n", exp);
	cout("F    = %s\n", fract);

	cout("FORM = (-1 ^ %s) * (2 ^ (b_%s - %u)) * b_1.%s\n", sign, exp, expBase, fract);

	str = calcLine_xx(
		calcPower(
			"-1",
			toValue(sign),
			2
			),
		'*',
		calcLine_xx(
			calcLine_xx(
				calcPower(
					"10",
					toValueDigits(exp, "01"),
					2
					),
				'/',
				calcPower(
					"10",
					expBase,
					2
					),
				2,
				2000
				),
			'*',
			xcout("1.%s", fract),
			2,
			0
			),
		2,
		0
		);

	cout("BVAL = %s\n", str);

	str = changeRadixCalcLine(str, 2, 10, basement);

	if(calcLastMarume)
		str = calcLineToMarume_x(str, basement);

	return str; // g
}
#if 1
static void ShowFloat(float value)
{
	cout("%.10f\n", value);
	cout("%s\n", GetString(&value, 4, 8, 127, 10));
}
static void ShowDouble(double value)
{
	cout("%.20f\n", value);
	cout("%s\n", GetString(&value, 8, 11, 1023, 20));
}
#else
static void ShowFloat(float value)
{
	cout("%.100f\n", value);
	cout("%s\n", GetString(&value, 4, 8, 127, 100));
}
static void ShowDouble(double value)
{
	cout("%.400f\n", value);
	cout("%s\n", GetString(&value, 8, 11, 1023, 400));
}
#endif
int main(int argc, char **argv)
{
	if(argIs("/INF"))
	{
		float f  = 3.0;
		double d = 3.0;
		uint c;

		for(c = 100; c; c--)
		{
			f *= f;
			d *= d;
		}
		ShowFloat(f);
		ShowDouble(d);
		ShowFloat(-f);
		ShowDouble(-d);
		return;
	}
	if(argIs("/H"))
	{
		autoBlock_t *block = makeBlockHexLine(nextArg());

		switch(getSize(block))
		{
		case 4:
			ShowFloat(*(float *)b(block));
			break;

		case 8:
			ShowDouble(*(double *)b(block));
			break;

		default:
			error();
		}
		return;
	}

	{
		char *arg = nextArg();
		float f;
		double d;

		sscanf(arg, "%f", &f);
		sscanf(arg, "%lf", &d);

		ShowFloat(f);
		ShowDouble(d);
	}
}
