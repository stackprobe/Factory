/*
	多分１通貨１年分くらいメモリに入る。-> クリアしなくて良い？
*/

#include "all.h"

PriceDayCache_t *CreatePriceDayCache(void)
{
	PriceDayCache_t *i = nb(PriceDayCache_t);

	i->Dummy = 0;

	return i;
}
void ReleasePriceDayCache(PriceDayCache_t *i)
{
	if(!i)
		return;

	memFree(i);
}

// <-- cdtor

autoList_t *PDC_GetPriceDay(PriceDayCache_t *i, uint date, char *pair)
{
	error(); // TODO
	return NULL;
}
void PDC_AddPriceDay(PriceDayCache_t *i, uint date, char *pair, autoList_t *list)
{
	error(); // TODO
}
void PDC_Clear(PriceDayCache_t *i)
{
	error(); // TODO
}

// <-- accessor
