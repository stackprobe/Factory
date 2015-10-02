#include "C:\Factory\Common\all.h"

static int NoShutdownFlag;

static void ToDecAlphaOnly(char *line)
{
	char *p;

	for(p = line; *p; p++)
	{
		if(!m_isdecimal(*p) && !m_isalpha(*p))
		{
			*p = '_';
		}
	}
}
static void ToOnomast(char *line)
{
	char *p;

	line[0] = m_toupper(line[0]);

	for(p = line + 1; *p; p++)
	{
		*p = m_tolower(*p);
	}
}

static autoList_t *GetTargetDirs(void)
{
	autoList_t *paths = ls("C:\\");
	autoList_t dirs;
	char *dir;
	uint index;
	autoList_t *retDirs = createAutoList(16);
	char *userName;

	dirs = gndSubElements(paths, 0, lastDirCount);

	foreach(&dirs, dir, index)
	{
		if(
			!_stricmp(dir, "C:\\Documents and Settings") ||
			!_stricmp(dir, "C:\\ProgramData") || // for 7
			!_stricmp(dir, "C:\\Program Files") ||
			!_stricmp(dir, "C:\\Program Files (x86)") || // for 7 x64
			!_stricmp(dir, "C:\\RECYCLER") ||
			!_stricmp(dir, "C:\\System Volume Information") ||
			!_stricmp(dir, "C:\\WINDOWS") || // for XP, 7
			!_stricmp(dir, "C:\\WINNT") || // for 2000
			!_stricmp(dir, "C:\\$Recycle.Bin") || // for 7
			!_stricmp(dir, "C:\\Config.Msi") || // for 7
			!_stricmp(dir, "C:\\Recovery") || // for 7
			!_stricmp(dir, "C:\\Users") || // for 7

			dir[3] == '_' || m_isdecimal(dir[3]) || // _ 0～9 で始まるフォルダは対象外

			!_stricmp(dir, "C:\\huge") || // 運用上

			0
			)
		{
		}
		else
		{
			addElement(retDirs, (uint)strx(dir));
		}
	}
	releaseDim(paths, 1);
	rapidSortLines(retDirs);

	userName = getenv("UserName");

	errorCase(!userName);
	errorCase(!userName[0]);

	if(existDir("C:\\Users")) // ? 7
	{
		addElement(retDirs, (uint)xcout("C:\\Users\\%s\\Favorites", userName));
//		addElement(retDirs, (uint)xcout("C:\\Users\\%s\\Documents", userName)); // 権限が無いとか言われる。
		addElement(retDirs, (uint)xcout("C:\\Users\\%s\\Desktop", userName));
	}
	else // ? xp
	{
		addElement(retDirs, (uint)xcout("C:\\Documents and Settings\\%s\\Favorites", userName));
		addElement(retDirs, (uint)xcout("C:\\Documents and Settings\\%s\\My Documents", userName));
		addElement(retDirs, (uint)xcout("C:\\Documents and Settings\\%s\\デスクトップ", userName));
	}
	return retDirs;
}
static void CheckTargetDirs(autoList_t *dirs)
{
	autoList_t *localDirs = createAutoList(getCount(dirs));
	char *dir;
	uint index;

	foreach(dirs, dir, index)
	{
		cout("%s\n", dir);
		errorCase(!existDir(dir));
		addElement(localDirs, (uint)getLocal(dir));
	}
	cout("\n");

	foreach(localDirs, dir, index)
	{
		autoList_t followDirs = gndFollowElements(localDirs, index + 1);
		errorCase(findJLineICase(&followDirs, dir) < getCount(&followDirs)); // ? 重複した。
	}
	releaseAutoList(localDirs);
}

#define BATCH_BACKUP "C:\\Factory\\tmp\\Backup.bat"

static void BackupDirs(autoList_t *targetDirs)
{
	char *targetDir;
	uint index;

	foreach(targetDirs, targetDir, index)
	{
		char *destDir = getLocal(targetDir);
		FILE *fp;
		char *line;

		execute_x(xcout("TITLE Backup - %u / %u (%u) -S=%d", index, getCount(targetDirs), getCount(targetDirs) - index, NoShutdownFlag));

		fp = fileOpen(BATCH_BACKUP, "wt");

		writeLine(fp, "SET COPYCMD=");
		writeLine(fp, line = xcout("MD \"%s\"", destDir)); memFree(line);
		writeLine(fp, line = xcout("XCOPY.EXE \"%s\" \"%s\" /E /H /K", targetDir, destDir)); memFree(line);

		fileClose(fp);

		cout("コピーしています...\n");

		execute(line = xcout("START /B /WAIT CMD /C %s", BATCH_BACKUP)); memFree(line);

		cout("コピーしました。\n");
		cout("\n");
	}
	execute("TITLE Backup - done");
}

int main(int argc, char **argv)
{
	char *strDestDrv;
	int destDrv;
	char *pcname;
	char *destDir;
	char *destBackDir;
	autoList_t *targetDirs;
	char *cmdln;
	int doShutdownFlag;

	NoShutdownFlag = argIs("/-S");

	if(!NoShutdownFlag)
	{
		cout("********************************\n");
		cout("** 終了後シャットダウンします **\n");
		cout("********************************\n");
	}
	cout("+---------------------------------------------+\n");
	cout("| メーラーなど、動作中のアプリを閉じて下さい。|\n");
	cout("+---------------------------------------------+\n");
	strDestDrv = dropPath();

	if(!strDestDrv)
		termination(0);

	destDrv = strDestDrv[0];

	errorCase(!m_isalpha(destDrv));
	errorCase(destDrv == 'C');

	pcname = getenv("ComputerName");

	errorCase(!pcname);
	errorCase(!pcname[0]);

	pcname = strx(pcname);
	ToDecAlphaOnly(pcname);
	ToOnomast(pcname);
	destDir = xcout("%c:\\%s", destDrv, pcname);
	destBackDir = xcout("%s_", destDir);
	memFree(pcname);

	cout("出力先: %s (%s)\n", destDir, destBackDir);
	cout("\n");

	targetDirs = GetTargetDirs();
	CheckTargetDirs(targetDirs);

	if(existDir(destBackDir))
	{
		cout("バックアップを削除しています...\n");

		cmdln = xcout("RD /S /Q %s", destBackDir);
		execute(cmdln);
		memFree(cmdln);

		cout("削除しました。\n");
		cout("\n");
	}
	errorCase(existPath(destBackDir));

	if(existDir(destDir))
	{
		cout("前回のデータをバックアップしています...\n");

		cmdln = xcout("REN %s %s", destDir, getLocal(destBackDir));
		execute(cmdln);
		memFree(cmdln);

		cout("バックアップしました。\n");
		cout("\n");
	}
	errorCase(existPath(destDir));
	createDir(destDir);

	addCwd(destDir);
	BackupDirs(targetDirs);
	unaddCwd();

	memFree(strDestDrv);
	memFree(destDir);
	memFree(destBackDir);
	releaseDim(targetDirs, 1);

	cout("\\e\n");

	if(!NoShutdownFlag)
	{
		coExecute("shutdown -s -t 30");
	}
}
