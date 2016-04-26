#include "FileExporter.h"

static char *GetFileExporterExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\Kit\\FileExporter\\FileExporter.exe");

	return file;
}
int FileExporter(char *rDir, char *wDir) // ret: ? successful
{
	char *successfulFile;
	int ret;

	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	errorCase(!existDir(rDir));
	errorCase(existDir(wDir));

	createDir(wDir);

	successfulFile = makeTempPath(NULL);

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" /E \"%s\" \"%s\" \"%s\"", GetFileExporterExeFile(), rDir, wDir, successfulFile));

	ret = existFile(successfulFile);

	if(ret)
	{
		removeFile(successfulFile);

		addCwd(wDir);
		{
			coExecute("ATTRIB.EXE -R +A -S -H *");
		}
		unaddCwd();
	}
	memFree(rDir);
	memFree(wDir);
	memFree(successfulFile);

	return ret;
}
int FileImporter(char *rDir) // ret: ? successful
{
	char *successfulFile;
	int ret;

	rDir = makeFullPath(rDir);
	errorCase(!existDir(rDir));

	successfulFile = makeTempPath(NULL);

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" /I \"%s\" \"%s\"", GetFileExporterExeFile(), rDir, successfulFile));

	ret = existFile(successfulFile);

	if(ret)
		removeFile(successfulFile);

	memFree(rDir);
	memFree(successfulFile);

	return ret;
}
