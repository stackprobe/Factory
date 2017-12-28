#include "GitJapanesePath.h"

#define ESC_LPATH "[Japanese-path-name]_0001"
//#define ESC_LPATH "JP_0001"

static autoList_t *RBLines; // restore batch lines

static int HasJpn(char *str)
{
	char *p;

	for(p = str; *p; p++)
		if(_ismbblead(*p))
			return 1;

	return 0;
}
static char *PutDq(char *path)
{
	/*
		REN abc d=e     NG
		REN abc "d=e"   OK

- - -

m_isasciikana について、

       COPY abc d<文字>e NG   COPY abc "d<文字>e" NG
----------------------------------------------------
\x20   NG
&      NG
+      NG
,      NG
/      NG                NG
:      NG ※1            NG ※1
;      NG
<      NG                NG
=      NG
>      NG                NG
?      NG ※2            NG ※2
\      NG ※1            NG ※1
|      NG                NG

※1 ドライブ・ディレクトリを指定したことになる。
※2 "?" を "" に置き換えてコピーは成功する。

- - -

'%' について、

d%tmp%f というファイルは（エクスプローラから）作成可能だが、
COPY abc d%tmp%f
COPY abc "d%tmp%f"
は環境変数に置き換えられて NG
COPY abc d%none%f
のように定義されていない環境変数であれば OK ???

execute (system) 関数から実行した場合、コマンドプロンプトに直接入力した場合と同じ結果。

バッチファイルの中から % -> %% でエスケープしてやれば問題ないっぽい。

	*/
	if(
		strchr(path, ' ') ||
		strchr(path, '&') ||
		strchr(path, '+') ||
		strchr(path, ',') ||
		strchr(path, ';') ||
		strchr(path, '=')
		)
	{
		path = insertChar(path, 0, '"');
		path = addChar(path, '"');
	}
	path = replaceLine(path, "%", "%%", 0);

	return path;
}
static void SolveJpnPath(char *rootDir, char *realRootDir)
{
	autoList_t *paths = ls(rootDir);
	char *path;
	uint index;

	RemoveGitPaths(paths);

	sortJLinesICase(paths);

	foreach(paths, path, index)
	{
		char *lPath = getLocal(path);

		if(HasJpn(lPath))
		{
			char *dest = changeExt_xc(changeLocal(path, ESC_LPATH), getExt(path));
			char *relDest;

			dest = toCreatablePath(dest, IMAX);
			relDest = changeRoot(strx(dest), realRootDir, NULL);

			movePath(path, dest);

			lPath = strx(lPath);
			lPath = PutDq(lPath);
			relDest = PutDq(relDest);

			addElement(RBLines, (uint)xcout("REN %s %s", relDest, lPath));

			memFree(lPath);
			memFree(dest);
			memFree(relDest);
		}
	}
	releaseDim(paths, 1);
	paths = lsDirs(rootDir);

	RemoveGitPaths(paths);

	sortJLinesICase(paths);

	foreach(paths, path, index)
	{
		SolveJpnPath(path, realRootDir);
	}
	releaseDim(paths, 1);
}
void SolveJapanesePath(char *rootDir)
{
	char *batFile = combine(rootDir, "_run_me_for_restore_japanese_path_name.bat_");

	LOGPOS();

	RBLines = newList();
	rootDir = makeFullPath(rootDir);

	SolveJpnPath(rootDir, rootDir);

	if(getCount(RBLines))
	{
		LOGPOS();
		reverseElements(RBLines);
		writeLines(batFile, RBLines);
	}
	releaseDim(RBLines, 1);
	memFree(rootDir);
	memFree(batFile);

	LOGPOS();
}
