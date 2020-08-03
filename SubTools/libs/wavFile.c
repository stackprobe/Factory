#include "wavFile.h"

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

static struct
{
	uint64 Pos;
	uint64 Size;
}
RawData;

static int AV_Return;

/*
	各行 11 バイト (改行を含むと 12 バイト) になるように 0 補填する。
	NNNNN,NNNNN<改行>
*/
static void AddValue(FILE *wfp, uint value)
{
	if(Fmt.ChannelNum == 1) // monoral
	{
		writeLine_x(wfp, xcout("%05u,%05u", value, value));
	}
	else // stereo
	{
		writeToken_x(wfp, xcout("%05u", value));
		writeChar(wfp, AV_Return);

		AV_Return ^= ',' ^ '\n';
	}
}
void readWAVFileToCSVFile(char *rFile, char *wFile)
{
	FILE *rfp = fileOpen(rFile, "rb");
	FILE *wfp = fileOpen(wFile, "wb");
	uint64 rFileSize;
	uint index;

	rFileSize = getFileSizeFP(rfp);
	fileSeek(rfp, SEEK_SET, 0);

	zeroclear(Header);
	zeroclear(Fmt);
	RawData.Pos = 0;
	AV_Return = ',';

	Header.Riff[0] = readChar(rfp);
	Header.Riff[1] = readChar(rfp);
	Header.Riff[2] = readChar(rfp);
	Header.Riff[3] = readChar(rfp);

	Header.Size = readValue(rfp);

	Header.Wave[0] = readChar(rfp);
	Header.Wave[1] = readChar(rfp);
	Header.Wave[2] = readChar(rfp);
	Header.Wave[3] = readChar(rfp);

	errorCase(strcmp(Header.Riff, "RIFF"));
	errorCase(strcmp(Header.Wave, "WAVE"));

	for(; ; )
	{
		int chr = readChar(rfp);
		char name[5];
		uint size;

		if(chr == EOF)
			break;

		name[0] = chr;
		name[1] = readChar(rfp);
		name[2] = readChar(rfp);
		name[3] = readChar(rfp);
		name[4] = '\0';
		size = readValue(rfp);

		errorCase(4200000000 < size); // HACK

		if(!strcmp(name, "fmt "))
		{
			Fmt.FormatID     = readValueWidth(rfp, 2);
			Fmt.ChannelNum   = readValueWidth(rfp, 2);
			Fmt.Hz           = readValue(rfp);
			Fmt.BytePerSec   = readValue(rfp);
			Fmt.BlockSize    = readValueWidth(rfp, 2);
			Fmt.BitPerSample = readValueWidth(rfp, 2);

			fileSeek(rfp, SEEK_CUR, (sint64)size - 16);
		}
		else if(!strcmp(name, "data"))
		{
			RawData.Pos = getSeekPos(rfp);
			RawData.Size = size;

			errorCase(rFileSize - RawData.Pos < RawData.Size);

			fileSeek(rfp, SEEK_CUR, size);
		}
		else
		{
			fileSeek(rfp, SEEK_CUR, size);
		}
	}
	errorCase(Fmt.FormatID != 1); // ? ! PCM
	errorCase(!m_isRange(Fmt.ChannelNum, 1, 2));
	errorCase(!m_isRange(Fmt.Hz, 1, IMAX));
	errorCase(!m_isRange(Fmt.BitPerSample, 8, 16) || Fmt.BitPerSample % 8);
	errorCase(Fmt.BlockSize != Fmt.ChannelNum * (Fmt.BitPerSample >> 3));
	errorCase(Fmt.BytePerSec != Fmt.Hz * Fmt.BlockSize);
	errorCase(RawData.Pos == 0);

	fileSeek(rfp, SEEK_SET, RawData.Pos);

	if(Fmt.BitPerSample == 8)
	{
		for(index = 0; index < RawData.Size; index++)
		{
			AddValue(wfp, (uint)readChar(rfp) * 0x0100);
		}
	}
	else // 16
	{
		for(index = 0; index < RawData.Size; index += 2)
		{
			uint v1;
			uint v2;

			v1 = readChar(rfp);
			v2 = readChar(rfp);

			AddValue(wfp, (v1 | v2 << 8) ^ 0x8000);
		}
	}
	fileClose(rfp);
	fileClose(wfp);

	lastWAV_Hz = Fmt.Hz;

	errorCase(AV_Return != ','); // ? ステレオ左音で終わってる。
	errorCase(getFileSize(wFile) % 12 != 0); // 2bs
}
void writeWAVFileFromCSVFile(char *rFile, char *wFile, uint hz)
{
	FILE *rfp = fileOpen(rFile, "rb");
	FILE *wfp = fileOpen(wFile, "wb");
	uint wavSize = 0;

	writeToken(wfp, "RIFF");
	writeValue(wfp, 0); // dummy
	writeToken(wfp,
		"WAVE"
		"fmt\40"
		);
	writeValue(wfp, 16);
	writeValueWidth(wfp, 1, 2);
	writeValueWidth(wfp, 2, 2);
	writeValue(wfp, hz);
	writeValue(wfp, hz * 4);
	writeValueWidth(wfp, 4, 2);
	writeValueWidth(wfp, 16, 2);
	writeToken(wfp, "data");
	writeValue(wfp, 0); // dummy

	for(; ; )
	{
		autoList_t *row = readCSVRow(rfp);
		uint v1;
		uint v2;

		if(!row)
			break;

		v1 = toValue(getLine(row, 0));
		v2 = toValue(getLine(row, 1));

		releaseDim(row, 1);

		errorCase(!m_isRange(v1, 0, 0xffff));
		errorCase(!m_isRange(v2, 0, 0xffff));

		writeValueWidth(wfp, v1 ^ 0x8000, 2);
		writeValueWidth(wfp, v2 ^ 0x8000, 2);

		wavSize += 4;
	}
	fileSeek(wfp, SEEK_SET, 4);
	writeValue(wfp, 4 + 4 + 4 + 16 + 4 + 4 + wavSize);

	fileSeek(wfp, SEEK_SET, 40);
	writeValue(wfp, wavSize);

	fileClose(rfp);
	fileClose(wfp);
}
