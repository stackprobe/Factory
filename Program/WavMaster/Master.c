/*
	.wav の音量マスタリングを行う。

	- - -

	Master.exe INPUT-WAV-FILE OUTPUT-WAV-FILE REPORT-CSV-FILE
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Progress.h"
#include "C:\Factory\SubTools\libs\wavFile.h"

#define LV_RANGE 1000
#define LV_START 2 // 0 ～
#define BORDER_RATE 0.999
#define DEST_RATE 0.5 // 0.0 ～ 1.0
#define MARGIN_RATE 0.07

static double Lvs[LV_RANGE + 1];
static int OutputCancelled = 0;

static void PutLv(uint low, uint hi)
{
	uint range;
	double weight;
	uint index;

	if(low == hi)
	{
		Lvs[low] += 1.0;
		return;
	}
	if(hi < low)
		m_swap(low, hi, uint);

	range = hi - low;
	weight = 1.0 / range;

	Lvs[low] += weight / 2.0;
	Lvs[hi]  += weight / 2.0;

	for(index = low + 1; index < hi; index++)
		Lvs[index] += weight;
}
static void DoConv(char *rFile, char *wFile, char *reportFile)
{
	FILE *rfp = fileOpen(rFile, "rb");
	FILE *wfp;
	uint rowCount = 0;
	uint count;
	uint lasti1 = 0;
	uint lasti2 = 0;
	uint index;
	double totalLv = 0.0;
	double borderLv;
	uint borderIndex;
	double borderRate;
	double changeRate;
	uint sampleCount = 0;
	uint otowareCount = 0;
	double otowareRate = 0.0;
	char *message = "(特に無し)";

	LOGPOS();

	for(; ; ) // 読み込みチェック & 行数カウント
	{
		autoList_t *row = readCSVRow(rfp);
		sint v1;
		sint v2;

		if(!row)
			break;

		v1 = (sint)toValue(getLine(row, 0));
		v2 = (sint)toValue(getLine(row, 1));

		errorCase(!m_isRange(v1, 0, 0xffff));
		errorCase(!m_isRange(v2, 0, 0xffff));

		rowCount++;

		releaseDim(row, 1);
	}
	cout("rowCount: %u\n", rowCount);

	LOGPOS();
	ProgressBegin();

	fileSeek(rfp, SEEK_SET, 0);

	for(count = 0; ; count++)
	{
		autoList_t *row = readCSVRow(rfp);
		sint v1;
		sint v2;
		double lval;
		double rval;
		uint i1;
		uint i2;

		if(!row)
			break;

		v1 = (sint)toValue(getLine(row, 0));
		v2 = (sint)toValue(getLine(row, 1));

		lval = (double)(v1 - 0x8000) / 0x8000;
		rval = (double)(v2 - 0x8000) / 0x8000;

		lval = fabs(lval);
		rval = fabs(rval);

		i1 = d2i(lval * LV_RANGE);
		i2 = d2i(rval * LV_RANGE);

		errorCase(!m_isRange(i1, 0, LV_RANGE));
		errorCase(!m_isRange(i2, 0, LV_RANGE));

		PutLv(lasti1, i1);
		PutLv(lasti2, i2);

		lasti1 = i1;
		lasti2 = i2;

		ProgressRate((double)count / rowCount);

		releaseDim(row, 1);
	}
	ProgressEnd(0);
	LOGPOS();

	for(index = 0; index <= LV_RANGE; index++)
		totalLv += Lvs[index];

	borderLv = totalLv *= BORDER_RATE;

	cout("totalLv: %f\n", totalLv);
	cout("borderLv: %f\n", borderLv);

	totalLv = 0.0;

	for(index = 0; index < LV_RANGE; index++)
	{
		totalLv += Lvs[index];

		if(borderLv < totalLv)
			break;
	}
	borderIndex = index; // 0 ～ LV_RANGE
	borderRate = (double)borderIndex / LV_RANGE; // 0.0 ～ 1.0
	changeRate = DEST_RATE / borderRate;

	cout("borderIndex: %u\n", borderIndex);
	cout("borderRate: %f\n", borderRate);
	cout("changeRate: %f\n", changeRate);

	if(1.0 - MARGIN_RATE < changeRate && changeRate < 1.0 + MARGIN_RATE)
	{
		message = "レートの振り幅が小さいため変換を行いません。(WAVを出力しません)";
		OutputCancelled= 1;
		goto outputReport;
	}
	LOGPOS();
	ProgressBegin();

	fileSeek(rfp, SEEK_SET, 0);
	wfp = fileOpen(wFile, "wb");

	for(count = 0; ; count++)
	{
		autoList_t *row = readCSVRow(rfp);
		sint v1;
		sint v2;
		double lval;
		double rval;
		uint i1;
		uint i2;

		if(!row)
			break;

		v1 = (sint)toValue(getLine(row, 0));
		v2 = (sint)toValue(getLine(row, 1));

		v1 -= 0x8000;
		v2 -= 0x8000;

		v1 = d2i((double)v1 * changeRate);
		v2 = d2i((double)v2 * changeRate);

		v1 += 0x8000;
		v2 += 0x8000;

		if(!m_isRange(v1, 0, 0xffff))
		{
			cout("音割れ-1 %d\n", v1);

			otowareCount++;
			m_range(v1, 0, 0xffff);
		}
		if(!m_isRange(v2, 0, 0xffff))
		{
			cout("音割れ-2 %d\n", v2);

			otowareCount++;
			m_range(v2, 0, 0xffff);
		}
		writeLine_x(wfp, xcout("%d,%d", v1, v2));

		ProgressRate((double)count / rowCount);

		releaseDim(row, 1);
	}
	ProgressEnd(0);
	LOGPOS();

	sampleCount = rowCount * 2;
	otowareRate = (double)otowareCount / sampleCount;

	cout("sampleCount: %u\n", sampleCount);
	cout("otowareCount: %u\n", otowareCount);
	cout("otowareRate: %f\n", otowareRate);

	fileClose(wfp);

outputReport:
	cout("message: %s\n", message);
	cout("OutputCancelled: %d\n", OutputCancelled);

	wfp = fileOpen(reportFile, "wt");

	writeLine_x(wfp, xcout("totalLv,%f", totalLv));
	writeLine_x(wfp, xcout("borderLv,%f", borderLv));
	writeLine_x(wfp, xcout("borderIndex,%u", borderIndex));
	writeLine_x(wfp, xcout("borderRate,%f", borderRate));
	writeLine_x(wfp, xcout("changeRate,%f", changeRate));
	writeLine_x(wfp, xcout("sampleCount,%u", sampleCount));
	writeLine_x(wfp, xcout("otowareCount,%u", otowareCount));
	writeLine_x(wfp, xcout("otowareRate,%f", otowareRate));
	writeLine_x(wfp, xcout("message,%s", message));
	writeLine_x(wfp, xcout("OutputCancelled,%d", OutputCancelled));

	for(index = 0; index < LV_RANGE; index++)
		writeLine_x(wfp, xcout("Lvs,%u,%f", index, Lvs[index]));

	fileClose(rfp);
	fileClose(wfp);

	LOGPOS();
}
static void Fnlz(void)
{
	LOGPOS();
	termination(1);
}
int main(int argc, char **argv)
{
	char *rFile;
	char *wFile;
	char *reportFile;
	char *csvFile1;
	char *csvFile2;

	addFinalizer(Fnlz); // エラーダイアログ抑止！

	rFile = nextArg();
	wFile = nextArg();
	reportFile = nextArg();

	errorCase(!existFile(rFile));
	removeFileIfExist(wFile);
	removeFileIfExist(reportFile);

	csvFile1 = makeTempFile(NULL);
	csvFile2 = makeTempFile(NULL);

	LOGPOS();

	readWAVFileToCSVFile(rFile, csvFile1);

	LOGPOS();

	DoConv(csvFile1, csvFile2, reportFile);

	LOGPOS();

	if(!OutputCancelled)
		writeWAVFileFromCSVFile(csvFile2, wFile, lastWAV_Hz);

	LOGPOS();

	removeFile_x(csvFile1);
	removeFile_x(csvFile2);

	termination(0);
}
