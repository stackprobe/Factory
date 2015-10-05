#pragma once

#include "C:\Factory\Common\all.h"
#include "uint64.h"

typedef struct uint128_st
{
	uint64_t L;
	uint64_t H;
}
uint128_t;

void ToUI128(uint src[4], uint128_t *dest);
void UI128_0(uint128_t *dest);
void UI128_x(uint x, uint128_t *dest);
void UI128_msb1(uint128_t *dest);
void UnUI128(uint128_t *src, uint dest[4]);

uint UI128_Add(uint128_t *a, uint128_t *b, uint128_t *ans) ;
uint UI128_Sub(uint128_t *a, uint128_t *b, uint128_t *ans) ;
void UI128_Mul(uint128_t *a, uint128_t *b, uint128_t *ans, uint128_t *ans_hi);
void UI128_Div(uint128_t *a, uint128_t *b, uint128_t *ans);

int UI128_IsZero(uint128_t *a);
int UI128_Comp(uint128_t *a, uint128_t *b);
uint UI128_rs(uint128_t *a, uint msb);
void UI128_or(uint128_t *a, uint128_t *b, uint128_t *ans);
