/*
	oCClient.exe mimiko o
	oCClient.exe mimiko C
*/

#include "C:\Factory\Common\Options\SClient.h"
#include "oCDefine.h"

static int Perform(int sock, uint prm)
{
	SockStream_t *ss = CreateSockStream(sock, 3600); // oto�̎�M(�I��DL,z9���Ă���Ԃ̑҂�)������̂ŁA���߂�
	char *command = (char *)prm;

	LOGPOS();

	SockSendToken(ss, COMMAND_PREFIX);
	SockSendLine(ss, command);

	LOGPOS();

	if(command[0] == 'o')
	{
		FILE *fp = fileOpen("oto.clu", "wb");
//		FILE *fp = fileOpen("oto.clu.gz.enc", "wb");

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

	sockConnectTimeoutSec = 2; // ���[�J�����Ȃ̂�

	if(argIs("/P"))
	{
		portNo = toValue(nextArg());
	}
	server  = nextArg();
	command = nextArg();

	LOGPOS();
	errorCase_m(!SClient(server, portNo, Perform, (uint)command), "�ڑ��o���Ȃ������l�ł��B");
	LOGPOS();
}
