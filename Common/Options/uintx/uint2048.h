#pragma once

#include "C:\Factory\Common\all.h"
#include "uint1024.h"

typedef struct uint2048_st
{
	uint1024_t L;
	uint1024_t H;
}
uint2048_t;

void ToUI2048(uint src[64], uint2048_t *dest);
void UI2048_0(uint2048_t *dest);
void UI2048_x(uint x, uint2048_t *dest);
void UI2048_msb1(uint2048_t *dest);
void UnUI2048(uint2048_t *src, uint dest[64]);

uint UI2048_Add(uint2048_t *a, uint2048_t *b, uint2048_t *ans) ;
uint UI2048_Sub(uint2048_t *a, uint2048_t *b, uint2048_t *ans) ;
void UI2048_Mul(uint2048_t *a, uint2048_t *b, uint2048_t *ans, uint2048_t *ans_hi);
void UI2048_Div(uint2048_t *a, uint2048_t *b, uint2048_t *ans);

int UI2048_IsZero(uint2048_t *a);
int UI2048_Comp(uint2048_t *a, uint2048_t *b);
uint UI2048_rs(uint2048_t *a, uint msb);
void UI2048_or(uint2048_t *a, uint2048_t *b, uint2048_t *ans);
