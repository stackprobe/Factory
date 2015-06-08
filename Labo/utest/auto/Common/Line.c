#include "C:\Factory\Common\all.h"

static void DoTest_copyBlock(char *beforeData, uint destPos, uint srcPos, uint size, char *afterData)
{
	char *line = strx(beforeData);

	cout("line_1: %s\n", line);

	copyBlock(line + destPos, line + srcPos, size);

	cout("line_2: %s\n", line);
	cout("answer: %s\n", afterData);

	errorCase(strcmp(line, afterData));

	memFree(line);
}
static void Test_copyBlock(void)
{
	DoTest_copyBlock("AAABBBCCC", 0, 3, 6, "BBBCCCCCC");
	DoTest_copyBlock("AAABBBCCC", 3, 0, 6, "AAAAAABBB");
	DoTest_copyBlock("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbcccccccccccccccccccc", 0, 20, 40, "bbbbbbbbbbbbbbbbbbbbcccccccccccccccccccccccccccccccccccccccc");
	DoTest_copyBlock("aaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbcccccccccccccccccccc", 20, 0, 40, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbb");

	cout("OK\n");
}

int main(int argc, char **argv)
{
	Test_copyBlock();
}
