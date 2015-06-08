/*
	全てグレゴリオ暦で計算する。

	1/1/1 は月曜日
*/

#include "Date2Day.h"

/*
	西暦1年1月1日から何日経過したかを返す。
	1/1/1 のとき 0 を返す。
	1/1/1 から 2^32-1/2^32-1/2^32-1 まで対応。
*/
uint64 Date2Day(uint y, uint m, uint d)
{
	uint64 y64;
	uint64 day;

	// ? 対応出来ない日付
	if(y < 1 || m < 1 || d < 1)
		return 0;

	y64 = y;

	// 13月以上 -> 12月以下
	{
		m--;
		y64 += m / 12;
		m %= 12;
		m++;
	}

	if(m <= 2)
		y64--;

	day = y64 / 400;
	day *= 365 * 400 + 97;

	y = (uint)(y64 % 400);

	day += y * 365;
	day += y / 4;
	day -= y / 100;

	if(2 < m)
	{
		day -= 31 * 10 - 4;
		m -= 3;
		day += (m / 5) * (31 * 5 - 2);
		m %= 5;
		day += (m / 2) * (31 * 2 - 1);
		m %= 2;
		day += m * 31;
	}
	else
		day += (m - 1) * 31;

	day += d - 1;
	return day;
}
void Day2Date(uint64 day, uint *py, uint *pm, uint *pd)
{
	uint64 y64 = (day / 146097) * 400 + 1;
	uint y;
	uint m = 1;
	uint d;

	day %= 146097;

	     if(109266 <= day) day += 3;
	else if( 72742 <= day) day += 2;
	else if( 36218 <= day) day += 1;

	y64 += (day / 1461) * 4;
	day %= 1461;

	     if(789 <= day) day += 3;
	else if(424 <= day) day += 2;
	else if( 59 <= day) day += 1;

	y64 += day / 366;
	day %= 366;

	if(60 <= day)
	{
		m += 2;
		day -= 60;
		m += (day / 153) * 5;
		day %= 153;
		m += (day / 61) * 2;
		day %= 61;
	}
	m += day / 31;
	day %= 31;

	if(UINTMAX < y64) // overflow
	{
		y = 0xffffffff;
		m = 12;
		d = 31;
	}
	else
	{
		y = (uint)y64;
		d = (uint)day + 1;
	}

	// 2bs
	errorCase(
		y < 1 ||
		m < 1 || 12 < m ||
		d < 1 || 31 < m
		);

	if(py) *py = y;
	if(pm) *pm = m;
	if(pd) *pd = d;
}
int CheckDate(uint y, uint m, uint d)
{
	uint ry;
	uint rm;
	uint rd;

	Day2Date(Date2Day(y, m, d), &ry, &rm, &rd);

	return y == ry && m == rm && d == rd;
}
uint GetEndOfMonth(uint y, uint m)
{
#if 1
	uint d;

	Day2Date(Date2Day(y, m, 32), NULL, NULL, &d);
	return 32 - d;
#else // same, old
	uint d;

	for(d = 31; 28 < d ; d--)
		if(CheckDate(y, m, d))
			break;

	return d;
#endif
}
char *Date2JWeekday(uint y, uint m, uint d)
{
#if 1
	return getJWeekDay((Date2Day(y, m, d) + 1) % 7);
#else // same, old
	char *weekdays[] =
	{
		"月", "火", "水", "木", "金", "土", "日",
	};

	return weekdays[Date2Day(y, m, d) % 7];
#endif
}
