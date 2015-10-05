#pragma once

#include "C:\Factory\Common\all.h"
#include "uint2048.h"

typedef struct uint4096_st
{
	uint2048_t L;
	uint2048_t H;
}
uint4096_t;

void ToUI4096(uint src[128], uint4096_t *dest);
void UI4096_0(uint4096_t *dest);
void UI4096_x(uint x, uint4096_t *dest);
void UI4096_msb1(uint4096_t *dest);
void UnUI4096(uint4096_t *src, uint dest[128]);

uint UI4096_Add(uint4096_t *a, uint4096_t *b, uint4096_t *ans) ;
uint UI4096_Sub(uint4096_t *a, uint4096_t *b, uint4096_t *ans) ;
void UI4096_Mul(uint4096_t *a, uint4096_t *b, uint4096_t *ans, uint4096_t *ans_hi);
void UI4096_Div(uint4096_t *a, uint4096_t *b, uint4096_t *ans);

int UI4096_IsZero(uint4096_t *a);
int UI4096_Comp(uint4096_t *a, uint4096_t *b);
uint UI4096_rs(uint4096_t *a, uint msb);
void UI4096_or(uint4096_t *a, uint4096_t *b, uint4096_t *ans);
