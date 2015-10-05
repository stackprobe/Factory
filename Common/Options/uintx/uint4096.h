#pragma once

#include "C:\Factory\Common\all.h"
#include "uint2048.h"

typedef struct uint4096_st
{
	uint2048_t L;
	uint2048_t H;
}
uint4096_t;

extern uint UI4096_Overflow;
extern uint UI4096_Ununderflow;
extern uint4096_t UI4096_Hi;

uint4096_t ToUI4096(uint src[128]);
uint4096_t UI4096_0(void);
uint4096_t UI4096_x(uint x);
uint4096_t UI4096_msb1(void);
void UnUI4096(uint4096_t src, uint dest[128]);

uint4096_t UI4096_Add(uint4096_t a, uint4096_t b);
uint4096_t UI4096_Sub(uint4096_t a, uint4096_t b);
uint4096_t UI4096_Mul(uint4096_t a, uint4096_t b);
uint4096_t UI4096_Div(uint4096_t a, uint4096_t b);

int UI4096_IsZero(uint4096_t a);
int UI4096_Comp(uint4096_t a, uint4096_t b);
uint UI4096_rs(uint4096_t *a, uint msb);
uint4096_t UI4096_or(uint4096_t a, uint4096_t b);
