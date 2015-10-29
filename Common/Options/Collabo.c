/*
	主に C:\\app\\Kit\\* との連携を想定する。
*/

#include "Collabo.h"

static char *S_GetCollaboPath(char *innerPath, int (*existFunc)(char *), int mode) // mode: "FD"
{
	char *path;

	errorCase(m_isEmpty(innerPath));
	errorCase(!existFunc);

	if(isAbsPath(innerPath))
	{
		path = combine(getSelfDir(), getLocal(innerPath));

		if(!_stricmp(path, getSelfFile()))
		{
			LOGPOS();
		}
		else
		{
			if(existFunc(path))
				goto foundPath;
		}
		memFree(path);
		path = makeFullPath(innerPath);
	}
	else
		path = combine(getSelfDir(), innerPath);

	if(existFunc(path))
		goto foundPath;

	if(mode == 'D')
	{
		createPath(path, 'D');
		goto foundPath;
	}
	error_m(xcout("[%s]が見つかりません。", innerPath));

foundPath:
	cout("COLLABO_PATH: %s\n", path);
	return path;
}
char *GetCollaboFile(char *innerPath)
{
	return S_GetCollaboPath(innerPath, existFile, 'F');
}
char *GetCollaboDir(char *innerPath)
{
	return S_GetCollaboPath(innerPath, existDir, 'D');
}
