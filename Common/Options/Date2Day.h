#pragma once

#include "C:\Factory\Common\all.h"

uint64 Date2Day(uint y, uint m, uint d);
void Day2Date(uint64 day, uint *py, uint *pm, uint *pd);
int CheckDate(uint y, uint m, uint d);
uint GetEndOfMonth(uint y, uint m);
char *Date2JWeekday(uint y, uint m, uint d);

uint64 FileStampToMillis(uint64 stamp);
uint64 MillisToFileStamp(uint64 millis);
int IsFairFileStamp(uint64 stamp);
