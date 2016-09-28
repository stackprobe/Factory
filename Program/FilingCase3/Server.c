#include "C:\Factory\Common\Options\SockServerTh.h"
#include "C:\Factory\Common\Options\SockStream.h"

#define EV_STOP "{49e9f81c-dae4-464f-a209-301eed85b011}"

static char *RootDir = "C:\\appdata\\FilingCase3\\Root";
static uint EvStop;

static void PerformTh(int sock, char *strip)
{
	SockStream_t *ss = CreateSockStream(sock, 0);
	autoBlock_t *buff = nobCreateBlock(20000000);

	for(; ; )
	{
		char *command;
		char *path;
		char *sDataSize;
		uint dataSize;

		command   = SockRecvLine(ss, 30);
		path      = SockRecvLine(ss, 1000);
		sDataSize = SockRecvLine(ss, 30);

		line2csym(command);
		toUpperLine(command);
		path = lineToFairRelPath_x(path, strlen(RootDir));
		line2csym(sDataSize);
		dataSize = toValue(sDataSize);

		cout("command: %s\n", command);
		cout("path: %s\n", path);
		cout("dataSize: %u\n", dataSize);

		updateDiskSpace(RootDir[0]);

		if(lastDiskFree < (uint64)dataSize)
			goto fault;

		if(!_stricmp(command, "GET"))
		{
			error(); // TODO
		}


		error(); // TODO


	fault:
		memFree(command);
		memFree(path);
		memFree(sDataSize);
	}
	ReleaseSockStream(ss);
	releaseAutoBlock(buff);
}
static int IdleTh(void)
{
	static int keep = 1;

	if(handleWaitForMillis(EvStop, 0))
		keep = 0;

	while(hasKey())
		if(getKey() == 0x1b)
			keep = 0;

	if(!keep)
		LOGPOS();

	return keep;
}
int main(int argc, char **argv)
{
	uint portNo = 65123;
	uint connectMax = 20;

readArgs:
	if(argIs("/P"))
	{
		portNo = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/C"))
	{
		connectMax = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/R"))
	{
		RootDir = nextArg();
		goto readArgs;
	}

	if(argIs("/S"))
	{
		LOGPOS();
		eventWakeup(EV_STOP);
		return;
	}

	cout("ポート番号: %u\n", portNo);
	cout("最大同時接続数: %u\n", connectMax);
	cout("RootDir.0: %s\n", RootDir);

	errorCase(!m_isRange(portNo, 1, 65535));
	errorCase(!m_isRange(connectMax, 1, IMAX));

	RootDir = makeFullPath(RootDir);
	RootDir = toFairFullPathFltr_x(RootDir);

	cout("RootDir.1: %s\n", RootDir);

	errorCase_m(isAbsRootDir(RootDir), "ルートディレクトリは指定出来ません。");

	createPath(RootDir, 'd');

	EvStop = eventOpen(EV_STOP);

	sockServerTh(PerformTh, portNo, connectMax, IdleTh);

	memFree(RootDir);
	handleClose(EvStop);
}
