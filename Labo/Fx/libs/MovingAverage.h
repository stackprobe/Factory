#pragma once

#include "C:\Factory\Common\all.h"
#include "Chart.h"
#include "FxTime.h"

typedef struct FxMA_st
{
	uint64 FxTime;
	uint64 Span;
	uint64 TotalIAsk;
	uint64 TotalIBid;
}
FxMA_t;

FxMA_t *CreateFxMA(uint64 fxTime, uint64 span);
void ReleaseFxMA(FxMA_t *i);

void FxMA_SetFxTime(FxMA_t *i, uint64 fxTime);
double FxMA_GetAsk(FxMA_t *i);
double FxMA_GetBid(FxMA_t *i);
