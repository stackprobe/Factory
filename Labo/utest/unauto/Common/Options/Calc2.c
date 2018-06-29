#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc2.h"

static void Test_calcComp(char *str1, char *str2)
{
	int ret1;
	int ret2;

	// テストver
	{
		calcOperand_t *op1 = calcFromString(str1);
		calcOperand_t *op2 = calcFromString(str2);

		ret1 = calcComp_OLD(op1, op2);

		calcRelease(op1);
		calcRelease(op2);
	}

	// 本物
	{
		calcOperand_t *op1 = calcFromString(str1);
		calcOperand_t *op2 = calcFromString(str2);

		ret2 = calcComp(op1, op2);

		calcRelease(op1);
		calcRelease(op2);
	}

	cout("ret1: %d\n", ret1);
	cout("ret2: %d\n", ret2);

	if(ret1 < 0) ret1 = -1;
	if(ret1 > 0) ret1 = 1;

	if(ret2 < 0) ret2 = -1;
	if(ret2 > 0) ret2 = 1;

	errorCase(ret1 != ret2);
}
int main(int argc, char **argv)
{
	if(argIs("/Comp"))
	{
		Test_calcComp(getArg(0), getArg(1));
		return;
	}
}
