#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	int simpleMode = 0;
	uint errorLevel = 0;
	uint remain;

	if(argIs("/S"))
	{
		simpleMode = 1;
	}

	if(argIs("-7")) // 次の 7, 17, 27, 37, 47, 57 秒を待つ。
	{
		cout("next x7 second\n");
		remain = (17 - (uint)(time(NULL) % 10i64)) % 10;
		cout("remain: %u\n", remain);
	}
	else if(argIs("-37")) // 次の 37 秒を待つ。
	{
		cout("next 37 second\n");
		remain = (97 - (uint)(time(NULL) % 60i64)) % 60;
		cout("remain: %u\n", remain);
	}
	else if(argIs("-0000")) // 次の hh:00:00 を待つ。
	{
		cout("next hh:00:00\n");
		remain = 3600 - (uint)(time(NULL) % 3600i64);
		cout("remain: %u\n", remain);
	}
	else if(argIs("-3000")) // 次の hh:00:00 or hh:30:00 を待つ。
	{
		cout("next hh:00:00 or hh:30:00\n");
		remain = 1800 - (uint)(time(NULL) % 1800i64);
		cout("remain: %u\n", remain);
	}
	else
	{
		remain = toValue(nextArg());
	}

	for(; ; )
	{
		if(simpleMode)
			cout("\r%u ", remain);
		else
			cout("\rwait for ESCAPE or other keys while %u seconds... ", remain);

		if(!remain)
			break;

		if(hasKey())
		{
			while(hasKey())
			{
				switch(getKey())
				{
				case 0x1b:
					errorLevel = 1;
					goto endLoop;

				case 0x0d:
					goto endLoop;

				case '+':
					remain += 60;
					break;

				case '-':
					if(remain < 65)
						remain = 5;
					else
						remain -= 60;

					break;

				case '*':
					remain += 3600;
					break;

				case '/':
					if(remain < 3605)
						remain = 5;
					else
						remain -= 3600;

					break;
				}
			}
		}
		sleep(1000);
		remain--;
	}
endLoop:
	cout("\n%u\n", errorLevel);
	termination(errorLevel);
}
