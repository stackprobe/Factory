/*
	Period.exe ID (/R MIN-SEC MAX-SEC | /T SEC)
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRRandom.h"

static char *S_Id;

// ---- time table ----

#define TIME_TABLE_FILE "C:\\Factory\\tmp\\TimeTable.dat"

static autoList_t *TTbl;
static uint TTPos;

static void ReadTimeTable(void)
{
	autoList_t *lines;
	uint index;

	if(existFile(TIME_TABLE_FILE))
		lines = readLines(TIME_TABLE_FILE);
	else
		lines = newList();

	for(index = 0; index < getCount(lines); index += 2)
	{
		char *id = getLine(lines, index);

		if(!strcmp(id, S_Id))
			break;
	}
	if(TTbl)
		releaseDim(TTbl, 1);

	TTbl = lines;
	TTPos = index;
}
static void SaveTimeTable(void)
{
	writeLines(TIME_TABLE_FILE, TTbl);
}

static time_t GetTime(void)
{
	ReadTimeTable();

	if(TTPos < getCount(TTbl)) // あった。
	{
		char *st = getLine(TTbl, TTPos + 1);
		time_t time;

		time = (time_t)toValue64(st);

		return time;
	}
	return -1; // 無かった。
}
static void SetTime(time_t time)
{
	ReadTimeTable();

	if(TTPos < getCount(TTbl)) // 既存
	{
		memFree(zSetLine(TTbl, TTPos + 1, xcout("%I64d", time)));
	}
	else // 新規
	{
		addElement(TTbl, (uint)strx(S_Id));
		addElement(TTbl, (uint)xcout("%I64d", time));
	}
	SaveTimeTable();
}

// ----

static void CheckTime(void)
{
	time_t nextTime = GetTime();
	time_t currTime = time(NULL);

	cout("次回時刻：%f\n", nextTime / 3600.0);
	cout("現在時刻：%f\n", currTime / 3600.0);

	if(currTime < nextTime)
	{
		cout("次の時刻まであと %f 時間あります。endCode=1\n", (nextTime - currTime) / 3600.0);

		termination(1); // 時間まだ
	}
}
static void UpdateTime(time_t addTime)
{
	time_t nextTime = time(NULL) + addTime;

	cout("時間キタ！次の時刻まで %f 時間です。endCode=0\n", addTime / 3600.0);
	cout("次回時刻：%f\n", nextTime / 3600.0);

	SetTime(nextTime);

	termination(0); // 時間キタ
}

// ---- update ----

static void Update_Range(uint minSec, uint maxSec)
{
	errorCase(maxSec < minSec);

	UpdateTime(mt19937_range(minSec, maxSec));
}
static void Update_RangeHour(uint minHour, uint maxHour)
{
	errorCase(maxHour < minHour);

	UpdateTime((time_t)mt19937_range(minHour, maxHour) * 3600);
}
static void Update_Time(uint sec)
{
	UpdateTime(sec);
}
static void Update_TimeHour(uint hour)
{
	UpdateTime((time_t)hour * 3600);
}

// ----

int main(int argc, char **argv)
{
	mt19937_initCRnd();

	S_Id = nextArg();

	errorCase(m_isEmpty(S_Id));

	CheckTime();

	if(argIs("/R"))
	{
		uint minSec;
		uint maxSec;

		minSec = toValue(nextArg());
		maxSec = toValue(nextArg());

		Update_Range(minSec, maxSec);
	}
	if(argIs("/RH"))
	{
		uint minHour;
		uint maxHour;

		minHour = toValue(nextArg());
		maxHour = toValue(nextArg());

		Update_RangeHour(minHour, maxHour);
	}
	if(argIs("/T"))
	{
		uint sec = toValue(nextArg());

		Update_Time(sec);
	}
	if(argIs("/TH"))
	{
		uint hour = toValue(nextArg());

		Update_TimeHour(hour);
	}
}
