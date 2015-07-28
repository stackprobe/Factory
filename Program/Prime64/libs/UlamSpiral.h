#pragma once

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Prime2.h"
#include "C:\Factory\Common\Options\csv.h"
#include "C:\Factory\SubTools\libs\bmp.h"

extern int US_OddMode;
extern int US_OddUn5Mode;
extern int US_Mul2Mode;
extern int US_Mul3Mode;
extern int US_Mul5Mode;
extern int US_Mul7Mode;
extern int US_Mul11Mode;
extern int US_Mul13Mode;
extern int US_Mul17Mode;
extern int US_Mul19Mode;

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
	);

void MakeUlamSpiral_Csv(sint64 l, sint64 t, sint64 r, sint64 b, int mode, char *outCsvFile);
