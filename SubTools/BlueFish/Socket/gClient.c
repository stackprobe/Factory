/*
	gClient.exe サーバー名 ポート番号 (u | d) レーン名 クライアント側ディレクトリ
*/

#include "C:\Factory\Common\Options\SClient.h"
#include "gDefine.h"

static char *Server;
static uint PortNo;
static char *Command;
static char *Lane;
static char *ClientDir;

#define BUFFSIZE (4 * 1024 * 1024)

static uchar Buff[BUFFSIZE];

static int RecvLineCheck(SockStream_t *ss, char *expectedLine)
{
	char *line = SockRecvLine(ss, 100);
	int ret;

	ret = !strcmp(line, expectedLine);
	memFree(line);

	cout("expectedLine: %s -> %d\n", expectedLine, ret);

	if(!ret)
		cout("★★★応答エラー！\n");

	return ret;
}
static void Upload(SockStream_t *ss)
{
	autoList_t *files;
	char *file;
	uint index;

	LOGPOS();
	files = lsFiles(ClientDir);
	LOGPOS();
	sortJLinesICase(files);
	LOGPOS();

	foreach(files, file, index)
	{
		uint64 fileSize;
		FILE *fp;

		LOGPOS();

		if(!RecvLineCheck(ss, "READY-NEXT-FILE"))
			goto netError;

		LOGPOS();
		fileSize = getFileSize(file);
		LOGPOS();
		SockSendValue(ss, 1);
		LOGPOS();
		SockSendValue64(ss, fileSize);
		LOGPOS();

		if(!RecvLineCheck(ss, "READY-FILE-ENTITY"))
			goto netError;

		LOGPOS();
		SockSendLine(ss, getLocal(file));
		LOGPOS();
		fp = fileOpen(file, "rb");

		while(0ui64 < fileSize)
		{
			uint sendSize = (uint)m_min((uint64)BUFFSIZE, fileSize);
			autoBlock_t gab;

			fileRead(fp, gndBlockVar(Buff, sendSize, gab));
			SockSendBlock(ss, Buff, sendSize);
			fileSize -= sendSize;
		}
		fileClose(fp);
		LOGPOS();
		SockSendChar(ss, 'E');
		SockFlush(ss);
		LOGPOS();

		if(!RecvLineCheck(ss, "RECV-FILE-COMPLETED"))
			goto netError;

		LOGPOS();
		removeFile(file);
		LOGPOS();
	}
	LOGPOS();

	if(!RecvLineCheck(ss, "READY-NEXT-FILE"))
		goto netError;

	LOGPOS();
	SockSendValue(ss, 0);
	LOGPOS();

	if(!RecvLineCheck(ss, "OK"))
		goto netError;

	cout("+--------------------------------+\n");
	cout("| アップロードは全て成功しました |\n");
	cout("+--------------------------------+\n");

netError:
	LOGPOS();
	releaseDim(files, 1);
	LOGPOS();
}
static void Download(SockStream_t *ss)
{
	int errorFlag = 0;

	LOGPOS();

	for(; ; )
	{
		uint64 fileSize;
		char *name;
		char *file;
		FILE *fp;

		LOGPOS();

		if(!SockRecvValue(ss))
		{
			cout("+--------------------------------+\n");
			cout("| ダウンロードは全て成功しました |\n");
			cout("+--------------------------------+\n");
			break;
		}
		LOGPOS();
		fileSize = SockRecvValue64(ss);
		cout("fileSize: %I64u\n", fileSize);
		name = SockRecvLine(ss, 100);
		name = lineToFairLocalPath_x(name, strlen(ClientDir));
		cout("name: %s\n", name);
		file = combine(ClientDir, name);
		cout("file: %s\n", file);
		fp = fileOpen(file, "wb");
		LOGPOS();

		while(0ui64 < fileSize)
		{
			uint recvSize = (uint)m_min((uint64)BUFFSIZE, fileSize);
			autoBlock_t gab;

			if(!SockRecvBlock(ss, Buff, recvSize))
			{
				cout("★★★ファイルデータ受信エラー！\n");
				errorFlag = 1;
				break;
			}
			writeBinaryBlock(fp, gndBlockVar(Buff, recvSize, gab));
			fileSize -= recvSize;
		}
		LOGPOS();
		fileClose(fp);
		LOGPOS();

		if(SockRecvChar(ss) != 'D')
		{
			cout("★★★ファイル終端符受信エラー！\n");
			errorFlag = 1;
		}
		LOGPOS();

		if(errorFlag)
		{
			LOGPOS();
			removeFile(file);
			LOGPOS();
		}
		LOGPOS();
		memFree(file);
		memFree(name);
		LOGPOS();

		if(errorFlag)
			break;

		LOGPOS();
		SockSendChar(ss, 'C');
		SockFlush(ss);
		LOGPOS();
	}
	LOGPOS();
}
static int Perform(int sock, uint prm)
{
	SockStream_t *ss = CreateSockStream(sock, 3600); // ファイル送受信があるので、長めに

	LOGPOS();

	SockSendToken(ss, COMMAND_PREFIX);
	SockSendLine(ss, Command);
	SockSendLine(ss, Lane);

	LOGPOS();

	if(Command[0] == 'u')
	{
		Upload(ss);
	}
	else // d
	{
		Download(ss);
	}
	LOGPOS();

	ReleaseSockStream(ss);
	return 1;
}
int main(int argc, char **argv)
{
	Server    = nextArg();
	PortNo    = toValue(nextArg());
	Command   = nextArg();
	Lane      = nextArg();
	ClientDir = nextArg();

	errorCase(m_isEmpty(Server));
	errorCase(m_isRange(PortNo, 1, 65535));
	errorCase(m_isEmpty(Command));
	errorCase(m_isEmpty(Lane));
	errorCase(!existDir(ClientDir));

	LOGPOS();

	if(!SClient(Server, PortNo, Perform, 0))
		cout("★★★接続エラー！\n");

	LOGPOS();
}
