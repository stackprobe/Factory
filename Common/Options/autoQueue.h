#pragma once

#include "C:\Factory\Common\all.h"

typedef struct autoQueue_st
{
	autoBlock_t *Buffer;
	uint NextPos;
}
autoQueue_t;

autoQueue_t *newQueue(void);
void releaseAutoQueue(autoQueue_t *i);

void *directGetQueBuffer(autoQueue_t *i);
uint getQueSize(autoQueue_t *i);

void enqByte(autoQueue_t *i, uint chr);
uint deqByte(autoQueue_t *i);

void enqValue(autoQueue_t *i, uint value);
uint deqValue(autoQueue_t *i);

void enqBytes(autoQueue_t *i, autoBlock_t *bytes);
autoBlock_t *deqBytes(autoQueue_t *i, uint size);
