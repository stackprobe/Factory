#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csvStream.h"

static char *HTTDir;
static char *LogDir;
static int OutputAndDelete;

static char *GetCaptureFile(void)
{
	static char *file;

	if(!file)
		file = combine(getSelfDir(), "Capture.exe");

	return file;
}
static char *GetWFileByRow(autoList_t *row)
{
	char *stamp = getLine(row, 0);
	char *file;

	cout("< %s\n", stamp);
	stamp = strx(stamp);

	errorCase(!lineExp("<4,09>//<2,09>//<2,09><>", stamp));

	stamp[10] = '\0';
	eraseChar(stamp + 7);
	eraseChar(stamp + 4);

	file = combine(LogDir, stamp);
	file = addExt(file, "log");

	memFree(stamp);
	cout("> %s\n", file);
	return file;
}
static void DistributeToLog(char *rFile)
{
	FILE *rfp = fileOpen(rFile, "rb");

	for(; ; )
	{
		autoList_t *row = readCSVRow(rfp);
		char *wFile;
		FILE *wfp;

		if(!row)
			break;

		wFile = GetWFileByRow(row);
		wfp = fileOpen(wFile, "ab");

		writeCSVRow_x(wfp, row);

		fileClose(wfp);
		memFree(wFile);
	}
	fileClose(rfp);
}
static void AddLog(void)
{
	char *midFile = makeTempPath(NULL);

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\"%s \"%s\" \"%s\"", GetCaptureFile(), OutputAndDelete ? " /OAD" : "", HTTDir, midFile));

	LOGPOS();

	DistributeToLog(midFile);

	LOGPOS();

	removeFile(midFile);
	memFree(midFile);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/OAD"))
	{
		LOGPOS();
		OutputAndDelete = 1;
		goto readArgs;
	}

	HTTDir = nextArg();
	LogDir = nextArg();

	errorCase(!existDir(HTTDir));
	errorCase(!existDir(LogDir));

	AddLog();
}
