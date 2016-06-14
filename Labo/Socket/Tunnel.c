#include "C:\Factory\Common\Options\SockClient.h"
#include "C:\Factory\Common\Options\SockServer.h"

static int DispBinaryMode;

static void DispData(autoBlock_t *block, int sock, char *title)
{
	if(getSize(block))
	{
		cout("%s: %d\n", title, sock);

		if(DispBinaryMode)
		{
			uint index;

			for(index = 0; index < getSize(block); index++)
			{
				cout("%02x", getByte(block, index));
			}
			cout("\n");
		}
		else
		{
			char *line = toPrintLine(block, 1);

			cout("%s\n", line);
			cout("\n");

			memFree(line);
		}
	}
}

static char *FwdDomain = "localhost";
static uint FwdPort = 80;
static uint SilentMode;

static void *CreateInfo(void)
{
	uchar ip[4];
	int sock;

	memset(ip, 0x00, 4);
	sock = sockConnect(ip, FwdDomain, FwdPort);

	cout("Ú‘±: %d\n", sock);
	return (void *)sock;
}
static void ReleaseInfo(void *vi)
{
	int sock = (int)vi;

	if(sock != -1)
		sockDisconnect(sock);

	cout("Ø’f: %d\n", sock);
}
static int Perform(int sock, void *vi)
{
	int fwdSock = (int)vi;
	int retval = 0;
	autoBlock_t *sendData = newBlock();
	autoBlock_t *recvData = newBlock();

	if(sock == -1) goto endfunc;

	if(SockRecvSequ(sock,    sendData, 1) == -1) goto endfunc;
	if(SockRecvSequ(fwdSock, recvData, 0) == -1) goto endfunc;

	if(!SilentMode) DispData(sendData, fwdSock, "ã‚è");
	if(!SilentMode) DispData(recvData, fwdSock, "‰º‚è");

	retval = 1;

	while(getSize(sendData) || getSize(recvData))
	{
		if(getSize(sendData) && SockSendSequ(fwdSock, sendData, 1) == -1)
		{
			setSize(sendData, 0);
			retval = 0;
		}
		if(getSize(recvData) && SockSendSequ(sock, recvData, 1) == -1)
		{
			setSize(recvData, 0);
			retval = 0;
		}
	}

endfunc:
	releaseAutoBlock(sendData);
	releaseAutoBlock(recvData);

	return retval;
}

static int Idle(void)
{
	while(hasKey())
	{
		if(getKey() == 0x1b)
		{
			return 0;
		}
	}
	return 1;
}
int main(int argc, char **argv)
{
	uint portno = 8080;
	uint connectmax = 10;

readArgs:
	// “]‘—æ
	if(argIs("/FD")) // Forward Domain
	{
		FwdDomain = nextArg();
		goto readArgs;
	}
	if(argIs("/FP")) // Forward Port
	{
		FwdPort = toValue(nextArg());
		goto readArgs;
	}

	// ‘Ò‚¿Žó‚¯
	if(argIs("/P")) // Port
	{
		portno = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/C")) // Connect max
	{
		connectmax = toValue(nextArg());
		goto readArgs;
	}

	if(argIs("/S")) // Silent mode
	{
		SilentMode = 1;
		goto readArgs;
	}
	if(argIs("/B")) // disp Binary mode
	{
		DispBinaryMode = 1;
		goto readArgs;
	}

	cmdTitle_x(xcout("Tunnel - %u to %s:%u (connectmax:%u)", portno, FwdDomain, FwdPort, connectmax));

	cout("ŠJŽn\n");
	SockStartup();
	sockServerUserTransmit(Perform, CreateInfo, ReleaseInfo, portno, connectmax, Idle);
	SockCleanup();
	cout("I—¹\n");
}
