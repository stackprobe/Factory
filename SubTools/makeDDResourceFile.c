#include "C:\Factory\Common\all.h"
#include "C:\Factory\Meteor\ZCluster.h"

static char *RootDir;
static char *DDResFile;

static FILE *DDResFp;

static void AddToDDResFile(char *file)
{
	char *wFile = makeTempPath(NULL);
	autoBlock_t *fileData;

	ZC_Pack(file, wFile);

	fileData = readBinary(wFile);

	if(getSize(fileData)) // not empty -> mask gz-signature
	{
		errorCase(getByte(fileData, 0) != 0x1f);
		errorCase(getByte(fileData, 1) != 0x8b);

		setByte(fileData, 0, 'D');
		setByte(fileData, 1, 'D');
	}
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

	sortJLinesICase(files);
	indexes = copyLines(files);
	changeRoots(indexes, RootDir, NULL);
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
	RootDir   = makeFullPath(nextArg());
	DDResFile = makeFullPath(nextArg());

	errorCase(!existDir(RootDir));

	MakeDDResFile();
}
