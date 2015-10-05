#pragma once

#include "C:\Factory\Common\all.h"

typedef struct uint64_st
{
	uint L;
	uint H;
}
uint64_t;

void ToUI64(uint src[2], uint64_t *dest);
void UI64_0(uint64_t *dest);
void UI64_x(uint x, uint64_t *dest);
void UI64_msb1(uint64_t *dest);
void UnUI64(uint64_t *src, uint dest[2]);

uint UI64_Add(uint64_t *a, uint64_t *b, uint64_t *ans);
uint UI64_Sub(uint64_t *a, uint64_t *b, uint64_t *ans);
void UI64_Mul(uint64_t *a, uint64_t *b, uint64_t *ans, uint64_t *ans_hi);
void UI64_Div(uint64_t *a, uint64_t *b, uint64_t *ans);

int UI64_IsZero(uint64_t *a);
int UI64_Comp(uint64_t *a, uint64_t *b);
uint UI64_rs(uint64_t *a, uint msb);
void UI64_or(uint64_t *a, uint64_t *b, uint64_t *ans);
