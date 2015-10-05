#pragma once

#include "C:\Factory\Common\all.h"
#include "uint64.h"

typedef struct uint128_st
{
	uint64_t L;
	uint64_t H;
}
uint128_t;

extern uint UI128_Overflow;
extern uint UI128_Ununderflow;
extern uint128_t UI128_Hi;

uint128_t ToUI128(uint src[4]);
uint128_t UI128_0(void);
uint128_t UI128_x(uint x);
uint128_t UI128_msb1(void);
void UnUI128(uint128_t src, uint dest[4]);

uint128_t UI128_Add(uint128_t a, uint128_t b);
uint128_t UI128_Sub(uint128_t a, uint128_t b);
uint128_t UI128_Mul(uint128_t a, uint128_t b);
uint128_t UI128_Div(uint128_t a, uint128_t b);

int UI128_IsZero(uint128_t a);
int UI128_Comp(uint128_t a, uint128_t b);
uint UI128_rs(uint128_t *a, uint msb);
uint128_t UI128_or(uint128_t a, uint128_t b);
