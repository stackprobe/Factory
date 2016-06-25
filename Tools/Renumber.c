#include "C:\Factory\Common\all.h"

static int IsNumbered(char *file)
{
	char *p;

	for(p = file; *p; p++)
	{
		if(*p == '_')
			return 1;

		if(!m_isdecimal(*p))
			break;
	}
	return 0;
}
static void DoFRenum(void)
{
	autoList_t *files = lsFiles(".");
	autoList_t *midFiles = newList();
	autoList_t *destFiles = newList();
	char *file;
	uint index;
	uint no = 10;

	eraseParents(files);
	sortJLinesICase(files);

	foreach(files, file, index)
	{
		char *name;
		char *lfile;

		if(IsNumbered(file))
			name = mbs_strchr(file, '_') + 1;
		else
			name = file;

		lfile = xcout("%04u_%s", no, name);

		cout("< %s\n", file);
		cout("> %s\n", lfile);

		addElement(midFiles, (uint)xcout("_$$$_%s", lfile));
		addElement(destFiles, (uint)lfile);

		no += 10;
	}
	cout("Press R to renumber\n");

	if(getKey() != 'R')
		termination(0);

	for(index = 0; index < getCount(files); index++)
		moveFile(
			getLine(files, index),
			getLine(midFiles, index)
			);

	for(index = 0; index < getCount(files); index++)
		moveFile(
			getLine(midFiles, index),
			getLine(destFiles, index)
			);

	cout("Renumbered!\n");

	releaseDim(files, 1);
	releaseDim(midFiles, 1);
	releaseDim(destFiles, 1);
}
int main(int argc, char **argv)
{
	addCwd(hasArgs(1) ? nextArg() : c_dropDir());
	DoFRenum();
	unaddCwd();
}
