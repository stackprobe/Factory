#include "MovingAverage.h"

#define IDENOM 1000000

static uint LastAsk;
static uint LastBid;

static void GetIPrice(uint64 sec)
{
	LastAsk = d2i(Fx_GetAsk(sec) * IDENOM);
	LastBid = d2i(Fx_GetBid(sec) * IDENOM);
}

/*
	(fxTime - span) Å` fxTime ÇÃïΩãœ
*/
FxMA_t *CreateFxMA(uint64 fxTime, uint64 span)
{
	FxMA_t *i = nb(FxMA_t);

	errorCase(fxTime & 1 != 0); // ? äÔêî
	errorCase(span & 1 != 0); // ? äÔêî
	errorCase(fxTime < span);

	i->FxTime = fxTime;
	i->Span = span;
//	i->TotalIAsk = 0;
//	i->TotalIBid = 0;

	{
		uint64 count;

		for(count = 0; count <= span; count += 2)
		{
			GetIPrice(FxTime2Sec(fxTime - count));

			i->TotalIAsk += LastAsk;
			i->TotalIBid += LastBid;
		}
	}
//cout("%I64u\n", i->TotalIAsk); // test
//cout("%I64u\n", i->TotalIBid); // test

	return i;
}
void ReleaseFxMA(FxMA_t *i)
{
	errorCase(!i);

	memFree(i);
}

void FxMA_SetFxTime(FxMA_t *i, uint64 fxTime)
{
	errorCase(!i);
	errorCase(fxTime & 1 != 0); // ? äÔêî

	while(i->FxTime < fxTime)
	{
		GetIPrice(FxTime2Sec(i->FxTime - i->Span));

		i->TotalIAsk -= LastAsk;
		i->TotalIBid -= LastBid;

		i->FxTime++;

		GetIPrice(FxTime2Sec(i->FxTime));

		i->TotalIAsk += LastAsk;
		i->TotalIBid += LastBid;
	}
	while(fxTime < i->FxTime)
	{
		GetIPrice(FxTime2Sec(i->FxTime));

		i->TotalIAsk -= LastAsk;
		i->TotalIBid -= LastBid;

		i->FxTime--;

		GetIPrice(FxTime2Sec(i->FxTime - i->Span));

		i->TotalIAsk += LastAsk;
		i->TotalIBid += LastBid;
	}
}
double FxMA_GetAsk(FxMA_t *i)
{
	errorCase(!i);

	return (double)(i->TotalIAsk / (i->Span / 2 + 1)) / IDENOM;
}
double FxMA_GetBid(FxMA_t *i)
{
	errorCase(!i);

	return (double)(i->TotalIBid / (i->Span / 2 + 1)) / IDENOM;
}
