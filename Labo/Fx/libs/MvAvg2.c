#include "all.h"

MvAvg_t *CreateMvAvg(uint64 fxTime, uint64 span, char *pair)
{
	MvAvg_t *i = nb(MvAvg_t);

	errorCase(fxTime & 1);
	errorCase(!span);
	errorCase(span & 1);
	errorCase(fxTime < span);

	i->Span = span;
	i->Pair = strx(pair);
	i->CurrFxTime = 0;
//	i->CurrTotal = 0.0;

	return i;
}
void ReleaseMvAvg(MvAvg_t *i)
{
	if(!i)
		return;

	memFree(i->Pair);
	memFree(i);
}

// <-- cdtor

static void DoShift(MvAvg_t *i, uint64 fxTime)
{
	if(i->CurrFxTime)
	if(fxTime < i->CurrFxTime - i->Span / 2 || i->CurrFxTime < fxTime - i->Span / 2)
		i->CurrFxTime = 0;

	if(i->CurrFxTime)
	{
		while(i->CurrFxTime < fxTime)
		{
			i->CurrFxTime += 2;
			i->CurrTotal -= GetMid(GetPrice(i->CurrFxTime - i->Span, i->Pair));
			i->CurrTotal += GetMid(GetPrice(i->CurrFxTime, i->Pair));
		}
		while(fxTime < i->CurrFxTime)
		{
			i->CurrTotal -= GetMid(GetPrice(i->CurrFxTime, i->Pair));
			i->CurrTotal += GetMid(GetPrice(i->CurrFxTime - i->Span, i->Pair));
			i->CurrFxTime -= 2;
		}
	}
	else
	{
		uint count;

		i->CurrFxTime = fxTime;
		i->CurrTotal = 0.0;

		for(count = 0; count < i->Span; count += 2)
		{
			i->CurrTotal += GetMid(GetPrice(fxTime - count, i->Pair));
		}
	}
}

double MA_GetMid(MvAvg_t *i, uint64 fxTime)
{
	errorCase(fxTime & 1);
	errorCase(fxTime < i->Span);

	DoShift(i, fxTime);

	return i->CurrTotal / (i->Span / 2);
}

// <-- accessor
