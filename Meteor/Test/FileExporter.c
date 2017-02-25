#include "C:\Factory\Common\all.h"
#include "..\FileExporter.h"

static int CBNoop(char *file, char *realPath)
{
	cout("f %s\n", file);
	cout("r %s\n", realPath);

	return 1;
}
int main(int argc, char **argv)
{
	FileExportTouchImport(nextArg(), CBNoop);
}
