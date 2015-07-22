#include "UlamSpiral.h"

#define W_MAX IMAX
#define H_MAX IMAX

#define X_MIN -0x7fffffffi64
#define X_MAX  0x80000000i64
#define Y_MIN -0x80000000i64
#define Y_MAX  0x7fffffffi64

static void CheckLTRB(sint64 l, sint64 t, sint64 r, sint64 b)
{
	errorCase(r < l);
	errorCase(b < t);
	errorCase(l < X_MIN);
	errorCase(X_MAX < r);
	errorCase(t < Y_MIN);
	errorCase(Y_MAX < b);
	errorCase(W_MAX <= r - l);
	errorCase(H_MAX <= b - t);
}
static uint64 XYToNumb(sint64 x, sint64 y) // ret: 2^64 ˆÈã‚Ì‚Æ‚« 0 ‚ð•Ô‚·B
{
	__int64 ax;
	__int64 ay;
	__int64 r;
	uint64 numb;

	if(y == Y_MIN)
	{
		if(x == X_MIN)
			return 0;

		return UINT64MAX - (x - X_MIN) + 1;
	}
	if(x == X_MAX)
		return UINT64MAX - UINTMAX - (y - Y_MIN) + 1;

	if(x == 0 && y == 0)
		return 1;

	ax = _abs64(x);
	ay = _abs64(y);

	r = m_max(ax, ay);
	numb = r;
	numb *= 2;
	numb++;
	numb *= numb;

	if(ax <= y) // ‰º
		return numb - (r - x);

	numb -= r * 2;

	if(x <= -ay) // ¶
		return numb - (r - y);

	numb -= r * 2;

	if(y <= -ax) // ã
		return numb - (x - (-r));

	numb -= r * 2;
	return numb - (y - (-r)); // ‰E
}
void MakeUlamSpiral(
	sint64 l,
	sint64 t,
	sint64 r,
	sint64 b,
	uint primeColor,
	uint notPrimeColor,
	uint centerColor,
	char *outBmpFile,
	char *cancelEvName,
	char *reportEvName,
	char *reportMtxName,
	char *reportFile
	)
{
	uint cancelEv;
	uint reportEv;
	uint reportMtx;
	uint w;
	uint h;
	uint x;
	uint y;
	autoList_t *bmp;
	int cancelled = 0;

	LOGPOS();

	CheckLTRB(l, t, r, b);
	// primeColor
	// notPrimeColor
	// centerColor
	errorCase(m_isEmpty(outBmpFile));
	errorCase(m_isEmpty(cancelEvName));
	errorCase(m_isEmpty(reportEvName));
	errorCase(m_isEmpty(reportMtxName));
	errorCase(m_isEmpty(reportFile));

	removeFileIfExist(outBmpFile);

	cancelEv = eventOpen(cancelEvName);
	reportEv = eventOpen(reportEvName);
	reportMtx = mutexOpen(reportMtxName);

	w = r - l + 1;
	h = b - t + 1;

	bmp = createAutoList(h);

	for(y = 0; y < h; y++)
	{
		autoList_t *row = createAutoList(w);

		for(x = 0; x < w; x++)
		{
			addElement(row, notPrimeColor);
		}
		addElement(bmp, (uint)row);
	}
	for(y = 0; y < h; y++)
	{
		autoList_t *row = getList(bmp, y);

		for(x = 0; x < w; x++)
		{
			sint64 rx = l + x;
			sint64 ry = t + y;
			uint64 numb;
			static uint ps_nextSec;

			if(x % 100 == 0 && pulseSec(10, &ps_nextSec))
			{
				if(handleWaitForMillis(cancelEv, 0))
				{
					cancelled = 1;
					break;
				}

				handleWaitForever(reportMtx);
				writeOneLine_cx(reportFile, xcout("%u\n%u\n%u\n%u", y, x, h, w));
				mutexRelease(reportMtx);

				eventSet(reportEv);
			}
			numb = XYToNumb(rx, ry);

			if(numb == 1)
			{
				setElement(row, x, centerColor);
			}
			else if(IsPrime(numb))
			{
				setElement(row, x, primeColor);
			}
		}
	}
	if(!cancelled)
		writeBMPFile_cx(outBmpFile, bmp);

	handleWaitForever(reportMtx);
	removeFileIfExist(reportFile);
	mutexRelease(reportMtx);

	handleClose(cancelEv);
	handleClose(reportEv);
	handleClose(reportMtx);

	LOGPOS();
}
void MakeUlamSpiral_N(sint64 l, sint64 t, sint64 r, sint64 b, char *outCsvFile)
{
	uint w;
	uint h;
	uint x;
	uint y;
	autoList_t *csv;

	LOGPOS();

	CheckLTRB(l, t, r, b);
	errorCase(m_isEmpty(outCsvFile));

	w = r - l + 1;
	h = b - t + 1;

	csv = newList();

	for(y = 0; y < h; y++)
	{
		autoList_t *row = newList();

		for(x = 0; x < w; x++)
		{
			static uint ps_nextSec;

			if(x % 100 == 0 && pulseSec(10, &ps_nextSec))
			{
				cout("ulam_csv %u %u now...\n", x, y);
			}
			addElement(row, (uint)xcout("%I64u", XYToNumb(l + x, t + y)));
		}
		addElement(csv, (uint)row);
	}
	writeCSVFile_cx(outCsvFile, csv);

	LOGPOS();
}
