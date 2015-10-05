#pragma once

#include "C:\Factory\Common\all.h"

typedef struct uint64_st
{
	uint L;
	uint H;
}
uint64_t;

extern uint UI64_Overflow;
extern uint UI64_Ununderflow;
extern uint64_t UI64_Hi;

uint64_t ToUI64(uint src[2]);
uint64_t UI64_0(void);
uint64_t UI64_x(uint x);
uint64_t UI64_msb1(void);
void UnUI64(uint64_t src, uint dest[2]);

uint64_t UI64_Add(uint64_t a, uint64_t b);
uint64_t UI64_Sub(uint64_t a, uint64_t b);
uint64_t UI64_Mul(uint64_t a, uint64_t b);
uint64_t UI64_Div(uint64_t a, uint64_t b);

int UI64_IsZero(uint64_t a);
int UI64_Comp(uint64_t a, uint64_t b);
uint UI64_rs(uint64_t *a, uint msb);
uint64_t UI64_or(uint64_t a, uint64_t b);
