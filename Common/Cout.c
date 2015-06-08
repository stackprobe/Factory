#include "all.h"

static FILE *WrFP;

static void CloseWrFP(void)
{
	errorCase(!WrFP);
	fileClose(WrFP);
	WrFP = NULL;
}
void setCoutWrFile(char *file, int appendMode)
{
	errorCase(WrFP);
	WrFP = fileOpen(file, appendMode ? "at" : "wt");
	addFinalizer(CloseWrFP);
}
void cout(char *format, ...)
{
	va_list marker;

	if(WrFP)
	{
		va_start(marker, format);

		if(vfprintf(WrFP, format, marker) < 0)
		{
			error();
		}
		va_end(marker);
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
