/*
	CheckGitRelease [/C ���ڃ`�F�b�NDIR | ���[�gDIR]
*/

#include "C:\Factory\Common\all.h"

#define LOCAL_GIT_RELEASE_BAT "GitRelease.bat"
#define LOCAL_RUN_ME_FIRST_BAT "__run_me_first.bat_"

static uint ErrorCount;

static void FoundError(char *message)
{
	cout("�� %s\n", message);

	ErrorCount++;
}
static void CheckGitRelease(char *dir)
{
	cout("�`�F�b�N�Ώۃf�B���N�g�� �� %s\n", dir);

	addCwd(dir);

	if(!existFile(LOCAL_GIT_RELEASE_BAT))
	{
		FoundError(LOCAL_GIT_RELEASE_BAT " ��������܂���B");
	}
	else
	{
		char *text = readText_b(LOCAL_GIT_RELEASE_BAT);
		char *text2 = xcout(
			"IF NOT EXIST .\\GitRelease.bat GOTO END\r\n"
			"CALL qq\r\n"
			"C:\\Factory\\SubTools\\GitFactory.exe /ow . C:\\huge\\GitHub\\%s\r\n"
			"rem CALL C:\\var\\go.bat\r\n"
			":END\r\n"
			,getLocal(dir)
			);

		if(strcmp(text, text2))
		{
			FoundError(LOCAL_GIT_RELEASE_BAT " �̓��e�ɖ�肪����܂��B");
		}
		memFree(text);
		memFree(text2);
	}

	if(!existFile(LOCAL_RUN_ME_FIRST_BAT))
	{
		FoundError(LOCAL_RUN_ME_FIRST_BAT " ��������܂���B");
	}
	else
	{
		char *text = readText_b(LOCAL_RUN_ME_FIRST_BAT);
		char *text2 = strx(
			"C:\\Factory\\dittybox\\devrunmefirst.bat\r\n"
			);

		if(strcmp(text, text2))
		{
			FoundError(LOCAL_RUN_ME_FIRST_BAT " �̓��e�ɖ�肪����܂��B");
		}
		memFree(text);
		memFree(text2);
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
	if(argIs("/C"))
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
		cout("������ [ %u ] ���̃G���[��������܂����B\n", ErrorCount);
	}
	else
	{
		cout("�G���[�͌�����܂���ł����B\n");
	}
}