#include "C:\Factory\Common\all.h"
#include "C:\Factory\Labo\Socket\libs\http\httpClient.h"

#define FILE_DL_BAT "Run.bat"

static char *DLUrl = "http://localhost/remote/DLRun.bat";

static void DLRun(void)
{
	char *dir = makeTempDir(NULL);
	char *batFile;

	batFile = combine(dir, FILE_DL_BAT);

	httpGetOrPost(DLUrl, NULL);

	// TODO

	memFree(dir);
	memFree(batFile);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/U"))
	{
		DLUrl = nextArg();
		goto readArgs;
	}
	errorCase(m_isEmpty(DLUrl));

	DLRun();
}
