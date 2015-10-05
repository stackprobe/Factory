#include "TimeData.h"

TimeData_t GetTimeData(uint y, uint m, uint d, uint h, uint i, uint s)
{
	TimeData_t td;

	td.Y = y;
	td.M = m;
	td.D = d;
	td.W = Date2Day(y, m, d) % 7;
	td.H = h;
	td.I = i;
	td.S = s;

	return td;
}

TimeData_t Sec2TimeData(uint64 sec)
{
	TimeData_t td;

	td.S = sec % 60;
	sec /= 60;
	td.I = sec % 60;
	sec /= 60;
	td.H = sec % 24;
	sec /= 24;

	Day2Date(sec, &td.Y, &td.M, &td.D);

	td.W = (uint)(sec % 7);

	return td;
}
uint64 TimeData2Sec(TimeData_t td)
{
	uint64 sec = Date2Day(td.Y, td.M, td.D);

	sec *= 24;
	sec += td.H;
	sec *= 60;
	sec += td.I;
	sec *= 60;
	sec += td.S;

	return sec;
}

TimeData_t Stamp2TimeData(uint64 stamp)
{
	TimeData_t td;

	td.S = stamp % 100;
	stamp /= 100;
	td.I = stamp % 100;
	stamp /= 100;
	td.H = stamp % 100;
	stamp /= 100;
	td.D = stamp % 100;
	stamp /= 100;
	td.M = stamp % 100;
	td.Y = stamp / 100;

	td.W = Date2Day(td.Y, td.M, td.D) % 7;

	return td;
}
uint64 TimeData2Stamp(TimeData_t td)
{
	uint64 stamp = td.Y;

	stamp *= 100;
	stamp += td.M;
	stamp *= 100;
	stamp += td.D;
	stamp *= 100;
	stamp += td.H;
	stamp *= 100;
	stamp += td.I;
	stamp *= 100;
	stamp += td.S;

	return stamp;
}

TimeData_t GetNowTimeData(void)
{
	stampData_t *sd = getStampDataTime(0);
	TimeData_t td;

	td.Y = sd->year;
	td.M = sd->month;
	td.D = sd->day;
	td.W = (sd->weekday + 6) % 7;
	td.H = sd->hour;
	td.I = sd->minute;
	td.S = sd->second;

	return td;
}
uint64 GetNowSec(void)
{
	return TimeData2Sec(GetNowTimeData());
}

TimeData_t ResStamp2TimeData(uint64 prmStamp)
{
	uint64 stamp = prmStamp;
	TimeData_t td;

	if(stamp <= 1231) // ? MMDD
	{
		stamp += GetNowTimeData().Y * 10000;
		stamp *= 1000000;
	}
	else if(stamp < 10000) // ? YYYY
	{
		stamp *= 10000;
		stamp += 101;
		stamp *= 1000000;
	}
	else if(stamp < 1000000) // ? YYYYMM
	{
		stamp *= 100;
		stamp += 1;
		stamp *= 1000000;
	}
	else if(stamp < 100000000) // ? YYYYMMDD
	{
		stamp *= 1000000;
	}
	else if(stamp < 10000000000) // ? YYYYMMDDHH
	{
		stamp *= 10000;
	}
	else if(stamp < 10000000000) // ? YYYYMMDDHHMM
	{
		stamp *= 100;
	}

	td = Stamp2TimeData(stamp);
	td = Sec2TimeData(TimeData2Sec(td)); // fltr

	cout("%I64u -> %04u/%02u/%02u %02u:%02u:%02u\n", prmStamp, td.Y, td.M, td.D, td.H, td.I, td.S); // debug-out

	return td;
}
TimeData_t Res2TimeData(char *prm)
{
	return ResStamp2TimeData(toValue64(prm));
}
TimeData_t Res2TimeData_x(char *prm)
{
	TimeData_t out = Res2TimeData(prm);
	memFree(prm);
	return out;
}
