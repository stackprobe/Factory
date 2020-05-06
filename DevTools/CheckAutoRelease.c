/*
	CheckAutoRelease [/C 直接チェックDIR | ルートDIR]
*/

#include "C:\Factory\Common\all.h"

#define AUTO_RELEASE_BAT_TEMPLATE_FILE "C:\\Factory\\Resource\\AutoRelease.bat_template.txt"

static autoList_t *AutoReleaseBatTemplateLines;

#define LOCAL_AUTO_RELEASE_BAT "AutoRelease.bat"
#define LOCAL_LEGACY_RELEASE_BAT "Release.bat"
#define LOCAL_LEGACY_CLEAN_BAT "Clean.bat"
#define LOCAL_RELEASE_BAT "_Release.bat"
#define LOCAL_CLEAN_BAT "_Clean.bat"

static uint ErrorCount;

static void FoundError(char *message)
{
	cout("★ %s\n", message);

	ErrorCount++;
}
static void RemoveIndentedLines(autoList_t *lines)
{
	uint index;

	for(index = getCount(lines); --index; )
	{
		if(getLine(lines, index)[0] == '\t')
		{
			do
			{
				memFree((char *)desertElement(lines, index));
			}
			while(index < getCount(lines) && getLine(lines, index)[0] == '\0');
		}
	}
}
static void CheckAutoRelease(char *dir)
{
	cout("チェック対象ディレクトリ ⇒ %s\n", dir);

	addCwd(dir);

	if(!existFile(LOCAL_AUTO_RELEASE_BAT))
	{
		FoundError(LOCAL_AUTO_RELEASE_BAT " が見つかりません。");
	}
	else
	{
		{
			autoList_t *lines = readLines(LOCAL_AUTO_RELEASE_BAT);

			RemoveIndentedLines(lines);

			if(!isSameLines(lines, AutoReleaseBatTemplateLines, 0))
			{
				FoundError(LOCAL_AUTO_RELEASE_BAT " の内容に問題があります。");
			}
			releaseDim(lines, 1);
		}

		if(existFile(LOCAL_LEGACY_RELEASE_BAT))
		{
			FoundError(LOCAL_LEGACY_RELEASE_BAT " が存在します。");
		}
		if(existFile(LOCAL_LEGACY_CLEAN_BAT))
		{
			FoundError(LOCAL_LEGACY_CLEAN_BAT " が存在します。");
		}
		if(!existFile(LOCAL_RELEASE_BAT))
		{
			FoundError(LOCAL_RELEASE_BAT " が見つかりません。");
		}
		if(!existFile(LOCAL_CLEAN_BAT))
		{
			FoundError(LOCAL_CLEAN_BAT " が見つかりません。");
		}
	}

	{
		char *rumDir = addExt(getCwd(), "rum");

		if(!existDir(rumDir))
		{
			FoundError(".rum が見つかりません。");
		}
		else
		{
			char *revRootDir = combine(rumDir, "revisions");
			autoList_t *revDirs;
			char *lastRevDir;
			char *lastCommentFile;
			char *lastComment;

			revDirs = lsDirs(revRootDir);
			sortJLinesICase(revDirs);
			lastRevDir = (char *)getLastElement(revDirs);
			lastCommentFile = combine(lastRevDir, "comment.txt");
			lastComment = readFirstLine(lastCommentFile);

			if(strcmp(lastComment, "rel"))
			{
				FoundError("最終コメントが rel ではありません。");
			}

			memFree(revRootDir);
			releaseDim(revDirs, 1);
//			lastRevDir
			memFree(lastCommentFile);
			memFree(lastComment);
		}
	}

	unaddCwd();
}

static void CheckDir(char *dir);

static void FindProjectDir(char *rootDir)
{
	autoList_t *dirs = lsDirs(rootDir);
	char *dir;
	uint index;

	foreach(dirs, dir, index)
	{
		CheckDir(dir);
	}
	releaseDim(dirs, 1);
}
static void CheckDir(char *dir)
{
	char *autoReleaseBat = combine(dir, LOCAL_AUTO_RELEASE_BAT);

	if(existFile(autoReleaseBat))
	{
		CheckAutoRelease(dir);
	}
	else
	{
		FindProjectDir(dir);
	}
	memFree(autoReleaseBat);
}
int main(int argc, char **argv)
{
	AutoReleaseBatTemplateLines = readLines(AUTO_RELEASE_BAT_TEMPLATE_FILE);

	if(argIs("/C"))
	{
		CheckAutoRelease(nextArg());
	}
	else if(hasArgs(1))
	{
		CheckDir(nextArg());
	}
	else
	{
		CheckDir("C:\\Dev");
	}

	cout("\n");

	if(ErrorCount)
	{
		cout("★★★ [ %u ] 件のエラーが見つかりました。\n", ErrorCount);
	}
	else
	{
		cout("エラーは見つかりませんでした。\n");
	}
}
