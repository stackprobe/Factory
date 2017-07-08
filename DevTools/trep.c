/*
	Search.exe で見つかった sections について、任意の文字列に置き換えを行います。

	- - -

	trep.exe [/-C] [/F] [/-] dest-ptn
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\rbTree.h"

static int NoCheckMode;
static int ForceMode;
static char *DestPtn;

static sint CompSection(uint v1, uint v2)
{
	uint64 *a = (uint64 *)v1;
	uint64 *b = (uint64 *)v2;

	errorCase_m(a[0] == b[0], "同じ始点のセクションが複数あります。");

	return m_simpleComp(a[0], b[0]);
}
static void DoTrepStream(FILE *rfp, FILE *wfp, autoList_t *sections)
{
	uint64 *section;
	uint index;
	uint64 rPos = 0;
	uint64 fileSize = getFileSizeFPSS(rfp);

	rapidSort(sections, CompSection);

	foreach(sections, section ,index)
	{
		errorCase(section[1] < section[0]);
		errorCase(fileSize < section[1]);

		if(rPos <= section[0])
		{
			readWriteBinary(rfp, wfp, section[0] - rPos);
			writeToken(wfp, DestPtn);
			fileSeek(rfp, SEEK_SET, section[1]);
			rPos = section[1];
		}
	}
	readWriteBinaryToEnd(rfp, wfp);
}
static void DoTrepFile(char *file, autoList_t *sections)
{
	char *midFile = makeTempPath(NULL);
	FILE *rfp;
	FILE *wfp;

	cout("> %s\n", file);
	cout("+ %s\n", midFile);

	rfp = fileOpen(file, "rb");
	wfp = fileOpen(midFile, "wb");

	DoTrepStream(rfp, wfp, sections);

	fileClose(rfp);
	fileClose(wfp);

#if 1
	semiRemovePath(file);
#else
	removeFile(file);
#endif
	moveFile(midFile, file);
	memFree(midFile);
}
static void ReleaseF2SectTreeValue(uint value)
{
	releaseDim((void *)value, 1);
}
static void DoTrep(void)
{
	autoList_t *lines = readLines(SECTIONLISTFILE);
	char *line;
	uint index;
	rbTree_t *f2SectTree = rbCreateTree((uint (*)(uint))strx, (sint (*)(uint, uint))mbs_stricmp, (void (*)(uint))memFree);
	uint count;

	foreach(lines, line, index)
	{
		autoList_t *tokens = tokenize(line, '\t');
		char *file;
		uint64 bgnPos;
		uint64 endPos;
		uint c = 0;
		autoList_t *sections;
		uint64 *section;

		file = getLine(tokens, c++);
		bgnPos = toValue64(getLine(tokens, c++));
		endPos = toValue64(getLine(tokens, c++));

		if(!rbtHasKey(f2SectTree, (uint)file))
			rbtAddValue(f2SectTree, (uint)file, (uint)newList());

		sections = (autoList_t *)rbtGetLastAccessValue(f2SectTree);
		section = memAlloc(2 * sizeof(uint64));
		section[0] = bgnPos;
		section[1] = endPos;
		addElement(sections, (uint)section);
	}
	count = rbtGetCount(f2SectTree);
	errorCase(!count);

	rbtJumpToLastAccess(f2SectTree);

	for(; count; count--)
	{
		char *file = (char *)rbtGetCurrKey(f2SectTree);
		autoList_t *sections = (autoList_t *)rbtGetCurrValue(f2SectTree);

		DoTrepFile(file, sections);

		rbtJumpForLeft(f2SectTree);
	}
	rbtCallAllValue(f2SectTree, ReleaseF2SectTreeValue);
	rbReleaseTree(f2SectTree);
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/-C"))
	{
		NoCheckMode = 1;
		goto readArgs;
	}
	if(argIs("/F"))
	{
		ForceMode = 1;
		goto readArgs;
	}
	argIs("/-");

	DestPtn = nextArg();

	if(!NoCheckMode)
	{
		autoList_t *lines = readLines(SECTIONLISTFILE);
		char *line;
		uint index;

		errorCase_m(!getCount(lines), "対象ファイルが１つもありません。");

		foreach(lines, line, index)
		{
			autoList_t *tokens = tokenize(line, '\t');
			char *file;
			uint64 bgnPos; // ここから、この位置を含む
			uint64 endPos; // ここまで、この位置を含まない
			uint c = 0;

			errorCase(getCount(tokens) != 3);

			file = getLine(tokens, c++);
			bgnPos = toValue64(getLine(tokens, c++));
			endPos = toValue64(getLine(tokens, c++));

			cout("%s\n", file);
			cout("%I64u > %I64u\n", bgnPos, endPos);

			errorCase_m(!existFile(file), "対象ファイルがありません。");
			errorCase_m(endPos < bgnPos, "始点より終点が前になっています。");

			{
				FILE *fp = fileOpen(file, "rb");
				uint64 pos;

				errorCase_m(getFileSize(file) < endPos, "終点がファイルサイズより後になっています。");

				fileSeek(fp, SEEK_SET, bgnPos);

				while(getSeekPos(fp) < endPos)
				{
					int chr = readChar(fp);
					m_toHalf(chr);
					cout("%c", chr);
				}
				fileClose(fp);
				cout("\n");
			}
		}
	}
	if(!ForceMode)
	{
		cout("続行？\n");

		if(getKey() == 0x1b)
			termination(0);

		cout("続行します。\n");
	}
	DoTrep();
}
