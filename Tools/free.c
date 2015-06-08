#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	updateMemory();

	cout("メモリ使用率 = %u %%\n", lastMemoryLoad);
	cout("物理メモリ空き = %I64u バイト\n", lastMemoryFree);
	cout("物理メモリ容量 = %I64u バイト\n", lastMemorySize);
	cout("仮想メモリ空き = %I64u バイト\n", lastVirtualFree);
	cout("仮想メモリ拡張 = %I64u バイト\n", lastExVirtualFree);
	cout("仮想メモリ容量 = %I64u バイト\n", lastVirtualSize);
	cout("ページサイズ空き = %I64u バイト\n", lastPageFileFree);
	cout("ページサイズ最大 = %I64u バイト\n", lastPageFileSize);
}
