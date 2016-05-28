#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Random.h"

#define NUMB_MAX 1000

static uchar Numer[NUMB_MAX];
static uint NumerSize;
static uchar Denom[NUMB_MAX];
static uint DenomSize;
static uchar Ans[NUMB_MAX];
static uint AnsSize;

// ---- div ----

static void DD_Add(uint64 b, uint aPos)
{
	error(); // TODO, AnsSize 更新忘れずに！
}
static void DD_Red(uint64 b, uint aPos)
{
	error(); // TODO, NumerSize 更新忘れずに！
}
static void DD_RedTry(void)
{
	error(); // TODO
}
static void DoDiv(void)
{
	uint ni;
	uint di;
	uint64 n;
	uint64 d;
	uint64 a;

	errorCase(!m_isRange(NumerSize, 8, NUMB_MAX));
	errorCase(!m_isRange(DenomSize, 4, NUMB_MAX));
	errorCase(NumerSize < DenomSize); // 0 余り Numer になることが分かっているので除外
	errorCase(!Numer[NumerSize - 1]);
	errorCase(!Denom[DenomSize - 1]);

	AnsSize = NumerSize - DenomSize + 1;
	memset(Ans, 0x00, AnsSize);

	di = DenomSize - 2;
	d = (uint64)Denom[di + 0] << 0 |
		(uint64)Denom[di + 1] << 8;

	for(; ; )
	{
		ni = NumerSize;
		n = 0;

		while(di < ni && (n & 0xff00000000000000ui64) == 0)
		{
			ni--;
			n <<= 8;
			n |= Numer[ni];
		}

		a = n / d;

		if(a == 0)
			break;

		DD_Add(a, ni - di);
		DD_Red(a, ni - di);
	}

	DD_RedTry();
}

// ---- test ----

static uchar Op1[NUMB_MAX];
static uint Op1Size;
static uchar Op2[NUMB_MAX];
static uint Op2Size;
static uchar Wk1[NUMB_MAX];
static uint Wk1Size;

static void DT_Print(uchar *data, uint size, char *title)
{
	uint index;

	cout("%s=", title);

	for(index = size; index; index--)
		cout("%02x", data[index - 1]);

	cout("\n");
}
static void DT_Mul(void)
{
	error(); // TODO, Wk1 = Op2 * Ans
}
static void DT_Add(void)
{
	error(); // TODO, Wk1 += Numer
}
static void DT_Check(void)
{
	error(); // TODO, Wk1 != Op1 -> error
}
static void DoTest(void)
{
	uint index;

	LOGPOS();

	for(index = 0; index < NUMB_MAX; index++)
	{
		Op1[index] = mt19937_rnd(256);
		Op2[index] = mt19937_rnd(256);
	}
	do
	{
		Op1Size = mt19937_range(8, NUMB_MAX);
		Op2Size = mt19937_range(4, NUMB_MAX);
	}
	while(Op1Size < Op2Size);

	while(!Op1[Op1Size - 1]) Op1[Op1Size - 1] = mt19937_rnd(256);
	while(!Op2[Op2Size - 1]) Op2[Op2Size - 1] = mt19937_rnd(256);

	memcpy(Numer, Op1, NUMB_MAX);
	NumerSize = Op1Size;
	memcpy(Denom, Op2, NUMB_MAX);
	DenomSize = Op2Size;

	DT_Print(Numer, NumerSize, "N");
	DT_Print(Denom, DenomSize, "D");

	DoDiv();

	DT_Print(Ans, AnsSize, "A");
	DT_Print(Numer, NumerSize, "R");

	DT_Mul();
	DT_Add();
	DT_Check();

	cout("OK!\n");
}

// ----

int main(int argc, char **argv)
{
	while(waitKey(0) != 0x1b)
	{
		DoTest();
	}
}
