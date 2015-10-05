#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Random.h"
#include "..\Chart.h"

static void DoTest_01(void)
{
	uint64 minSec = TimeData2Sec(Res2TimeData("201507"));
	uint64 maxSec = TimeData2Sec(Res2TimeData("201511"));
	uint64 sec;

	for(; ; )
	{
		if(hasKey() && getKey() == 0x1b)
			break;

		sec = mt19937_range64(minSec, maxSec);

		// todo
	}
}
int main(int argc, char **argv)
{
	DoTest_01();
}
