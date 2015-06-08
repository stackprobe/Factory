#include "C:\Factory\Common\all.h"
#include "C:\Factory\DevTools\libs\RandData.h"

#define IsBase64Char(chr) \
	( \
		'A' <= (chr) && (chr) <= 'Z' || \
		'a' <= (chr) && (chr) <= 'z' || \
		'0' <= (chr) && (chr) <= '9' || \
		(chr) == '+' || \
		(chr) == '/' || \
		(chr) == '=' \
	)

static void Test_encodeBase64_decodeBase64(void)
{
	uint testcnt;

	for(testcnt = 0; testcnt < 1000; testcnt++)
	{
		autoBlock_t *src = MakeRandBinaryBlock(mt19937_rnd(100000));
		autoBlock_t *enc;
		autoBlock_t *dec;
		uint index;

		cout("src: %u\n", getSize(src));
		enc = encodeBase64(src);
		cout("enc: %u\n", getSize(enc));

		for(index = 0; index < getSize(enc); index++)
			errorCase(!IsBase64Char(getByte(enc, index)));

		dec = decodeBase64(enc);
		cout("dec: %u\n", getSize(dec));

		errorCase(!isSameBlock(src, dec));

		releaseAutoBlock(src);
		releaseAutoBlock(enc);
		releaseAutoBlock(dec);
	}
	cout("OK\n");
}

int main(int argc, char **argv)
{
	Test_encodeBase64_decodeBase64();
}
