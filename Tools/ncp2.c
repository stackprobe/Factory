/*
	ncp2.exe [/S SERVER-DOMAIN] [/P SERVER-PORT] [/R RETRY-COUNT] [/T RETRY-WAIT-MILLIS] ...

	ncp2.exe ... (/MUP | MUP) LOCAL-DIR [SERVER-DIR]

	ncp2.exe ... (/MDL | MDL) LOCAL-DIR SERVER-DIR

	ncp2.exe ... (/MDL | MDL) * SERVER-DIR
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

static char *ServerDomain;
static uint ServerPort = UINTMAX;
static uint RetryCount = UINTMAX;
static uint RetryWaitMillis = UINTMAX;

#define NCP_EXE "C:\\Factory\\Tools\\ncp.exe"

#define MIRROR_DIR_RETRY_COUNT 7
#define MIRROR_DIR_RETRY_WAIT_MILLIS 3000

static char *GetNcpOptions(void) // ret: c_
{
	static char *ret;

	if(!ret)
		ret = strx("");

	*ret = '\0';

	if(ServerDomain)
		ret = addLine_x(ret, xcout(" /S \"%s\"", ServerDomain));

	if(ServerPort != UINTMAX)
		ret = addLine_x(ret, xcout(" /P %u", ServerPort));

	if(RetryCount != UINTMAX)
		ret = addLine_x(ret, xcout(" /R %u", RetryCount));

	if(RetryWaitMillis != UINTMAX)
		ret = addLine_x(ret, xcout(" /T %u", RetryWaitMillis));

	return ret;
}
static autoList_t *GetServerPaths(char *serverDir)
{
	char *molp = MakeUUID(1);
	char *outFile = makeTempPath(NULL);
	autoList_t *serverPaths = NULL;

	coExecute_x(xcout(NCP_EXE " //MOLP %s //O %s%s /LS \"%s\"", molp, outFile, GetNcpOptions(), serverDir));

	if(!lastSystemRet) // ? コマンド実行成功
	{
		char *line;
		uint index;

		serverPaths = readLines(outFile);

		foreach(serverPaths, line, index)
		{
			if(startsWith(line, molp))
				eraseLine(line, strlen(molp));
			else
				*line = '\0';
		}
		trimLines(serverPaths);
	}
	memFree(molp);
	removeFile(outFile);
	memFree(outFile);
	return serverPaths;
}
static int RemoveServerPath(char *serverPath)
{
	coExecute_x(xcout(NCP_EXE "%s /RM \"%s\"", GetNcpOptions(), serverPath));

	return !lastSystemRet;
}
static int UploadFile(char *clientFile, char *serverFile)
{
	coExecute_x(xcout(NCP_EXE "%s /UP \"%s\" \"%s\"", GetNcpOptions(), clientFile, serverFile));

	return !lastSystemRet;
}
static int DownloadFile(char *clientFile, char *serverFile)
{
	coExecute_x(xcout(NCP_EXE "%s /DL \"%s\" \"%s\"", GetNcpOptions(), clientFile, serverFile));

	return !lastSystemRet;
}
static int MirrorDirMain(char *clientDir, char *serverDir, int direction)
{
	autoList_t *serverPaths = GetServerPaths(serverDir);
	autoList_t *serverDirs;
	autoList_t *serverFiles;
	int retval = 0;

	createDirIfNotExist(clientDir);

	if(!serverPaths)
		goto endFunc;

	retval = 1;

	serverDirs  = newList();
	serverFiles = newList();

	{
		char *serverPath;
		uint index;

		foreach(serverPaths, serverPath, index)
		{
			int dirFlag = 0;

			escapeYen(serverPath);

			if(endsWith(serverPath, "/"))
			{
				strchr(serverPath, '\0')[-1] = '\0';
				dirFlag = 1;
			}
			restoreYen(serverPath);

			if(dirFlag)
				addElement(serverDirs, (uint)getLocal(serverPath));
			else
				addElement(serverFiles, (uint)getLocal(serverPath));
		}
	}

	{
		autoList_t *dirs = lsDirs(clientDir);
		char *dir;
		uint index;

		eraseParents(dirs);

		releaseDim(mergeConstLinesICase(dirs, serverDirs), 1);

		foreach(dirs, dir, index)
		{
			char *clientSubDir = combine(clientDir, dir);
			char *serverSubDir = combine(serverDir, dir);

			if(direction == 'U') // Upload
			{
				retval = MirrorDirMain(clientSubDir, serverSubDir, direction);
			}
			else // Download
			{
				recurRemoveDir(clientSubDir);
			}
			memFree(clientSubDir);
			memFree(serverSubDir);

			if(!retval)
				break;
		}
		releaseDim(dirs, 1);

		if(!retval)
			goto freeVars;

		foreach(serverDirs, dir, index)
		{
			char *clientSubDir = combine(clientDir, dir);
			char *serverSubDir = combine(serverDir, dir);

			if(direction == 'U') // Upload
			{
				retval = RemoveServerPath(serverSubDir);
			}
			else // Download
			{
				retval = MirrorDirMain(clientSubDir, serverSubDir, direction);
			}
			memFree(clientSubDir);
			memFree(serverSubDir);

			if(!retval)
				break;
		}
	}

	if(!retval)
		goto freeVars;

	{
		autoList_t *files = lsFiles(clientDir);
		char *file;
		uint index;

		eraseParents(files);

		releaseDim(mergeConstLinesICase(files, serverFiles), 1);

		foreach(files, file, index)
		{
			char *clientFile = combine(clientDir, file);
			char *serverFile = combine(serverDir, file);

			if(direction == 'U') // Upload
			{
				retval = UploadFile(clientFile, serverFile);
			}
			else // Download
			{
				removeFile(clientFile);
			}
			memFree(clientFile);
			memFree(serverFile);

			if(!retval)
				break;
		}
		releaseDim(files, 1);

		if(!retval)
			goto freeVars;

		foreach(serverFiles, file, index)
		{
			char *clientFile = combine(clientDir, file);
			char *serverFile = combine(serverDir, file);

			if(direction == 'U') // Upload
			{
				retval = RemoveServerPath(serverFile);
			}
			else // Download
			{
				retval = DownloadFile(clientFile, serverFile);
			}
			memFree(clientFile);
			memFree(serverFile);

			if(!retval)
				break;
		}
	}

freeVars:
	releaseDim(serverPaths, 1);
	releaseAutoList(serverDirs);
	releaseAutoList(serverFiles);

endFunc:
	cout("retval: %d\n", retval);
	return retval;
}
static void MirrorDir(char *clientDir, char *serverDir, int direction)
{
	uint retryCount = 0;

restart:
	if(!MirrorDirMain(clientDir, serverDir, direction))
	{
		if(retryCount < MIRROR_DIR_RETRY_COUNT)
		{
			retryCount++;
			cout("リトライ %u 回目\n", retryCount);
			coSleep(MIRROR_DIR_RETRY_WAIT_MILLIS);
			goto restart;
		}

		cout("+--------------------------------------------------------+\n");
		cout("| 失敗しましたがリトライ回数の上限に達したので終了します |\n");
		cout("+--------------------------------------------------------+\n");
	}
}
int main(int argc, char **argv)
{
readArgs:
	if(argIs("/S"))
	{
		ServerDomain = nextArg();
		goto readArgs;
	}
	if(argIs("/P"))
	{
		ServerPort = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/R"))
	{
		RetryCount = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/RWTM"))
	{
		RetryWaitMillis = toValue(nextArg());
		goto readArgs;
	}

	if(argIs("/MUP") || argIs("MUP")) // Mirror Upload
	{
		char *clientDir;
		char *serverDir;

		cout("MIRROR UPLOAD\n");

		clientDir = nextArg();

		if(clientDir[0] == '*')
			clientDir = dropDirFile(); // g

		if(hasArgs(1))
			serverDir = nextArg();
		else
			serverDir = getLocal(clientDir);

		cout("< %s\n", clientDir);
		cout("> %s\n", serverDir);

		errorCase(!*clientDir);
		errorCase(!*serverDir);

		errorCase(!existDir(clientDir));
		errorCase(!*serverDir);

		MirrorDir(clientDir, serverDir, 'U');
	}
	else if(argIs("/MDL") || argIs("MDL")) // Mirror Download
	{
		char *clientDir;
		char *serverDir;
		char *willOpenDir = NULL;
		int type;

		cout("MIRROR DOWNLOAD\n");

		clientDir = nextArg();
		serverDir = nextArg();

		errorCase(!*clientDir);
		errorCase(!*serverDir);

		if(clientDir[0] == '*')
			clientDir = combine(willOpenDir = makeFreeDir(), getLocal(serverDir)); // g

		cout("> %s\n", clientDir);
		cout("< %s\n", serverDir);

		errorCase(!existDir(clientDir) && !creatable(clientDir));

		MirrorDir(clientDir, serverDir, 'D');
	}
	else
		cout("不明なコマンド\n");
}
