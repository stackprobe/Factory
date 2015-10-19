#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Random.h"
#include "..\all.h"

static double AsmAsk;
static double AsmBid;
static int AsmLossFlag;

static double RetAsk;
static double RetBid;
static int RetLossFlag;

static void GetAssumePrice(uint64 sec)
{
	TimeData_t td = Sec2TimeData(sec);
	char *file;
	char *stamp;
	autoList_t *csv;
	autoList_t *row;
	uint rowidx;

	AsmAsk = 999.9;
	AsmBid = 999.9;
	AsmLossFlag = 1;

	file = xcout("C:\\tmp\\Fx\\%04u%02u%02u_USDJPY.csv", td.Y, td.M, td.D);

	if(existFile(file))
	{
		stamp = xcout("%04u%02u%02u%02u%02u%02u", td.Y, td.M, td.D, td.H, td.I, td.S);
		csv = readCSVFileTR(file);

		foreach(csv, row, rowidx)
		{
			char *row_stamp = getLine(row, 0);

			if(!strcmp(row_stamp, stamp))
			{
				char *row_ask = getLine(row, 1);
				char *row_bid = getLine(row, 2);

				AsmAsk = atof(row_ask);
				AsmBid = atof(row_bid);
				AsmLossFlag = 0;

				break;
			}
		}
		releaseDim(csv, 2);
	}
	memFree(file);
}
static void GetPrice(uint64 sec)
{
	RetAsk = Fx_GetAsk(sec);
	RetBid = Fx_GetBid(sec);
	RetLossFlag = Fx_GetLossFlag(sec);
}
static void DoTest(uint64 sec)
{
	cout("----\n");

	{
		TimeData_t td = Sec2TimeData(sec);

		cout("%04u/%02u/%02u %02u:%02u:%02u\n", td.Y, td.M, td.D, td.H, td.I, td.S);
	}

	LOGPOS();
	GetAssumePrice(sec);
	LOGPOS();
	GetPrice(sec);
	LOGPOS();

	cout("AsmAsk: %f\n", AsmAsk);
	cout("AsmBid: %f\n", AsmBid);
	cout("AsmLossFlag: %d\n", AsmLossFlag);

	cout("RetAsk: %f\n", RetAsk);
	cout("RetBid: %f\n", RetBid);
	cout("RetLossFlag: %d\n", RetLossFlag);

	if(AsmLossFlag)
	{
		errorCase(!RetLossFlag);
	}
	else
	{
		errorCase(RetAsk != AsmAsk); // double ÇæÇØÇ«ÅAëΩï™çáÇ§ÇæÇÎ...
		errorCase(RetBid != AsmBid);
		errorCase(RetLossFlag);
	}
}
static void TestCase_01(void)
{
	uint64 minSec = TimeData2Sec(Res2TimeData("201507"));
	uint64 maxSec = TimeData2Sec(Res2TimeData("20151031"));
	uint64 sec;

	while(waitKey(0) != 0x1b)
	{
		sec = mt19937_range64(minSec, maxSec);
		sec &= ~(uint64)1; // to_ãÙêî

		DoTest(sec);
	}
}
static void TestCase_02(void)
{
	uint64 minSec = TimeData2Sec(Res2TimeData("201507"));
	uint64 maxSec = TimeData2Sec(Res2TimeData("201510"));

	while(waitKey(0) != 0x1b)
	{
		uint64 bgnSec = mt19937_range64(minSec, maxSec) & ~(uint64)1; // to_ãÙêî
		uint64 endSec;
		uint64 sec;

		endSec = bgnSec + mt19937_rnd(1000) * 2;

		for(sec = bgnSec; sec <= endSec; sec += 2)
		{
			DoTest(sec);
		}
	}
}
int main(int argc, char **argv)
{
	if(argIs("/2"))
	{
		TestCase_02();
		return;
	}
	TestCase_01();
}
