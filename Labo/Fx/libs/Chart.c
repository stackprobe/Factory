#include "all.h"

#define DATA_DIR "C:\\tmp\\Fx"
#define CURR_PAIR "USDJPY"
#define DEF_PRICE 120.0
#define DAY_MAX 50

typedef struct Price_st
{
	double Ask;
	double Bid;
	int LossFlag;
}
Price_t;

typedef struct DayPrice_st
{
	Price_t Prices[43200];
	int Loaded;
}
DayPrice_t;

static DayPrice_t DayPrices[DAY_MAX];
static uint BasePos;
static uint64 BaseDay;

// ---- IsLossBegin ----

static uint ILB_Bgn;
static uint ILB_End;

static int IsLossBegin(DayPrice_t *i, uint pr_index)
{
	uint index;

	for(index = pr_index; index < 43200; index++)
		if(i->Prices[index].LossFlag == 0)
			break;

	if(pr_index < index)
	{
		ILB_Bgn = pr_index;
		ILB_End = index - 1;
		return 1;
	}
	return 0;
}
static void ILB_HotenLossRange(DayPrice_t *i)
{
	uint index;

	if(ILB_Bgn == 0) // この日の先頭に接触
	{
		if(ILB_End == 43200 - 1) // この日が欠落 -> デフォ値のまま
			return;

		for(index = 0; index <= ILB_End; index++)
		{
			i->Prices[index].Ask = i->Prices[ILB_End + 1].Ask;
			i->Prices[index].Bid = i->Prices[ILB_End + 1].Bid;
		}
		return;
	}
	else if(ILB_End == 43200 - 1) // この日の終端に接触
	{
		for(index = ILB_Bgn; index < 43200; index++)
		{
			i->Prices[index].Ask = i->Prices[ILB_Bgn - 1].Ask;
			i->Prices[index].Bid = i->Prices[ILB_Bgn - 1].Bid;
		}
		return;
	}
	else
	{
		uint denom = ILB_End - ILB_Bgn + 3;

		for(index = ILB_Bgn; index <= ILB_End; index++)
		{
			uint numer = index - ILB_Bgn + 1;

			i->Prices[index].Ask = i->Prices[ILB_Bgn - 1].Ask + (i->Prices[ILB_End + 1].Ask - i->Prices[ILB_Bgn - 1].Ask) * numer / denom;
			i->Prices[index].Bid = i->Prices[ILB_Bgn - 1].Bid + (i->Prices[ILB_End + 1].Bid - i->Prices[ILB_Bgn - 1].Bid) * numer / denom;
		}
	}
}

// ----

static void LoadDayPrice(DayPrice_t *i, uint64 day)
{
	char *file;
	uint pr_index;

	LOGPOS();

	for(pr_index = 0; pr_index < 43200; pr_index++) // 初期化 -- この日が欠落していた場合の値で初期化
	{
		i->Prices[pr_index].Ask = DEF_PRICE;
		i->Prices[pr_index].Bid = DEF_PRICE;
		i->Prices[pr_index].LossFlag = 1;
	}
	i->Loaded = 1;

	{
		uint y;
		uint m;
		uint d;

		Day2Date(day, &y, &m, &d);

		file = combine_cx(DATA_DIR, xcout("%04u%02u%02u_%s.csv", y, m, d, CURR_PAIR));
	}

	if(existFile(file))
	{
		autoList_t *csv = readCSVFileTR(file);
		autoList_t *row;
		uint rowidx;

		foreach(csv, row, rowidx)
		{
			uint64 stamp = toValue64(getLine(row, 0));
			double ask = atof(getLine(row, 1));
			double bid = atof(getLine(row, 2));
			TimeData_t td;
			uint64 sec;

			td = Stamp2TimeData(stamp);
			sec = TimeData2Sec(td);
			pr_index = (uint)(sec % 86400) / 2;

			i->Prices[pr_index].Ask = ask;
			i->Prices[pr_index].Bid = bid;
			i->Prices[pr_index].LossFlag = 0;
		}
		releaseDim(csv, 2);

#if 1
		for(pr_index = 0; pr_index < 43200; pr_index++)
		{
			if(IsLossBegin(i, pr_index))
			{
				ILB_HotenLossRange(i);
				pr_index = ILB_End;
			}
		}
#else // old
		// 欠落を、直前の有効な相場で補完
		{
			double lastAsk = DEF_PRICE;
			double lastBid = DEF_PRICE;

			for(pr_index = 0; pr_index < 43200; pr_index++)
			{
				if(!i->Prices[pr_index].LossFlag)
				{
					lastAsk = i->Prices[pr_index].Ask;
					lastBid = i->Prices[pr_index].Bid;
					break;
				}
			}
			for(pr_index = 0; pr_index < 43200; pr_index++)
			{
				if(i->Prices[pr_index].LossFlag)
				{
					i->Prices[pr_index].Ask = lastAsk;
					i->Prices[pr_index].Bid = lastBid;
				}
				else
				{
					lastAsk = i->Prices[pr_index].Ask;
					lastBid = i->Prices[pr_index].Bid;
				}
			}
		}
#endif
	}
	memFree(file);
}
static Price_t *GetPrice(uint64 sec)
{
	uint64 day = sec / 86400;
	uint pr_index = (uint)(sec % 86400) / 2;
	uint index;
	uint day_index;

	if(day + DAY_MAX < BaseDay || BaseDay + DAY_MAX * 2 <= day)
	{
		for(index = 0; index < DAY_MAX; index++)
			DayPrices[index].Loaded = 0;

		BasePos = 0;
		BaseDay = day;
	}
	else if(day < BaseDay)
	{
		while(day < BaseDay)
		{
			BasePos += DAY_MAX - 1;
			BasePos %= DAY_MAX;
			BaseDay--;

			DayPrices[BasePos].Loaded = 0;
		}
	}
	else
	{
		while(BaseDay + DAY_MAX <= day)
		{
			DayPrices[BasePos].Loaded = 0;

			BasePos++;
			BasePos %= DAY_MAX;
			BaseDay++;
		}
	}
	day_index = (uint)(day - BaseDay);

	errorCase(DAY_MAX <= day_index); // 2bs

	day_index += BasePos;
	day_index %= DAY_MAX;

	if(!DayPrices[day_index].Loaded)
		LoadDayPrice(DayPrices + day_index, day);

	return DayPrices[day_index].Prices + pr_index;
}
double Fx_GetAsk(uint64 sec)
{
	return GetPrice(sec)->Ask;
}
double Fx_GetBid(uint64 sec)
{
	return GetPrice(sec)->Bid;
}
int Fx_GetLossFlag(uint64 sec)
{
	return GetPrice(sec)->LossFlag;
}
