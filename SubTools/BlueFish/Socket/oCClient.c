/*
	oCClient.exe mimiko o
	oCClient.exe mimiko C
*/

#include "C:\Factory\Common\Options\SClient.h"
#include "oCDefine.h"

static int Perform(int sock, uint prm)
{
	SockStream_t *ss = CreateSockStream(sock, 30);
	char *command = (char *)prm;

	LOGPOS();

	SockSendToken(ss, COMMAND_PREFIX);
	SockSendLine(ss, command);

	LOGPOS();

	if(command[0] == 'o')
	{
		FILE *fp = fileOpen("oto.clu.gz.enc", "wb");

		LOGPOS();

		for(; ; )
		{
			int chr = SockRecvChar(ss);

			if(chr == EOF)
				break;

			writeChar(fp, chr);
		}
		LOGPOS();

		fileClose(fp);
	}
	LOGPOS();

	ReleaseSockStream(ss);
	return 1;
}
int main(int argc, char **argv)
{
	char *server;
	char *command;
	uint portNo = PORTNO;

	if(argIs("/P"))
	{
		portNo = toValue(nextArg());
	}
	server  = nextArg();
	command = nextArg();

	LOGPOS();
	SClient(server, portNo, Perform, (uint)command);
	LOGPOS();
}
