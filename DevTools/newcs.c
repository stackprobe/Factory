/*
	newcs c プロジェクト名
	newcs f プロジェクト名
	newcs t プロジェクト名
	newcs tt プロジェクト名
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static void RenamePaths(char *fromPtn, char *toPtn)
{
	autoList_t *paths = ls(".");
	char *path;
	char *nPath;
	uint index;

	eraseParents(paths);

	foreach(paths, path, index)
	{
		if(index < lastDirCount)
		{
			addCwd(path);
			{
				RenamePaths(fromPtn, toPtn);
			}
			unaddCwd();
		}
		nPath = replaceLine(strx(path), fromPtn, toPtn, 1);

		if(replaceLine_getLastReplacedCount())
		{
			coExecute_x(xcout("REN \"%s\" \"%s\"", path, nPath));
		}
		memFree(nPath);
	}
	releaseDim(paths, 1);
}
static void ChangeAppIdent(char *srcFile)
{
	char *src = readText(srcFile);
	char *p;
	char *q;
	char *uuid;

	p = strstrNext(src, "public const string APP_IDENT = \"");
	errorCase(!p);
	q = strstr(p, "\";");
	errorCase(!q);
	errorCase((uint)q - (uint)p != 38); // {} 付き UUID の長さ

	uuid = MakeUUID(1);

	memcpy(p, uuid, 38);

	writeOneLineNoRet(srcFile, src);

	memFree(src);
	memFree(uuid);
}
static void Main2(char *tmplProject, char *tmplDir)
{
	char *project = nextArg();

	errorCase(!existDir(tmplDir)); // 2bs ?

	errorCase_m(!lineExp("<1,30,__09AZaz>", project), "不正なプロジェクト名です。");
	errorCase_m(existPath(project), "既に存在します。");

	createDir(project);
	copyDir(tmplDir, project);

	addCwd(project);
	{
		coExecute("qq -f");

		RenamePaths(tmplProject, project);

		addCwd(project);
		{
			ChangeAppIdent("Program.cs");
		}
		unaddCwd();

		removeFileIfExist("C:\\Factory\\tmp\\Sections.txt"); // 意図しない検索結果を trep しないように、念のため検索結果をクリア

		coExecute_x(xcout("Search.exe %s", tmplProject));
		coExecute_x(xcout("trep.exe /F %s", project));

//		execute("START .");

		execute_x(xcout("%s.sln", project)); // zantei
		execute("START /MAX C:\\Dev\\CSharp\\Module2\\Module2"); // zantei
	}
	unaddCwd();
}
static char *FindUserTemplate(void)
{
	char *dir = getCwd();

	for(; ; )
	{
		char *tmplDir = combine(dir, "Template");
		char *tmplSln;

		tmplSln = combine(tmplDir, "TTTT.sln");

		if(existFile(tmplSln))
		{
			memFree(dir);
			memFree(tmplSln);

			cout("User_Template: %s\n", tmplDir);

			return tmplDir;
		}
		memFree(tmplDir);
		memFree(tmplSln);

		dir = changeLocal_xc(dir, "");

		errorCase(strlen(dir) == 2);
	}
	error(); // never
	return NULL;
}
int main(int argc, char **argv)
{
	if(argIs("C"))
	{
		Main2("CCCC", "C:\\Dev\\CSharp\\Template\\CUIProgramTemplate");
		return;
	}
	if(argIs("F"))
	{
		Main2("FFFF", "C:\\Dev\\CSharp\\Template\\FormApplicationTemplate");
		return;
	}
	if(argIs("T"))
	{
		Main2("TTTT", "C:\\Dev\\CSharp\\Template\\TaskTrayTemplate");
		return;
	}
	if(argIs("TT"))
	{
		Main2("TTTT", FindUserTemplate()); // g
		return;
	}
	cout("usage: newcs (C｜F｜T｜TT) プロジェクト名\n");
}
