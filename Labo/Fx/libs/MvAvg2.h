typedef struct MvAvg_st
{
	uint64 Span;
	char *Pair;
	uint64 CurrFxTime;
	double CurrTotal;
}
MvAvg_t;

MvAvg_t *CreateMvAvg(uint64 fxTime, uint64 span, char *pair);
void ReleaseMvAvg(MvAvg_t *i);

// <-- cdtor

double MA_GetMid(MvAvg_t *i, uint64 fxTime);

// <-- accessor
