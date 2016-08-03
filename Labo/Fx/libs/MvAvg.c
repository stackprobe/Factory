#include "all.h"

double MvAvg_GetMid(uint64 fxTime, uint64 span, char *pair)
{
	return MA_GetMid(MAC_GetMvAvg(fxTime, span, pair), fxTime);
}
