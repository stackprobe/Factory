#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Thread.h"

static void TestFunc(void *prm) // ts_
{
	uint c;

	critical();
	{
		for(c = 0; c < 10; c++)
		{
			cout("%s:%u\n", prm, c);

			inner_uncritical();
			{
				// noop -- ここでスレッドの切り替えが起きて欲しい。
			}
			inner_critical();
		}
	}
	uncritical();
}
int main(int argc, char **argv)
{
	uint ths[3];

	critical();
	{
		ths[0] = runThread(TestFunc, "Th_0");
		ths[1] = runThread(TestFunc, "Th_1");
		ths[2] = runThread(TestFunc, "Th_2");
	}
	uncritical();

	TestFunc("Th_M");

	waitThread(ths[0]);
	waitThread(ths[1]);
	waitThread(ths[2]);
}
