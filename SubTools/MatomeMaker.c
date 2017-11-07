#include "C:\Factory\Common\all.h"

#define FILE_SIZE_LIMIT 16400 // 16384 == 1024 * 16

static char *RDir;

#define W_DIR "C:\\temp\\Matome"

static int IsCollectExt(char *ext)
{
	return

		// extra >

		!_stricmp(ext, "TPF") ||

		// < extra

		!_stricmp(ext, "C") ||
		!_stricmp(ext, "H") ||
		!_stricmp(ext, "CS") ||
		!_stricmp(ext, "CPP") ||
		!_stricmp(ext, "BAT") ||
		!_stricmp(ext, "TXT");
}
static int IsCorrect(char *entFile, char *relFile)
{
	return IsCollectExt(getExt(relFile)) && getFileSize(entFile) < FILE_SIZE_LIMIT;
}
static void TrmRum(char *rumDir)
{
	char *entsDir = combine(rumDir, "files");
	char *revsDir = combine(rumDir, "revisions");
	autoList_t *revs;
	char *rev;
	uint rev_index;

	errorCase(!existDir(rumDir));
	errorCase(!existDir(entsDir));
	errorCase(!existDir(revsDir));

	revs = lsDirs(revsDir);

	foreach(revs, rev, rev_index)
	{
		char *filesFile = combine(rev, "files.txt");
		autoList_t *lines;
		char *line;
		uint line_index;

		errorCase(!existFile(filesFile));

		lines = readLines(filesFile);

		foreach(lines, line, line_index)
		{
			char *entFile;
			char *relFile;

			errorCase(strlen(line) < 34); // Å’Z -> "0123456789abcdef0123456789abcdef x"
			errorCase(line[32] != ' ');

			line[32] = '\0';

			entFile = combine(entsDir, line);
			relFile = strx(line + 33);

			line[32] = ' ';

			errorCase_m(!existFile(entFile), "need rum /t");

			if(!IsCorrect(entFile, relFile))
				line[0] = '\0';

			memFree(entFile);
			memFree(relFile);
		}
		trimLines(lines);

		writeLines(filesFile, lines);

		memFree(filesFile);
		releaseDim(lines, 1);
	}
	memFree(entsDir);
	memFree(revsDir);
	releaseDim(revs, 1);

	coExecute_x(xcout("C:\\Factory\\Tools\\rum.exe /TT \"%s\"", rumDir));
}
static void CpRums(void)
{
	autoList_t *dirs = lssDirs(RDir);
	char *dir;
	uint index;

	LOGPOS();

	foreach(dirs, dir, index)
	{
		if(!_stricmp("rum", getExt(dir)))
		{
			char *destDir = changeRoot(strx(dir), RDir, W_DIR);

			cout("< %s\n", dir);
			cout("> %s\n", destDir);

			LOGPOS();
			createPath(destDir, 'D');
			LOGPOS();
			copyDir(dir, destDir);
			LOGPOS();
			TrmRum(destDir);
			LOGPOS();

			memFree(destDir);
		}
	}
	releaseDim(dirs, 1);

	LOGPOS();
}
int main(int argc, char **argv)
{
	RDir = nextArg();
	RDir = makeFullPath(RDir);

	errorCase(!existDir(RDir));

	removeDirIfExist(W_DIR);
	createDir(W_DIR);

	CpRums();

	coExecute("START " W_DIR);
}
