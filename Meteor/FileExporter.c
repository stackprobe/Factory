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

// ---- FileExportTouchImport ----

static char *FETI_TargetDir;

static void FETI_Import(void)
{
	FileImporter(FETI_TargetDir);
}
void FileExportTouchImport(char *targetDir, int (*callback)(char *entityFile, char *realPath))
{
	char *wrkBnchDir = makeTempPath(NULL);

	errorCase(m_isEmpty(targetDir));
	errorCase(!callback);

	FileExporter(targetDir, wrkBnchDir);
	FETI_TargetDir = targetDir;
	addFinalizer(FETI_Import);

	{
		autoList_t *files = lsFiles(wrkBnchDir);
		char *file;
		uint index;

		foreach(files, file, index)
		{
			if(!_stricmp("dat", getExt(file)))
			{
				char *file_s = changeExt(file, "file_s");
				char *realFile;
				char *entityFile = makeTempPath("entity");
				char *midFile = makeTempPath(NULL);
				int ret;

				realFile = readText_b(file_s);

				LOGPOS();
				copyFile(file, entityFile);
				LOGPOS();

				ret = callback(entityFile, realFile);

				LOGPOS();

				moveFile(entityFile, midFile);
				removeFile(file);
				moveFile(midFile, file);

				memFree(file_s);
				memFree(realFile);
				memFree(entityFile);
				memFree(midFile);

				if(!ret)
					break;
			}
		}
		releaseDim(files, 1);
	}

	unaddFinalizer(FETI_Import);
	FETI_Import();
	FETI_TargetDir = NULL;

	removeDir(wrkBnchDir);
	memFree(wrkBnchDir);
}

// ----
