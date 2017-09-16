#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Thread.h"

static int KeepTh;
static CRITICAL_SECTION CrSec;

static void Th_01(void *dmy)
{
	uint c;

	EnterCriticalSection(&CrSec);
	{
		for(c = 0; c < 1000 && KeepTh; c++)
		{
			cout(".");
			Sleep(10);

			LeaveCriticalSection(&CrSec);
			{
				Sleep(0); // MSDN
			}
			EnterCriticalSection(&CrSec);
		}
	}
	LeaveCriticalSection(&CrSec);
}
static void Th_02(void *dmy)
{
	uint c;

	EnterCriticalSection(&CrSec);
	{
		for(c = 0; c < 1000 && KeepTh; c++)
		{
			cout(".");
			Sleep(10);

			LeaveCriticalSection(&CrSec);
			{
				sleep(0); // my sleep(), will be context switching
			}
			EnterCriticalSection(&CrSec);
		}
	}
	LeaveCriticalSection(&CrSec);
}
static void Test01(void (*funcTh)(void *))
{
	uint th;
	uint c;

	KeepTh = 1;

	EnterCriticalSection(&CrSec);
	{
		th = runThread(funcTh, NULL);
	}
	LeaveCriticalSection(&CrSec);

	for(c = 0; c < 30; c++)
	{
		Sleep(1); // switch to Th_0x

		EnterCriticalSection(&CrSec);
		{
			cout("!");
		}
		LeaveCriticalSection(&CrSec);
	}
	KeepTh = 0;
	waitThread(th);

	cout("\n");
}
int main(int argc, char **argv)
{
	InitializeCriticalSection(&CrSec);

	cout("Th_01 -- MSDN Sleep();\n");
	Test01(Th_01);
	cout("Th_02 -- my sleep(); will be context swigching\n");
	Test01(Th_02);

	DeleteCriticalSection(&CrSec);
}
