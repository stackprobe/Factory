/*
	Žg—p—á

		HTTPProxy 80 a 1 /f- "HTTPPSimpleSaver.exe C:\temp"
*/

#include "C:\Factory\Common\all.h"
#include "libs\HTTPParse.h"

#define MARGIN_PTN "000"
//#define MARGIN_PTN "0000"
//#define MARGIN_PTN "00000"

int main(int argc, char **argv)
{
	char *outDir = nextArg();
	char *outFile;

	outFile = combine(outDir, xcout("%s" MARGIN_PTN ".txt", makeCompactStamp(NULL)));
	outFile = toCreatablePath(outFile, IMAX);

	LoadHttpDat(DEF_HTTP_DAT_FILE);

	writeBinary(outFile, HttpDat.Body);

	memFree(outFile);
}
