#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Prime2.h"
#include "C:\Factory\Common\Options\csv.h"
#include "libs\UlamSpiral.h"

#define PRIME_MAX 18446744073709551557ui64 // uint64 最大の素数

#define DUMMY_UUID "{905cac95-9d4c-482d-a189-e6a941c68fbe}"

// ---- WrUI64 ----

#define WR_BUFF_SIZE 67000000

static uchar WrBuff[WR_BUFF_SIZE];
static uchar *WrPos = WrBuff;

static void WrUI64Flush(FILE *fp)
{
	uint size = (uint)WrPos - (uint)WrBuff;

	errorCase(fwrite(WrBuff, 1, size, fp) != size); // ? 失敗
	WrPos = WrBuff;
}
static void WrUI64(FILE *fp, uint64 value)
{
	static char buff[] = "01234567890123456789\n";
	char *p = buff + 20;
	uint s;

	if(WrBuff + WR_BUFF_SIZE < WrPos + 21)
		WrUI64Flush(fp);

	do
	{
		p--;
		*p = '0' + (value % 10);
		value /= 10;
	}
	while(value != 0);

	s = ((uint)buff + 21) - (uint)p;

//	errorCase(fwrite(p, 1, s, fp) != s);
	memcpy(WrPos, p, s);
	WrPos += s;
}

// ----

