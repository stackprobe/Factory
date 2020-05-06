#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	updateDiskSpace_Dir("C:\\Nothing_Nothing_Nothing");

	cout("%I64u\n", lastDiskFree_User);
	cout("%I64u\n", lastDiskFree);
	cout("%I64u\n", lastDiskSize);
}
