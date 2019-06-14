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

#define BUFFSIZE 4000000

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

	addElement(files, (uint)NULL);

	foreach(files, file, index)
	{
		uint64 fileSize;
		FILE *fp;

		if(!RecvLineCheck(ss, "READY.1"))
			break;

		if(!file)
		{
			SockSendValue(ss, 0);
			break;
		}
		fileSize = getFileSize(file);
		LOGPOS();
		SockSendValue(ss, 1);
		LOGPOS();
		SockSendValue64(ss, fileSize);
		LOGPOS();

		if(!RecvLineCheck(ss, "READY.2"))
			break;

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

		if(!RecvLineCheck(ss, "READY.3"))
			break;

		LOGPOS();
		removeFile(file);
		LOGPOS();
	}
	LOGPOS();
	releaseDim(files, 1);
	LOGPOS();
}
static void Download(SockStream_t *ss)
{
	error(); // TODO
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
