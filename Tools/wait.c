#pragma comment(lib, "user32.lib") // for mouse

#include "C:\Factory\Common\all.h"

static void DoAntiScreenSaver(void)
{
#if 1
	SetThreadExecutionState(ES_SYSTEM_REQUIRED);
	SetThreadExecutionState(ES_DISPLAY_REQUIRED);
#else
	SetCursorPos(0, 0); // 画面左上
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	sleep(100);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
#endif
}
int main(int argc, char **argv)
{
	int simpleMode = 0;
	int antiScreenSaver = 0;
	uint errorLevel = 0;
	uint remain;

readArgs:
	if(argIs("/S"))
	{
		simpleMode = 1;
		goto readArgs;
	}
	if(argIs("/ANTSS"))
	{
		cout("+--------+\n");
		cout("| ANT-SS |\n");
		cout("+--------+\n");

		antiScreenSaver = 1;
		goto readArgs;
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
		if(!remain)
			break;

		if(antiScreenSaver && remain % 30 == 0)
		{
			DoAntiScreenSaver();
		}
		sleep(1000);
		remain--;
	}
endLoop:
	cout("\n%u\n", errorLevel);
	termination(errorLevel);
}
