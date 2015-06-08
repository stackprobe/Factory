/*
	SendFile.exe [/T SOCK-TIMEOUT] [/-W] R-FILE W-FILE

		SOCK-TIMEOUT ... 通信タイムアウト [秒]
		/-W ... エラー時にダイアログを表示しない。(ブロックしない)

		R-FILE, W-FILE ...

			xxx\xxx.xxx
			C:\xxx\xxx.xxx
			host*xxx\xxx.xxx
			host:PORT*xxx\xxx.xxx
*/

#include "C:\Factory\Common\Options\SockClient.h"
#include "C:\Factory\Common\Options\SockStream.h"
#include "SendFileCommon.h"

#define LOCALPC_HOST "*"
#define DEF_SOCK_TIMEOUT_SEC 86400

static uint SockTimeoutSec = DEF_SOCK_TIMEOUT_SEC;

typedef struct FileInfo_st
{
	char *Host;
	uint PortNo;
	char *File;

	FILE *FP; // NULL == use Sock
	int Sock;
	SockStream_t *SS;
	uint64 FDSize;
	uint64 FDIndex;
}
FileInfo_t;

FileInfo_t RFI;
FileInfo_t WFI;

static void LoadFI(FileInfo_t *fi, char *remotePath)
{
	autoList_t *rpParts = tokenize(remotePath, '*');

	if(getCount(rpParts) == 2) // ? リモートパス
	{
		autoList_t *hostTkns = tokenize(getLine(rpParts, 0), ':');

		if(getCount(hostTkns) == 2) // ? ホスト名 + ポート番号
		{
			fi->Host = strx(getLine(hostTkns, 0));
			fi->PortNo = toValue(getLine(hostTkns, 1));
		}
		else // ? ホスト名のみ
		{
			fi->Host = strx(getLine(rpParts, 0));
			fi->PortNo = DEF_PORTNO;
		}
		fi->File = strx(getLine(rpParts, 1));
		releaseDim(hostTkns, 1);
	}
	else // ? ローカルパス
	{
		fi->Host = strx(LOCALPC_HOST);
		fi->PortNo = 1; // 使用しない。
		fi->File = strx(remotePath);
	}
	releaseDim(rpParts, 1);

	errorCase(m_isEmpty(fi->Host));
	errorCase(!fi->PortNo || 0xffff < fi->PortNo);
	errorCase(m_isEmpty(fi->File));
}
static void PrintFI(FileInfo_t *fi, char *prompt)
{
	cout("%s %s:%u %s\n", prompt, fi->Host, fi->PortNo, fi->File);
}
static void OpenStream(FileInfo_t *fi, int forWrite, uint64 readFileSize)
{
	if(!strcmp(fi->Host, LOCALPC_HOST))
	{
		if(!forWrite)
		{
			fi->FDSize = getFileSize(fi->File);
		}
		fi->FP = fileOpen(fi->File, forWrite ? "wb" : "rb");
	}
	else
	{
		uchar ip[4] = { 0 };

		SockStartup();
		fi->Sock = sockConnect(ip, fi->Host, fi->PortNo);
		fi->SS = CreateSockStream(fi->Sock, SockTimeoutSec);

		SockSendLine(fi->SS, SIGNATURE);
		SockSendLine(fi->SS, forWrite ? S_MODE_W : S_MODE_R);
		SockSendLine(fi->SS, fi->File);
		SockFlush(fi->SS);

		if(!forWrite)
		{
			fi->FDSize = toValue64(SockRecvLine(fi->SS, 20));
		}
		else
		{
			SockSendLine_x(fi->SS, xcout("%I64u", readFileSize));
		}
	}
}
static void CloseStream(FileInfo_t *fi)
{
	if(fi->FP)
	{
		fileClose(fi->FP);
		fi->FP = NULL;
	}
	else
	{
		ReleaseSockStream(fi->SS);
		sockDisconnect(fi->Sock);
		SockCleanup();
	}
}

static int ReadCharStream(FileInfo_t *fi)
{
	int chr;

	if(fi->FDSize <= fi->FDIndex)
		return EOF;

	fi->FDIndex++;

	if(fi->FP)
	{
		chr = readChar(fi->FP);
	}
	else
	{
		chr = SockRecvChar(fi->SS);
	}
	errorCase(chr == EOF);
	return chr;
}
static void WriteCharStream(FileInfo_t *fi, int chr)
{
	if(fi->FP)
	{
		writeChar(fi->FP, chr);
	}
	else
	{
		SockSendChar(fi->SS, chr);
	}
}

static void ErrorCloseStream(void)
{
	if(WFI.FP)
	{
		fileClose(WFI.FP);
		removeFile(WFI.File);
	}
}
static void ErrorNoWindow(void)
{
	exit(0);
}

int main(int argc, char **argv)
{
	if(argIs("/T"))
	{
		SockTimeoutSec = toValue(nextArg());
	}
	if(argIs("/-W"))
	{
		addFinalizer(ErrorNoWindow);
	}

	addFinalizer(ErrorCloseStream);

	LoadFI(&RFI, nextArg());
	LoadFI(&WFI, nextArg());

	PrintFI(&RFI, "<");
	PrintFI(&WFI, ">");

	OpenStream(&RFI, 0, 0);
	OpenStream(&WFI, 1, RFI.FDSize);

	for(; ; )
	{
		int chr = ReadCharStream(&RFI);

		if(chr == EOF)
			break;

		WriteCharStream(&WFI, chr);
	}
	if(!RFI.FP) // ? 入力元がネットワーク
	{
		char *buff = SockRecvLine(RFI.SS, strlen(S_ENDFILEDATA) + 1);
		errorCase(strcmp(buff, S_ENDFILEDATA));
		memFree(buff);
	}
	if(!WFI.FP) // ? 出力先がネットワーク
	{
		SockSendLine(WFI.SS, S_ENDFILEDATA);
		SockFlush(WFI.SS);

		{
			char *buff = SockRecvLine(WFI.SS, strlen(S_ENDRECV) + 1);
			errorCase(strcmp(buff, S_ENDRECV));
			memFree(buff);
		}
	}

	CloseStream(&RFI);
	CloseStream(&WFI);
}
