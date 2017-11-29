/*
	乱数列に同じ乱数列をXORすると全部ゼロになるが、
	乱数列に同じ乱数列をずらしてXORするのもマズいんじゃないか。
	-> マズいっぽい。
*/

#include "C:\Factory\Common\all.h"

//#define SHIFT_B 1
#define SHIFT_B 2

static uint Counters[0x10000];

static uint ShiftV(uint v)
{
	return ((v >> SHIFT_B) | (v << (16 - SHIFT_B))) & 0xffff;
}
static void PutV(uint v)
{
	v ^= ShiftV(v);

	Counters[v]++;
}
int main(int argc, char **argv)
{
	uint v;

	for(v = 0x0000; v <= 0xffff; v++)
	{
		PutV(v);
	}
	for(v = 0x0000; v <= 0xffff; v++)
	{
		cout("0x%04x <- %u\n", v, Counters[v]);
	}
}
