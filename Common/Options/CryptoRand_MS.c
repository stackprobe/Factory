#include "CryptoRand_MS.h"

void createKeyContainer(void)
{
	HCRYPTPROV hp;

	if(CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET)) // エラー無視
		CryptReleaseContext(hp, 0);
}
void deleteKeyContainer(void)
{
	HCRYPTPROV hp;

	CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_DELETEKEYSET); // エラー無視
}
void getCryptoBlock_MS(uchar *buffer, uint size)
{
	HCRYPTPROV hp;

	cout("Read sequence of %u bytes from 'CryptGenRandom' function.\n", size);

	if(!CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, 0) &&
		(GetLastError() != NTE_BAD_KEYSET ||
			(cout("Create key container.\n"),
			!CryptAcquireContext(&hp, 0, 0, PROV_RSA_FULL, CRYPT_NEWKEYSET))))
		error();

	if(!CryptGenRandom(hp, size, buffer))
	{
		CryptReleaseContext(hp, 0);
		error();
	}
	CryptReleaseContext(hp, 0);
}
autoBlock_t *makeCryptoBlock_MS(uint count)
{
	autoBlock_t *block = nobCreateBlock(count);

	getCryptoBlock_MS(directGetBuffer(block), count);
	return block;
}
