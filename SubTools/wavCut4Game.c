#include "C:\Factory\Common\all.h"
#include "libs\wav.h"

#define HZ 44100

static autoList_t *WavData;

static void Fadeout(uint startPos, uint count)
{
	uint index;

	for(index = 0; index < count; index++)
	{
		double rate = (double)(count - index) / count;
		uint value = getElement(WavData, startPos + index);
		uint v1;
		uint v2;

		v1 = value >> 16;
		v2 = value & 0xffff;

		v1 = m_d2i(((double)v1 - 0x8000) * rate + 0x8000);
		v2 = m_d2i(((double)v2 - 0x8000) * rate + 0x8000);

		value = v1 << 16 | v2;
		setElement(WavData, startPos + index, value);
	}
	setCount(WavData, startPos + index);
}
static void TrimEnd(void)
{
	while(getLastElement(WavData) == 0x80008000)
		unaddElement(WavData);
}
static void TrimBgn(void)
{
	uint rPos;
	uint wPos;

	for(rPos = 0; getElement(WavData, rPos) == 0x80008000; rPos++)
		noop();

	for(wPos = 0; rPos < getCount(WavData); wPos++, rPos++)
		setElement(WavData, wPos, getElement(WavData, rPos));

	setCount(WavData, wPos);
}
static void PutSilentEnd(uint count)
{
	while(count)
	{
		addElement(WavData, 0x80008000);
		count--;
	}
}
static void PutSilentBgn(uint count)
{
	autoList_t *nwd = newList();

	while(count)
	{
		addElement(nwd, 0x80008000);
		count--;
	}
	addElements(nwd, WavData);
	releaseAutoList(WavData);
	WavData = nwd;
}
static void WavCut4Game(char *rFile, char *wFile, uint fadeoutPos)
{
	WavData = readWAVFile(rFile);

	errorCase(lastWAV_Hz != HZ);

	Fadeout(fadeoutPos, HZ / 2);
	TrimEnd();
	TrimBgn();
	PutSilentEnd(HZ / 2);
	PutSilentBgn(HZ / 2);

	writeWAVFile(wFile, WavData, HZ);

	releaseAutoList(WavData);
	WavData = NULL;
}
static void WavCut4Game_F(char *file, uint fadeoutPos)
{
	char *rFile = file;
	char *wFile;

	wFile = strx(rFile);
	wFile = changeExt(wFile, "");
	wFile = addLine_x(wFile, xcout("_wc4g_fo%u.wav", fadeoutPos / HZ));
	wFile = toCreatablePath(wFile, 1000);

	WavCut4Game(rFile, wFile, fadeoutPos);

	memFree(wFile);
}
static uint StrToSec(char *str)
{
	char *p;
	uint sec;

	str = strx(str);
	p = strchr(str, ':');

	if(p)
	{
		*p = '\0';
		p++;
		sec = toValue(str) * 60 + toValue(p);
	}
	else
		sec = toValue(str);

	memFree(str);
	return sec;
}
int main(int argc, char **argv)
{
	uint fadeoutPos;

	fadeoutPos = StrToSec(nextArg()) * HZ;

	if(hasArgs(1))
	{
		char *file = nextArg();

		WavCut4Game_F(file, fadeoutPos);
		return;
	}

	{
		char *file = c_dropFile();

		WavCut4Game_F(file, fadeoutPos);
	}
}
