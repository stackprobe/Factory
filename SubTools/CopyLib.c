/*
	CopyLib.exe ����DIR �o��DIR

		����DIR, �o��DIR ... ���݂���f�B���N�g���ł��邱�ƁB
*/

#include "C:\Factory\Common\all.h"

static autoList_t *ResAutoComment;

static void AutoComment_Range(autoList_t *range, uint range_index)
{
	char *line;
	uint index;
	int commentEntered = 0;

	foreach(range, line, index)
	{
		int insCmt = 0;
		int forceInsCmt = 0;

		line = strx(line);
		nn_strstr(line, "//")[0] = '\0'; // �C�����C���R�����g�̏���
		ucTrimTrail(line);

		{
			char *tmp = strx(line);

			ucTrim(tmp);

			if(commentEntered)
			{
				if(!strcmp(tmp, "*/"))
					commentEntered = 0;
			}
			else
			{
				if(!strcmp(tmp, "/*"))
				{
					commentEntered = 1;
					forceInsCmt = 1;
				}
			}
			memFree(tmp);
		}

		if(m_isalpha(line[0]) || line[0] == '_')
			insCmt = 1;

		if(endsWith(line, ":"))
			insCmt = 0;

		if(index && !strcmp(getLine(range, index - 1), "}") && endsWith(line, ";"))
			insCmt = 0;

		if(index && !strcmp(getLine(range, index - 1), "*/"))
			insCmt = 0;

		if(index && startsWith(getLine(range, index - 1), "template <"))
			insCmt = 0;

		if(commentEntered)
			insCmt = 0;

		if(!index && !range_index)
			insCmt = 1;

		if(insCmt || forceInsCmt)
		{
			char *comment;
			uint comment_index;

			foreach(ResAutoComment, comment, comment_index)
			{
				insertElement(range, index++, (uint)strx(comment));
			}
		}
		memFree(line);
	}
	errorCase(commentEntered);
}
static void AutoComment(autoList_t *ranges)
{
	autoList_t *range;
	uint index;

	foreach(ranges, range, index)
		if(index % 2 == 0)
			AutoComment_Range(range, index);
}
static autoList_t *ReadCommonAndAppSpecRanges(char *file)
{
	autoList_t *ranges = newList();
	autoList_t *lines = newList();
	FILE *fp = fileOpen(file, "rt");
	char *line;
	int appSpecEntered = 0;

	while(line = readLine(fp))
	{
		int enter;
		int leave;

		{
			char *tmp = strx(line);

			ucTrim(tmp);

			enter = startsWith(tmp, "// app >");
			leave = startsWith(tmp, "// < app");

			memFree(tmp);
		}

		errorCase(enter && leave);

		if(leave)
		{
			errorCase(!appSpecEntered);
			appSpecEntered = 0;

			addElement(ranges, (uint)lines);
			lines = newList();
		}
		addElement(lines, (uint)line);

		if(enter)
		{
			errorCase(appSpecEntered);
			appSpecEntered = 1;

			addElement(ranges, (uint)lines);
			lines = newList();
		}
	}
	errorCase(appSpecEntered);

	addElement(ranges, (uint)lines);

	fileClose(fp);
	return ranges;
}
static void CheckAppSpecRangesPair(autoList_t *rRanges, autoList_t *wRanges)
{
	uint index;

	errorCase_m(getCount(rRanges) != getCount(wRanges), "�A�v���ŗL�R�[�h�̐�������Ȃ����ߏ㏑���o���܂���B");

	errorCase(getCount(rRanges) % 2 != 1); // 2bs
	errorCase(getCount(wRanges) % 2 != 1); // 2bs

	for(index = 2; index < getCount(rRanges); index += 2)
	{
		{
			char *r = (char *)getLastElement(getList(rRanges, index - 2));
			char *w = (char *)getLastElement(getList(wRanges, index - 2));

coutJLine_x(xcout("r: %s", r)); // test
coutJLine_x(xcout("w: %s", w)); // test

			errorCase_m(strcmp(r, w), "�A�v���ŗL�R�[�h�J�n�s�s��v");
		}

		{
			char *r = getLine(getList(rRanges, index), 0);
			char *w = getLine(getList(wRanges, index), 0);

coutJLine_x(xcout("r: %s", r)); // test
coutJLine_x(xcout("w: %s", w)); // test

			errorCase_m(strcmp(r, w), "�A�v���ŗL�R�[�h�I���s�s��v");
		}
	}
}
static void DoCopyLib(char *rDir, char *wDir, int testMode)
{
	autoList_t *rSubDirs = lssDirs(rDir);
	autoList_t *wSubDirs = lssDirs(wDir);
	autoList_t *rFiles = lssFiles(rDir);
	autoList_t *wFiles = lssFiles(wDir);
	autoList_t *owFiles;
	char *dir;
	char *file;
	uint index;

	changeRoots(rSubDirs, rDir, NULL);
	changeRoots(wSubDirs, wDir, NULL);
	changeRoots(rFiles, rDir, NULL);
	changeRoots(wFiles, wDir, NULL);

	releaseDim(mergeLinesICase(rSubDirs, wSubDirs), 1);
	reverseElements(wSubDirs); // ������ �폜�͌�납��s�����߁A�����ŋt�]���Ă����B������

	owFiles = mergeLinesICase(rFiles, wFiles);

	foreach(rSubDirs, dir, index)
		if(!testMode)
			createPath_x(combine(rDir, dir), 'X');

	foreach(rFiles, file, index)
	{
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);

		cout("R %s\n", rFile);
		cout("> %s\n", wFile);

		if(!testMode)
			copyFile(rFile, wFile);

		memFree(rFile);
		memFree(wFile);
	}
	foreach(wFiles, file, index)
	{
		char *wFile = combine(wDir, file);

		cout("D %s\n", wFile);

		{
			autoList_t *ranges = ReadCommonAndAppSpecRanges(wFile);

			cout("Dr %u\n", getCount(ranges));

			errorCase_m(1 < getCount(ranges), "�A�v���ŗL�R�[�h���܂ނ��ߍ폜�o���܂���B�蓮�ō폜���ĂˁB");

			releaseDim(ranges, 2);
		}

		if(!testMode)
			removeFile(wFile);

		memFree(wFile);
	}
	foreach(owFiles, file, index)
	{
		char *rFile = combine(rDir, file);
		char *wFile = combine(wDir, file);
		autoList_t *rRanges;
		autoList_t *wRanges;

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);

		rRanges = ReadCommonAndAppSpecRanges(rFile);
		wRanges = ReadCommonAndAppSpecRanges(wFile);

		cout("<r %u\n", getCount(rRanges));
		cout(">r %u\n", getCount(wRanges));

		CheckAppSpecRangesPair(rRanges, wRanges);

		AutoComment(rRanges);

		{
			autoList_t *lines = newList();
			uint index;

			for(index = 0; index < getCount(rRanges); index++)
				addElements(lines, getList(index % 2 ? wRanges : rRanges, index));

			if(!testMode)
			{
				semiRemovePath(wFile);
				writeLines(wFile, lines);
			}
			releaseAutoList(lines);
		}

		memFree(rFile);
		memFree(wFile);
		releaseDim(rRanges, 2);
		releaseDim(wRanges, 2);
	}
	foreach(wSubDirs, dir, index)
		if(!testMode)
			removeDir(combine(wDir, dir));

	releaseDim(rSubDirs, 1);
	releaseDim(wSubDirs, 1);
	releaseDim(rFiles, 1);
	releaseDim(wFiles, 1);
	releaseDim(owFiles, 1);
}
static void CopyLib(char *rDir, char *wDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));
	errorCase(!_stricmp(rDir, wDir)); // ? �����f�B���N�g��

	LOGPOS();
	DoCopyLib(rDir, wDir, 1);
	LOGPOS();
	DoCopyLib(rDir, wDir, 0);
	LOGPOS();

	memFree(rDir);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	{
		char *cmtFile = changeExt(getSelfFile(), "txt");

		ResAutoComment = readLines(cmtFile);

		memFree(cmtFile);
	}

	CopyLib(getArg(0), getArg(1));
}
