#include "C:\Factory\Common\Options\SockClient.h"

static char *SendFile;
static autoBlock_t *SendData;
static char *RecvFile;
static uint TimeoutSec;

static void Perform(int sock)
{
	autoBlock_t *rq = newBlock();
	autoBlock_t *sq = newBlock();
	FILE *wfp = NULL;
	uint timeout = TimeoutSec ? TimeoutSec + now() : 0;

	if(SendFile)
	{
		ab_addBytes_x(sq, readBinary(SendFile));
	}
	if(SendData)
	{
		ab_addBytes(sq, SendData);
	}
	if(RecvFile)
	{
		wfp = fileOpen(RecvFile, "wb");
	}
	while(!timeout || now() < timeout)
	{
		if(getSize(rq))
		{
			char *line;

			if(wfp)
				writeBinaryBlock(wfp, rq);

			line = unbindBlock2Line(rq);

			line2JLine(line, 1, 1, 1, 1);
			cout("%s", line);
			memFree(line);

			rq = newBlock();
		}

		if(SockRecvSequ(sock, rq, 1) == -1)
			break;

		if(SockSendSequ(sock, sq, 0) == -1)
			break;

		if(hasKey())
//		if(sock_hasKey())
		{
			int chr;
			
			execute("TITLE Client - stdin");
			chr = getKey();
			execute("TITLE Client");

			if(chr == 0x1b)
				break;

			if(chr == 'I')
			{
				execute("TITLE Client - Input");

				ab_addLine_x(sq, coInputLine());

				// CR-LF
				addByte(sq, '\r');
				addByte(sq, '\n');

				execute("TITLE Client");
			}
			if(chr == 'i')
			{
				execute("TITLE Client - Input (no cr-lf)");

				ab_addLine_x(sq, coInputLine());

				execute("TITLE Client");
			}
		}
	}
	releaseAutoBlock(rq);
	releaseAutoBlock(sq);

	if(wfp)
	{
		fileClose(wfp);
	}
}
int main(int argc, char **argv)
{
	uchar ip[4];
	char *domain = "localhost";
	uint portno = 23;

readArgs:
	if(argIs("/S"))
	{
		SendFile = nextArg();
		goto readArgs;
	}
	if(argIs("/I"))
	{
		SendData = inputTextAsBinary();
		goto readArgs;
	}
	if(argIs("/R"))
	{
		RecvFile = nextArg();
		goto readArgs;
	}
	if(argIs("/T"))
	{
		TimeoutSec = toValue(nextArg());
		goto readArgs;
	}

	if(hasArgs(1))
	{
		domain = nextArg();
	}
	if(hasArgs(1))
	{
		portno = toValue(nextArg());
	}

	*(uint *)ip = 0;
	sockClientUserTransmit(ip, domain, portno, Perform);
}
