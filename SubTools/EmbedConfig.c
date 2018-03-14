#include "C:\Factory\Common\all.h"

#define HEADER "{fa2b2611-7ff1-4e17-a692-5d99b415aacc}" // shared_uuid:2

static uint GetHeaderPos(autoBlock_t *fileData, uint startPos)
{
	uint index;

	for(index = startPos; index + strlen(HEADER) <= getSize(fileData); index++)
	{
		if(!memcmp((uchar *)directGetBuffer(fileData) + index, HEADER, strlen(HEADER)))
		{
			cout("foundHeader: %u\n", index);
			return index;
		}
	}
	return UINTMAX; // not found
}
static void ChangeEmbedConfig_File(char *file, uint cfgPos, int cfgVal)
{
	autoBlock_t *fileData = readBinary(file);
	uint embPos;
	uint embLen;

	embPos = GetHeaderPos(fileData, 0);
	errorCase_m(embPos == UINTMAX, "EmbCfgは埋め込まれていません。");
	embPos += strlen(HEADER);
	errorCase_m(GetHeaderPos(fileData, embPos) != UINTMAX, "EmbCfgが複数見つかりました。");

	{
		uint index;

		for(index = 0; ; index++)
			if(getByte(fileData, embPos + index) == 0x00)
				break;

		embLen = index;
	}
	errorCase(embLen <= cfgPos);

	setByte(fileData, embPos + cfgPos, cfgVal);

	LOGPOS();
	writeBinary(file, fileData);
	LOGPOS();
	releaseAutoBlock(fileData);
}
static void ChangePETimeDateStamp(char *file, uint t)
{
	FILE *fp = fileOpen(file, "r+b");
	uint peHedPos;

	LOGPOS();

	errorCase(readChar(fp) != 'M');
	errorCase(readChar(fp) != 'Z');

	fileSeek(fp, SEEK_SET, 0x3c);

	peHedPos = readValue(fp);

	fileSeek(fp, SEEK_SET, peHedPos);

	errorCase(readChar(fp) != 'P');
	errorCase(readChar(fp) != 'E');
	errorCase(readChar(fp) != 0x00);
	errorCase(readChar(fp) != 0x00);

	readChar(fp); // skip
	readChar(fp); // skip
	readChar(fp); // skip
	readChar(fp); // skip

	fileSeek(fp, SEEK_CUR, 0); // 書き込み前のおまじないシーク

	writeValue(fp, t);

	fileSeek(fp, SEEK_CUR, 0x4c);

	writeValue(fp, 0x00000000); // checksum

	fileClose(fp);

	LOGPOS();
}
static void ChangeEmbedConfig(uint cfgPos, int cfgVal)
{
	char *file = makeFullPath(nextArg());

	cout("# %s\n", file);
	cout("[%u] = %02x\n", cfgPos, cfgVal);

	errorCase(!existFile(file));
//	errorCase(_stricmp("exe", getExt(file))); // ? 実行ファイルではない。

	ChangeEmbedConfig_File(file, cfgPos, cfgVal);

	ChangePETimeDateStamp(file, 0x5aaaaaaa); // 2018/03/16 02:17:30

	memFree(file);
}
int main(int argc, char **argv)
{
	if(argIs("--factory-dir-disabled"))
	{
		ChangeEmbedConfig(EMBCFG_FACTORY_DIR_DISABLED, 'D');
		return;
	}
	if(argIs("--dummy-01"))
	{
		ChangeEmbedConfig(EMBCFG_DUMMY_01, 'Z');
		return;
	}
	if(argIs("--dummy-02"))
	{
		ChangeEmbedConfig(EMBCFG_DUMMY_02, 'Z');
		return;
	}
	if(argIs("--dummy-03"))
	{
		ChangeEmbedConfig(EMBCFG_DUMMY_03, 'Z');
		return;
	}
	error_m("bad_opt");
}
