#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"

static char *GetToolkitExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\Kit\\Toolkit\\Toolkit.exe");

	return file;
}

static char *JihoWavFile;

static void PlayJiho(void)
{
	LOGPOS();
	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" /PLAY-WAV \"%s\"", GetToolkitExeFile(), JihoWavFile));
	LOGPOS();
}
int main(int argc, char **argv)
{
	JihoWavFile = nextArg();

	errorCase(!existFile(JihoWavFile));
	errorCase(_stricmp(getExt(JihoWavFile), "wav"));

	if(argIs("/P"))
	{
		PlayJiho();
		return;
	}

	LOGPOS();

	for(; ; )
	{
		uint rem = (3600 - (uint)((time(NULL) + 2260) % 3600)) % 3600;
		uint millis;

		if(rem == 0)
		{
			PlayJiho();
		}
		else if(rem == 1)
		{
			millis = 100;
		}
		else if(rem == 2)
		{
			millis = 300;
		}
		else if(rem == 3)
		{
			millis = 500;
		}
		else if(rem <= 10)
		{
			millis = 1000;
		}
		else
		{
			millis = 5000;
		}

		LOGPOS();
		cout("rem, millis: %u, %u\n", rem, millis);

		{
			int key = waitKey(millis);

			if(key == 0x1b)
				break;

			if(key == 0x0d)
			{
				cout("################\n");
				cout("## �e�X�g���� ##\n");
				cout("################\n");

				PlayJiho();
			}
		}
	}
	LOGPOS();
}
