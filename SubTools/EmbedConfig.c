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
static void ChangeEmbedConfig(uint cfgPos, int cfgVal)
{
	char *file = makeFullPath(nextArg());

	cout("# %s\n", file);
	cout("[%u] = %02x\n", cfgPos, cfgVal);

	errorCase(!existFile(file));
//	errorCase(_stricmp("exe", getExt(file))); // ? 実行ファイルではない。

	ChangeEmbedConfig_File(file, cfgPos, cfgVal);

	memFree(file);
}
int main(int argc, char **argv)
{
	if(argIs("--factory-dir-disabled"))
	{
		ChangeEmbedConfig(EMBCFG_FACTORY_DIR_DISABLED, 'D');
		return;
	}
	if(argIs("--change-color-disabled"))
	{
		ChangeEmbedConfig(EMBCFG_CHANGE_COLOR_DISABLED, 'D');
		return;
	}
	error_m("bad_opt");
}
