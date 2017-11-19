#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CryptoRand_MS.h"

//#define BUFFERSIZE 1024
#define BUFFERSIZE (1024 * 64)
//#define BUFFERSIZE (1024 * 256)
//#define BUFFERSIZE (1024 * 1024)

int main(int argc, char **argv)
{
	static uchar buffer[BUFFERSIZE];
	uint c;

	for(c = toValue(nextArg()); c; )
	{
		uint size = m_min(c, BUFFERSIZE);

		getCryptoBlock_MS(buffer, size);

		c -= size;
	}
}
