/*
	newcpp g �v���W�F�N�g��
*/

#include "C:\Factory\Common\all.h"

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
static void Main2(char *tmplProject, char *tmplDir)
{
	char *project = nextArg();

	errorCase(!existDir(tmplDir));

	errorCase_m(!lineExp("<1,30,__09AZaz>", project), "�s���ȃv���W�F�N�g���ł��B");
	errorCase_m(existPath(project), "���ɑ��݂��܂��B");

	createDir(project);
	copyDir(tmplDir, project);

	addCwd(project);
	{
		coExecute("qq -f");

		RenamePaths(tmplProject, project);

		removeFileIfExist("C:\\Factory\\tmp\\Sections.txt"); // �Ӑ}���Ȃ��������ʂ� trep ���Ȃ��悤�ɁA�O�̂��ߌ������ʂ��N���A

		coExecute_x(xcout("Search.exe %s", tmplProject));
		coExecute_x(xcout("trep.exe /F %s", project));

//		coExecute("runsub _copylib");

		execute("START .");
	}
	unaddCwd();
}
int main(int argc, char **argv)
{
	if(argIs("G"))
	{
		Main2("GGGGTMPL", "C:\\Dev\\Annex\\Template\\GameTemplate");
		return;
	}
	if(argIs("G2"))
	{
		Main2("GGGGTMPL", "C:\\Dev\\Annex\\Template\\GameTemplate2");
		return;
	}
	cout("usage: newcpp (G�bG2�b?�b?) �v���W�F�N�g��\n");
}
