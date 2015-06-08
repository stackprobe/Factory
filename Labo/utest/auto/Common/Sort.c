#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Random.h"
#include "C:\Factory\Common\Options\Sequence.h"

static void ST_DispList(autoList_t *list)
{
	uint index;

	for(index = 0; index < getCount(list); index++)
	{
		cout("[%u]: %u\n", index, getElement(list, index));
	}
}
static autoList_t *ST_MakeList(uint start, uint count, uint step)
{
	autoList_t *list = newList();
	uint index;

	for(index = 0; index < count; index++)
		addElement(list, start + index * step);

	shuffle(list);
	return list;
}
static void ST_DoTest(void (*funcSort)(autoList_t *, sint (*)(uint, uint)), uint start, uint count, uint step)
{
	autoList_t *list = ST_MakeList(start, count, step);
	uint index;

	errorCase(getCount(list) != count); // 2bs

	cout("sort start...\n");
//ST_DispList(list);
	funcSort(list, simpleComp);
//ST_DispList(list);
	cout("sort end\n");

	errorCase(getCount(list) != count);

	for(index = 0; index < count; index++)
	{
		errorCase(getElement(list, index) != start + index * step);
	}
}
static void SortTest_MMMC(void (*funcSort)(autoList_t *, sint (*)(uint, uint)), uint startMax, uint countMax, uint stepMax, uint testCount)
{
	cout("startMax: %u\n", startMax);
	cout("countMax: %u\n", countMax);
	cout("stepMax: %u\n", stepMax);

	while(testCount)
	{
		uint start = mt19937_range(0, startMax);
		uint count = mt19937_range(0, countMax);
		uint step = mt19937_range(0, stepMax);

		cout("testCount: %u\n", testCount);
		cout("start: %u\n", start);
		cout("count: %u\n", count);
		cout("step: %u\n", step);

		ST_DoTest(funcSort, start, count, step);

		testCount--;
	}
	cout("OK\n");
}
static void SortTest(void (*funcSort)(autoList_t *, sint (*)(uint, uint)))
{
	SortTest_MMMC(funcSort, 4000000000, 100, 100, 10000);
	SortTest_MMMC(funcSort, 2000000000, 20000, 100000, 100);
//	SortTest_MMMC(funcSort, 2000000000, 200000, 10000, 100);
}
static void Test_gnomeSort(void)
{
	SortTest(gnomeSort);
}
static void Test_combSort(void)
{
	SortTest(combSort);
}
static void Test_insertSort(void)
{
	SortTest(insertSort);
}
static void Test_rapidSort(void)
{
	SortTest(rapidSort);
}

static autoList_t *GB_MakeList(uint count, uint minval, uint maxval)
{
	autoList_t *list = newList();

	while(count)
	{
		addElement(list, mt19937_range(minval, maxval));
		count--;
	}
	rapidSort(list, simpleComp);
	return list;
}
static int GB_Test(autoList_t *list, uint target, uint bound[2])
{
	uint index;

	for(index = 0; index < getCount(list); index++)
		if(getElement(list, index) == target)
			break;

	if(index == getCount(list))
		return 0;

	bound[0] = index;

	while(++index < getCount(list))
		if(getElement(list, index) != target)
			break;

	bound[1] = index - 1;
	return 1;
}
static void GB_DoTest(uint count, uint minval, uint maxval)
{
	autoList_t *list = GB_MakeList(count, minval, maxval);
	uint target;
	uint bound1[2];
	uint bound2[2];
	int ret1;
	int ret2;

	cout("GB_DoTest: %u %u %u\n", count, minval, maxval);

	for(target = minval; target <= maxval; target++)
	{
		cout("target: %u\n", target);

		ret1 = GB_Test(list, target, bound1);
		ret2 = getBound(list, target, simpleComp, bound2);

		ret1 = ret1 ? 1 : 0;
		ret2 = ret2 ? 1 : 0;

		errorCase(ret1 != ret2);

		if(ret1)
		{
			cout("bounds: %u %u %u %u\n", bound1[0], bound1[1], bound2[0], bound2[1]);

			errorCase(bound1[0] != bound2[0]);
			errorCase(bound1[1] != bound2[1]);
		}
	}
	releaseAutoList(list);
}
static void Test_getBound(void)
{
	uint testcnt;

	for(testcnt = 0; testcnt < 1000; testcnt++)
	{
		uint c = mt19937_rnd(10000);
		uint n = mt19937_rnd(1000);
		uint x = mt19937_rnd(1000);

		GB_DoTest(c, n, n + x);
	}
	cout("OK\n");
}

int main(int argc, char **argv)
{
	mt19937_initRnd((uint)time(NULL));

	Test_gnomeSort();
	Test_combSort();
	Test_insertSort();
	Test_rapidSort();
	Test_getBound();
}
