/*
	CsvToTsv.exe [入力CSVファイル 出力TSVファイル | /T2C 入力TSVファイル 出力CSVファイル]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csv.h"

static void XsvFileToXsvFile(char *rFile, int rCDlm, char *wFile, int wCDlm)
{
	autoList_t *tbl;

	CSVCellDelimiter = rCDlm;
	tbl = readCSVFile(rFile);
	CSVCellDelimiter = wCDlm;
	writeCSVFile(wFile, tbl);
	releaseDim(tbl, 2);
}
static void CsvFileToTsvFile(char *rFile, char *wFile)
{
	cout(".csv => .tsv\n");

	XsvFileToXsvFile(rFile, ',', wFile, '\t');
}
static void TsvFileToCsvFile(char *rFile, char *wFile)
{
	cout(".tsv => .csv\n");

	XsvFileToXsvFile(rFile, '\t', wFile, ',');
}
int main(int argc, char **argv)
{
	if(argIs("/T2C")) // TSV to CSV
	{
		if(hasArgs(2))
		{
			TsvFileToCsvFile(getArg(0), getArg(1));
			return;
		}
	}

	if(hasArgs(2))
	{
		CsvFileToTsvFile(getArg(0), getArg(1));
		return;
	}

	{
		char *file = c_dropFile();

		if(!_stricmp(getExt(file), "csv"))
		{
			CsvFileToTsvFile(file, c_getOutFile("output.tsv"));
			openOutDir();
		}
		else if(!_stricmp(getExt(file), "tsv"))
		{
			TsvFileToCsvFile(file, c_getOutFile("output.csv"));
			openOutDir();
		}
		else
		{
			cout("拡張子？\n");
		}
	}
}
