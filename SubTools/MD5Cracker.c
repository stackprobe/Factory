/*
	MD5Cracker.exe [/F md5リストファイル | md5]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5\md5.h"

#define HASH_SIZE 16

static autoList_t *HashTable;
static uint RemHashCount;

static void InitHashTable(autoList_t *lines)
{
	char *line;
	uint index;

	HashTable = newList();

	for(index = 0; index < 0x10000; index++)
		addElement(HashTable, (uint)newList());

	foreach(lines, line, index)
	{
		uchar *hash;
		uint segment;
		autoList_t *hashes;

		cout("INPUT_HASH: %s\n", line);

		errorCase(!lineExp("<32,09AFaf>", line));

		hash = (uchar *)unbindBlock(ab_fromHexLine(line));
		segment = (uint)*(uint16 *)hash;
		hashes = (autoList_t *)getElement(HashTable, segment);
		addElement(hashes, (uint)hash);
	}

	RemHashCount = getCount(lines);
	errorCase(!RemHashCount);
}
static void Found(uchar *msg, uint msgLen, uchar msgHash[HASH_SIZE])
{
	char *h1;
	char *h2;
	autoBlock_t gab;

	h1 = makeHexLine(gndBlockVar(msgHash, HASH_SIZE, gab));
	h2 = makeHexLine(gndBlockVar(msg, msgLen, gab));

	cout("%s = %s\n", h1, h2);

	memFree(h1);
	memFree(h2);

	if(!--RemHashCount)
		termination(0);
}
static void Check(uchar *msg, uint msgLen, uchar msgHash[HASH_SIZE])
{
	uint segment = (uint)*(uint16 *)msgHash;
	autoList_t *hashes;
	uchar *hash;
	uint index;

	hashes = (autoList_t *)getElement(HashTable, segment);

	foreach(hashes, hash, index)
	{
		if(!memcmp(hash, msgHash, HASH_SIZE))
		{
			Found(msg, msgLen, hash);

			desertElement(hashes, index);
			break;
		}
	}
}
static void Search(uchar *msg, uint msgLen, uint msgIdx, md5_CTX *baseCtx)
{
	if(msgIdx < msgLen)
	{
		uint count;

		for(count = 0x00; count <= 0xff; count++)
		{
			md5_CTX ctx = *baseCtx;

			msg[msgIdx] = count;

			md5_Update(&ctx, msg + msgIdx, 1);

			Search(msg, msgLen, msgIdx + 1, &ctx);
		}
	}
	else
	{
		md5_CTX ctx = *baseCtx;

		md5_Final(&ctx);

		Check(msg, msgLen, ctx.digest);
	}
}
static void MD5Crack_List(autoList_t *lines)
{
	uchar msg[100];
	uint msgLen;

	distinct2(lines, (sint (*)(uint, uint))_stricmp, (void (*)(uint))memFree); // MD5Crack()の場合は1つなので問題無いはず。

	InitHashTable(lines);

	for(msgLen = 0; ; msgLen++)
	{
		md5_CTX ctx;

		md5_Init(&ctx);

		cout("msgLen: %u\n", msgLen);

		Search(msg, msgLen, 0, &ctx);
	}
}
static void MD5Crack(char *line)
{
	autoList_t gal;
	uint box;

	MD5Crack_List(gndOneElementVar((uint)line, box, gal));
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
