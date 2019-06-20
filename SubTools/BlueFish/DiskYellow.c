/*
	DiskYellow.exe TARGET-DRIVE DISK-FREE-LIMIT YELLOW-COMMAND
*/

#include "C:\Factory\Common\all.h"

static int TargetDrive;
static uint64 DiskFreeLimit;
static char *YellowCommand;

static void Monitor(void)
{
	cout("���j�^�[�J�n @ %s\n", c_makeCompactStamp(NULL));

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
			cout("## �R�}���h�e�X�g���s ##\n");
			cout("########################\n");

			diskFree = 0ui64;
		}
		else if(key)
		{
			cout("+---------------------+\n");
			cout("| ESC �� ���j�^�[�I�� |\n");
			cout("| T �� �R�}���h�e�X�g |\n");
			cout("+---------------------+\n");
		}
		cout("DiskFree: %I64u (DiskFreeLimit: %I64u, rate: %.3f)\n", diskFree, DiskFreeLimit, diskFree * 1.0 / DiskFreeLimit);

		if(diskFree < DiskFreeLimit)
		{
			cout("�R�}���h���s @ %s\n", c_makeCompactStamp(NULL));
			coExecute(YellowCommand);
			cout("�R�}���h�I�� @ %s\n", c_makeCompactStamp(NULL));
		}
	}
	cout("���j�^�[�I�� @ %s\n", c_makeCompactStamp(NULL));
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
