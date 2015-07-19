#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Prime2.h"
#include "C:\Factory\Common\Options\Random.h"

#define PBIT_P_NUM 3936092160

static void DoTest_One(uint64 value)
{
	int ret1 = IsPrime(value);
	int ret2 = IsPrime_R(value);

	if(ret1 ? !ret2 : ret2)
	{
		cout("value: %I64u\n", value);
		error();
	}
}
static void DoTest(uint64 minval, uint64 maxval)
{
	uint64 value;

	cout("%I64u to %I64u\n", minval, maxval);

	errorCase(maxval < minval);

	for(value = minval; value < maxval; value++)
	{
		DoTest_One(value);
	}
	DoTest_One(maxval);
}
static void Test_01(void)
{
	DoTest(0, 1000000);
	DoTest(
		(uint64)UINTMAX - 1000,
		(uint64)UINTMAX + 1000
		);
	DoTest(
		(uint64)PBIT_P_NUM * 2 - 1000,
		(uint64)PBIT_P_NUM * 2 + 1000
		);
	DoTest(
		(UINT64MAX / PBIT_P_NUM) * PBIT_P_NUM - 1000,
		(UINT64MAX / PBIT_P_NUM) * PBIT_P_NUM + 1000
		);
	DoTest(
		UINT64MAX - 1000,
		UINT64MAX
		);

	cout("OK!\n");
}
int main(int argc, char **argv)
{
	mt19937_initRnd(time(NULL));

	Test_01();
}
