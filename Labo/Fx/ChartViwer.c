#include "libs\all.h"

#define Y_SIZE 20

static uint64 CurrFxTime; // 0 Å` FXTIME_MAX
static uint CountPerY;    // 1 Å`

static FxMA_t *Ma12h;
static FxMA_t *Ma1d;
static FxMA_t *Ma5d;
static FxMA_t *Ma25d;

static void DispChart(void)
{
	uint64 fxTime = CurrFxTime;
	uint y;
	uint count;
	autoList_t *lines = newList();

	for(y = 0; y < Y_SIZE; y++)
	{
		autoBlock_t *buff = newBlock();
		double sum = 0.0;
		double avg;
		double ma12hSum = 0.0;
		double ma1dSum  = 0.0;
		double ma5dSum  = 0.0;
		double ma25dSum = 0.0;
		double ma12hAvg;
		double ma1dAvg;
		double ma5dAvg;
		double ma25dAvg;

		ab_addLine_x(buff, xcout("%I64u", TimeData2Stamp(Sec2TimeData(FxTime2Sec(fxTime)))));

		for(count = 0; count < CountPerY; count++)
		{
			sum += Fx_GetBid(FxTime2Sec(fxTime));

			FxMA_SetFxTime(Ma12h, fxTime);
			FxMA_SetFxTime(Ma1d,  fxTime);
			FxMA_SetFxTime(Ma5d,  fxTime);
			FxMA_SetFxTime(Ma25d, fxTime);

			ma12hSum += FxMA_GetBid(Ma12h);
			ma1dSum  += FxMA_GetBid(Ma1d);
			ma5dSum  += FxMA_GetBid(Ma5d);
			ma25dSum += FxMA_GetBid(Ma25d);

			fxTime += 2; // fixme: FXTIME_MAX í¥Ç¶ÇÁÇÍÇÈÇØÇ«ÅAÇ‹ÇüÇ¢Ç¢Ç‚..
		}
		avg = sum / CountPerY;

		ab_addLine_x(buff, xcout(" %3.3f", avg));

		ma12hAvg = ma12hSum / CountPerY;
		ma1dAvg  = ma1dSum  / CountPerY;
		ma5dAvg  = ma5dSum  / CountPerY;
		ma25dAvg = ma25dSum / CountPerY;

		ab_addLine_x(buff, xcout(" %3.3f", ma12hAvg));
		ab_addLine_x(buff, xcout(" %3.3f", ma1dAvg));
		ab_addLine_x(buff, xcout(" %3.3f", ma5dAvg));
		ab_addLine_x(buff, xcout(" %3.3f", ma25dAvg));

		addElement(lines, (uint)unbindBlock2Line(buff));
	}

	{
		char *line;
		uint line_index;

		cout("----\n");

		foreach(lines, line, line_index)
			cout("%s\n", line);

		cout("----\n");
	}

	releaseDim(lines, 1);
}
static void Main2(void)
{
	int key;

	CurrFxTime = (Sec2FxTime(GetNowSec()) / 2) * 2;
	CountPerY = 1;

	Ma12h = CreateFxMA(CurrFxTime, 43200);
	Ma1d  = CreateFxMA(CurrFxTime, 86400);
	Ma5d  = CreateFxMA(CurrFxTime, 86400 * 5);
	Ma25d = CreateFxMA(CurrFxTime, 86400 * 25);

	for(; ; )
	{
		uint64 mvFxTimeSpan = CountPerY * 2;

		DispChart();

		cout("stdn ");
		key = getKey();
		cout("%02x\n", key);

		if(key == 0x1b)
			break;

		if(key == 0x1f48) // è„
		{
			if(mvFxTimeSpan <= CurrFxTime)
				CurrFxTime -= mvFxTimeSpan;
		}
		else if(key == 0x1f50) // â∫
		{
			if(CurrFxTime < FXTIME_MAX)
				CurrFxTime += mvFxTimeSpan;
		}
		else if(key == '+')
		{
			if(CountPerY < 43200)
				CountPerY++;
		}
		else if(key == '-')
		{
			if(CountPerY)
				CountPerY--;
		}
		else if(key == 's')
		{
			CountPerY = 1;
		}
		else if(key == 'm')
		{
			CountPerY = 30;
		}
		else if(key == 'h')
		{
			CountPerY = 1800;
		}
		else if(key == 'd')
		{
			CountPerY = 43200;
		}
	}
	ReleaseFxMA(Ma12h);
	ReleaseFxMA(Ma1d);
	ReleaseFxMA(Ma5d);
	ReleaseFxMA(Ma25d);
}
int main(int argc, char **argv)
{
	Main2();
}
