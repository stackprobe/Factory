#include "C:\Factory\Common\all.h"

static void ViewPassword(char *pw)
{
	uint pwLen = strlen(pw);
	uint index;

	errorCase(!isAsciiLine(pw, 0, 0, 0));

	for(index = 0; index < pwLen; index++)
	{
		if(index && index % 4 == 0)
			cout(" ");

		cout("%c", pw[index]);
	}
	cout("\n");

	for(index = 0; index < pwLen; index++)
	{
		uint c = index % 8 / 4;
		uint d = index / 8 % 2;

		if(index && index % 4 == 0)
			cout(" ");

		cout("%c", "^~ "[c ? d : 2]);
	}
	cout("\n");

	for(index = 0; index < pwLen; index++)
	{
		uint c = index % 16 / 12;
		uint d = index / 16 + 1;

		if(index && index % 4 == 0)
			cout(" ");

		if(c)
			cout("%u", d);
		else
			cout(" ");
	}
	cout("\n");
}
int main(int argc, char **argv)
{
	ViewPassword(nextArg());
}
