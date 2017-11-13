/*
	getCryptoByte()のバイト列:

		xor(a, ca)

	a:
		sha-512(b[0]){0 -> 64} + sha-512(b[1]){0 -> 64} + sha-512(b[2]){0 -> 64} + ...

	b:
		b[0] = s
		b[1] = s + c[0]
		b[2] = s + c[1]
		b[3] = s + c[2]
		...

	s:
		GetCryptoSeed(){0 -> 4096}

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

	ca:
		camellia(cb[0], cs){0 -> 16} + camellia(cb[1], cs){0 -> 16} + camellia(cb[2], cs){0 -> 16} + ...

	cb:
		cb[0] = 0x00, 0x00, 0x00, ... 0x00 // 16 バイト
		cb[1] = 0x01, 0x00, 0x00, ... 0x00
		cb[2] = 0x02, 0x00, 0x00, ... 0x00
		...
		cb[255] = 0xff, 0x00, 0x00, ... 0x00
		cb[256] = 0x00, 0x01, 0x00, ... 0x00
		cb[257] = 0x01, 0x01, 0x00, ... 0x00
		cb[258] = 0x02, 0x01, 0x00, ... 0x00
		...

	cs:
		Ca_GetCryptoSeed(){0 -> 32}

	- - -

	{a -> b} = 添字 a から b の直前まで

	camellia(plain, raw-key) = 鍵長 256 bit の camellia によるブロックの暗号化
*/

#include "CryptoRand.h"

#define SEED_DIR "C:\\Factory\\tmp"
#define SEED_FILE SEED_DIR "\\CSeed.dat"
#define CA_SEED_FILE SEED_DIR "\\CSeedCa.dat"

#define SEEDSIZE 4096

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

#define BUFFERSIZE 64 // == sha512 hash size

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
		sha512_makeHash(ctx);
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
		sha512_makeHash(ictx);
		sha512_release(ictx);
	}
	memcpy(buffer, sha512_hash, BUFFERSIZE);
	sha512_unlocalize();
}
static uchar *Ca_GetCryptoBlock(void)
{
	static camellia_keyTable_t *cam_kt;
	static uchar counter[16];
	static uchar dest[16];

	if(!cam_kt)
	{
		uchar cam_seed[32];
		autoBlock_t gab;

		GetCryptoSeed(cam_seed, 32, CA_SEED_FILE);

		cam_kt = camellia_createKeyTable(gndBlockVar(cam_seed, 32, gab));
	}
	camellia_encrypt(cam_kt, counter, dest, 1);

	// counter更新
	{
		uint index;

		for(index = 0; index < 16; index++)
		{
			if(counter[index] < 0xff)
			{
				counter[index]++;
				break;
			}
			counter[index] = 0x00;
		}
	}

	return dest;
}
static void Ca_Xor(uchar *b1, uchar *b2)
{
	((uint *)b1)[0] ^= ((uint *)b2)[0];
	((uint *)b1)[1] ^= ((uint *)b2)[1];
	((uint *)b1)[2] ^= ((uint *)b2)[2];
	((uint *)b1)[3] ^= ((uint *)b2)[3];
}
static void Ca_XorCryptoBlock(uchar buffer[64])
{
	Ca_Xor(buffer +  0, Ca_GetCryptoBlock());
	Ca_Xor(buffer + 16, Ca_GetCryptoBlock());
	Ca_Xor(buffer + 32, Ca_GetCryptoBlock());
	Ca_Xor(buffer + 48, Ca_GetCryptoBlock());
}
uint getCryptoByte(void)
{
	static uchar buffer[BUFFERSIZE];
	static uint index = BUFFERSIZE;

	if(index == BUFFERSIZE)
	{
		GetCryptoBlock(buffer);
		Ca_XorCryptoBlock(buffer);
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
