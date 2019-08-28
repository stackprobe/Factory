#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CryptoRand.h"

static void Test_01(void)
{
	uint c;

	for(c = 0; c < 1000; c++)
	{
		cout("%08x %08x %08x %016I64x\n"
			,getCryptoRand16()
			,getCryptoRand24()
			,getCryptoRand()
			,getCryptoRand64()
			);
	}
}
int main(int argc, char **argv)
{
	Test_01();
}
