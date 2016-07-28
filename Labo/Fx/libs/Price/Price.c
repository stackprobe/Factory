#include "all.h"

Price_t *CreatePrice(double bid, double ask)
{
	Price_t *i = nb(Price_t);

	i->Bid = bid;
	i->Ask = ask;

	return i;
}
void ReleasePrice(Price_t *i)
{
	if(!i)
		return;

	memFree(i);
}

// <-- cdtor

double GetBid(Price_t *i)
{
	return i->Bid;
}
double GetAsk(Price_t *i)
{
	return i->Ask;
}
double GetMid(Price_t *i)
{
	return (i->Bid + i->Ask) / 2.0;
}

// <-- accessor
