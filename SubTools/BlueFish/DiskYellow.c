/*
	DiskYellow.exe TARGET-DRIVE DISK-FREE-LIMIT YELLOW-COMMAND
*/

#include "C:\Factory\Common\all.h"

static int TargetDrive;
static uint64 DiskFreeLimit;
static char *YellowCommand;

static void Monitor(void)
{
	cout("モニター開始 @ %s\n", c_makeCompactStamp(NULL));

	for(; ; )
	{
		int key = waitKey(2000);
		uint64 diskFree;

		if(key == 0x1b)
			break;

		updateDiskSpace(TargetDrive);
		diskFree = lastDiskFree_User;

		if(key == 'T')
		{
			cout("########################\n");
			cout("## コマンドテスト実行 ##\n");
			cout("########################\n");

			diskFree = 0ui64;
		}
		else if(key)
		{
			cout("+---------------------+\n");
			cout("| ESC ⇒ モニター終了 |\n");
			cout("| T ⇒ コマンドテスト |\n");
			cout("+---------------------+\n");
		}
		cout("DiskFree: %I64u (DiskFreeLimit: %I64u, rate: %.3f)\n", diskFree, DiskFreeLimit, diskFree * 1.0 / DiskFreeLimit);

		if(diskFree < DiskFreeLimit)
		{
			cout("コマンド実行 @ %s\n", c_makeCompactStamp(NULL));
			coExecute(YellowCommand);
			cout("コマンド終了 @ %s\n", c_makeCompactStamp(NULL));
		}
	}
	cout("モニター終了 @ %s\n", c_makeCompactStamp(NULL));
}
int main(int argc, char **argv)
{
	TargetDrive = nextArg()[0];
	DiskFreeLimit = toValue64(nextArg());
	YellowCommand = nextArg();

	errorCase(!m_isalpha(TargetDrive));

	cout("TargetDrive: %c\n", TargetDrive);
	cout("DiskFreeLimit: %I64u\n", DiskFreeLimit);
	cout("YellowCommand: %s\n", YellowCommand);

	errorCase(!m_isRange(DiskFreeLimit, 1ui64, IMAX_64));
	errorCase(m_isEmpty(YellowCommand));

	cmdTitle_x(xcout("DiskYellow - [%c:] %I64u = %s", TargetDrive, DiskFreeLimit, YellowCommand));

	Monitor();

	cmdTitle("DiskYellow");
}
