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
	uint64 tmp;
	uint y;
	uint m = 1;
	uint d;

	day %= 146097;

	tmp = (day + 306) / 36524;
	m_minim(tmp, 3);
	day += tmp;

	y64 += (day / 1461) * 4;
	day %= 1461;

	tmp = (day + 306) / 365;
	m_minim(tmp, 3);
	day += tmp;

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

#define FILE_STAMP_Y_MAX 1844674

uint64 FileStampToMillis(uint64 stamp)
{
	uint y;
	uint m;
	uint d;
	uint h;
	uint i;
	uint s;
	uint l;
	uint64 ret;

	l = stamp % 1000; stamp /= 1000;
	s = stamp % 100; stamp /= 100;
	i = stamp % 100; stamp /= 100;
	h = stamp % 100; stamp /= 100;
	d = stamp % 100; stamp /= 100;
	m = stamp % 100; stamp /= 100;
	y = stamp;

	m_range(y, 1, FILE_STAMP_Y_MAX);
	m_range(m, 1, 12);
	m_range(d, 1, 31);
	m_range(h, 0, 23);
	m_range(i, 0, 59);
	m_range(s, 0, 59);
	m_range(l, 0, 999);

	ret = Date2Day(y, m, d);
	ret *= 24;
	ret += h;
	ret *= 60;
	ret += i;
	ret *= 60;
	ret += s;
	ret *= 1000;
	ret += l;

	return ret;
}
uint64 MillisToFileStamp(uint64 millis)
{
	uint y;
	uint m;
	uint d;
	uint h;
	uint i;
	uint s;
	uint l;
	uint64 ret;

	l = millis % 1000; millis /= 1000;
	s = millis % 60; millis /= 60;
	i = millis % 60; millis /= 60;
	h = millis % 24; millis /= 24;

	Day2Date(millis, &y, &m, &d);

	m_range(y, 1, FILE_STAMP_Y_MAX);
//	m_range(m, 1, 12);
//	m_range(d, 1, 31);
//	m_range(h, 0, 23);
//	m_range(i, 0, 59);
//	m_range(s, 0, 59);
//	m_range(l, 0, 999);

	ret = y;
	ret *=  100; ret += m;
	ret *=  100; ret += d;
	ret *=  100; ret += h;
	ret *=  100; ret += i;
	ret *=  100; ret += s;
	ret *= 1000; ret += l;

	return ret;
}
int IsFairFileStamp(uint64 stamp)
{
	return stamp == MillisToFileStamp(FileStampToMillis(stamp));
}
/*
	DateToDay.java
*/
uint IDate2Day(uint date)
{
	uint y;
	uint m;
	uint d;

	d = date % 100;
	date /= 100;
	m = date % 100;
	date /= 100;
	y = date;

	if(y < 1000 || 9999 < y ||
		m < 1 || 12 < m ||
		d < 1 || 31 < d
		)
		return 0; // dummy day

	return Date2Day(y, m, d);
}
uint Day2IDate(uint day)
{
	uint y;
	uint m;
	uint d;

	Day2Date(day, &y, &m, &d);

	if(y < 1000 || 9999 < y ||
		m < 1 || 12 < m ||
		d < 1 || 31 < d
		)
		return 10000101; // dummy date

	return y * 10000 + m * 100 + d;
}
