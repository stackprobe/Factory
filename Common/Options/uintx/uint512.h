#pragma once

#include "C:\Factory\Common\all.h"
#include "uint256.h"

typedef struct uint512_st
{
	uint256_t L;
	uint256_t H;
}
uint512_t;

void ToUI512(uint src[16], uint512_t *dest);
void UI512_0(uint512_t *dest);
void UI512_x(uint x, uint512_t *dest);
void UI512_msb1(uint512_t *dest);
void UnUI512(uint512_t *src, uint dest[16]);

uint UI512_Add(uint512_t *a, uint512_t *b, uint512_t *ans) ;
uint UI512_Sub(uint512_t *a, uint512_t *b, uint512_t *ans) ;
void UI512_Mul(uint512_t *a, uint512_t *b, uint512_t *ans, uint512_t *ans_hi);
void UI512_Div(uint512_t *a, uint512_t *b, uint512_t *ans);

int UI512_IsZero(uint512_t *a);
int UI512_Comp(uint512_t *a, uint512_t *b);
uint UI512_rs(uint512_t *a, uint msb);
void UI512_or(uint512_t *a, uint512_t *b, uint512_t *ans);
