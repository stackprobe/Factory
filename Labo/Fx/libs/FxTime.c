/*
	sec == 0 ‚Í 1/1/1 Œ—j“ú
*/

#include "FxTime.h"

#define START_TIME (0 * 86400 + 7 * 3600 +  0 * 60) // Œ—j“ú(0“úŒã) 07:00:00
#define END_TIME   (5 * 86400 + 5 * 3600 + 50 * 60) // “y—j“ú(5“úŒã) 05:50:00
#define TIME_CYCLE (7 * 86400) // 1TŠÔ

#define TRADING_TIME (END_TIME - START_TIME) // æˆø‰Â”\ŠúŠÔ
#define INTERVAL_TIME (TIME_CYCLE - TRADING_TIME) // ‚¨‹x‚İŠúŠÔ

uint64 Sec2FxTime(uint64 sec)
{
	uint64 count = sec / TIME_CYCLE;
	uint64 rem   = sec % TIME_CYCLE;

	m_range(rem, START_TIME, END_TIME);
	rem -= START_TIME;

	return count * TRADING_TIME + rem;
}
uint64 FxTime2Sec(uint64 fxTime)
{
	uint64 count = fxTime / TRADING_TIME;
	uint64 rem   = fxTime % TRADING_TIME;

	rem += START_TIME;

	return count * TIME_CYCLE + rem;
}
