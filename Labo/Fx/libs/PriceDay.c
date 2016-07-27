#include "all.h"

/*
	date: YYYYMMDD Œ`Ž®
	pair: "USDJPY" etc.

	ret: { Price_t *, ... }
*/
autoList_t *GetPriceDay(uint date, char *pair)
{
	return NULL; // TODO
}

// ----

void ReleasePriceDay(autoList_t *list)
{
	Price_t *i;
	uint index;

	foreach(list, i, index)
		ReleasePrice(i);

	releaseAutoList(list);
}
