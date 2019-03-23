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
	stamp[10] = '\0'; // �P���Ԃŋ�؂�B�T�C�Y�͌��Ȃ��I�f�J���Ȃ��Ă��t�@�C����؂�ւ��Ȃ��I
	file = xcout("%s_%s0000.log", fileBase, stamp);
	WrFP = fileOpen(file, "ab");
	addFinalizer(CloseWrFP);
	memFree(file);
	memFree(stamp);

	writeLine_x(WrFP, xcout("[%s] ���O�ǋL�J�n", makeJStamp(NULL, 0)));
}
void cout(char *format, ...)
{
	va_list marker;

	if(coutOff)
		return;

#if 0 // moved @ 2019.3.21
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
				WrFP = NULL; // error(); �΍�
				OpenLogFile();
				LogFileSize = 0;
			}
		}
	}
#endif
	va_start(marker, format);

#if 1
	if(!strcmp(format, "%s"))
	{
		coutLongText(va_arg(marker, char *));
	}
	else if(!strcmp(format, "%s\n"))
	{
		coutLongText(va_arg(marker, char *));
		coutLongText("\n");
	}
	else
	{
		coutLongText_x(vxcout(format, marker));
	}
#else // del @ 2019.3.21
	if(vprintf(format, marker) < 0)
	{
		error();
	}
#endif
	va_end(marker);
}
char *xcout(char *format, ...)
{
	char *ret;
	va_list marker;

	va_start(marker, format);
	ret = vxcout(format, marker);
	va_end(marker);

	return ret;
}
char *vxcout(char *format, va_list marker)
{
	char *buffer;
	uint size;

	for(size = strlen(format) + 100; ; size *= 2)
	{
		sint ret;

#define MARGIN 10

		buffer = (char *)memAlloc(size + MARGIN * 2);
		ret = _vsnprintf(buffer, size + MARGIN, format, marker);

#undef MARGIN

		if(0 <= ret && ret <= size)
			break;

		memFree(buffer);
		errorCase(UINTMAX / 4 < size); // ANTI OVER-FLOW
	}
	return strr(buffer);
}
void coutJLine(char *line)
{
	coutJLine_x(strx(line));
}
void coutJLine_x(char *line)
{
	line2JLine(line, 1, 0, 1, 1);
#if 1
	coutLongText_x(line);
	coutLongText("\n");
#else // old_same
	cout("%s\n", line);
	memFree(line);
#endif
}

/*
	2019.3.21
	printf("%s", text); �� text �������ꍇ�A�\�������� printf �� -1 ��Ԃ��B
	vprintf, fputs �ł��������ۂ��N����B
	text �̓��e�ɂ����B
	"<br/>�t�@�C��<br/>" x 1000 �ł� 4100 �����ڂ�����ŕ\���������B�Ōオ "�t�@�EC" �ɂȂ�BC �� 4099 ������
		--> https://github.com/stackprobe/Annex/blob/master/Labo/printf_problem.c
	HTML�^�O�ƃJ�^�J�i���܂܂�Ă���ƋN����₷���͗l�B���p�����������ƋN����Ȃ��͗l�B
	�Z���ƋN����Ȃ��B
	---> �����e�L�X�g�͕������ĕ\�����ĉ������B
	vfprintf, _vsnprintf �͖�薳�����ۂ��Bstdout �ɏo�͂���ƃ}�Y���̂��B
*/
void coutLongText(char *text)
{
	char format[6]; // max: "%.99s"
	char *p;
	char *q;
	uint d;

	for(p = text; *p; p = q)
	{
#define PRINT_LMT 98

		for(q = p; *q && (uint)q - (uint)p < PRINT_LMT; q = mbsNext(q))
		{}

#undef PRINT_LMT

		d = (uint)q - (uint)p;

		errorCase(sprintf(format, "%%.%us", d) < 0);
		errorCase(printf(format, p) != d);
	}
	if(WrFP)
	{
		errorCase(fputs(text, WrFP) < 0);

		if(LogFileBase)
		{
			uint textLen = (uint)q - (uint)p;

			LogFileSize += textLen;

			if(LOGFILESIZE_MAX <= LogFileSize || LOGFILE_PERIOD <= now() - LogFileTime)
			{
				fileClose(WrFP);
				WrFP = NULL; // error(); �΍�
				OpenLogFile();
				LogFileSize = 0;
			}
		}
	}
}
void coutLongText_x(char *text)
{
	coutLongText(text);
	memFree(text);
}
