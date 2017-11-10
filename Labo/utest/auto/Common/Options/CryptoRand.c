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
	autoBlock_t *seeds[16];
	autoBlock_t *rSeeds[16];

	seeds[0] = readBinary("C:\\Factory\\tmp\\CSeed.dat");
	seeds[1] = readBinary("C:\\Factory\\tmp\\CSeedEx_0.dat");
	seeds[2] = readBinary("C:\\Factory\\tmp\\CSeedEx_1.dat");
	seeds[3] = readBinary("C:\\Factory\\tmp\\CSeedEx_2.dat");
	seeds[4] = readBinary("C:\\Factory\\tmp\\CSeedEx_3.dat");
	seeds[5] = readBinary("C:\\Factory\\tmp\\CSeedEx_4.dat");
	seeds[6] = readBinary("C:\\Factory\\tmp\\CSeedEx_5.dat");
	seeds[7] = readBinary("C:\\Factory\\tmp\\CSeedEx_6.dat");
	seeds[8] = readBinary("C:\\Factory\\tmp\\CSeedEx_7.dat");
	seeds[9] = readBinary("C:\\Factory\\tmp\\CSeedEx_8.dat");
	seeds[10] = readBinary("C:\\Factory\\tmp\\CSeedEx_9.dat");
	seeds[11] = readBinary("C:\\Factory\\tmp\\CSeedEx_10.dat");
	seeds[12] = readBinary("C:\\Factory\\tmp\\CSeedEx_11.dat");
	seeds[13] = readBinary("C:\\Factory\\tmp\\CSeedEx_12.dat");
	seeds[14] = readBinary("C:\\Factory\\tmp\\CSeedEx_13.dat");
	seeds[15] = readBinary("C:\\Factory\\tmp\\CSeedEx_14.dat");

	errorCase(getSize(seeds[0]) != 4096);
	errorCase(getSize(seeds[1]) != 4096);
	errorCase(getSize(seeds[2]) != 4096);
	errorCase(getSize(seeds[3]) != 4096);
	errorCase(getSize(seeds[4]) != 4096);
	errorCase(getSize(seeds[5]) != 4096);
	errorCase(getSize(seeds[6]) != 4096);
	errorCase(getSize(seeds[7]) != 4096);
	errorCase(getSize(seeds[8]) != 4096);
	errorCase(getSize(seeds[9]) != 4096);
	errorCase(getSize(seeds[10]) != 4096);
	errorCase(getSize(seeds[11]) != 4096);
	errorCase(getSize(seeds[12]) != 4096);
	errorCase(getSize(seeds[13]) != 4096);
	errorCase(getSize(seeds[14]) != 4096);
	errorCase(getSize(seeds[15]) != 4096);

	coExecute(CRAND_B_EXE " 1");

	IncrementSeed(seeds[0]);
	IncrementSeed(seeds[1]);
	IncrementSeed(seeds[2]);
	IncrementSeed(seeds[3]);
	IncrementSeed(seeds[4]);
	IncrementSeed(seeds[5]);
	IncrementSeed(seeds[6]);
	IncrementSeed(seeds[7]);
	IncrementSeed(seeds[8]);
	IncrementSeed(seeds[9]);
	IncrementSeed(seeds[10]);
	IncrementSeed(seeds[11]);
	IncrementSeed(seeds[12]);
	IncrementSeed(seeds[13]);
	IncrementSeed(seeds[14]);
	IncrementSeed(seeds[15]);

	rSeeds[0] = readBinary("C:\\Factory\\tmp\\CSeed.dat");
	rSeeds[1] = readBinary("C:\\Factory\\tmp\\CSeedEx_0.dat");
	rSeeds[2] = readBinary("C:\\Factory\\tmp\\CSeedEx_1.dat");
	rSeeds[3] = readBinary("C:\\Factory\\tmp\\CSeedEx_2.dat");
	rSeeds[4] = readBinary("C:\\Factory\\tmp\\CSeedEx_3.dat");
	rSeeds[5] = readBinary("C:\\Factory\\tmp\\CSeedEx_4.dat");
	rSeeds[6] = readBinary("C:\\Factory\\tmp\\CSeedEx_5.dat");
	rSeeds[7] = readBinary("C:\\Factory\\tmp\\CSeedEx_6.dat");
	rSeeds[8] = readBinary("C:\\Factory\\tmp\\CSeedEx_7.dat");
	rSeeds[9] = readBinary("C:\\Factory\\tmp\\CSeedEx_8.dat");
	rSeeds[10] = readBinary("C:\\Factory\\tmp\\CSeedEx_9.dat");
	rSeeds[11] = readBinary("C:\\Factory\\tmp\\CSeedEx_10.dat");
	rSeeds[12] = readBinary("C:\\Factory\\tmp\\CSeedEx_11.dat");
	rSeeds[13] = readBinary("C:\\Factory\\tmp\\CSeedEx_12.dat");
	rSeeds[14] = readBinary("C:\\Factory\\tmp\\CSeedEx_13.dat");
	rSeeds[15] = readBinary("C:\\Factory\\tmp\\CSeedEx_14.dat");

	errorCase(!isSameBlock(seeds[0], rSeeds[0]));
	errorCase(!isSameBlock(seeds[1], rSeeds[1]));
	errorCase(!isSameBlock(seeds[2], rSeeds[2]));
	errorCase(!isSameBlock(seeds[3], rSeeds[3]));
	errorCase(!isSameBlock(seeds[4], rSeeds[4]));
	errorCase(!isSameBlock(seeds[5], rSeeds[5]));
	errorCase(!isSameBlock(seeds[6], rSeeds[6]));
	errorCase(!isSameBlock(seeds[7], rSeeds[7]));
	errorCase(!isSameBlock(seeds[8], rSeeds[8]));
	errorCase(!isSameBlock(seeds[9], rSeeds[9]));
	errorCase(!isSameBlock(seeds[10], rSeeds[10]));
	errorCase(!isSameBlock(seeds[11], rSeeds[11]));
	errorCase(!isSameBlock(seeds[12], rSeeds[12]));
	errorCase(!isSameBlock(seeds[13], rSeeds[13]));
	errorCase(!isSameBlock(seeds[14], rSeeds[14]));
	errorCase(!isSameBlock(seeds[15], rSeeds[15]));
}
static void DoTest_01(void)
{
	uint c;

	LOGPOS();

	for(c = 0; c < 1000; c++)
	{
		cout("%u\n", c);

		DoTest_01_2();
	}
	LOGPOS();
}
static void AddToCr2(autoBlock_t *cr2, autoBlock_t *seeds[16], uint v1, uint v1_on, uint v2, uint v2_on)
{
	autoBlock_t *text = newBlock();
	uint index;

	for(index = 0; index < 16; index++)
	{
		if(index)
		{
			if(v1_on) addByte(text, v1);
			if(v2_on) addByte(text, v2);
		}
		ab_addBytes(text, seeds[index]);
	}
	sha512_makeHashBlock(text);
	releaseAutoBlock(text);

	ab_addBlock(cr2, sha512_hash, 50);
}
static void DoTest_02(void)
{
	autoBlock_t *seeds[16];
	autoBlock_t *cr1;
	autoBlock_t *cr2;
	uint val;

	LOGPOS();

#define CR_FILE "C:\\Factory\\tmp\\cr.tmp"

	removeFileIfExist(CR_FILE);
	coExecute(CRAND_B_EXE " 13000 " CR_FILE);
	cr1 = readBinary(CR_FILE);
	removeFile(CR_FILE);

#undef CR_FILE

	cr2 = newBlock();

	// seed の読み込みは、読み込み -> increment -> 書き出し -> 使う
	// なので後から読み込まないとダメ

	seeds[0] = readBinary("C:\\Factory\\tmp\\CSeed.dat");
	seeds[1] = readBinary("C:\\Factory\\tmp\\CSeedEx_0.dat");
	seeds[2] = readBinary("C:\\Factory\\tmp\\CSeedEx_1.dat");
	seeds[3] = readBinary("C:\\Factory\\tmp\\CSeedEx_2.dat");
	seeds[4] = readBinary("C:\\Factory\\tmp\\CSeedEx_3.dat");
	seeds[5] = readBinary("C:\\Factory\\tmp\\CSeedEx_4.dat");
	seeds[6] = readBinary("C:\\Factory\\tmp\\CSeedEx_5.dat");
	seeds[7] = readBinary("C:\\Factory\\tmp\\CSeedEx_6.dat");
	seeds[8] = readBinary("C:\\Factory\\tmp\\CSeedEx_7.dat");
	seeds[9] = readBinary("C:\\Factory\\tmp\\CSeedEx_8.dat");
	seeds[10] = readBinary("C:\\Factory\\tmp\\CSeedEx_9.dat");
	seeds[11] = readBinary("C:\\Factory\\tmp\\CSeedEx_10.dat");
	seeds[12] = readBinary("C:\\Factory\\tmp\\CSeedEx_11.dat");
	seeds[13] = readBinary("C:\\Factory\\tmp\\CSeedEx_12.dat");
	seeds[14] = readBinary("C:\\Factory\\tmp\\CSeedEx_13.dat");
	seeds[15] = readBinary("C:\\Factory\\tmp\\CSeedEx_14.dat");

	AddToCr2(cr2, seeds, 0x00, 0, 0x00, 0);

	for(val = 0x00; val <= 0xff; val++)
	{
		AddToCr2(cr2, seeds, val, 1, 0x00, 0);
	}
	AddToCr2(cr2, seeds, 0x00, 1, 0x00, 1);
	AddToCr2(cr2, seeds, 0x01, 1, 0x00, 1);
	AddToCr2(cr2, seeds, 0x02, 1, 0x00, 1);

//writeBinary("1.bin", cr1); // test
//writeBinary("2.bin", cr2); // test

	errorCase(!isSameBlock(cr1, cr2));

	releaseAutoBlock(cr1);
	releaseAutoBlock(cr2);

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
