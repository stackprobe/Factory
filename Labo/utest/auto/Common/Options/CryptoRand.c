#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CryptoRand.h"
#include "C:\Factory\OpenSource\sha512.h"

#define CRAND_B_EXE "C:\\Factory\\Labo\\Tools\\CryptoRand_B.exe"

static void IncrementSeed(autoBlock_t *seed)
{
	uint val = 1;
	uint index;

	for(index = 0; index < getSize(seed); index++)
	{
		val += getByte(seed, index);
		setByte(seed, index, val & 0xff);
		val >>= 8;
	}
}
static void DoTest_01_2(void)
{
	autoBlock_t *seed;
	autoBlock_t *seed2;
	autoBlock_t *rSeed;
	autoBlock_t *rSeed2;

	seed  = readBinary("C:\\Factory\\tmp\\CSeed.dat");
	seed2 = readBinary("C:\\Factory\\tmp\\CSeedEx.dat");

	errorCase(getSize(seed)  != 4096);
	errorCase(getSize(seed2) != 64);

	coExecute(CRAND_B_EXE " 1");

	IncrementSeed(seed);
	IncrementSeed(seed2);

	rSeed  = readBinary("C:\\Factory\\tmp\\CSeed.dat");
	rSeed2 = readBinary("C:\\Factory\\tmp\\CSeedEx.dat");

	errorCase(!isSameBlock(seed,  rSeed));
	errorCase(!isSameBlock(seed2, rSeed2));

	releaseAutoBlock(seed);
	releaseAutoBlock(seed2);
	releaseAutoBlock(rSeed);
	releaseAutoBlock(rSeed2);
}
static void DoTest_01(void)
{
	uint c;

	LOGPOS();

	for(c = 0; c < 550; c++)
	{
		cout("%u\n", c);

		DoTest_01_2();
	}
	LOGPOS();
}
static void AddToCr2(autoBlock_t *cr2, autoBlock_t *seed, uint v1, uint v2, uint v_num, autoBlock_t *seed2)
{
	autoBlock_t *text = newBlock();

	ab_addBytes(text, seed);

	if(1 <= v_num)
		addByte(text, v1);

	if(2 <= v_num)
		addByte(text, v2);

	ab_addBytes(text, seed2);
	sha512_makeHashBlock(text);
	releaseAutoBlock(text);

	ab_addBlock(cr2, sha512_hash, 63);
}
static void DoTest_02_2(void)
{
	autoBlock_t *seed;
	autoBlock_t *seed2;
	autoBlock_t *cr1;
	autoBlock_t *cr2;
	uint val;

	LOGPOS();

#define CR_FILE "C:\\Factory\\tmp\\cr.tmp"

	removeFileIfExist(CR_FILE);
	coExecute(CRAND_B_EXE " 16380 " CR_FILE);
	cr1 = readBinary(CR_FILE);
	removeFile(CR_FILE);

#undef CR_FILE

	cr2 = newBlock();

	// seed の読み込みは、読み込み -> increment -> 書き出し -> 使う
	// なので後から読み込まないとダメ
	seed  = readBinary("C:\\Factory\\tmp\\CSeed.dat");
	seed2 = readBinary("C:\\Factory\\tmp\\CSeedEx.dat");

	AddToCr2(cr2, seed, 0x00, 0x00, 0, seed2);

	for(val = 0x00; val <= 0xff; val++)
	{
		AddToCr2(cr2, seed, val, 0x00, 1, seed2);
	}
	AddToCr2(cr2, seed, 0x00, 0x00, 2, seed2);
	AddToCr2(cr2, seed, 0x01, 0x00, 2, seed2);
	AddToCr2(cr2, seed, 0x02, 0x00, 2, seed2);

//writeBinary("1.bin", cr1); // test
//writeBinary("2.bin", cr2); // test

	errorCase(!isSameBlock(cr1, cr2));

	releaseAutoBlock(cr1);
	releaseAutoBlock(cr2);

	LOGPOS();
}
static void DoTest_02(void)
{
	uint c;

	LOGPOS();

	for(c = 0; c < 10; c++)
	{
		cout("%u\n", c);

		DoTest_02_2();
	}
	LOGPOS();
}
static void DoTest(void)
{
	coExecute(CRAND_B_EXE " 1");

	DoTest_01();
	DoTest_02();
}
int main(int argc, char **argv)
{
	DoTest();
}
