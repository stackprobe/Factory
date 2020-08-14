/*
	PipeLogger.exe [/DT ���O�o��DIR | /UDT ���O�o��DIR | /B ���O�o�̓t�@�C���x�[�X�� | /BH ���O�o�̓t�@�C���x�[�X�� | ���O�o�̓t�@�C��]

		/BH ... 1���Ԗ��Ƀ��O�o�̓t�@�C�������X�V����B

	- - -

	�g����

		�C�ӂ̃R�}���h | PipeLogger

		�ƁA���s���邱�ƂŔC�ӂ̃R�}���h�̕W���o�͂�\�����A���O�t�@�C���ɏ����o����B
		�e�s���Ƀ^�C���X�^���v���t���B
*/

#include "C:\Factory\Common\all.h"

#define LOGLINELENMAX 1000000
#define LOGFILE_CHANGE_PERIOD_SEC 3600

static char *LogFile;
static FILE *LogFp;

static char *LogFileBase = NULL; // NULL == ���O�o�̓t�@�C�����̍X�V�A����
static uint NextChangeTime;

static char *AutoGenLogFileFilter(char *file)
{
	return toCreatablePath(file, 9999); // ���� 40 �� (60�`99), ���� 4000 �� (6000�`9999) �̑��݂��Ȃ����b�̒��Ō�����͂��B�Ƃ����z��
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
	line2JLine(line, 1, 0, 1, 1); // �\�������ɋ���
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
		char *localFile = xcout("%s.log", c_makeCompactStamp(NULL));

//		LogFile = combine(dir, localFile); // �l�b�g���[�N�p�X�s��
		LogFile = xcout("%s\\%s", dir, localFile); // dir �Ƀl�b�g���[�N�p�X���z�肷��B
		LogFile = AutoGenLogFileFilter(LogFile);
		PipeLogger();
		memFree(localFile);
		return;
	}
	if(argIs("/UDT"))
	{
		char *dir = nextArg();
		char *localFile = xcout("%s_%s.log", getEnvLine("USERNAME"), c_makeCompactStamp(NULL));

//		LogFile = combine(dir, localFile); // �l�b�g���[�N�p�X�s��
		LogFile = xcout("%s\\%s", dir, localFile); // dir �Ƀl�b�g���[�N�p�X���z�肷��B
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

	errorCase_m(hasArgs(2), "�s���ȃR�}���h����");

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
