/*
	PipeLogger.exe [/DT ���O�o��DIR | /UDT ���O�o��DIR | ���O�o�̓t�@�C��]

	- - -

	�g����

		�C�ӂ̃R�}���h | PipeLogger

		�ƁA���s���邱�ƂŔC�ӂ̃R�}���h�̕W���o�͂�\�����A���O�t�@�C���ɏ����o����B
		�e�s���Ƀ^�C���X�^���v���t���B
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
	line2JLine(line, 1, 0, 1, 1); // �\�������ɋ���
	cout("%s\n", line);
	memFree(line);
}
static void PipeLogger(void)
{
	char *line = memAlloc(LOGLINELENMAX + 1);
	uint wPos = 0;

	cout("���O�t�@�C���F%s\n", LogFile);

	LogFp = fileOpen(LogFile, "wt");

	WrLog("���O�J�n");

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
	WrLog("���O�I��");

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

		LogFile = combine(dir, lFile); // �l�b�g���[�N�p�Xng
		PipeLogger();
		memFree(lFile);
		return;
	}
	if(argIs("/UDT"))
	{
		char *dir = nextArg();
		char *lFile = xcout("%s_%s.log", getEnvLine("USERNAME"), c_makeCompactStamp(NULL));

		LogFile = xcout("%s\\%s", dir, lFile);
//		LogFile = combine(dir, lFile); // �l�b�g���[�N�p�Xng
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