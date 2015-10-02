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

	td.S = (uint)(sec % 60);
	sec /= 60;
	td.I = (uint)(sec % 60);
	sec /= 60;
	td.H = (uint)(sec % 24);
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

	td.S = (uint)(stamp % 100);
	stamp /= 100;
	td.I = (uint)(stamp % 100);
	stamp /= 100;
	td.H = (uint)(stamp % 100);
	stamp /= 100;
	td.D = (uint)(stamp % 100);
	stamp /= 100;
	td.M = (uint)(stamp % 100);
	td.Y = (uint)(stamp / 100);

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
