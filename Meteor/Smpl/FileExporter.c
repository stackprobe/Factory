/*
	FileExporter.exe /E 入力ディレクトリ [出力ディレクトリ]
	FileExporter.exe /I 入力ディレクトリ
*/

#include "C:\Factory\Common\all.h"
#include "..\FileExporter.h"

int main(int argc, char **argv)
{
	if(argIs("/E"))
	{
		char *rDir;
		char *wDir;

		rDir = nextArg();
		wDir = hasArgs(1) ? nextArg() : makeFreeDir();

		rDir = makeFullPath(rDir);
		wDir = makeFullPath(wDir);

		cout("< %s\n", rDir);
		cout("> %s\n", wDir);

		if(existDir(wDir) && lsCount(wDir))
		{
			cout("出力先ディレクトリは空ではありません。\n");
			cout("空にする？\n");

			if(getKey() == 0x1b)
				termination(0);

			cout("空にします。\n");
		}
		recurRemoveDirIfExist(wDir);

		errorCase_m(!FileExporter(rDir, wDir), "エクスポート失敗");

		memFree(rDir);
		memFree(wDir);
		return;
	}
	if(argIs("/I"))
	{
		char *rDir;

		rDir = makeFullPath(nextArg());
		cout("< %s\n", rDir);

		errorCase_m(!FileImporter(rDir), "インポート失敗");

		memFree(rDir);
		return;
	}
}
