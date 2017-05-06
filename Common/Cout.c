#include "all.h"

#define LOGFILESIZE_MAX 10000000 // 10m
#define LOGFILE_PERIOD 3600

int coutOff;

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
void setCoutLogFileAdd(char *fileBase)
{
	char *file;
	char *stamp;

	errorCase(WrFP);
	stamp = makeCompactStamp(NULL);
	stamp[10] = '\0'; // １時間で区切る。サイズは見ない！デカくなってもファイルを切り替えない！
	file = xcout("%s_%s0000.log", fileBase, stamp);
	WrFP = fileOpen(file, "ab");
	addFinalizer(CloseWrFP);
	memFree(file);
	memFree(stamp);

	writeLine_x(WrFP, xcout("[%s] ログ追記開始", makeJStamp(NULL, 0)));
}
void cout(char *format, ...)
{
	va_list marker;

	if(coutOff)
		return;

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
				WrFP = NULL; // error(); 対策
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
		buffer[size + 10] = '\0'; // 強制的に閉じる。

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
