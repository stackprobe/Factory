/*
	MD5Cracker.exe [/F md5リストファイル | md5]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5.h"

static void Hashes_HexToBin(autoList_t *hashes)
{
	char *hash;
	uint index;

	foreach(hashes, hash, index)
	{
		autoBlock_t *bHash;

		cout("INPUT_HASH: %s\n", hash);

		errorCase(!lineExp("<32,09AFaf>", hash));

		bHash = ab_fromHexLine(hash);
		setElement(hashes, index, (uint)bHash); // g
	}
}
static sint Comp_Hash(uint v1, uint v2)
{
	autoBlock_t *h1 = (autoBlock_t *)v1;
	autoBlock_t *h2 = (autoBlock_t *)v2;

	return memcmp(b(h1), b(h2), 16);
}
static void NextPw(autoBlock_t *pw)
{
	uint index;

	for(index = 0; index < getSize(pw); index++)
	{
		if(b(pw)[index] < 0xff)
		{
			b(pw)[index]++;
			return;
		}
		b(pw)[index] = 0x00;
	}
	addByte(pw, 0x00);

	cout("pw_size: %u\n", getSize(pw));
}
static void MD5Crack_List(autoList_t *hashes)
{
	autoBlock_t *pw = newBlock();

	Hashes_HexToBin(hashes);
	distinct2(hashes, Comp_Hash, noop_u); // g

	for(; ; )
	{
		autoBlock_t *pwHash = md5_makeHashBlock(pw);

		if(binSearch(hashes, (uint)pwHash, Comp_Hash) < getCount(hashes))
		{
			char *pwHex = makeHexLine(pw);
			char *pwHashHex = makeHexLine(pwHash);

			cout("pw_hash: %s\n", pwHashHex);
			cout("pw: %s\n", pwHex);

			// todo

			memFree(pwHex);
			memFree(pwHashHex);
			goto endLoop;
		}
		releaseAutoBlock(pwHash);

		NextPw(pw);
	}
endLoop:
	releaseAutoBlock(pw);
}
static void MD5Crack(char *hash)
{
	autoList_t gal;
	uint box;

	MD5Crack_List(gndOneElementVar((uint)hash, box, gal));
}
int main(int argc, char **argv)
{
	if(argIs("/F"))
	{
		MD5Crack_List(readLines(nextArg())); // g
		return;
	}

	if(hasArgs(1))
	{
		MD5Crack(nextArg());
		return;
	}
	else
	{
		MD5Crack_List(readLines(c_dropFile())); // g
		return;
	}
}
