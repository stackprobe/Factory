#include "C:\Factory\Common\all.h"

#define MUTEX_LOG "{4e08b31a-9280-4ee0-9f9a-2ef462589893}" // shared_uuid

static char *HTTDir;
static char *WFormat;
static char *WFile;

static void CaptureMain(void)
{
	// TODO
}
int main(int argc, char **argv)
{
	HTTDir  = nextArg();
	WFormat = nextArg();
	WFile   = nextArg();

	errorCase(m_isEmpty(HTTDir));
	errorCase(m_isEmpty(WFormat));
	errorCase(m_isEmpty(WFile));

	HTTDir = makeFullPath(HTTDir);
	WFile  = makeFullPath(WFile);

	errorCase(!existDir(HTTDir));

	createFileIfNotExist(WFile);

	CaptureMain();
}
