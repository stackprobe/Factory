#pragma once

#include "C:\Factory\Common\all.h"
#include "uint128.h"

typedef struct uint256_st
{
	uint128_t L;
	uint128_t H;
}
uint256_t;

void ToUI256(uint src[8], uint256_t *dest);
void UI256_0(uint256_t *dest);
void UI256_x(uint x, uint256_t *dest);
void UI256_msb1(uint256_t *dest);
void UnUI256(uint256_t *src, uint dest[8]);

uint UI256_Add(uint256_t *a, uint256_t *b, uint256_t *ans) ;
uint UI256_Sub(uint256_t *a, uint256_t *b, uint256_t *ans) ;
void UI256_Mul(uint256_t *a, uint256_t *b, uint256_t *ans, uint256_t *ans_hi);
void UI256_Div(uint256_t *a, uint256_t *b, uint256_t *ans);

int UI256_IsZero(uint256_t *a);
int UI256_Comp(uint256_t *a, uint256_t *b);
uint UI256_rs(uint256_t *a, uint msb);
void UI256_or(uint256_t *a, uint256_t *b, uint256_t *ans);
