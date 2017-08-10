#include "GitSourceMask.h"

#define FLAG_FILE      "_gitsrcmsk"
#define RES_FILES_FILE "_gitsrcmsk_files"

static void MskSrcFile(char *file)
{
	autoList_t *lines = readLines(file);
	char *line;
	uint index;
	char *p;

	LOGPOS();

	foreach(lines, line, index)
		for(p = line; *p; p++)
			if(' ' < *p)
				*p = '/';

	writeLines_cx(file, lines);

	LOGPOS();
}
static void MaskSourceFile(char *file)
{
	char *ext = getExt(file);

	cout("* %s\n", file);

	     if(!_stricmp(ext, "c"    )) MskSrcFile(file);
	else if(!_stricmp(ext, "h"    )) MskSrcFile(file);
	else if(!_stricmp(ext, "cs"   )) MskSrcFile(file);
	else if(!_stricmp(ext, "cpp"  )) MskSrcFile(file);
	else if(!_stricmp(ext, "java" )) MskSrcFile(file);
}
static void MaskSourceByResFile(autoList_t *files)
{
	char *file;
	uint index;

	LOGPOS();

	foreach(files, file, index)
	{
		cout("* %s\n", file);

		if(!_stricmp(RES_FILES_FILE, getLocal(file)))
		{
			autoList_t *mskfiles = readResourceLines(file);
			char *mskfile;
			uint mskfile_index;

			LOGPOS();

			foreach(mskfiles, mskfile, mskfile_index)
			{
				cout("** %s\n", mskfile);

				errorCase(!isFairRelPath(mskfile, 0));
				mskfile = changeLocal(file, mskfile);
				errorCase(!existFile(mskfile));

				MskSrcFile(mskfile);

				memFree(mskfile);
			}
			releaseDim(mskfiles, 1);

			LOGPOS();
		}
	}
	LOGPOS();
}
void GitSourceMask(char *rootDir)
{
	autoList_t *files = lssFiles(rootDir);
	char *file;
	uint index;
	autoList_t *targets = newList();

	LOGPOS();

	RemoveGitPaths(files);

	foreach(files, file, index)
	{
		if(!_stricmp(FLAG_FILE, getLocal(file)))
		{
			char *prefix = addChar(changeLocal(file, ""), '\\');
			char *subfile;
			uint subfile_index;

			foreach(files, subfile, subfile_index)
				if(startsWithICase(subfile, prefix))
					addElement(targets, (uint)subfile);

			memFree(prefix);

			distinct2(targets, simpleComp, noop_u);
		}
	}
	foreach(targets, file, index)
	{
		MaskSourceFile(file);
	}

	MaskSourceByResFile(files);

	releaseDim(files, 1);
	releaseAutoList(targets);

	LOGPOS();
}
