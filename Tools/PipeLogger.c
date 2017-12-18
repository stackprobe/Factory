/*
	PipeLogger.exe [/DT ログ出力DIR | /UDT ログ出力DIR | ログ出力ファイル]

	- - -

	使い方

		任意のコマンド | PipeLogger

		と、実行することで任意のコマンドの標準出力を表示しつつ、ログファイルに書き出せる。
		各行頭にタイムスタンプが付く。
*/

#include "C:\Factory\Common\all.h"

#define LOGLINELENMAX 1000000

static char *LogFile;
static FILE *LogFp;

static void WrLog(char *line)
{
	line = xcout("[%s] %s", c_makeJStamp(NULL, 0), line);
	writeLine(LogFp, line);
	fflush(LogFp);
	line2JLine(line, 1, 0, 1, 1); // 表示向けに矯正
	cout("%s\n", line);
	memFree(line);
}
static void PipeLogger(void)
{
	char *line = memAlloc(LOGLINELENMAX + 1);
	uint wPos = 0;

	cout("ログファイル：%s\n", LogFile);

	LogFp = fileOpen(LogFile, "wt");

	WrLog("ログ開始");

	for(; ; )
	{
		int chr = fgetc(stdin);

		if(chr == EOF)
			break;

		if(chr == '\r')
		{
			// noop
		}
		else if(chr == '\n')
		{
			line[wPos] = '\0';
			WrLog(line);
			wPos = 0;
		}
		else
		{
			if(LOGLINELENMAX <= wPos)
			{
				line[wPos] = '\0';
				WrLog(line);
				wPos = 0;
			}
			line[wPos++] = chr;
		}
	}
	if(wPos)
	{
		line[wPos] = '\0';
		WrLog(line);
	}
	WrLog("ログ終了");

	fileClose(LogFp);
	LogFp = NULL;

	memFree(line);
}
int main(int argc, char **argv)
{
	if(argIs("/DT"))
	{
		char *dir = nextArg();
		char *lFile = xcout("%s.log", c_makeCompactStamp(NULL));

		LogFile = combine(dir, lFile); // ネットワークパスng
		PipeLogger();
		memFree(lFile);
		return;
	}
	if(argIs("/UDT"))
	{
		char *dir = nextArg();
		char *lFile = xcout("%s_%s.log", getEnvLine("USERNAME"), c_makeCompactStamp(NULL));

		LogFile = xcout("%s\\%s", dir, lFile);
//		LogFile = combine(dir, lFile); // ネットワークパスng
		PipeLogger();
		memFree(lFile);
		return;
	}
	if(hasArgs(1))
	{
		LogFile = nextArg();
		PipeLogger();
		return;
	}

	{
		LogFile = getOutFile("PipeLog.txt");
		PipeLogger();
		openOutDir();
		return;
	}
}
