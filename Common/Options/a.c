#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	char *str = nextArg();

	str = xcout("%01234s", str);

	cout("%s\n", str);
}
