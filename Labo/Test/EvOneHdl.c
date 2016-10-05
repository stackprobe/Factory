#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Thread.h"

static void Test2Th(void *prm)
{
	uint ev = (uint)prm;

	LOGPOS();
	handleWaitForever(ev); // XXX no crit
	LOGPOS();
}
int main(int argc, char **argv)
{
	uint ev = eventOpen("EvOneHdl-Test");
	uint th;

	// ---- 1 ----

	LOGPOS();
	eventSet(ev);

	LOGPOS();
	handleWaitForever(ev);

	cout("1_OK!\n");

	// ---- 2 ----

	th = runThread(Test2Th, (void *)ev);

	sleep(100);

	LOGPOS();
	eventSet(ev); // XXX no crit

	sleep(100);

	LOGPOS();
	waitThread(th);
	th = 0;

	cout("2_OK!\n");

	// ----

	handleClose(ev);
}
