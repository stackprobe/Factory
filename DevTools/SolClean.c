#include "C:\Factory\Common\all.h"

static void RemoveFileEx(char *file)
{
	cout("rf: %s\n", file);
	remove(file);
}
static void RemoveDirEx(char *dir)
{
	cout("rd: %s\n", dir);
	execute_x(xcout("RD /S /Q \"%s\"", dir));
}
static void ClearDirEx(char *dir)
{
	cout("clearDir: %s\n", dir);
	execute_x(xcout("RD /S /Q \"%s\"", dir));
	execute_x(xcout("MD \"%s\"", dir));
}

// ---- Java ----

static void CleanJavaProject(char *file) // file - Javaプロジェクトファイル
{
	char *dir = getParent(file);

	cout("CleanJavaProject: %s\n", file);

	addCwd(dir);
	ClearDirEx("bin");
	unaddCwd();

	memFree(dir);
}
static void FindCleanJava(autoList_t *files)
{
	char *file;
	uint index;

	foreach(files, file, index)
	{
		if(!_stricmp(getLocal(file), ".project"))
		{
			char *cpFile = changeLocal(file, ".classpath");

			if(existFile(cpFile))
			{
				CleanJavaProject(file);
			}
			memFree(cpFile);
		}
	}
}

// ----

static void CleanProject(char *file) // file - プロジェクトファイル
{
	char *dir = getParent(file);
	char *userWCard = xcout("%s.*.user", getLocal(file));

	cout("CleanProject: %s\n", file);

	addCwd(dir);

	// vb2008
	// vcs2008
	RemoveDirEx("bin");
	RemoveDirEx("obj");

	// vc2008
	RemoveDirEx("Debug");
	RemoveDirEx("Release");
//	execute_x(xcout("DEL \"%s\"", userWCard)); // 実行時のカレントなどの情報が入っているので消さない。

	unaddCwd();

	memFree(dir);
	memFree(userWCard);
}
static void FindCleanProjects(char *dir) // dir - ソリューションのルートDIR
{
	autoList_t *files = lssFiles(dir);
	char *file;
	uint index;

	foreach(files, file, index)
	{
		if(
			!_stricmp("vbproj", getExt(file)) || // vb2008, vb2010
			!_stricmp("vcproj", getExt(file)) || // vc2008
			!_stricmp("vcxproj", getExt(file)) || // vc2010
			!_stricmp("csproj", getExt(file)) || // vcs2008, vcs2010
			0
			)
		{
			CleanProject(file);
		}
	}
	releaseDim(files, 1);
}
static void CleanSolution(char *file) // file - ソリューションファイル
{
	char *dir = getParent(file);
	char *ncbFile = changeExt(getLocal(file), "ncb"); // 2008
	char *suoFile = changeExt(getLocal(file), "suo"); // 2008, 2010
	char *sdfFile = changeExt(getLocal(file), "sdf"); // 2010
	char *opensdfFile = changeExt(getLocal(file), "opensdf"); // 2010 (tmp)

	cout("CleanSolution: %s\n", file);

	addCwd(dir);

	// vc2008
	RemoveDirEx("Debug");
	RemoveDirEx("Release");
	RemoveFileEx(ncbFile);

	// vb2008
	// vc2008
	// vcs2008
	RemoveFileEx(suoFile);

	// vc2010
	RemoveDirEx("ipch");
	RemoveFileEx(sdfFile);
	RemoveFileEx(opensdfFile);

	unaddCwd();

	FindCleanProjects(dir);

	memFree(dir);
	memFree(ncbFile);
	memFree(suoFile);
	memFree(sdfFile);
}
static void FindClean(char *dir) // dir - 検索ルートDIR
{
	autoList_t *files;
	char *file;
	uint index;

	dir = makeFullPath(dir);
	cout("RootDir: %s\n", dir);
	errorCase(!existDir(dir));

	files = lssFiles(dir);

	foreach(files, file, index)
	{
		if(!_stricmp("sln", getExt(file))) // ? ソリューションファイル
		{
			CleanSolution(file);
		}
	}
	FindCleanJava(files);

	releaseDim(files, 1);
	memFree(dir);

	cout("\\e\n");
}
int main(int argc, char **argv)
{
	if(hasArgs(1))
	{
		FindClean(nextArg());
	}
	else
	{
		char *path = dropPath();

		if(path)
		{
			if(existFile(path))
			{
				path = getParent(path);
			}
			FindClean(path);
			memFree(path);
		}
	}
}
