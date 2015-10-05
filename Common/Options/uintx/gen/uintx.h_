#pragma once

#include "C:\Factory\Common\all.h"
#include "uint%LBIT%.h"

typedef struct uint%HBIT%_st
{
	uint%LBIT%_t L;
	uint%LBIT%_t H;
}
uint%HBIT%_t;

void ToUI%HBIT%(uint src[%HSZ%], uint%HBIT%_t *dest);
void UI%HBIT%_0(uint%HBIT%_t *dest);
void UI%HBIT%_x(uint x, uint%HBIT%_t *dest);
void UI%HBIT%_msb1(uint%HBIT%_t *dest);
void UnUI%HBIT%(uint%HBIT%_t *src, uint dest[%HSZ%]);

uint UI%HBIT%_Add(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans) ;
uint UI%HBIT%_Sub(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans) ;
void UI%HBIT%_Mul(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans, uint%HBIT%_t *ans_hi);
void UI%HBIT%_Div(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans);

int UI%HBIT%_IsZero(uint%HBIT%_t *a);
int UI%HBIT%_Comp(uint%HBIT%_t *a, uint%HBIT%_t *b);
uint UI%HBIT%_rs(uint%HBIT%_t *a, uint msb);
void UI%HBIT%_or(uint%HBIT%_t *a, uint%HBIT%_t *b, uint%HBIT%_t *ans);
