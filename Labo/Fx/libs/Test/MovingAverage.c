#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\TimeData.h"
#include "..\MovingAverage.h"

int main(int argc, char **argv)
{
	uint64 sec = GetNowSec();
	uint64 fxTime;
	TimeData_t td;
	FxMA_t *i;

	sec &= ~(uint64)1;
	fxTime = Sec2FxTime(sec);
	i = CreateFxMA(fxTime, hasArgs(1) ? toValue(nextArg()) : 86400 * 7);

	for(; ; )
	{
		sec = FxTime2Sec(fxTime);
		td = Sec2TimeData(sec);
		FxMA_SetFxTime(i, fxTime);

		cout("%04u/%02u/%02u %02u:%02u:%02u %f %f %f %f\n"
			,td.Y
			,td.M
			,td.D
			,td.H
			,td.I
			,td.S
			,Fx_GetAsk(sec)
			,Fx_GetBid(sec)
			,FxMA_GetAsk(i)
			,FxMA_GetBid(i)
			);

		switch(getKey())
		{
		case '-':
			fxTime -= 2;
			break;

		case '+':
			fxTime += 2;
			break;

		case '/':
			fxTime -= 60;
			break;

		case '*':
			fxTime += 60;
			break;

		case 0x1f49: // Page Up
			fxTime -= 3600;
			break;

		case 0x1f51: // Page Down
			fxTime += 3600;
			break;

		case 0x1f47: // Home
			fxTime -= 86400;
			break;

		case 0x1f4f: // End
			fxTime += 86400;
			break;

		case 0x1b: // Escape
			termination(0);
		}
	}
}
