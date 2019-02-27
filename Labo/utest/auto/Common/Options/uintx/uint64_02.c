#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRRandom.h"
#include "C:\Factory\Common\Options\Calc2.h"
#include "C:\Factory\Common\Options\uintx\uint4096.h"

static char *TrimValueString(char *str)
{
	while(*str == '0')
		eraseChar(str);

	if(!*str)
		str = addChar(str, '0');

	return str;
}
static char *GetHexValue(uint scale)
{
	autoBlock_t *buff = newBlock();
	uint index;

	for(index = 0; index < scale; index++)
		addByte(buff, hexadecimal[mt19937_rnd(16)]);

	return TrimValueString(unbindBlock2Line(buff));
}
static UI4096_t FromString(char *str)
{
	uint arr[128] = { 0 };
	uint index;
	uint sLen = strlen(str);

	for(index = 0; index < sLen; index++)
		arr[index / 8] |= m_c2i(str[sLen - 1 - index]) << ((index % 8) * 4);

	return ToUI4096(arr);
}
static char *ToString(UI4096_t value)
{
	autoBlock_t *buff = newBlock();
	uint arr[128];
	uint index;

	FromUI4096(value, arr);

	for(index = 128; index; index--)
		ab_addLine_x(buff, xcout("%08x", arr[index - 1]));

	return TrimValueString(unbindBlock2Line(buff));
}

#define H_4096 "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"

static void Test01(void)
{
	uint testCnt;

	calcRadix = 16;
	calcBasement = 0;

	LOGPOS();
	for(testCnt = 0; testCnt < 1000; testCnt++)
	{
		// Add
		{
			char *s1 = GetHexValue(mt19937_range(0, 1024));
			char *s2 = GetHexValue(mt19937_range(0, 1024));
			char *sa;
			UI4096_t t1;
			UI4096_t t2;
			UI4096_t ta;
			char *t1s;
			char *t2s;
			char *tas;

			sa = calc(s1, '+', s2);

			t1 = FromString(s1);
			t2 = FromString(s2);

			ta = UI4096_Add(t1, t2, NULL);

			t1s = ToString(t1);
			t2s = ToString(t2);
			tas = ToString(ta);

//			cout("s1: %s\n", s1);
//			cout("s2: %s\n", s2);
//			cout("sa: %s\n", sa);
//			cout("t1: %s\n", t1s);
//			cout("t2: %s\n", t2s);
//			cout("ta: %s\n", tas);

			errorCase(strcmp(s1, t1s));
			errorCase(strcmp(s2, t2s));
			errorCase(strcmp(sa, tas));

			memFree(s1);
			memFree(s2);
			memFree(sa);
			memFree(t1s);
			memFree(t2s);
			memFree(tas);
		}

		// Sub
		{
			char *s1 = GetHexValue(mt19937_range(0, 1024));
			char *s2 = GetHexValue(mt19937_range(0, 1024));
			char *sa;
			UI4096_t t1;
			UI4096_t t2;
			UI4096_t ta;
			char *t1s;
			char *t2s;
			char *tas;

			sa = calc(s1, '-', s2);

			if(sa[0] == '-')
				sa = calc(sa, '+', H_4096);

			t1 = FromString(s1);
			t2 = FromString(s2);

			ta = UI4096_Sub(t1, t2);

			t1s = ToString(t1);
			t2s = ToString(t2);
			tas = ToString(ta);

//			cout("s1: %s\n", s1);
//			cout("s2: %s\n", s2);
//			cout("sa: %s\n", sa);
//			cout("t1: %s\n", t1s);
//			cout("t2: %s\n", t2s);
//			cout("ta: %s\n", tas);

			errorCase(strcmp(s1, t1s));
			errorCase(strcmp(s2, t2s));
			errorCase(strcmp(sa, tas));

			memFree(s1);
			memFree(s2);
			memFree(sa);
			memFree(t1s);
			memFree(t2s);
			memFree(tas);
		}

		// Mul // TODO

		// Div // TODO
	}
	LOGPOS();
}
int main(int argc, char **argv)
{
	mt19937_initCRnd();

	Test01();

	cout("OK!\n");
}
