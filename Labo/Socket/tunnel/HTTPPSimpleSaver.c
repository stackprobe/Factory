/*
	使用例

		HTTPProxy 80 a 1 /f- "HTTPPSimpleSaver.exe C:\temp"
		SmplProxy /f- "HTTPPSimpleSaver.exe /C image/ /C audio/ /C video/"
		SmplProxy /f- "HTTPPSimpleSaver.exe /C image/"
		SmplProxy /f- HTTPPSimpleSaver.exe
*/

#include "C:\Factory\Common\all.h"
#include "libs\HTTPParse.h"
#include "C:\Factory\Labo\Socket\libs\http\ExtToContentType.h"

#define MARGIN_PTN "000"
//#define MARGIN_PTN "0000"
//#define MARGIN_PTN "00000"

static autoList_t *OutContentPtns;

static char *GetContentType(void)
{
	char *line;
	uint index;

	// DEF_HTTP_DAT_FILE は再形成なので、きっちり KEY + ": " + VALUE になっているはず。

	foreach(HttpDat.Header, line, index)
		if(startsWithICase(line, "Content-Type: "))
			return strchr(line, ':') + 2;

	return ""; // not found
}
static int IsCorrectContentType(char *contentType)
{
	char *line;
	uint index;

	if(!OutContentPtns) // ? 未指定時 -> 何でもアリ
		return 1;

	foreach(OutContentPtns, line, index)
		if(mbs_stristr(contentType, line))
			return 1;

	return 0;
}
int main(int argc, char **argv)
{
	char *outDir;
	char *outFile;
	char *contentType;

readArgs:
	if(argIs("/C"))
	{
		if(!OutContentPtns)
			OutContentPtns = newList();

		addElement(OutContentPtns, (uint)nextArg());
		goto readArgs;
	}
	if(hasArgs(1))
		outDir = nextArg();
	else
		outDir = "C:\\temp";

	LoadHttpDat(DEF_HTTP_DAT_FILE);
	contentType = GetContentType();

	if(IsCorrectContentType(contentType))
	{
		outFile = combine(outDir, xcout("%s" MARGIN_PTN ".%s", makeCompactStamp(NULL), httpContentTypeToExt(contentType)));
		outFile = toCreatablePath(outFile, IMAX);

		writeBinary(outFile, HttpDat.Body);
	}
	memFree(outFile);
}
