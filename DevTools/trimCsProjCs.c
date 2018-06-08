/*
	trimCsProjCs.exe [/D | �\�����[�V����������f�B���N�g��]
*/

#include "C:\Factory\Common\all.h"

#define Clean() \
	(coExecute("solclean ."))
//	(coExecute("qq -f")) // 1.txt �Ƀ��_�C���N�g�����Ƃ��}�Y���B

#define Build() \
	coExecute("cx *")

static char *SlnFile;
static char *ProjFile;
static char *ProjName;
static char *ProjDir;
static char *OutFile;
static char *ProjBackupFile;

static void ProcProj(int checkOnly)
{
	cout("checkOnly: %d\n", checkOnly);
	cout("ProjFile: %s\n", ProjFile);

	ProjName = changeExt(getLocal(ProjFile), "");
	ProjDir = getParent(ProjFile);

	cout("ProjName: %s\n", ProjName);
	cout("ProjDir: %s\n", ProjDir);

	Clean();
	Build();

	{
		autoList_t *files = lssFiles(ProjDir);
		char *file;
		uint index;

		foreach(files, file, index)
		{
			if(
				!mbs_stristr(file, "\\bin\\Release\\") ||
				!mbs_stristr(getLocal(file), ProjName) ||
				_stricmp(getExt(file), "exe") && _stricmp(getExt(file), "dll")
				)
				file[0] = '\0';
		}
		trimLines(files);

		errorCase_m(getCount(files) != 1, "�o�̓t�@�C�����P�ɍi��܂���B");

		OutFile = getLine(files, 0);
		releaseAutoList(files);
	}

	cout("OutFile: %s\n", OutFile);

	Clean();

	errorCase_m(existFile(OutFile), "�N���[���A�b�v���Ă��o�̓t�@�C�����폜����܂���B");

	ProjBackupFile = addExt(strx(ProjFile), "bak");

	cout("ProjBackupFile: %s\n", ProjBackupFile);

	errorCase_m(existPath(ProjBackupFile), "�o�b�N�A�b�v�t�@�C�����c���Ă��܂��B");

	if(checkOnly)
		goto checkOnlyEnd;

	LOGPOS();



	LOGPOS();

checkOnlyEnd:
	memFree(ProjName);
	memFree(ProjDir);
	memFree(OutFile);
	memFree(ProjBackupFile);
	ProjName = NULL;
	ProjDir = NULL;
	OutFile = NULL;
	ProjBackupFile = NULL;

	LOGPOS();
}
static void TrimCsProjCs(void)
{
	// set SlnFile
	{
		autoList_t *files = lsFiles(".");
		char *file;
		uint index;

		foreach(files, file, index)
			if(_stricmp(getExt(file), "sln"))
				file[0] = '\0';

		trimLines(files);

		errorCase_m(getCount(files) != 1, "�\�����[�V�������P�ɍi��܂���B");

		SlnFile = getLine(files, 0);
		releaseAutoList(files);
	}

	cout("SlnFile: %s\n", SlnFile);

	// set ProjFile + call (CheckProj, ProcProj)
	{
		autoList_t *files = lssFiles(".");
		char *file;
		uint index;

		foreach(files, file, index)
			if(_stricmp(getExt(file), "csproj"))
				file[0] = '\0';

		trimLines(files);

		errorCase_m(!getCount(files), "�v���W�F�N�g���P���L��܂���B");

		foreach(files, file, index)
		{
			ProjFile = file;
			ProcProj(1);
			ProjFile = NULL;
		}
		foreach(files, file, index)
		{
			ProjFile = file;
			ProcProj(0);
			ProjFile = NULL;
		}
		releaseDim(files, 1);
	}

	memFree(SlnFile);
	SlnFile = NULL;
}
int main(int argc, char **argv)
{
	if(argIs("/D"))
	{
		char *slnDir = c_dropDir();

		addCwd(slnDir);
		{
			TrimCsProjCs();
		}
		unaddCwd();
	}
	else if(hasArgs(1))
	{
		char *slnDir = nextArg();

		addCwd(slnDir);
		{
			TrimCsProjCs();
		}
		unaddCwd();
	}
	else
	{
		TrimCsProjCs();
	}
}
