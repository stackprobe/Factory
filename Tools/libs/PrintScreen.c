#include "PrintScreen.h"

autoList_t *PrintScreen(void)
{
	char *dir = makeTempDir(NULL);
	char *fileBase;
	autoList_t *files;
	char *file;
	uint index;
	autoList_t *bmps = newList();

	errorCase(!existFile(FILE_TOOLKIT_EXE)); // 外部コマンド存在確認

	fileBase = combine(dir, "Screen_");

	coExecute_x(xcout(FILE_TOOLKIT_EXE " /PRINT-SCREEN %s", fileBase));

	files = lsFiles(dir);
	sortJLinesICase(files);

	foreach(files, file, index)
	{
		addElement(bmps, (uint)readBinary(file));
		removeFile(file);
	}
	removeDir(dir);
	memFree(dir);
	memFree(fileBase);
	releaseDim(files, 1);
	return bmps;
}
void PrintScreen_Dir(char *dir)
{
	autoList_t *bmps = PrintScreen();
	autoBlock_t *bmp;
	uint index;

	errorCase(!existDir(dir));

	foreach(bmps, bmp, index)
	{
		char *file = combine_cx(dir, xcout("Screen_%02u.bmp", (index + 1)));

		writeBinary(file, bmp);

		memFree(file);
		releaseAutoBlock(bmp);
	}
	releaseAutoList(bmps);
}
