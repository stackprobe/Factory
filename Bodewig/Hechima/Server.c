/*
	使い方

		Server.exe 52525
		crypTunnel.exe 52255 localhost 52525 /C 10 /R *aa9999[x22]

		クライアント側は、サーバーの 52255 ポートに、パスワード aa9999[x22] で接続する。
*/

#include "C:\Factory\Common\Options\SockServerTh.h"
#include "C:\Factory\Common\Options\SockStream.h"

#define REMARKS_SAVE_FILE "C:\\appdata\\HechimaServerRemarks.txt"

#define COMMAND_LENMAX 50
#define SVAL64_LENMAX 30
#define NAME_LENMAX 100
#define IDENT_LENMAX (NAME_LENMAX + 30)
#define MESSAGE_LENMAX 1024

#define REMARK_MAX 1000
#define REMARK_CLEAR_PCT 20
#define REMARK_CLEAR_NUM ((REMARK_MAX * REMARK_CLEAR_PCT) / 100)

typedef struct Remark_st
{
	uint64 Stamp;
	char *Ident;
	char *Message;
}
Remark_t;

static autoList_t *Remarks;

static void ReleaseRemark(Remark_t *i)
{
	memFree(i->Ident);
	memFree(i->Message);
	memFree(i);
}
static void SaveRemarks(void)
{
	FILE *fp = fileOpen(REMARKS_SAVE_FILE, "wt");
	Remark_t *i;
	uint index;

	foreach(Remarks, i, index)
	{
		writeLine_x(fp, xcout("%I64u", i->Stamp));
		writeLine(fp, i->Ident);
		writeLine(fp, i->Message);
	}
	fileClose(fp);
}
static void LoadRemarks(void)
{
	if(existFile(REMARKS_SAVE_FILE))
	{
		FILE *fp = fileOpen(REMARKS_SAVE_FILE, "rt");

		for(; ; )
		{
			char *line = readLine(fp);
			Remark_t *i;

			if(!line)
				break;

			errorCase(REMARK_MAX + 10 < getCount(Remarks)); // + margin <

			i = nb(Remark_t);
			i->Stamp = toValue64_x(line);
			i->Ident = nnReadLine(fp);
			i->Message = nnReadLine(fp);

			// adjust {

			m_range(i->Stamp, 10000101000000ui64, 99991231235959ui64);

			if(getCount(Remarks))
			{
				uint64 nxtLwStmp = ((Remark_t *)getLastElement(Remarks))->Stamp + 1ui64;

				m_maxim(i->Stamp, nxtLwStmp);
			}

			setStrLenMax(i->Ident, IDENT_LENMAX);
			line2JLine(i->Ident, 1, 0, 0, 1); // " @ " があるので、okSpc == 1

			setStrLenMax(i->Message, MESSAGE_LENMAX);
			line2JLine(i->Message, 1, 0, 0, 1);
			trimEdge(i->Message, ' ');

			// }

			addElement(Remarks, (uint)i);
		}
		fileClose(fp);
	}
}
static char *GetIdent(char *name, char *ip)
{
	return xcout("%s @ %s", name, ip);
}
static sint RemarkStampComp(uint v1, uint v2)
{
	Remark_t *a = (Remark_t *)v1;
	Remark_t *b = (Remark_t *)v2;

	return m_simpleComp(a->Stamp, b->Stamp);
}
static uint GetKnownNextRemarkIndex(uint64 knownStamp)
{
	Remark_t ferret;

	ferret.Stamp = knownStamp;

	return findBoundLeftestRight(Remarks, (uint)&ferret, RemarkStampComp);
}
static uint64 GetStamp(void)
{
	return toValue64_x(makeCompactStamp(NULL));
}
static uint64 GetNextStamp(void)
{
	uint64 stamp = GetStamp();

	if(getCount(Remarks))
	{
		uint64 nxtLwStmp = ((Remark_t *)getLastElement(Remarks))->Stamp + 1ui64;

		m_maxim(stamp, nxtLwStmp);
	}
	return stamp;
}
static void PerformTh(int sock, char *ip)
{
	SockStream_t *ss = CreateSockStream(sock, 15);
	char *command;

	command = SockRecvLine(ss, COMMAND_LENMAX);
	line2JLine(command, 0, 0, 0, 0);
	cout("command: %s\n", command);

	if(!strcmp(command, "GET-REMARKS"))
	{
		char *name = SockRecvLine(ss, NAME_LENMAX);
		char *ident;
		uint64 stamp;
		uint index;
		autoList_t *lines = newList();
		char *line;

		cout("GET-REMARKS.1\n");

		line2JLine(name, 1, 0, 0, 0);

		ident = GetIdent(name, ip);
		stamp = toValue64_x(SockRecvLine(ss, SVAL64_LENMAX));

		cout("ident: %s\n", ident);
		cout("stamp: %I64u\n", stamp);

		index = GetKnownNextRemarkIndex(stamp);

		cout("range: %u - %u\n", index, getCount(Remarks));

		for(; index < getCount(Remarks); index++)
		{
			Remark_t *i = (Remark_t *)getElement(Remarks, index);

			addElement(lines, (uint)xcout("%I64u", i->Stamp));
			addElement(lines, (uint)strx(i->Ident));
			addElement(lines, (uint)strx(i->Message));
			addElement(lines, 0); // Ender
		}
		addElement(lines, 0); // Ender x2

		// Remarks 読み込み中にスレッドが切り替わらないように、全て読み込んでから SockSend* する。

		foreach(lines, line, index)
		{
			if(line)
				SockSendLine(ss, line);
			else
				SockSendChar(ss, 0xff); // Ender
		}
		SockFlush(ss);

		memFree(name);
		memFree(ident);
		releaseDim(lines, 1);

		cout("GET-REMARKS.2\n");
	}
	else if(!strcmp(command, "REMARK"))
	{
		char *name = SockRecvLine(ss, NAME_LENMAX);
		char *ident;
		uint64 stamp = GetNextStamp();
		char *message;
		Remark_t *i;

		cout("REMARK.1\n");

		line2JLine(name, 1, 0, 0, 0);

		ident = GetIdent(name, ip);
		message = SockRecvLine(ss, MESSAGE_LENMAX);

		line2JLine(message, 1, 0, 0, 1);
		trimEdge(message, ' ');

		cout("ident: %s\n", ident);
		cout("stamp: %I64u\n", stamp);
		cout("message: %s\n", message);

		i = (Remark_t *)memAlloc(sizeof(Remark_t));
		i->Ident = ident;
		i->Stamp = stamp;
		i->Message = message;

		addElement(Remarks, (uint)i);

		if(REMARK_MAX < getCount(Remarks))
		{
			uint index;

			for(index = 0; index < REMARK_CLEAR_NUM; index++)
				ReleaseRemark((Remark_t *)zSetElement(Remarks, index, 0));

			removeZero(Remarks);
		}
		memFree(name);

		cout("REMARK.2\n");
	}
	else
		cout("Unknown command!\n");

	ReleaseSockStream(ss);
	memFree(command);
}
static int IdleTh(void)
{
	while(hasKey())
	{
		if(getKey() == 0x1b)
			return 0;

		cout("ESCAPE-TO-STOP\n");
	}
	return 1;
}
int main(int argc, char **argv)
{
	uint portNo = toValue(nextArg());

	cout("へちまサーバー★ポート＝%u\n", portNo);

	LOGPOS();
	Remarks = newList();
	LOGPOS();
	LoadRemarks();
	LOGPOS();
	sockServerTh(PerformTh, portNo, 10, IdleTh);
	LOGPOS();
	SaveRemarks();
	LOGPOS();
	releaseDim_BR(Remarks, 1, ReleaseRemark);
	LOGPOS();
}