static uint64 GetLowPrime(uint64 value)
{
	while(2 < value)
	{
		value--;

		if(IsPrime(value))
			return value;
	}
	return 0;
}
static uint64 GetHiPrime(uint64 value)
{
	while(value < PRIME_MAX)
	{
		value++;

		if(IsPrime(value))
			return value;
	}
	return 0;
}
static int IsShortRange(uint64 minval, uint64 maxval)
{
	uint64 range = maxval - minval;
	int ret;

	ret =
//		maxval < 11000000000 && range < 1100000 ||
//		maxval < 110000000000 && range < 310000 ||
		maxval < 1100000000000 && range < 110000 ||
		maxval < 11000000000000 && range < 31000 ||
		maxval < 110000000000000 && range < 11000 ||
		maxval < 1100000000000000 && range < 3100 ||
		maxval < 11000000000000000 && range < 1100;

	cout("IsShortRange_ret: %d\n", ret);
	return ret;
}
static void PrimeRange(uint64 minval, uint64 maxval, char *outFile, char *cancelEvName, char *reportEvName, char *reportMtxName, char *reportFile)
{
	FILE *fp = fileOpen(outFile, "wb");
	uint cancelEv = eventOpen(cancelEvName);
	uint reportEv = eventOpen(reportEvName);
	uint reportMtx = mutexOpen(reportMtxName);
	uint64 value;

//	errorCase(setvbuf(fp, NULL, _IOFBF, 64 * 1024 * 1024)); // ? 失敗 // old

	if(minval <= 2)
	{
		if(2 <= maxval)
			errorCase(fprintf(fp, "2\n") < 0);

		minval = 3;
	}
	else
		minval |= 1;

	m_minim(maxval, PRIME_MAX);

	if(IsShortRange(minval, maxval))
	{
		for(value = minval; value <= maxval; value += 2)
			if(IsPrime(value))
				WrUI64(fp, value);
	}
	else
	{
		for(value = minval; value <= maxval; value += 2)
		{
			if(value % 0x08000000 == 1)
			{
				if(handleWaitForMillis(cancelEv, 0))
				{
					WrUI64Flush(fp);
					errorCase(fprintf(fp, "中止しました。\n") < 0);
					break;
				}

				handleWaitForever(reportMtx);
				writeOneLine_cx(reportFile, xcout("%I64u", value));
				mutexRelease(reportMtx);

				eventSet(reportEv);
			}
			if(IsPrime_R(value))
//				errorCase(fprintf(fp, "%I64u\n", value) < 0); // 遅い！
				WrUI64(fp, value);
		}
	}
	WrUI64Flush(fp);
	fileClose(fp);

	handleWaitForever(reportMtx);
	removeFileIfExist(reportFile);
	mutexRelease(reportMtx);

	handleClose(cancelEv);
	handleClose(reportEv);
	handleClose(reportMtx);
}
static void PrimeCount(uint64 minval, uint64 maxval, char *outFile, char *cancelEvName, char *reportEvName, char *reportMtxName, char *reportFile)
{
	uint cancelEv = eventOpen(cancelEvName);
	uint reportEv = eventOpen(reportEvName);
	uint reportMtx = mutexOpen(reportMtxName);
	uint64 value;
	uint64 count = 0;
	int cancelled = 0;

	removeFileIfExist(outFile);

	if(minval <= 2)
	{
		if(2 <= maxval)
			count++;

		minval = 3;
	}
	else
		minval |= 1;

	m_minim(maxval, PRIME_MAX);

	if(IsShortRange(minval, maxval))
	{
		for(value = minval; value <= maxval; value += 2)
			if(IsPrime(value))
				count++;
	}
	else
	{
		for(value = minval; value <= maxval; value += 2)
		{
			if(value % 0x10000000 == 1)
			{
				if(handleWaitForMillis(cancelEv, 0))
				{
					cancelled = 1;
					break;
				}

				handleWaitForever(reportMtx);
				writeOneLine_cx(reportFile, xcout("%I64u\n%I64u", value, count));
				mutexRelease(reportMtx);

				eventSet(reportEv);
			}
			if(IsPrime_R(value))
				count++;
		}
	}
	if(!cancelled)
		writeOneLine_cx(outFile, xcout("%I64u", count));

	handleWaitForever(reportMtx);
	removeFileIfExist(reportFile);
	mutexRelease(reportMtx);

	handleClose(cancelEv);
	handleClose(reportEv);
	handleClose(reportMtx);
}
static uint64 ToValue_Check(char *str)
{
	uint64 value = toValue64(str);
	char *tmp;

	tmp = xcout("%I64u", value);
	errorCase_m(strcmp(str, tmp), "10進整数ではない又は値域外");
	memFree(tmp);
	return value;
}
static void Values_CheckRange(uint64 minval, uint64 maxval)
{
	errorCase_m(maxval < minval, "最小値が最大値より大きい");
}
static void DoBatch(int mode, char *rFile, char *wFile) // mode: "PFC"
{
	autoList_t *rCsv = readCSVFileTrim(rFile);
	autoList_t *rRow;
	uint rowidx;
	autoList_t *wCsv = newList();

	errorCase_m(!overwritable(wFile), "出力ファイルを作成できません。"); // テスト

	foreach(rCsv, rRow, rowidx)
	{
		autoList_t *wRow = newList();

		cout("%u 行目を処理しています...\n", rowidx + 1);

		switch(mode)
		{
		case 'P': // Prime
			{
				uint64 value = ToValue_Check(getLine(rRow, 0));
				char *ans;

				cout("%I64u -> ", value);
				addElement(wRow, (uint)xcout("%I64u", value));

				if(IsPrime(value))
					ans = "IS_PRIME";
				else
					ans = "IS_NOT_PRIME";

				cout("%s\n", ans);
				addElement(wRow, (uint)strx(ans));
			}
			break;

		case 'F': // Factorization
			{
				uint64 value = ToValue_Check(getLine(rRow, 0));
				uint64 factors[64];
				uint64 *fp;

				cout("%I64u ->", value);
				addElement(wRow, (uint)xcout("%I64u", value));
				Factorization(value, factors);

				for(fp = factors; *fp; fp++)
				{
					cout(" %I64u", *fp);
					addElement(wRow, (uint)xcout("%I64u", *fp));
				}
				cout("\n");
			}
			break;

		case 'C': // Count
			{
				uint64 minval;
				uint64 maxval;
				char *outFile = makeTempFile("tmp");
				char *tmpFile = makeTempFile("tmp");
				char *line;

				minval = ToValue_Check(getLine(rRow, 0));
				maxval = ToValue_Check(getLine(rRow, 1));

				Values_CheckRange(minval, maxval);

				cout("%I64u to %I64u -> ", minval, maxval);

				addElement(wRow, (uint)xcout("%I64u", minval));
				addElement(wRow, (uint)xcout("%I64u", maxval));

				PrimeCount(
					minval,
					maxval,
					outFile,
					DUMMY_UUID "_1",
					DUMMY_UUID "_2",
					DUMMY_UUID "_3",
					tmpFile
					);
				line = readFirstLine(outFile);
				removeFile(outFile);
				cout("%s\n", line);
				addElement(wRow, (uint)line);

				memFree(outFile);
				memFree(tmpFile);
			}
			break;

		default:
			error();
		}
		addElement(wCsv, (uint)wRow);
	}
	cout("全行処理しました。\n");

	writeCSVFile(wFile, wCsv);

	releaseDim(rCsv, 2);
	releaseDim(wCsv, 2);
}
static void Main2(void)
{
	if(argIs("/P"))
	{
		uint64 value = ToValue_Check(nextArg());

		if(IsPrime(value))
			cout("IS_PRIME\n");
		else
			cout("IS_NOT_PRIME\n");

		return;
	}
	if(argIs("/F"))
	{
		uint64 value = ToValue_Check(nextArg());
		uint64 dest[64];
		uint index;

		Factorization(value, dest);

		for(index = 0; dest[index] != 0; index++)
		{
			cout("%I64u\n", dest[index]);
		}
		return;
	}
	if(argIs("/L"))
	{
		uint64 value = ToValue_Check(nextArg());

		cout("%I64u\n", GetLowPrime(value));
		return;
	}
	if(argIs("/H"))
	{
		uint64 value = ToValue_Check(nextArg());

		cout("%I64u\n", GetHiPrime(value));
		return;
	}
	if(argIs("/LH"))
	{
		uint64 value = ToValue_Check(nextArg());

		cout("%I64u\n", GetLowPrime(value));
		cout("%I64u\n", GetHiPrime(value));
		return;
	}
	if(argIs("/R"))
	{
		uint64 minval;
		uint64 maxval;
		char *outFile;

		minval = ToValue_Check(nextArg());
		maxval = ToValue_Check(nextArg());
		outFile = nextArg();

		Values_CheckRange(minval, maxval);

		PrimeRange(
			minval,
			maxval,
			outFile,
			DUMMY_UUID "_1",
			DUMMY_UUID "_2",
			DUMMY_UUID "_3",
			makeTempFile("tmp")
			);
		return;
	}
	if(argIs("/R2"))
	{
		uint64 minval;
		uint64 maxval;
		char *outFile;
		char *cancelEvName;
		char *reportEvName;
		char *reportMtxName;
		char *reportFile;

		minval = toValue64(nextArg());
		maxval = toValue64(nextArg());
		outFile = nextArg();
		cancelEvName = nextArg();
		reportEvName = nextArg();
		reportMtxName = nextArg();
		reportFile = nextArg();

		PrimeRange(minval, maxval, outFile, cancelEvName, reportEvName, reportMtxName, reportFile);
		return;
	}
	if(argIs("/C"))
	{
		uint64 minval;
		uint64 maxval;
		char *outFile;

		minval = ToValue_Check(nextArg());
		maxval = ToValue_Check(nextArg());
		outFile = nextArg();

		Values_CheckRange(minval, maxval);

		PrimeCount(
			minval,
			maxval,
			outFile,
			DUMMY_UUID "_1",
			DUMMY_UUID "_2",
			DUMMY_UUID "_3",
			makeTempFile("tmp")
			);
		return;
	}
	if(argIs("/C2"))
	{
		uint64 minval;
		uint64 maxval;
		char *outFile;
		char *cancelEvName;
		char *reportEvName;
		char *reportMtxName;
		char *reportFile;

		minval = toValue64(nextArg());
		maxval = toValue64(nextArg());
		outFile = nextArg();
		cancelEvName = nextArg();
		reportEvName = nextArg();
		reportMtxName = nextArg();
		reportFile = nextArg();

		PrimeCount(minval, maxval, outFile, cancelEvName, reportEvName, reportMtxName, reportFile);
		return;
	}
	if(argIs("/BP"))
	{
		char *rFile;
		char *wFile;

		rFile = nextArg();
		wFile = nextArg();

		DoBatch('P', rFile, wFile);
		return;
	}
	if(argIs("/BF"))
	{
		char *rFile;
		char *wFile;

		rFile = nextArg();
		wFile = nextArg();

		DoBatch('F', rFile, wFile);
		return;
	}
	if(argIs("/BC"))
	{
		char *rFile;
		char *wFile;

		rFile = nextArg();
		wFile = nextArg();

		DoBatch('C', rFile, wFile);
		return;
	}
	if(argIs("/US"))
	{
		sint64 l;
		sint64 t;
		sint64 r;
		sint64 b;
		uint primeColor;
		uint notPrimeColor;
		uint centerColor;
		char *outBmpFile;

		l = _atoi64(nextArg());
		t = _atoi64(nextArg());
		r = _atoi64(nextArg());
		b = _atoi64(nextArg());
		primeColor    = toValueDigits(nextArg(), hexadecimal);
		notPrimeColor = toValueDigits(nextArg(), hexadecimal);
		centerColor   = toValueDigits(nextArg(), hexadecimal);
		outBmpFile = nextArg();

		MakeUlamSpiral(
			l,
			t,
			r,
			b,
			primeColor,
			notPrimeColor,
			centerColor,
			outBmpFile,
			DUMMY_UUID "_1",
			DUMMY_UUID "_2",
			DUMMY_UUID "_3",
			makeTempFile("tmp")
			);
		return;
	}
	if(argIs("/USN"))
	{
		sint64 l;
		sint64 t;
		sint64 r;
		sint64 b;
		char *outCsvFile;

		l = _atoi64(nextArg());
		t = _atoi64(nextArg());
		r = _atoi64(nextArg());
		b = _atoi64(nextArg());
		outCsvFile = nextArg();

		MakeUlamSpiral_N(l, t, r, b, outCsvFile);
		return;
	}
	error_m("不明な引数");
}
int main(int argc, char **argv)
{
	Main2();
	termination(0);
}
