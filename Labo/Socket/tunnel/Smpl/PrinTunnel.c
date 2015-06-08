/*
	Tunnel.exe RECV-PORT FWD-HOST FWD-PORT /S

		/S ... ’â~‚·‚éB

	Tunnel.exe RECV-PORT FWD-HOST FWD-PORT [/C CONNECT-MAX]

		CONNECT-MAX ... Å‘åÚ‘±”, È—ª‚Í 1000
*/

#include "..\libs\Tunnel.h"

static void PrintDataFltr(autoBlock_t *buff, uint header)
{
	char *pLine = strx("");

	if(getSize(buff))
	{
		uint index;

		for(index = 0; index < getSize(buff); index++)
		{
			int chr = getByte(buff, index);
			int pchr;

			pchr = chr;
			m_toHalf(pchr);
			pLine = addChar(pLine, pchr);

			if(chr == '\n')
				pLine = addChar(pLine, '\n');
		}
	}
	if(*pLine)
	{
		char *p = strchr(pLine, '\0') - 1;

		if(*p == '\n')
			*p = '\0';

		if(*pLine)
		{
			cout("%s\n", header);
			cout("%s\n", pLine);
		}
	}
	memFree(pLine);
}
static void Perform(int sock, int fwdSock)
{
	cout("Ú‘±\n");

	CrossChannel(sock, fwdSock, PrintDataFltr, (uint)"ã‚è", PrintDataFltr, (uint)"‰º‚è");

	cout("Ø’f\n");
}
static int ReadArgs(void)
{
	return 0;
}
int main(int argc, char **argv)
{
	TunnelMain(ReadArgs, Perform, "Tunnel", NULL);
}
