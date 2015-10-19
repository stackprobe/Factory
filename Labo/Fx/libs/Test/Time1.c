#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\TimeData.h"
#include "..\all.h"

int main(int argc, char **argv)
{
	cout("%I64u\n", TimeData2Sec(Stamp2TimeData(99991231235958))); // 10000/1/1 00:00:00 - 2s
	cout("%I64u\n", Sec2FxTime(TimeData2Sec(Stamp2TimeData(99991231235958)))); // 10000/1/1 00:00:00 - 2s
}
