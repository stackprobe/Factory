/*
	zcp.exe [/F] [/M | /R] SOURCE-PATH DESTINATION-PATH

		/F ... 強制モード

			コピー先が既に存在する場合、問い合わせすることなく削除する。

		/M, /R ... 移動モード

			コピー元を削除する。つまり移動する。
*/

#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	char *srcPath;
	char *destPath;
	int force_mode = 0;
	int move_mode = 0;

readArgs:
	if(argIs("/F"))
	{
		force_mode = 1;
		goto readArgs;
	}
	if(argIs("/M") || argIs("/R"))
	{
		move_mode = 1;
		goto readArgs;
	}

	srcPath = makeFullPath(nextArg());
	destPath = makeFullPath(nextArg());

	cout("< %s\n", srcPath);
	cout("> %s\n", destPath);

	if(existPath(destPath))
	{
		if(!force_mode)
		{
			cout("コピー先が存在します。\n");
			cout("削除？\n");

			if(clearGetKey() == 0x1b)
				termination(0);

			cout("削除します。\n");
		}
		forceRemovePath(destPath);
	}

	if(existFile(srcPath))
	{
		createPath(destPath, 'X');

		if(move_mode)
		{
			moveFile(srcPath, destPath);
		}
		else
		{
			copyFile(srcPath, destPath);
		}
	}
	else
	{
		errorCase(!existDir(srcPath));
		createPath(destPath, 'D');

		if(move_mode)
		{
			moveDir(srcPath, destPath);
			removeDir(srcPath);
		}
		else
		{
			copyDir(srcPath, destPath);
		}
	}

	memFree(srcPath);
	memFree(destPath);
}
