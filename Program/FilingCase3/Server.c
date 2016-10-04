#include "C:\Factory\Common\Options\SockServerTh.h"
#include "C:\Factory\Common\Options\SockStream.h"
#include "C:\Factory\Common\Options\CRRandom.h"

#define EV_STOP "{49e9f81c-dae4-464f-a209-301eed85b011}"

static uint64 KeepDiskFree = 2500000000ui64; // 2.5 GB
static char *RootDir = "C:\\appdata\\FilingCase3";
static char *DataDir;
static char *TempDir;
static uint EvStop;
static critical_t CritFileIO;

static int RecvPrmData(SockStream_t *ss, char *dataFile, uint64 dataSize) // ret: ? 成功
{
	uint64 count;

	enterCritical(&CritFileIO);
	{
		FILE *fp = fileOpen(dataFile, "wb");

		for(count = 0; count < dataSize; count++)
		{
			int chr = SockRecvChar(ss);

			if(chr == EOF)
				break;

			writeChar(fp, chr);
		}
		fileClose(fp);
	}
	leaveCritical(&CritFileIO);

	return count == dataSize;
}
static void PerformTh(int sock, char *strip)
{
	SockStream_t *ss = CreateSockStream2(sock, 0, 30, 0);

	for(; ; )
	{
		char *command;
		char *path;
		char *sDataSize;
		uint64 dataSize;
		char *dataFile;

		command   = SockRecvLine(ss, 30);
		path      = SockRecvLine(ss, 1000);
		sDataSize = SockRecvLine(ss, 30);

		coutJLine_x(xcout("command: %s\n", command));
		coutJLine_x(xcout("path: %s\n", path));
		coutJLine_x(xcout("dataSize: %s\n", sDataSize));

		dataSize = toValue64(sDataSize);

		if(!isFairRelPath(path, strlen(RootDir)))
		{
			cout("不正なパスです。\n");
			goto fault;
		}
		updateDiskSpace(RootDir[0]);

		if(lastDiskFree < KeepDiskFree)
		{
			cout("実行に必要なディスクの空き領域が不足しています。\n");
			goto fault;
		}
		if(lastDiskFree - KeepDiskFree < dataSize)
		{
			cout("ディスクの空き領域が要求データサイズに対して不足しています。\n");
			goto fault;
		}
		dataFile = combine_cx(TempDir, MakeUUID(1));

		if(!RecvPrmData(ss, dataFile, dataSize))
		{
			cout("データの受信に失敗しました。\n");
			goto fault2;
		}

		{
			char *ender = SockRecvLine(ss, 30);

			if(_stricmp(ender, "/e"))
			{
				cout("不正な終端です。\n");
				memFree(ender);
				goto fault2;
			}
			memFree(ender);
		}

		if(!_stricmp(command, "GET"))
		{
			error(); // TODO
		}

		error(); // TODO

	fault2:
		removeFile(dataFile);
		memFree(dataFile);

	fault:
		memFree(command);
		memFree(path);
		memFree(sDataSize);
	}
	ReleaseSockStream(ss);
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
	if(argIs("/D"))
	{
		KeepDiskFree = toValue64(nextArg());
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
	cout("確保するディスクの空き領域: %I64u\n", KeepDiskFree);
	cout("RootDir.0: %s\n", RootDir);

	errorCase(!m_isRange(portNo, 1, 65535));
	errorCase(!m_isRange(connectMax, 1, IMAX));

	RootDir = makeFullPath(RootDir);
	RootDir = toFairFullPathFltr_x(RootDir);

	cout("RootDir.1: %s\n", RootDir);

	DataDir = combine(RootDir, "d");
	TempDir = combine(RootDir, "e");

	cout("DataDir: %s\n", DataDir);
	cout("TempDir: %s\n", TempDir);

	createPath(DataDir, 'd');
	createPath(TempDir, 'd');

	recurClearDir(TempDir);

	EvStop = eventOpen(EV_STOP);
	initCritical(&CritFileIO);

	sockServerTh(PerformTh, portNo, connectMax, IdleTh);

	memFree(RootDir);
	memFree(DataDir);
	memFree(TempDir);
	handleClose(EvStop);
	fnlzCritical(&CritFileIO);
}
