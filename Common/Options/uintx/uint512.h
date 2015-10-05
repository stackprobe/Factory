#pragma once

#include "C:\Factory\Common\all.h"
#include "uint256.h"

typedef struct uint512_st
{
	uint256_t L;
	uint256_t H;
}
uint512_t;

extern uint UI512_Overflow;
extern uint UI512_Ununderflow;
extern uint512_t UI512_Hi;

uint512_t ToUI512(uint src[16]);
uint512_t UI512_0(void);
uint512_t UI512_x(uint x);
uint512_t UI512_msb1(void);
void UnUI512(uint512_t src, uint dest[16]);

uint512_t UI512_Add(uint512_t a, uint512_t b);
uint512_t UI512_Sub(uint512_t a, uint512_t b);
uint512_t UI512_Mul(uint512_t a, uint512_t b);
uint512_t UI512_Div(uint512_t a, uint512_t b);

int UI512_IsZero(uint512_t a);
int UI512_Comp(uint512_t a, uint512_t b);
uint UI512_rs(uint512_t *a, uint msb);
uint512_t UI512_or(uint512_t a, uint512_t b);
