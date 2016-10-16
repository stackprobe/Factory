/*
	cp.exe [コピー元DIR コピー先DIR]

		コピー元DIR ... 存在するディレクトリであること。
		コピー先DIR ... 存在するディレクトリであること。

		コピー先の直下にコピー元のローカル名を作成して、そこへコピーする。

	cp.exe /D [コピー先DIR]

		コピー先DIR ... 存在するディレクトリであること。

		コピー先を固定して、複数のコピー元を指定する。
*/

#include "C:\Factory\Common\all.h"

static void DoCopy(char *rDir, char *wDir)
{
	rDir = makeFullPath(rDir);
	wDir = makeFullPath(wDir);

	cout("0.< %s\n", rDir);
	cout("0.> %s\n", wDir);

	errorCase_m(!existDir(rDir), "コピー元ディレクトリは存在しません。");
	errorCase_m(!existDir(wDir), "コピー先ディレクトリ(ルート)は存在しません。");

	{
		char *rLDir = getLocal(rDir);

		if(*rLDir)
			rLDir = strx(rLDir);
		else
			rLDir = xcout("%c$$", rDir[0]);

		wDir = combine_xx(wDir, rLDir);
		wDir = toCreatableTildaPath(wDir, IMAX);
	}

	cout("1.> %s\n", wDir);

	createDir(wDir);
	coExecute_x(xcout("START \"\" /MIN ROBOCOPY.EXE \"%s\" \"%s\" /MIR", rDir, wDir));

	memFree(rDir);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	if(argIs("/D"))
	{
		char *rDir;
		char *wDir;

		cout("+------------------------------+\n");
		cout("| コピー先を固定して複数コピー |\n");
		cout("+------------------------------+\n");

		if(hasArgs(1))
		{
			wDir = strx(nextArg());
		}
		else
		{
			cout("固定されたコピー先ディレクトリ(ルート):\n");
			wDir = dropDir();
		}

		for(; ; )
		{
			cout("コピー元ディレクトリ:\n");
			rDir = dropDir();

			DoCopy(rDir, wDir);

			memFree(rDir);

			cout("\n");
		}
		return; // dummy
	}

	if(hasArgs(2))
	{
		DoCopy(getArg(0), getArg(1));
		return;
	}

	for(; ; )
	{
		char *rDir;
		char *wDir;

		cout("コピー元ディレクトリ:\n");
		rDir = dropDir();
		cout("コピー先ディレクトリ(ルート):\n");
		wDir = dropDir();

		DoCopy(rDir, wDir);

		memFree(rDir);
		memFree(wDir);

		cout("\n");
	}
}
