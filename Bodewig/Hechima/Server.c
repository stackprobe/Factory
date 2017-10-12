/*
	使い方

		Server.exe 52525
		crypTunnel.exe 52255 localhost 52525 aa9999[x22]

		クライアント側は、サーバーの 52255 ポートに、パスワード aa9999[x22] で接続する。
*/

#include "C:\Factory\Common\Options\SockServerTh.h"
#include "C:\Factory\Common\Options\SockStream.h"

#define COMMAND_LENMAX 50
#define SVAL64_LENMAX 30
#define NAME_LENMAX 100
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
static char *GetIdent(char *name, char *ip)
{
	return xcout("%s @ %s", name, ip);
}
static uint GetNextRemarkIndex(uint64 knownStamp)
{
	uint n = 0;
	uint f = getCount(Remarks);

	while(n < f)
	{
		uint m = (n + f) / 2;
		Remark_t *i;

		i = (Remark_t *)getElement(Remarks, m);

		if(i->Stamp == knownStamp)
			return m + 1;

		if(i->Stamp < knownStamp)
			n = m + 1;
		else
			f = m;
	}
	return n;
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
		uint64 stamp2 = ((Remark_t *)getElement(Remarks, getCount(Remarks) - 1))->Stamp + 1ui64;

		m_maxim(stamp, stamp2);
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

		for(index = GetNextRemarkIndex(stamp); index < getCount(Remarks); index++)
		{
			Remark_t *i = (Remark_t *)getElement(Remarks, index);

			addElement(lines, (uint)xcout("%I64u", i->Stamp));
			addElement(lines, (uint)strx(i->Ident));
			addElement(lines, (uint)strx(i->Message));
		}
		foreach(lines, line, index)
			SockSendLine(ss, line);

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
	sockServerTh(PerformTh, portNo, 10, IdleTh);
	LOGPOS();
	releaseDim_BR(Remarks, 1, ReleaseRemark);
	LOGPOS();
}
