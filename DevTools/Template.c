#include "C:\Factory\Common\all.h"

#define TEMPLATE_DIR "C:\\Factory\\DevTools\\TemplateFiles"
#define ESCAPE_PTN "\1"

static char *GetAllUpper(char *userParam)
{
	char *ret = strx("");
	char *p;

	for(p = userParam; *p; p++)
		if(m_isupper(*p))
			ret = addChar(ret, *p);

	return ret;
}
static void MakeTemplate(char *templateName, char *userParam)
{
	char *templateFile = xcout(TEMPLATE_DIR "\\%s.txt", templateName);
	autoList_t *lines;
	char *line;
	uint index;
	char *initial = xcout("%c", userParam[0]);
	char *allUpper = GetAllUpper(userParam);

	errorCase(!existFile(templateFile));

	lines = readLines(templateFile);

	foreach(lines, line, index)
	{
		line = replaceLine(line, "$$", ESCAPE_PTN, 0);
		line = replaceLine(line, "$I", initial, 1);
		line = replaceLine(line, "$U", allUpper, 1);
		line = replaceLine(line, "$P", userParam, 1);
		line = replaceLine(line, ESCAPE_PTN, "$", 0);

		setElement(lines, index, (uint)line);
	}
	viewLines(lines);
	releaseDim(lines, 1);
	memFree(templateFile);
	memFree(initial);
}
int main(int argc, char **argv)
{
	if(argIs("/L"))
	{
		autoList_t *files = lsFiles(TEMPLATE_DIR);
		char *file;
		uint index;

		eraseParents(files);

		foreach(files, file, index)
			toUpperLine(file);

		rapidSortLines(files);

		foreach(files, file, index)
		{
			if(!_stricmp("txt", getExt(file)))
			{
				char *templateName = changeExt(file, "");

				cout("%s\n", templateName);
				memFree(templateName);
			}
		}
		releaseDim(files, 1);
		return;
	}
	MakeTemplate(getArg(0), getArg(1));
}
