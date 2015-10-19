#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\TimeData.h"
#include "..\all.h"

int main(int argc, char **argv)
{
	uint64 sec = GetNowSec();
	uint64 fxTime;
	TimeData_t td;

	fxTime = Sec2FxTime(sec);

	for(; ; )
	{
		sec = FxTime2Sec(fxTime);
		td = Sec2TimeData(sec);

		cout("%I64u %I64u %04u/%02u/%02u %02u:%02u:%02u\n", fxTime, sec, td.Y, td.M, td.D, td.H, td.I, td.S);

		switch(getKey())
		{
		case '-':
			fxTime--;
			break;

		case '+':
			fxTime++;
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
