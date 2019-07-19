/*
	newcs c プロジェクト名
	newcs f プロジェクト名
	newcs t プロジェクト名
	newcs d プロジェクト名
	newcs c2 プロジェクト名
	newcs f2 プロジェクト名
	newcs t2 プロジェクト名
	newcs d2 プロジェクト名
	newcs tt プロジェクト名

	newcs
	newcs /r
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

#define TESTER_PROJ_LDIR "Test01"

static uint UTIntoParentStep;

static char *FindUserTemplate(void)
{
	char *dir = getCwd();

	for(; ; )
	{
		char *tmplDir = combine(dir, "Template");
		char *tmplSln;

		tmplSln = combine(tmplDir, "UUUUTMPL.sln");

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
	uint dirCount;
	uint index;

	eraseParents(paths);
	dirCount = lastDirCount;

	foreach(paths, path, index)
	{
		if(index < dirCount)
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

		addCwd(existDir(TESTER_PROJ_LDIR) ? TESTER_PROJ_LDIR : project);
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
			execute("START C:\\Dev\\CSharp\\Module2\\Module2");
	}
	unaddCwd();
}
static uint64 GetChocolateSourceLeastStamp(void)
{
	autoList_t *files = lssFiles("C:\\Dev\\CSharp\\Chocolate\\Chocolate");
	char *file;
	uint index;
	uint64 leastStamp = 0;

	foreach(files, file, index)
	{
		uint64 stamp;

		if(mbs_stristr(file, "\\bin\\")) // bin の配下は除外
			continue;

		if(mbs_stristr(file, "\\obj\\")) // obj の配下は除外
			continue;

		getFileStamp(file, NULL, NULL, &stamp);

		m_maxim(leastStamp, stamp);
	}
	return leastStamp;
}
static int IsChocolateSourceUpdated(void)
{
	uint64 dllStamp;
	uint64 leastStamp = GetChocolateSourceLeastStamp();

	getFileStamp("C:\\Dev\\CSharp\\Chocolate\\Chocolate\\bin\\Release\\Chocolate.dll", NULL, NULL, &dllStamp);

//	cout("%I64u\n", dllStamp);
//	cout("%I64u\n", leastStamp);

	return dllStamp < leastStamp;
}
int main(int argc, char **argv)
{
	// Chocolate.dll をビルドする。
	{
		if(argIs("/R")) // リビルド
		{
			addCwd("C:\\Dev\\CSharp\\Chocolate");
			{
				coExecute("qq");
				coExecute("cx Chocolate.sln");
			}
			unaddCwd();
		}
		else if(!existFile("C:\\Dev\\CSharp\\Chocolate\\Chocolate\\bin\\Release\\Chocolate.dll") || IsChocolateSourceUpdated())
		{
			addCwd("C:\\Dev\\CSharp\\Chocolate");
			{
				coExecute("cx Chocolate.sln");
			}
			unaddCwd();
		}
	}

	// DLL をビルドする。(要オプション指定)
	{
		if(argIs("/DR"))
		{
			addCwd("C:\\Dev\\CSharp\\DLL");
			{
				coExecute("qq");
				coExecute("cx **");
			}
			unaddCwd();

			return;
		}
		if(argIs("/D"))
		{
			addCwd("C:\\Dev\\CSharp\\DLL");
			{
				coExecute("cx **");
			}
			unaddCwd();

			return;
		}
	}

	if(argIs("C"))
	{
		Main2("CCCCTMPL", "C:\\Dev\\CSharp\\Template\\CUIProgramTemplate", 0, 1);
		return;
	}
	if(argIs("F"))
	{
		Main2("FFFFTMPL", "C:\\Dev\\CSharp\\Template\\FormApplicationTemplate", 0, 1);
		return;
	}
	if(argIs("T"))
	{
		Main2("TTTTTMPL", "C:\\Dev\\CSharp\\Template\\TaskTrayTemplate", 0, 1);
		return;
	}
	if(argIs("D"))
	{
		Main2("DDDDTMPL", "C:\\Dev\\CSharp\\Template\\DLLTemplate", 0, 1);
		return;
	}
	if(argIs("C2"))
	{
		Main2("CCCCTMPL", "C:\\Dev\\CSharp\\Template2\\CUIProgramTemplate", 0, 0);
		return;
	}
	if(argIs("F2"))
	{
		Main2("FFFFTMPL", "C:\\Dev\\CSharp\\Template2\\FormApplicationTemplate", 0, 0);
		return;
	}
	if(argIs("T2"))
	{
		Main2("TTTTTMPL", "C:\\Dev\\CSharp\\Template2\\TaskTrayTemplate", 0, 0);
		return;
	}
	if(argIs("D2"))
	{
		Main2("DDDDTMPL", "C:\\Dev\\CSharp\\Template2\\DLLTemplate", 0, 0);
		return;
	}
	if(argIs("TT"))
	{
		Main2("UUUUTMPL", FindUserTemplate(), 1, 0); // g
		return;
	}
	cout("usage: newcs (C｜F｜T｜D｜C2｜F2｜T2｜D2｜TT) プロジェクト名\n");
}
