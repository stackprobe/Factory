#pragma once

#include "C:\Factory\Common\all.h"

extern uint httpGetOrPostRetryMax;
extern uint httpGetOrPostRetryDelayMillis;
extern uint httpGetOrPostTimeoutSec;
extern uint64 httpGetOrPostRecvBodySizeMax;
extern char *httpGetOrPostProxyDomain;
extern uint httpGetOrPostProxyPortNo;

int httpGetOrPostFile(char *url, char *sendBodyFile, char *recvBodyFile);
autoBlock_t *httpGetOrPost(char *url, autoBlock_t *sendBody);
