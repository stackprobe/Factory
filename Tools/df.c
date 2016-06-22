#include "C:\Factory\Common\all.h"

static void DispDiskFree(int drive)
{
	updateDiskSpace(drive);

	cout("%cドライブ\n", m_toupper(drive));
	cout("ディスク使用率 = %.3f %%\n", (lastDiskSize - lastDiskFree) * 100.0 / lastDiskSize);
	cout("ディスク空き = %I64u バイト\n", lastDiskFree);
	cout("ディスク容量 = %I64u バイト\n", lastDiskSize);
	cout("利用可能領域 = %I64u バイト\n", lastDiskFree_User);
	cout("利用不可領域 = %I64u バイト\n", lastDiskFree - lastDiskFree_User);
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
