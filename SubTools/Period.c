/*
	Period.exe ID (/CTT | /C | /R MIN-SEC MAX-SEC | /RH MIN-HOUR MAX-HOUR | /T SEC | /TH HOUR)

		ID ... 大文字小文字を区別する。
			/CTT を指定する場合は、使用しない。適当な文字列をセットすること。
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
static void ClearTime(void)
{
	ReadTimeTable();

	if(TTPos < getCount(TTbl)) // あった。
	{
		memFree(getLine(TTbl, TTPos + 0));
		memFree(getLine(TTbl, TTPos + 1));

		setElement(TTbl, TTPos + 0, 0);
		setElement(TTbl, TTPos + 1, 0);

		removeZero(TTbl);

		SaveTimeTable();
	}
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

	if(argIs("/CTT"))
	{
		cout("Clear Time Table\n");
		removeFileIfExist(TIME_TABLE_FILE);
		return;
	}
	if(argIs("/C"))
	{
		cout("Clear\n");
		ClearTime();
		return;
	}

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
