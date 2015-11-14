#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"

#define REV_MAX 100

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

	sortJLinesICase(revDirs);
	reverseElements(revDirs); // 終端 == 最も旧いリビジョン

	while(REV_MAX < getCount(revDirs))
	{
		char *revDir = (char *)unaddElement(revDirs); // 最も旧いリビジョンを取り出す。

		cout("[DEL_REV] %s\n", revDir);

		errorCase(!lineExp("<4,09>.<3,09>.<5,09>", getLocal(revDir))); // 2bs

		forceRemoveDir(revDir);
		memFree(revDir);
	}
	releaseDim(revDirs, 1);
}
static void AddRev_File(char *arcFile, char *docRoot)
{
	char *lfile = getLocal(arcFile);
	char *ext;
	char *appName = NULL;
	char *appDir = NULL;
	char *revision = NULL;
	char *revDir = NULL;
	char *wFile = NULL;

	cout("arcFile: %s\n", arcFile);
	cout("docRoot: %s\n", docRoot);

	ext = getExt(lfile);
	appName= changeExt(lfile, "");

	if(!IsAsciiStr(appName))
	{
		cout("アスキーコードの文字列じゃないのでスキップ\n");
		goto endFunc;
	}
	appDir = combine(docRoot, appName);
	cout("appDir: %s\n", appDir);

	if(!existDir(appDir))
	{
		cout("アプリケーションが無いのでスキップ\n");
		goto endFunc;
	}
	revision = GetRevision();
	revDir = combine(appDir, revision);
	cout("revDir: %s\n", revDir);

	createDir(revDir);

	wFile = combine(revDir, lfile);
	cout("wFile: %s\n", wFile);

	moveFile(arcFile, wFile);

	cout("★★★移動しました★★★\n");

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

	LOGPOS();

	lArcFile = getLocal(arcFile);
	name = strxl(lArcFile, strlen(lArcFile) - 9); // "_v999.zip" を削る。
	wDir = combine(rootDir, name);
	wFile = combine(wDir, lArcFile);

	cout("< %s\n", arcFile);
	cout("> %s\n", rootDir);
	cout("1.> %s\n", lArcFile);
	cout("2.> %s\n", name);
	cout("3.> %s\n", wDir);
	cout("4.> %s\n", wFile);

	if(!existDir(wDir))
	{
		addCwd(rootDir);
		addLine2File(GAME_ORDER_FILE, name); // 仮追加
		unaddCwd();

		createDir(wDir);

		addCwd(wDir);
		writeOneLine(GAME_TITLE_FILE, name); // 仮作成
		unaddCwd();
	}
	if(!existFile(wFile)) // 既に存在したら何もしない。
		moveFile(arcFile, wFile);

	memFree(name);
	memFree(wDir);
	memFree(wFile);

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
			AddRev_File(file, wDir);

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
		"C:\\pub\\リリース物",
		"C:\\BlueFish\\BlueFish\\HTT\\stackprobe\\home",
		"C:\\BlueFish\\BlueFish\\HTT\\cerulean\\home\\charlotte",
		"C:\\BlueFish\\BlueFish\\HTT\\extra"
		);
}
