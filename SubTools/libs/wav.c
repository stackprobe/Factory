#include "wav.h"

static struct
{
	char Riff[5];
	uint Size;
	char Wave[5];
}
Header;

static struct
{
	uint FormatID;
	uint ChannelNum;
	uint Hz;
	uint BytePerSec;
	uint BlockSize;
	uint BitPerSample;
}
Fmt;

static autoBlock_t *RawData;

uint lastWAV_Hz;

autoList_t *readWAVFile(char *file)
{
	FILE *fp = fileOpen(file, "rb");
	autoList_t *wavData;
	uint index;

	Header.Riff[0] = readChar(fp);
	Header.Riff[1] = readChar(fp);
	Header.Riff[2] = readChar(fp);
	Header.Riff[3] = readChar(fp);

	Header.Size = readValue(fp);

	Header.Wave[0] = readChar(fp);
	Header.Wave[1] = readChar(fp);
	Header.Wave[2] = readChar(fp);
	Header.Wave[3] = readChar(fp);

	errorCase(strcmp(Header.Riff, "RIFF"));
	errorCase(strcmp(Header.Wave, "WAVE"));

	for(; ; )
	{
		int chr = readChar(fp);
		char name[5];
		uint size;

		if(chr == EOF)
			break;

		name[0] = chr;
		name[1] = readChar(fp);
		name[2] = readChar(fp);
		name[3] = readChar(fp);
		name[4] = '\0';
		size = readValue(fp);

		errorCase(IMAX < size); // zantei

		if(!strcmp(name, "fmt "))
		{
			Fmt.FormatID     = readValueWidth(fp, 2);
			Fmt.ChannelNum   = readValueWidth(fp, 2);
			Fmt.Hz           = readValue(fp);
			Fmt.BytePerSec   = readValue(fp);
			Fmt.BlockSize    = readValueWidth(fp, 2);
			Fmt.BitPerSample = readValueWidth(fp, 2);

			fileSeek(fp, SEEK_CUR, (sint64)size - 16);
		}
		else if(!strcmp(name, "data"))
		{
			if(RawData)
				releaseAutoBlock(RawData);

			RawData = neReadBinaryBlock(fp, size);
		}
		else
		{
			fileSeek(fp, SEEK_CUR, size);
		}
	}
	fileClose(fp);

	errorCase(!RawData); // ? no 'data'

	wavData = newList();

	if(Fmt.BitPerSample == 8)
	{
		for(index = 0; index < getSize(RawData); index++)
		{
			addElement(wavData, getByte(RawData, index) * 0x0101);
		}
	}
	else // 16
	{
		for(index = 0; index < getSize(RawData); index += 2)
		{
			addElement(wavData, (getByte(RawData, index) | getByte(RawData, index + 1) << 8) ^ 0x8000);
		}
	}
	if(Fmt.ChannelNum == 1) // to ƒXƒeƒŒƒI
	{
		uint *buff;

		index = getCount(wavData);
		nobSetCount(wavData, index * 2);
		buff = (uint *)directGetList(wavData);

		while(index)
		{
			index--;
			buff[index * 2 + 0] = buff[index];
			buff[index * 2 + 1] = buff[index];
		}
	}
	releaseAutoBlock(RawData);
	RawData = NULL;

	lastWAV_Hz = Fmt.Hz;

	return wavData;
}
void writeWAVFile(char *file, autoList_t *wavData, uint hz)
{
	// TODO
}
