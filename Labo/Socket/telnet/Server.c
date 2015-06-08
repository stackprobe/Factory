#include "C:\Factory\Common\Options\SockServer.h"

static FILE *RecvFp;
static uint TimeoutSec;
static uint ConnectCount;

typedef struct Info_st
{
	autoBlock_t *RecvQueue;
	autoBlock_t *SendQueue;
	uint Timeout;
}
Info_t;

static void DispRecvData(Info_t *i)
{
	if(getSize(i->RecvQueue))
	{
		char *line = toPrintLine(i->RecvQueue, 1);

		cout("%08x > %s\n", i, line);
		memFree(line);

		if(RecvFp)
			writeBinaryBlock(RecvFp, i->RecvQueue);

		setSize(i->RecvQueue, 0);
	}
}

static void *CreateInfo(void)
{
	Info_t *i = (Info_t *)memAlloc(sizeof(Info_t));

	cout("%08x Ú‘±\n", i);

	ConnectCount++;
	execute_x(xcout("TITLE Server - Connect %u", ConnectCount));

	i->RecvQueue = newBlock();
	i->SendQueue = newBlock();
	i->Timeout = TimeoutSec ? now() + TimeoutSec : 0;

	return i;
}
static void ReleaseInfo(void *vi)
{
	Info_t *i = (Info_t *)vi;

	cout("%08x Ø’f\n", i);

	ConnectCount--;
	execute_x(xcout("TITLE Server - Connect %u", ConnectCount));

	DispRecvData(i);

	releaseAutoBlock(i->RecvQueue);
	releaseAutoBlock(i->SendQueue);

	memFree(i);
}

static int StopServer;

static int Perform(int sock, void *vi)
{
	Info_t *i = (Info_t *)vi;

	if(StopServer)
		return 0;

	if(SockRecvSequ(sock, i->RecvQueue, 1) == -1)
		return 0;

	DispRecvData(i);

	if(SockSendSequ(sock, i->SendQueue, 0) == -1)
		return 0;

	while(hasKey())
	{
		int chr = getKey();

		if(chr == 0x1b)
		{
			StopServer = 1;
			return 0;
		}
		if(chr == 'D')
			return 0;

		if(chr == 'I')
		{
			execute("TITLE Server - Input");

			ab_addLine_x(i->SendQueue, coInputLine());

			// CR-LF
			addByte(i->SendQueue, '\r');
			addByte(i->SendQueue, '\n');

			execute("TITLE Server");
		}
	}
	return !i->Timeout || now() <= i->Timeout;
}
static int Idle(void)
{
	if(!ConnectCount)
	{
		while(hasKey())
		{
			if(getKey() == 0x1b)
			{
				StopServer = 1;
			}
		}
	}
	return !StopServer;
}
int main(int argc, char **argv)
{
	uint portno = 23;

readArgs:
	if(argIs("/R"))
	{
		RecvFp = fileOpen(nextArg(), "wb");
		goto readArgs;
	}
	if(argIs("/T"))
	{
		TimeoutSec = toValue(nextArg());
		goto readArgs;
	}

	if(hasArgs(1))
	{
		portno = toValue(nextArg());
	}
	sockServerUserTransmit(Perform, CreateInfo, ReleaseInfo, portno, 10, Idle);

	if(RecvFp)
	{
		fileClose(RecvFp);
	}
}
