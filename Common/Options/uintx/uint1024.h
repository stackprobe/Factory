#pragma once

#include "C:\Factory\Common\all.h"
#include "uint512.h"

typedef struct uint1024_st
{
	uint512_t L;
	uint512_t H;
}
uint1024_t;

extern uint UI1024_Overflow;
extern uint UI1024_Ununderflow;
extern uint1024_t UI1024_Hi;

uint1024_t ToUI1024(uint src[32]);
uint1024_t UI1024_0(void);
uint1024_t UI1024_x(uint x);
uint1024_t UI1024_msb1(void);
void UnUI1024(uint1024_t src, uint dest[32]);

uint1024_t UI1024_Add(uint1024_t a, uint1024_t b);
uint1024_t UI1024_Sub(uint1024_t a, uint1024_t b);
uint1024_t UI1024_Mul(uint1024_t a, uint1024_t b);
uint1024_t UI1024_Div(uint1024_t a, uint1024_t b);

int UI1024_IsZero(uint1024_t a);
int UI1024_Comp(uint1024_t a, uint1024_t b);
uint UI1024_rs(uint1024_t *a, uint msb);
uint1024_t UI1024_or(uint1024_t a, uint1024_t b);
