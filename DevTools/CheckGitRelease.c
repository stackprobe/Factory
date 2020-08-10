/*
	CheckGitRelease [/D 直接チェックDIR | ルートDIR]
*/

#include "C:\Factory\Common\all.h"

#define LOCAL_GIT_RELEASE_BAT "GitRelease.bat"
#define LOCAL_RUN_ME_FIRST_BAT "__run_me_first.bat_"
#define LOCAL_LICENSE "LICENSE"
#define LOCAL_LICENSE_IGNORE "LIGNORE" // LICENSEファイルをまだ？設定しない場合、暫定的に？これを置いておく。

static uint ErrorCount;

static void FoundError(char *message)
{
	cout("★ %s\n", message);

	ErrorCount++;
}
static char *ProjectLocalDirToRepositoryName(char *localDir)
{
	if(lineExp("<1,azAZ><8,09>_<1,,>", localDir))
	{
		localDir += 10;
	}
	return localDir;
}
static void CheckGitRelease(char *dir)
{
	cout("チェック対象ディレクトリ ⇒ %s\n", dir);

	addCwd(dir);

	if(!existFile(LOCAL_GIT_RELEASE_BAT))
	{
		FoundError(LOCAL_GIT_RELEASE_BAT " が見つかりません。");
	}
	else
	{
		{
			char *text = readText_b(LOCAL_GIT_RELEASE_BAT);
			char *text2 = xcout(
				"IF NOT EXIST .\\GitRelease.bat GOTO END\r\n"
				"CALL qq\r\n"
				"C:\\Factory\\SubTools\\GitFactory.exe /ow . C:\\huge\\GitHub\\%s\r\n"
				"rem CALL C:\\var\\go.bat\r\n"
				":END\r\n"
				,ProjectLocalDirToRepositoryName(getLocal(dir))
				);

			if(strcmp(text, text2))
			{
				FoundError(LOCAL_GIT_RELEASE_BAT " の内容に問題があります。(bad contents)");
			}
			memFree(text);
			memFree(text2);
		}

		{
			char *repoDir = combine("C:\\huge\\GitHub", ProjectLocalDirToRepositoryName(getLocal(dir)));

			if(!existDir(repoDir))
			{
				FoundError(LOCAL_GIT_RELEASE_BAT " の内容に問題があります。(no repoDir)");
			}
			memFree(repoDir);
		}
	}

	if(!existFile(LOCAL_RUN_ME_FIRST_BAT))
	{
		FoundError(LOCAL_RUN_ME_FIRST_BAT " が見つかりません。");
	}
	else
	{
		char *text = readText_b(LOCAL_RUN_ME_FIRST_BAT);
		char *text2 = strx(
			"C:\\Factory\\dittybox\\devrunmefirst.bat\r\n"
			);

		if(strcmp(text, text2))
		{
			FoundError(LOCAL_RUN_ME_FIRST_BAT " の内容に問題があります。");
		}
		memFree(text);
		memFree(text2);
	}

	if(m_01(existFile(LOCAL_LICENSE)) + m_01(existFile(LOCAL_LICENSE_IGNORE)) != 1)
	{
		FoundError("ライセンスファイル (LICENSE) が見つかりません。");
	}
	if(existFile(LOCAL_LICENSE) && getFileSize(LOCAL_LICENSE) == 0)
	{
		FoundError("ライセンスファイル (LICENSE) が空っぽです。");
	}
	if(existFile(LOCAL_LICENSE_IGNORE) && getFileSize(LOCAL_LICENSE_IGNORE) != 0)
	{
		FoundError(LOCAL_LICENSE_IGNORE " が空ではありません。");
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
	char *gitReleaseBat = combine(dir, LOCAL_GIT_RELEASE_BAT);

	if(existFile(gitReleaseBat))
	{
		CheckGitRelease(dir);
	}
	else
	{
		FindProjectDir(dir);
	}
	memFree(gitReleaseBat);
}
int main(int argc, char **argv)
{
	errorCase_m(argIs("/C"), "廃止オプション"); // zantei

	if(argIs("/D"))
	{
		CheckGitRelease(nextArg());
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
