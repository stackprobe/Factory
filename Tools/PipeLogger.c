/*
	PipeLogger.exe [/DT ログ出力DIR | /UDT ログ出力DIR | /B ログ出力ファイルベース名 | /BH ログ出力ファイルベース名 | ログ出力ファイル]

		/BH ... 1時間毎にログ出力ファイル名を更新する。

	- - -

	使い方

		任意のコマンド | PipeLogger

		と、実行することで任意のコマンドの標準出力を表示しつつ、ログファイルに書き出せる。
		各行頭にタイムスタンプが付く。
*/

#include "C:\Factory\Common\all.h"

#define LOGLINELENMAX 1000000
#define LOGFILE_CHANGE_PERIOD_SEC 3600

static char *LogFile;
static FILE *LogFp;

static char *LogFileBase = NULL; // NULL == ログ出力ファイル名の更新、無効
static uint NextChangeTime;

static char *AutoGenLogFileFilter(char *file)
{
	return toCreatablePath(file, 9999); // 毎分 40 個 (60～99), 毎時 4000 個 (6000～9999) の存在しない分秒の中で見つかるはず。という想定
}
static char *GetNextLogFile(void)
{
	return xcout("%s%s.log", LogFileBase, c_makeCompactStamp(NULL));
}
static void WrLog(char *line)
{
	line = xcout("[%s] %s", c_makeJStamp(NULL, 0), line);
	writeLine(LogFp, line);
	fflush(LogFp);
	line2JLine(line, 1, 0, 1, 1); // 表示向けに矯正
	cout("%s\n", line);
	memFree(line);

	if(LogFileBase)
	{
		uint currTime = now();

		if(NextChangeTime < currTime)
		{
			memFree(LogFile);
			LogFile = GetNextLogFile();
			LogFile = AutoGenLogFileFilter(LogFile);

			fileClose(LogFp);
			LogFp = fileOpen(LogFile, "wt");

			NextChangeTime = currTime + LOGFILE_CHANGE_PERIOD_SEC;
		}
	}
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
		char *localFile = xcout("%s.log", c_makeCompactStamp(NULL));

//		LogFile = combine(dir, localFile); // ネットワークパス不可
		LogFile = xcout("%s\\%s", dir, localFile); // dir にネットワークパスも想定する。
		LogFile = AutoGenLogFileFilter(LogFile);
		PipeLogger();
		memFree(localFile);
		return;
	}
	if(argIs("/UDT"))
	{
		char *dir = nextArg();
		char *localFile = xcout("%s_%s.log", getEnvLine("USERNAME"), c_makeCompactStamp(NULL));

//		LogFile = combine(dir, localFile); // ネットワークパス不可
		LogFile = xcout("%s\\%s", dir, localFile); // dir にネットワークパスも想定する。
		LogFile = AutoGenLogFileFilter(LogFile);
		PipeLogger();
		memFree(localFile);
		return;
	}
	if(argIs("/B"))
	{
		char *fileBase = nextArg();

		LogFile = xcout("%s%s.log", fileBase, c_makeCompactStamp(NULL));
		LogFile = AutoGenLogFileFilter(LogFile);
		PipeLogger();
		return;
	}
	if(argIs("/BH"))
	{
		LogFileBase = nextArg();

		LogFile = GetNextLogFile();
		LogFile = AutoGenLogFileFilter(LogFile);
		NextChangeTime = now() + LOGFILE_CHANGE_PERIOD_SEC;

		PipeLogger();
		return;
	}

	errorCase_m(hasArgs(2), "不明なコマンド引数");

	if(hasArgs(1))
	{
		LogFile = nextArg();
		PipeLogger();
		return;
	}

	{
		LogFile = getOutFile_x(xcout("PipeLog_%s.txt", c_makeCompactStamp(NULL)));
		PipeLogger();
		openOutDir();
		return;
	}
}
