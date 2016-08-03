#include "C:\Factory\Common\all.h"
#include "..\all.h"

#define PAIR "USDJPY"

static void Test01(void)
{
	cout("%I64u\n", TimeData2Stamp(TSec2TimeData(FxTime2TSec(0x00000000))));
	cout("%I64u\n", TimeData2Stamp(TSec2TimeData(FxTime2TSec(0xffffffff))));
}
static void Test02(void)
{
	uint64 bgnFxTime = TSec2FxTime(TimeData2TSec(Stamp2TimeData(20160728000000)));
	uint64 endFxTime = TSec2FxTime(TimeData2TSec(Stamp2TimeData(20160803000000)));
	uint64 fxTime;

	for(fxTime = bgnFxTime; fxTime <= endFxTime; fxTime += 2)
	{
		cout("%I64u,%I64u,%I64u\n"
			,TimeData2Stamp(TSec2TimeData(FxTime2TSec(fxTime)))
			,FxTime2TSec(fxTime)
			,fxTime
			);
	}
}

int main(int argc, char **argv)
{
//	Test01();
	Test02();
}
