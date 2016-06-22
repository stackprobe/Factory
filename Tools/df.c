#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Calc2.h"

static char *GetPct(uint64 numer, uint64 denom)
{
	calcBasement = 2;
	return calc_xx(xcout("%I64u00", numer), '/', xcout("%I64u", denom));
}
static void DispDiskFree(int drive)
{
	updateDiskSpace(drive);

	cout("%cドライブ\n", m_toupper(drive));
	cout("ディスク使用率 = %.3f %%\n", 100.0 - lastDiskFree / (double)lastDiskSize * 100.0);
	cout("ディスク空き = %I64u バイト (使用可能 = %I64u バイト, %s %%)\n", lastDiskFree, lastDiskFree_User, GetPct(lastDiskFree_User, lastDiskFree));
	cout("ディスク容量 = %I64u バイト\n", lastDiskSize);
}
int main(int argc, char **argv)
{
	if(argIs("*"))
	{
		char drive[] = "_:\\";

		for(drive[0] = 'A'; drive[0] <= 'Z'; drive[0]++)
			if(existDir(drive))
				DispDiskFree(drive[0]);

		return;
	}
	DispDiskFree((hasArgs(1) ? nextArg() : c_getCwd())[0]);
}
