/*
	newcs c プロジェクト名
	newcs f プロジェクト名
	newcs t プロジェクト名
	newcs c2 プロジェクト名
	newcs f2 プロジェクト名
	newcs t2 プロジェクト名
	newcs tt プロジェクト名
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static uint UTIntoParentStep;

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

		UTIntoParentStep++;
	}
	error(); // never
	return NULL;
}
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
static void ResolveRelHintPath(char *file)
{
	char *text = readText_b(file);
	char *p;
	int modified = 0;

	text = strrm(text, 128000000); // XXX
	p = text;

	while(p = strstrNext(p, "<HintPath>"))
	{
		if(startsWith(p, "..\\"))
		{
			char *trailer = strx(p);
			uint c;

			for(c = 0; c < UTIntoParentStep; c++)
			{
				*p++ = '.';
				*p++ = '.';
				*p++ = '\\';
			}
			strcpy(p, trailer);

			memFree(trailer);

			p = strstrNext(p, "</HintPath>");
			errorCase(!p);

			modified = 1;
		}
	}
	if(modified)
		writeOneLineNoRet_b(file, text);

	memFree(text);
}
static void Main2(char *tmplProject, char *tmplDir, int utFlag, int m2Flag)
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

			if(utFlag)
			{
				char *csprojFile = xcout("%s.csproj", project);

				if(existFile(csprojFile))
				{
					ResolveRelHintPath(csprojFile);
				}
				memFree(csprojFile);
			}
		}
		unaddCwd();

		removeFileIfExist("C:\\Factory\\tmp\\Sections.txt"); // 意図しない検索結果を trep しないように、念のため検索結果をクリア

		coExecute_x(xcout("Search.exe %s", tmplProject));
		coExecute_x(xcout("trep.exe /F %s", project));

//		execute("START .");

		execute_x(xcout("%s.sln", project));

		if(m2Flag)
			execute("START /MAX C:\\Dev\\CSharp\\Module2\\Module2");
	}
	unaddCwd();

	// zantei -- Chcolate.dll のビルド
	{
		if(!existFile("C:\\Dev\\CSharp\\Chocolate\\Chcolate\\bin\\Release\\Chcolate.dll"))
		{
			addCwd("C:\\Dev\\CSharp\\Chocolate");
			{
				coExecute("cx **");
			}
			unaddCwd();
		}
	}
}
int main(int argc, char **argv)
{
	if(argIs("C"))
	{
		Main2("CCCC", "C:\\Dev\\CSharp\\Template\\CUIProgramTemplate", 0, 1);
		return;
	}
	if(argIs("F"))
	{
		Main2("FFFF", "C:\\Dev\\CSharp\\Template\\FormApplicationTemplate", 0, 1);
		return;
	}
	if(argIs("T"))
	{
		Main2("TTTT", "C:\\Dev\\CSharp\\Template\\TaskTrayTemplate", 0, 1);
		return;
	}
	if(argIs("C2"))
	{
		Main2("CCCC", "C:\\Dev\\CSharp\\Template2\\CUIProgramTemplate", 0, 0);
		return;
	}
	if(argIs("F2"))
	{
		Main2("FFFF", "C:\\Dev\\CSharp\\Template2\\FormApplicationTemplate", 0, 0);
		return;
	}
	if(argIs("T2"))
	{
		Main2("TTTT", "C:\\Dev\\CSharp\\Template2\\TaskTrayTemplate", 0, 0);
		return;
	}
	if(argIs("TT"))
	{
		Main2("TTTT", FindUserTemplate(), 1, 0); // g
		return;
	}
	cout("usage: newcs (C｜F｜T｜C2｜F2｜T2｜TT) プロジェクト名\n");
}
