#include "all.h"

// ---- cdtor ----

Price_t *CreatePrice(double bid, double ask)
{
	Price_t *i = (Price_t *)memAlloc(sizeof(Price_t));

	i->Bid = bid;
	i->Ask = ask;

	return i;
}
void ReleasePrice(Price_t *i)
{
	memFree(i);
}

// ---- accessor ----

double P_GetBid(Price_t *i)
{
	return i->Bid;
}
double P_GetAsk(Price_t *i)
{
	return i->Ask;
}
double P_GetMid(Price_t *i)
{
	return (i->Bid + i->Ask) / 2.0;
}

// ----
