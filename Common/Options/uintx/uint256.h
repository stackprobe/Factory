#pragma once

#include "C:\Factory\Common\all.h"
#include "uint128.h"

typedef struct uint256_st
{
	uint128_t L;
	uint128_t H;
}
uint256_t;

extern uint UI256_Overflow;
extern uint UI256_Ununderflow;
extern uint256_t UI256_Hi;

uint256_t ToUI256(uint src[8]);
uint256_t UI256_0(void);
uint256_t UI256_x(uint x);
uint256_t UI256_msb1(void);
void UnUI256(uint256_t src, uint dest[8]);

uint256_t UI256_Add(uint256_t a, uint256_t b);
uint256_t UI256_Sub(uint256_t a, uint256_t b);
uint256_t UI256_Mul(uint256_t a, uint256_t b);
uint256_t UI256_Div(uint256_t a, uint256_t b);

int UI256_IsZero(uint256_t a);
int UI256_Comp(uint256_t a, uint256_t b);
uint UI256_rs(uint256_t *a, uint msb);
uint256_t UI256_or(uint256_t a, uint256_t b);
