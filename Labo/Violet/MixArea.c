/*

--

世界には n 種類の言語があるものとし、それぞれ g_1, g_2, ... g_n とする。


ある言語 g_X を話せる人の数を h(g_X) とする。

{ h(g_1), h(g_2), ... h(g_n) } の合計を i(1) とする。


少なくとも g_a1, g_a2, ... g_aY 全て話せる人の数を hk(g_a1, g_a2, ... g_aY) とする。

あらゆる m 個の言語の組み合わせ k_1, k_2, ... k_Z について、※
{ hk(k_1), hk(k_2), ... hk(k_Z) } の合計を i(m) とする。

※ n=3, m=2 の場合、{ (g_1, g_2), (g_1, g_3), (g_2, g_3) } の3つになる。


少なくとも1つの言語を話せる人の数は次の式で表せる。


n=1の場合

i(1)

n=2の場合

i(1) - i(2)

n=3の場合

i(1) - i(2) + i(3)

n=4の場合

i(1) - i(2) + i(3) - i(4)

n=奇数の場合

i(1) - i(2) + i(3) - i(4) + i(5) - ... + i(n - 2) - i(n - 1) + i(n)

n=偶数の場合

i(1) - i(2) + i(3) - i(4) + i(5) - ... - i(n - 2) + i(n - 1) - i(n)


-- というのが正しいかどうか確認するプログラム

*/

// use int

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRRandom.h"

#define K_MAX 16
#define T_MAX 100000

static int Map[K_MAX][T_MAX];
static int KK;
static int TT;
static double RR;

static double GetDRand(void) // ret: 0.0 ～ 1.0
{
	return mt19937_rnd32() / (double)UINTMAX;
}
static void MkMap(void)
{
	int k;
	int t;

	for(k = 0; k < KK; k++)
	for(t = 0; t < TT; t++)
	{
		Map[k][t] = GetDRand() < RR;
	}
}
static void PrintMap(void)
{
	int k;
	int t;

	cout("---- Map ----\n");

	for(k = 0; k < KK; k++)
	{
		for(t = 0; t < TT; t++)
		{
			cout("%c", Map[k][t] ? '1' : '0');
		}
		cout("\n");
	}
	cout("----\n");
}
static int GetArea_1(void)
{
	int ret = 0;
	int k;
	int t;

	for(t = 0; t < TT; t++)
	for(k = 0; k < KK; k++)
	{
		if(Map[k][t])
		{
			ret++;
			break; // next t
		}
	}
	return ret;
}

static int ATbl[K_MAX + 1];
static int KMap[K_MAX];

static int GA_HasKMap(int t) // ret: ? Map[][t] について、KMap[k] == 1 なら Map[k][t] == 1 である。
{
	int k;

	for(k = 0; k < KK; k++)
		if(KMap[k] && !Map[k][t])
			return 0;

	return 1;
}
static void GA_Main(void)
{
	int k;
	int cnt = 0;
	int t;

	for(k = 0; k < KK; k++)
		if(KMap[k])
			cnt++;

	for(t = 0; t < TT; t++)
		if(GA_HasKMap(t))
			ATbl[cnt]++;
}
static int GA_Next(void)
{
	int k;

	for(k = 0; k < KK; k++)
	{
		if(!KMap[k])
		{
			KMap[k] = 1;
			return 1;
		}
		KMap[k] = 0;
	}
	return 0;
}
static int GetArea_2(void)
{
	int ret;
	int k;

	zeroclear(ATbl);
	zeroclear(KMap);

	do
	{
		GA_Main();
	}
	while(GA_Next());

	ret = 0;

	for(k = 1; k <= KK; k++)
	{
		if(k % 2)
			ret += ATbl[k];
		else
			ret -= ATbl[k];
	}
	return ret;
}
static void DoTest_2(int k, int t, double r)
{
	int a1;
	int a2;

	LOGPOS();

	if(waitKey(0))
		termination(0);

	KK = k;
	TT = t;
	RR = r;

	cout("DoTest_2: %d %d %f\n", KK, TT, RR);

	MkMap();
//	PrintMap();

	a1 = GetArea_1();
	a2 = GetArea_2();

	cout("a1: %d\n", a1);
	cout("a2: %d\n", a2);

	errorCase(a1 != a2);

	LOGPOS();
}
static void DoTest(int k, int t)
{
	DoTest_2(k, t, 0.01);
	DoTest_2(k, t, 0.1);
	DoTest_2(k, t, 0.5);
	DoTest_2(k, t, 1.0 / k);
	DoTest_2(k, t, 1.0 / t);
	DoTest_2(k, t, 0.9);
	DoTest_2(k, t, 0.99);
}
int main(int argc, char **argv)
{
	int k;
	int t;

	mt19937_initCRnd();

	for(k = 1; k <= K_MAX; k++)
	for(t = 1; t <= T_MAX; t *= 10)
	{
		DoTest(k, t);
	}
	for(; ; )
	{
		DoTest_2(
			mt19937_rnd(K_MAX) + 1,
			mt19937_rnd(T_MAX) + 1,
			GetDRand()
			);
	}
}
