#include "all.h"

Price_t *GetPrice_T(time_t t)
{
	error(); // TODO
	return NULL;
}
Price_t *GetPrice(uint fxTime)
{
	return GetPrice_T(FxTime2Time(fxTime));
}
