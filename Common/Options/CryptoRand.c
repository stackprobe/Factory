/*
	getCryptoByte()のバイト列:

		sha-512(b[0]){0 -> 32} + sha-512(b[1]){0 -> 32} + sha-512(b[2]){0 -> 32} + ...

	b:
		b[0] = s + x
		b[1] = s + c[0] + x
		b[2] = s + c[1] + x
		b[3] = s + c[2] + x
		...

	c:
		c[0] = 0x00
		c[1] = 0x01
		c[2] = 0x02
		...
		c[255] = 0xff
		c[256] = 0x00, 0x00
		c[257] = 0x01, 0x00
		c[258] = 0x02, 0x00
		...
		c[511] = 0xff, 0x00
		c[512] = 0x00, 0x01
		c[513] = 0x01, 0x01
		c[514] = 0x02, 0x01
		...
		c[65791] = 0xff, 0xff
		c[65792] = 0x00, 0x00, 0x00
		c[65793] = 0x01, 0x00, 0x00
		c[65794] = 0x02, 0x00, 0x00
		...

	s:
		GetCryptoSeed(){0 -> 4096}

	x:
		GetCryptoSeedEx(){0 -> 65536}

		★ {a -> b} = 添字 a から b の直前まで
*/

#include "CryptoRand.h"

#define SEED_DIR "C:\\Factory\\tmp"
#define SEED_FILE SEED_DIR "\\CSeed.dat"
#define SEEDEX_FILE SEED_DIR "\\CSeedEx.dat"

/*
	SHA512 の内部状態は 512 bit らしい？
	sizeof(SHA512_CTX) == 216 ？？？ -> 多分冗長なんだろう。。。
	仮に 256 バイトと見なす。
	クラスタサイズ 4096 に合わせたい。-> 4096 / 256 == 16
	1/e == 0.36787944*
	1/e ^ 16 == 0.000000112535*
	内部状態の 16 倍もあれば、内部状態をほぼ網羅できるだろう。
	256 * 16 == 4096
*/
#define SEEDSIZE 4096

/*
	ある内部状態から予測可能な短いカウンタを読ませて連続してダイジェストを生成したとき、
	そのダイジェスト列から内部状態を予測出来る？？？
	少なくとも、最初のダイジェストとそれ以降全てのダイジェストの組み合わせは一つしかない。
	内部状態は 2 ^ 512 しかない。-> ダイジェスト列は 2 ^ 512 通りしかない。
	tremorの後に追加のシードを読ませてダイジェスト列のパターン数を稼ぐ。
	ダイジェスト列は 2 ^ (512 + SEEDEXSIZE * 8) 通りになる。・・・はず。
*/
#define SEEDEXSIZE 65536

static void GetCryptoSeed(uchar *seed, uint seed_size, char *seed_file)
{
	if(isFactoryDirEnabled())
	{
		autoBlock_t gab;

		mutex();

		// zantei >

		if(existFile(seed_file) && getFileSize(seed_file) != (uint64)seed_size)
		{
			cout("#########################################################\n");
			cout("## SEED_FILE SIZE ERROR -- どっかに古い exe があるで！ ##\n");
			cout("#########################################################\n");

			removeFile(seed_file);
		}

		// < zantei

		if(existFile(seed_file))
		{
			FILE *fp;
			uint index;

			fp = fileOpen(seed_file, "rb");
			fileRead(fp, gndBlockVar(seed, seed_size, gab));
			fileClose(fp);

			for(index = 0; index < seed_size; index++)
			{
				if(seed[index] < 0xff)
				{
					seed[index]++;
					break;
				}
				seed[index] = 0x00;
			}
		}
		else
			getCryptoBlock_MS(seed, seed_size);

		writeBinary(seed_file, gndBlockVar(seed, seed_size, gab));
		unmutex();
	}
	else
		getCryptoBlock_MS(seed, seed_size);
}
static autoBlock_t *GetCryptoSeedEx(void)
{
	static autoBlock_t *seedEx;

	if(!seedEx)
	{
		seedEx = nobCreateBlock(SEEDEXSIZE);
		GetCryptoSeed((uchar *)directGetBuffer(seedEx), SEEDEXSIZE, SEEDEX_FILE);
	}
	return seedEx;
}

//#define BUFFERSIZE 64 // == sha512 hash size

/*
	1/e == 0.36787944*
	1/e ^ 8 == 0.0003354626*
	なので、1バイト(8ビット)削る。
*/
#define BUFFERSIZE 63

static void GetCryptoBlock(uchar *buffer)
{
	static sha512_t *ctx;

	sha512_localize();

	if(!ctx)
	{
		uchar seed[SEEDSIZE];
		autoBlock_t gab;

		GetCryptoSeed(seed, SEEDSIZE, SEED_FILE);

		ctx = sha512_create();
		sha512_update(ctx, gndBlockVar(seed, SEEDSIZE, gab));
//		sha512_makeHash(ctx);

		{
			sha512_t *ictx = sha512_copy(ctx);

			sha512_update(ictx, GetCryptoSeedEx());
			sha512_makeHash(ictx);
			sha512_release(ictx);
		}
	}
	else
	{
		static autoBlock_t *tremor;
		sha512_t *ictx = sha512_copy(ctx);

		if(!tremor)
			tremor = newBlock();

		// tremor更新
		{
			uint index;

			for(index = 0; index < getSize(tremor); index++)
			{
				uint byteVal = getByte(tremor, index);

				if(byteVal < 0xff)
				{
					setByte(tremor, index, byteVal + 1);
					break;
				}
				setByte(tremor, index, 0x00);
			}
			if(index == getSize(tremor))
			{
				addByte(tremor, 0x00);
			}
		}

		sha512_update(ictx, tremor);
		sha512_update(ictx, GetCryptoSeedEx());
		sha512_makeHash(ictx);
		sha512_release(ictx);
	}
	memcpy(buffer, sha512_hash, BUFFERSIZE);
	sha512_unlocalize();
}
uint getCryptoByte(void)
{
	static uchar buffer[BUFFERSIZE];
	static uint index = BUFFERSIZE;

	if(index == BUFFERSIZE)
	{
		GetCryptoBlock(buffer);
		index = 0;
	}
	return buffer[index++];
}
uint getCryptoRand16(void)
{
	return getCryptoByte() | getCryptoByte() << 8;
}
uint getCryptoRand24(void)
{
	return getCryptoByte() | getCryptoByte() << 8 | getCryptoByte() << 16;
}
uint getCryptoRand(void)
{
	return getCryptoByte() | getCryptoByte() << 8 | getCryptoByte() << 16 | getCryptoByte() << 24;
}
uint64 getCryptoRand64(void)
{
	return
		(uint64)getCryptoByte() <<  0 |
		(uint64)getCryptoByte() <<  8 |
		(uint64)getCryptoByte() << 16 |
		(uint64)getCryptoByte() << 24 |
		(uint64)getCryptoByte() << 32 |
		(uint64)getCryptoByte() << 40 |
		(uint64)getCryptoByte() << 48 |
		(uint64)getCryptoByte() << 56;
}
autoBlock_t *makeCryptoRandBlock(uint count)
{
	autoBlock_t *block = createBlock(count);

	while(count)
	{
		addByte(block, getCryptoByte());
		count--;
	}
	return block;
}
