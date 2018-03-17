#include "ChangePETimeDateStamp.h"

void ChangePETimeDateStamp(char *file, uint t)
{
	FILE *fp = fileOpen(file, "r+b");
	uint peHedPos;
	uint optHedSize;

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

	fileSeek(fp, SEEK_CUR, 0x04);

	writeValue(fp, t);

	fileSeek(fp, SEEK_CUR, 0x08);

	optHedSize = readValueWidth(fp, 2);

	cout("PE optional header size: %u\n", optHedSize);

errorCase(optHedSize != 224); // zantei

	if(0x44 <= optHedSize)
	{
		fileSeek(fp, SEEK_CUR, 0x42);

		writeValue(fp, 0x00000000); // clear checksum
	}
	fileClose(fp);

	LOGPOS();
}
void ChangeAllPETimeDateStamp(char *dir, uint t)
{
	autoList_t *files = lssFiles(dir);
	char *file;
	uint index;

	foreach(files, file, index)
	{
		if(
			!_stricmp("EXE", getExt(file)) ||
			!_stricmp("DLL", getExt(file))
			)
		{
			cout("file: %s\n", file);

			ChangePETimeDateStamp(file, t);
		}
	}
	releaseDim(files, 1);
}
