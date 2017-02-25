#include "FileExporter.h"

static char *GetFileExporterExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\Kit\\FileExporter\\FileExporter.exe");

	return file;
}
/*
	rDir: ネットワークパス ok
*/
int FileExporter(char *rDir, char *wDir) // ret: ? successful
{
	char *successfulFile;
	int ret;

//	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

//	errorCase(!existDir(rDir));
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
//	memFree(rDir);
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

static char *FETI_WrkBnchDir;

static void FETI_Import(void)
{
	if(!FileImporter(FETI_WrkBnchDir))
		cout("Warning: FETI_Import error!\n");
}
/*
	targetDir: ネットワークパス ok
*/
void FileExportTouchImport(char *targetDir, int (*callback)(char *entityFile, char *realPath))
{
	char *wrkBnchDir = makeTempPath(NULL);

	errorCase(m_isEmpty(targetDir));
	errorCase(!callback);

	errorCase(!FileExporter(targetDir, wrkBnchDir));
	FETI_WrkBnchDir = wrkBnchDir;
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

				// FETI_Import() が runFinalizers() でちゃんと動くように copy -> move -> move している。

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
	FETI_WrkBnchDir = NULL;

	removeDir(wrkBnchDir);
	memFree(wrkBnchDir);
}

// ----
