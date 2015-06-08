#pragma once

#include "C:\Factory\Common\all.h"

typedef struct bitList_st
{
	autoList_t *Buffer;
}
bitList_t;

bitList_t *newBitList(void);
void releaseBitList(bitList_t *i);

// <-- cdtor

uint refBit(bitList_t *i, uint index);
void putBit(bitList_t *i, uint index, uint value);
uint getBitListSize(bitList_t *i);

// <-- accessor
