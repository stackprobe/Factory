#include "all.h"

#define LOGFILESIZE_MAX 10000000 // 10m
#define LOGFILE_PERIOD 3600

static FILE *WrFP;
static char *LogFileBase;
static uint64 LogFileSize;
static uint LogFileTime;

static void CloseWrFP(void)
{
	if(WrFP)
	{
		fileClose(WrFP);
		WrFP = NULL;
	}
}
void setCoutWrFile(char *file, char *mode)
{
	if(WrFP)
	{
		fileClose(WrFP);
		WrFP = fileOpen(file, mode);
	}
	else
	{
		WrFP = fileOpen(file, mode);
		addFinalizer(CloseWrFP);
	}
}
void unsetCoutWrFile(void)
{
	CloseWrFP();
}
static void OpenLogFile(void)
{
	char *file;
	char *stamp = makeCompactStamp(NULL);

	file = xcout("%s_%s000.log", LogFileBase, stamp);
	file = toCreatablePath(file, 1000);
	WrFP = fileOpen(file, "wb");
	memFree(file);
	memFree(stamp);
	LogFileTime = now();
}
void setCoutLogFile(char *fileBase)
{
	errorCase(WrFP);
	LogFileBase = fileBase;
	OpenLogFile();
	addFinalizer(CloseWrFP);
}
void cout(char *format, ...)
{
	va_list marker;

	if(WrFP)
	{
		int ret;

		va_start(marker, format);
		ret = vfprintf(WrFP, format, marker);

		if(ret < 0)
		{
			error();
		}
		va_end(marker);

		if(LogFileBase)
		{
			LogFileSize += ret;

			if(LOGFILESIZE_MAX <= LogFileSize || LOGFILE_PERIOD <= now() - LogFileTime)
			{
				fileClose(WrFP);
				WrFP = NULL; // error(); ‘Îô
				OpenLogFile();
				LogFileSize = 0;
			}
		}
	}
	va_start(marker, format);

	if(vprintf(format, marker) < 0)
	{
		error();
	}
	va_end(marker);
}
char *xcout(char *format, ...)
{
	char *buffer;
	uint size;
	va_list marker;

	va_start(marker, format);

	for(size = strlen(format) + 128; ; size *= 2)
	{
		sint retval;

		buffer = (char *)memAlloc(size + 20);
		retval = _vsnprintf(buffer, size + 10, format, marker);
		buffer[size + 10] = '\0'; // ‹­§“I‚É•Â‚¶‚éB

		if(0 <= retval && retval <= size)
		{
			break;
		}
		memFree(buffer);

		if(128 * 1024 * 1024 < size) // ANTI OVER-FLOW
		{
			error();
		}
	}
	va_end(marker);

	return buffer;
}
void coutJLine(char *line)
{
	coutJLine_x(strx(line));
}
void coutJLine_x(char *line)
{
	line2JLine(line, 1, 0, 1, 1);
	cout("%s\n", line);
	memFree(line);
}
