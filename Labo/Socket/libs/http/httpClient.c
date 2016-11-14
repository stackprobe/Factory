#include "httpClient.h"

uint httpGetOrPostRetryMax = 2;
uint httpGetOrPostRetryDelayMillis = 2000;
uint httpGetOrPostTimeoutSec = 180;
uint64 httpGetOrPostRecvBodySizeMax = 270000000ui64; // 270 MB
char *httpGetOrPostProxyDomain = NULL;
uint httpGetOrPostProxyPortNo = 8080;

/*
	recvBodyFile ... ïKÇ∏çÏê¨Ç∑ÇÈÅBé∏îséûÇÕãÛÇ…Ç∑ÇÈÅB
*/
int httpGetOrPostFile(char *url, char *sendBodyFile, char *recvBodyFile) // sendBodyFile: NULL == GET, ret: ? ê¨å˜
{
	uint retry;
	char *domain;
	uint portNo;
	char *path;

	errorCase(!url);
	errorCase(sendBodyFile && !existFile(sendBodyFile));
	errorCase(m_isEmpty(recvBodyFile));

	error(); // TODO

	for(retry = 0; retry <= httpGetOrPostRetryMax; retry++)
	{
		error(); // TODO
	}
	return 1;
}
autoBlock_t *httpGetOrPost(char *url, autoBlock_t *sendBody) // sendBody: NULL == GET, ret: NULL == é∏îs
{
	char *sendBodyFile;
	char *recvBodyFile;
	autoBlock_t *recvBody;

	errorCase(!url);
//	sendBody

	if(sendBody)
	{
		sendBodyFile = makeTempPath(NULL);
		writeBinary(sendBodyFile, sendBody);
	}
	else
		sendBodyFile = NULL;

	recvBodyFile = makeTempPath(NULL);

	if(httpGetOrPostFile(url, sendBodyFile, recvBodyFile))
		recvBody = readBinary(recvBodyFile);
	else
		recvBody = NULL;

	if(sendBodyFile)
		removeFile_x(sendBodyFile);

	removeFile_x(recvBodyFile);
	return recvBody;
}
