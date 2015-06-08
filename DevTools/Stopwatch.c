#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	char *cmdln = untokenize(allArgs(), " ");
	uint64 st;
	uint64 ed;

	st = nowTick();
	system(cmdln);
	ed = nowTick();

	cout("ÀsŠÔ %I64u ƒ~ƒŠ•b\n", ed - st);
}
