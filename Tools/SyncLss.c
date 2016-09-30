#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\md5.h"

static void SyncLss(void)
{
	autoList_t *files = readLines(FOUNDLISTFILE);
	char *file;
	uint index;
	autoList_t *lines = newList();
	char *selLine;
	char *selFile;

	foreach(files, file, index)
		addElement(lines, (uint)xcout("%s %s", md5_makeHexHashFile(file), file));

	selLine = selectLine(lines);

	if(!selLine)
		termination(0);

	selFile = strx(selLine + 33);

	cout("< %s\n", selFile);
	cout("> *\n");

	{
		const char *CFM_PTN = "SYNC";
		char *cfmInput;

		cout("### 確認のため %s と入力してね。### (ignore case)\n", CFM_PTN);
		cfmInput = coInputLine();

		if(_stricmp(cfmInput, CFM_PTN)) // ? 一致しない。
		{
			memFree(cfmInput);
			termination(0);
		}
		memFree(cfmInput);
	}

	foreach(files, file, index)
	{
		if(!_stricmp(file, selFile))
			continue;

		cout("< %s\n", selFile);
		cout("> %s\n", file);

		semiRemovePath(file);

		copyFile(selFile, file);
	}
	releaseDim(files, 1);
	releaseDim(lines, 1);
	memFree(selLine);
	memFree(selFile);
}
int main(int argc, char **argv)
{
	SyncLss();
}
