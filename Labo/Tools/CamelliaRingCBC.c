#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\RingCipher2.h"

int main(int argc, char **argv)
{
	autoBlock_t *rawKey = NULL;
	autoBlock_t *rData = NULL;
	autoBlock_t *wData;
	int mode = 0;
	char *wFile = NULL;
	autoList_t *keyTableList;

readArgs:
	if(argIs("/K"))
	{
		errorCase(rawKey);

		rawKey = readBinary(nextArg());
		goto readArgs;
	}
	if(argIs("/R"))
	{
		errorCase(rData);

		rData = readBinary(nextArg());
		goto readArgs;
	}
	if(argIs("/E"))
	{
		mode = 'E';
		goto readArgs;
	}
	if(argIs("/D"))
	{
		mode = 'D';
		goto readArgs;
	}
	if(argIs("/W"))
	{
		errorCase(wFile);

		wFile = nextArg();
		goto readArgs;
	}

	errorCase(!rawKey);
	errorCase(!rData);
	errorCase(!mode);
	errorCase(!wFile);
LOGPOS();

	keyTableList = rngcphrCreateKeyTableList(rawKey);
	wData = copyAutoBlock(rData);
	( mode == 'E' ? rngcphrEncryptBlock : rngcphrDecryptBlock )(wData, keyTableList);
LOGPOS();

	writeBinary(wFile, wData);
LOGPOS();

	releaseAutoBlock(rawKey);
	releaseAutoBlock(rData);
	releaseAutoBlock(wData);
//	wFile
	cphrReleaseKeyTableList(keyTableList);
LOGPOS();
}
