#include "C:\Factory\Common\all.h"
#include "C:\Factory\Meteor\ZCluster.h"

#define INDEX_FILE_ON_INDEX "_index"

static char *RootDir;
static autoList_t *ResSubDirs;
static char *DDResFile;
static char *MaskingExeFile;

static FILE *DDResFp;

static int FF_FindStartPtn(autoList_t *startPtns, char *file)
{
	char *stPtn;
	uint stPtn_index;

	foreach(startPtns, stPtn, stPtn_index)
		if(startsWithICase(file, stPtn))
			return 1;

	return 0;
}
static void FilesFilter(autoList_t *files)
{
	char *file;
	uint index;

	foreach(files, file, index)
	{
		if(*getLocal(file) == '_') // '_' �Ŏn�܂�t�@�C���� DDResFile �Ɋ܂߂Ȃ��B
		{
			cout("_d: %s\n", file);

			memFree((char *)fastDesertElement(files, index));
			index--;
		}
	}
	sortJLinesICase(files);

	if(getCount(ResSubDirs)) // ? �T�u�f�B���N�g���w��L��
	{
		autoList_t *startPtns = newList();
		char *subDir;
		uint subDir_index;

		cout("�T�u�f�B���N�g���̎w��L��.1\n");

		foreach(ResSubDirs, subDir, subDir_index)
		{
			char *stPtn = xcout("%s\\%s\\", RootDir, subDir);

			trimPath(stPtn);
			cout("stPtn: %s\n", stPtn);
			addElement(startPtns, (uint)stPtn);
		}
		foreach(files, file, index)
		{
			if(FF_FindStartPtn(startPtns, file))
			{
				cout("[��] %s\n", file);
			}
			else
			{
				cout("[�~] %s\n", file);

				file[0] = '\0';
			}
		}
		trimLines(files);
		releaseDim(startPtns, 1);

		cout("�T�u�f�B���N�g���̎w��L��.2\n");
	}
	else
		cout("�T�u�f�B���N�g���̎w�薳��\n");
}
static void MaskFileData(autoBlock_t *fileData)
{
	char *file = makeTempPath(NULL);
	autoBlock_t *fileDataNew;

	writeBinary(file, fileData);

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" MASK-GZ-DATA \"%s\"", MaskingExeFile, file));

	errorCase(!existFile(file)); // 2bs?

	fileDataNew = readBinary(file);
	ab_swap(fileData, fileDataNew);
	releaseAutoBlock(fileDataNew);
	removeFile_x(file);
}
static void AddToDDResFile(char *file)
{
	char *wFile = makeTempPath(NULL);
	autoBlock_t *fileData;

	ZC_Pack(file, wFile);

	fileData = readBinary(wFile);

	MaskFileData(fileData);

	writeValue(DDResFp, getSize(fileData));
	writeBinaryBlock(DDResFp, fileData);

	removeFile_x(wFile);
	releaseAutoBlock(fileData);
}
static void MakeDDResFile(void)
{
	autoList_t *files = lssFiles(RootDir);
	autoList_t *indexes;
	char *file;
	uint index;
	char *indexFile = makeTempPath(NULL);

	FilesFilter(files);

	sortJLinesICase(files);
	indexes = copyLines(files);
	changeRoots(indexes, RootDir, NULL);
	insertElement(indexes, 0, (uint)strx(INDEX_FILE_ON_INDEX));
	writeLines(indexFile, indexes);

	DDResFp = fileOpen(DDResFile, "wb");

	AddToDDResFile(indexFile);

	foreach(files, file, index)
		AddToDDResFile(file);

	fileClose(DDResFp);
	DDResFp = NULL;

	releaseDim(files, 1);
	releaseDim(indexes, 1);
	removeFile_x(indexFile);
}
int main(int argc, char **argv)
{
	RootDir = makeFullPath(nextArg());
	ResSubDirs = newList();

	while(argIs("/SD"))
		addElement(ResSubDirs, (uint)nextArg());

	DDResFile      = makeFullPath(nextArg());
	MaskingExeFile = makeFullPath(nextArg());

	errorCase_m(hasArgs(1), "�s���ȃR�}���h����");

	errorCase(!existDir(RootDir));
//	ResSubDirs
//	DDResFile
	errorCase(!existFile(MaskingExeFile));

	MakeDDResFile();
}
