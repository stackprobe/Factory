#include "C:\Factory\Common\all.h"

#define REV_MAX 1000
#define REV_MAX_AT_DELETED 950

#define GAME_ORDER_FILE "order.txt"
#define GAME_TITLE_FILE "title.txt"

static int IsAsciiStr(char *str)
{
	char *p;

	for(p = str; *p; p++)
		if(!m_isRange(*p, '\x21', '\x7e'))
			return 0;

	return 1;
}
static char *GetRevision(void)
{
	char *file = makeTempFile(NULL);
	char *revision;

	coExecute_x(xcout("C:\\Factory\\DevTools\\rev.exe /P > \"%s\"", file));
	revision = readFirstLine(file);

	errorCase(!lineExp("<4,09>.<3,09>.<5,09>", revision)); // 2bs

	removeFile(file);
	memFree(file);
	return revision;
}
static void TrimRev(char *appDir)
{
	autoList_t *revDirs = lsDirs(appDir);

	if(REV_MAX < getCount(revDirs))
	{
		sortJLinesICase(revDirs);
		reverseElements(revDirs); // �I�[ == �ł��������r�W����

		while(REV_MAX_AT_DELETED < getCount(revDirs))
		{
			char *revDir = (char *)unaddElement(revDirs); // �ł��������r�W���������o���B

			cout("[DEL_REV] %s\n", revDir);

			errorCase(!lineExp("<4,09>.<3,09>.<5,09>", getLocal(revDir))); // 2bs

			recurRemoveDir(revDir);
			memFree(revDir);
		}
	}
	releaseDim(revDirs, 1);
}
static void AddRev_File(char *arcFile, char *docRoot)
{
	char *localFile = getLocal(arcFile);
	char *ext;
	char *appName = NULL;
	char *appDir = NULL;
	char *revision = NULL;
	char *revDir = NULL;
	char *wFile = NULL;

	cout("arcFile: %s\n", arcFile);
	cout("docRoot: %s\n", docRoot);

	ext = getExt(localFile);
	appName= changeExt(localFile, "");

	if(!IsAsciiStr(appName))
	{
		cout("�A�X�L�[�R�[�h�̕����񂶂�Ȃ��̂ŃX�L�b�v\n");
		goto endFunc;
	}
	appDir = combine(docRoot, appName);
	cout("appDir: %s\n", appDir);

	if(!existDir(appDir))
	{
		cout("�A�v���P�[�V�����������̂ŃX�L�b�v\n");
		goto endFunc;
	}
	revision = GetRevision();
	revDir = combine(appDir, revision);
	cout("revDir: %s\n", revDir);

	createDir(revDir);

	wFile = combine(revDir, localFile);
	cout("wFile: %s\n", wFile);

	moveFile(arcFile, wFile);

	cout("�������ړ����܂���������\n");

	TrimRev(appDir);

endFunc:
	memFree(appName);
	memFree(appDir);
	memFree(revision);
	memFree(revDir);
	memFree(wFile);
}
static void AddGameVer(char *arcFile, char *rootDir)
{
	char *lArcFile;
	char *name;
	char *wDir;
	char *wFile;
	char *md5File;
	char *wMD5File;

	LOGPOS();

	lArcFile = getLocal(arcFile);
	name = strxl(lArcFile, strlen(lArcFile) - 9); // "_v999.zip" �����B
	wDir = combine(rootDir, name);
	wFile = combine(wDir, lArcFile);
	md5File = addExt(strx(arcFile), "md5");
	wMD5File = addExt(strx(wFile), "md5");

	cout("< %s\n", arcFile);
	cout("< %s\n", md5File);
	cout("> %s\n", rootDir);
	cout("1.> %s\n", lArcFile);
	cout("2.> %s\n", name);
	cout("3.> %s\n", wDir);
	cout("4.> %s\n", wFile);
	cout("5.> %s\n", wMD5File);

	if(!existFile(md5File)) // ? .md5 �t�@�C���������B
		goto cancel;

	if(existFile(wFile) || existFile(wMD5File)) // ? ���̃o�[�W�����͊��ɑ��݂���B
		goto cancel;

	if(!existDir(wDir))
	{
		autoList_t *lines;

		addCwd(rootDir);
		lines = readResourceLines(GAME_ORDER_FILE);
		insertElement(lines, 0, (uint)strx(name)); // ���ǉ�
		writeLines_cx(GAME_ORDER_FILE, lines);
		unaddCwd();

		createDir(wDir);

		addCwd(wDir);
		writeOneLine(GAME_TITLE_FILE, name); // ���쐬
		unaddCwd();
	}
	moveFile(arcFile, wFile);
	moveFile(md5File, wMD5File);

cancel:
	memFree(name);
	memFree(wDir);
	memFree(wFile);
	memFree(md5File);
	memFree(wMD5File);

	LOGPOS();
}
static void ExtractCluster(char *cluster, char *rootDir)
{
	char *lCluster;
	char *node;
	char *wDir;

	LOGPOS();

	lCluster = getLocal(cluster);
	node = changeExt(lCluster, "");
	wDir = combine(rootDir, node);

	cout("wDir: %s\n", wDir);

	if(existDir(wDir))
		coExecute_x(xcout("C:\\Factory\\Tools\\Cluster.exe /OAD /OW /R \"%s\" \"%s\"", cluster, wDir));

	memFree(node);
	memFree(wDir);

	LOGPOS();
}
static void AddRev(char *rDir, char *wDir, char *gameWDir, char *extCluWDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);
	extCluWDir = makeFullPath(extCluWDir);

	cout("< %s\n", rDir);
	cout("> %s\n", wDir);
	cout("> %s\n", gameWDir);
	cout("> %s\n", extCluWDir);

	errorCase(!existDir(rDir));
	errorCase(!existDir(wDir));
	errorCase(!existDir(gameWDir));
	errorCase(!existDir(extCluWDir));

	{
		autoList_t *files = lsFiles(rDir);
		char *file;
		uint index;

		foreach(files, file, index)
		{
			if(existFile(file)) // .md5 �t�@�C���ȂǁA��������ꍇ������B
			{
				AddRev_File(file, wDir);
			}
			if(existFile(file) && lineExpICase("<>_v<3,09>.zip", file))
			{
				AddGameVer(file, gameWDir);
			}
			if(existFile(file) && !_stricmp("clu", getExt(file)))
			{
				ExtractCluster(file, extCluWDir);
			}
		}
		releaseDim(files, 1);
	}

	memFree(rDir);
	memFree(wDir);
	memFree(extCluWDir);
}
int main(int argc, char **argv)
{
	if(hasArgs(3))
	{
		AddRev(getArg(0), getArg(1), getArg(2), getArg(3));
		return;
	}
	AddRev(
		"C:\\pub\\�����[�X��",
		"C:\\BlueFish\\BlueFish\\HTT\\stackprobe\\home",
		"C:\\BlueFish\\BlueFish\\HTT\\cerulean\\home\\charlotte",
		"C:\\BlueFish\\BlueFish\\HTT\\extra"
		);
}
