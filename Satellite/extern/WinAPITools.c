#include "C:\Factory\Common\all.h"
#include "C:\Factory\Satellite\libs\Flowertact\Fortewave.h"
#include <tlhelp32.h>

static int IsSameSelfExeFile(char *file1, char *file2)
{
	FILE *fp1 = fileOpen(file1, "rb");
	FILE *fp2 = fileOpen(file2, "rb");
	uint diffCnt = 0;

	for(; ; )
	{
		int chr1 = readChar(fp1);
		int chr2 = readChar(fp2);

		if(chr1 == EOF)
		{
			if(chr2 != EOF)
				diffCnt = UINTMAX;

			break;
		}
		if(chr2 == EOF)
		{
			diffCnt = UINTMAX;
			break;
		}
		if(chr1 != chr2)
			diffCnt++;
	}
	fileClose(fp1);
	fileClose(fp2);

//addLine2File_cx("C:\\temp\\WinAPITools_Debug.log", xcout("diffCnt: %u", diffCnt)); // test

	return diffCnt <= 32;
}

// ---- Process ----

static char *ParentProcMonitorName;

static int IsParentAlive(void)
{
	int alive = 1;
	uint hdl = mutexOpen(ParentProcMonitorName);

	if(handleWaitForMillis(hdl, 0)) // ? ロックできた。== ロックされていない。-> 親プロセス停止中
	{
		alive = 0;
		mutexRelease(hdl);
	}
	handleClose(hdl);
	return alive;
}
static int IsProcessAlive(uint targetProcId)
{
	int alive = 0;
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if(hSnapshot != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if(Process32First(hSnapshot, &pe32))
		{
			do
			{
				int procId = (int)pe32.th32ProcessID;

				if(procId == targetProcId)
					alive = 1;
			}
			while(Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
	return alive;
}

// ----

static int HdlWaitForMillisEx(int hdl, uint millis)
{
	uint elapse = 0;

	for(; ; )
	{
		if(millis == INFINITE)
		{
			if(handleWaitForMillis(hdl, 2000))
				return 1;
		}
		else
		{
			uint m = m_min(2000, millis - elapse);

			if(handleWaitForMillis(hdl, m))
				return 1;

			elapse += m;

			if(millis <= elapse)
				break;
		}
		if(!IsParentAlive())
			break;
	}
	return 0;
}
int main(int argc, char **argv)
{
	if(argIs("/MUTEX-WAIT-ONE"))
	{
		char *targetName;
		uint millis;
		char *beganName;
		char *wObj0File;
		char *endName;
		uint targetMutex;
		uint beganEvent;
		uint endEvent;

		targetName = nextArg();
		millis = toValue(nextArg());
		beganName = nextArg();
		wObj0File = nextArg();
		endName = nextArg();
		ParentProcMonitorName = nextArg();

		targetMutex = mutexOpen(targetName);
		beganEvent = eventOpen(beganName);
		endEvent = eventOpen(endName);

		if(HdlWaitForMillisEx(targetMutex, millis))
		{
			createFile(wObj0File);
			eventSet(beganEvent);
			HdlWaitForMillisEx(endEvent, INFINITE);
			mutexRelease(targetMutex);
		}
		else
		{
			eventSet(beganEvent);
			HdlWaitForMillisEx(endEvent, INFINITE);
		}
		handleClose(targetMutex);
		handleClose(beganEvent);
		handleClose(endEvent);
		return;
	}
	if(argIs("/EVENT-CREATE"))
	{
		char *targetName;
		char *beganName;
		char *endName;
		uint targetEvent;
		uint beganEvent;
		uint endEvent;

		targetName = nextArg();
		beganName = nextArg();
		endName = nextArg();
		ParentProcMonitorName = nextArg();

		targetEvent = eventOpen(targetName);
		beganEvent = eventOpen(beganName);
		endEvent = eventOpen(endName);

		eventSet(beganEvent);
		HdlWaitForMillisEx(endEvent, INFINITE);

		handleClose(targetEvent);
		handleClose(beganEvent);
		handleClose(endEvent);
		return;
	}
	if(argIs("/EVENT-SET"))
	{
		char *targetName;
		uint targetEvent;

		targetName = nextArg();

		targetEvent = eventOpen(targetName);

		eventSet(targetEvent);

		handleClose(targetEvent);
		return;
	}
	if(argIs("/EVENT-WAIT-ONE"))
	{
		char *targetName;
		uint millis;
		uint targetEvent;

		targetName = nextArg();
		millis = toValue(nextArg());
		ParentProcMonitorName = nextArg();

		targetEvent = eventOpen(targetName);

		HdlWaitForMillisEx(targetEvent, millis);

		handleClose(targetEvent);
		return;
	}
	if(argIs("/GET-ENV"))
	{
		char *envName;
		char *envValFile;
		char *envVal;

		envName = nextArg();
		envValFile = nextArg();

		envVal = getEnvLine(envName);

		writeOneLineNoRet(envValFile, envVal);
		return;
	}
	if(argIs("/DEAD-AND-REMOVE"))
	{
		char *beganName;
		char *deadName;
		char *mtxName;
		char *targetPath;
		uint beganEvent;
		uint deadEvent;
		uint mtx;

		beganName = nextArg();
		deadName = nextArg();
		mtxName = nextArg();
		targetPath = nextArg();
		ParentProcMonitorName = nextArg();

		beganEvent = eventOpen(beganName);
		deadEvent = eventOpen(deadName);
		mtx = mutexOpen(mtxName);

		eventSet(beganEvent);
		HdlWaitForMillisEx(deadEvent, INFINITE);

		handleWaitForever(mtx);
		recurRemovePathIfExist(targetPath);
		mutexRelease(mtx);

		handleClose(beganEvent);
		handleClose(deadEvent);
		handleClose(mtx);
		return;
	}
	if(argIs("/DELETE-DELAY-UNTIL-REBOOT"))
	{
		char *targetPath;

		targetPath = nextArg();

		MoveFileEx(targetPath, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
		return;
	}
	if(argIs("/CHECK-PROCESS-ALIVE"))
	{
		uint targetProcId;
		char *trueFile;

		targetProcId = toValue(nextArg());
		trueFile = nextArg();

		if(IsProcessAlive(targetProcId))
			createFile(trueFile);

		return;
	}
	if(argIs("/CHECK-MUTEX-LOCKED"))
	{
		char *targetName;
		char *trueFile;
		uint mtx;

		targetName = nextArg();
		trueFile = nextArg();

		mtx = mutexOpen(targetName);

		if(handleWaitForMillis(mtx, 0))
			mutexRelease(mtx);
		else
			createFile(trueFile);

		handleClose(mtx);
		return;
	}
	if(argIs("/SEND-TO-FORTEWAVE"))
	{
		char *identHash;
		char *dir;

		identHash = nextArg();
		dir = nextArg();

		{
			Frtwv_t *i = Frtwv_CreateIH(identHash);
			uint index;

			for(index = 0; ; index++)
			{
				char *file = combine_cx(dir, xcout("%04u", index));
				autoBlock_t *sendData;

				if(!existFile(file))
				{
					memFree(file);
					break;
				}
				sendData = readBinary(file);
				Frtwv_Send(i, sendData);
				releaseAutoBlock(sendData);
				removeFile(file);
				memFree(file);
			}
			Frtwv_Release(i);
		}
		removeDir(dir);
		return;
	}
	if(argIs("/RECV-FROM-FORTEWAVE"))
	{
		char *identHash;
		char *dir;
		uint millis;
		uint recvLimit;
		uint recvLimitSize;

		identHash = nextArg();
		dir = nextArg();
		millis = toValue(nextArg());
		recvLimit = toValue(nextArg());
		recvLimitSize = toValue(nextArg());

		{
			Frtwv_t *i = Frtwv_CreateIH(identHash);
			uint index;
			uint totalSize = 0;

			for(index = 0; index < recvLimit && totalSize < recvLimitSize; index++)
			{
				autoBlock_t *recvData = Frtwv_Recv(i, index ? 0 : millis);
				char *file;

				if(!recvData)
					break;

				file = combine_cx(dir, xcout("%04u", index));
				writeBinary(file, recvData);
				totalSize += getSize(recvData);
				releaseAutoBlock(recvData);
				memFree(file);
			}
			Frtwv_Release(i);
		}
		return;
	}
	if(argIs("/EXTRACT"))
	{
		char *rFile;
		char *wFile;
		uint hdl;

		rFile = nextArg();
		wFile = nextArg();

		hdl = mutexLock("{aed96b6d-8a77-40fb-9285-9b75405fc3b2}");
		{
//			if(_access(wFile, 0) || !IsSameSelfExeFile(rFile, wFile))
//			if(_access(wFile, 0) || !isSameFile(rFile, wFile)) // 没、実行ファイル内を書き換えているので、常に内容は一致しない。
			if(_access(wFile, 0))
//			if(!existFile(wFile))
			{
				createPath(wFile, 'X');
				removeFileIfExist(wFile);
				moveFile(rFile, wFile);
			}
		}
		mutexUnlock(hdl);

		return;
	}
	if(argIs("/MONITOR"))
	{
		uint parentProcId;
		uint hdl;
		uint mtx;

		parentProcId = toValue(nextArg());
		ParentProcMonitorName = nextArg();

		hdl = (uint)OpenProcess(PROCESS_ALL_ACCESS, 0, parentProcId);
		errorCase(hdl == 0);

		mtx = mutexOpen(ParentProcMonitorName);

		handleWaitForever(mtx);
		handleWaitForever(hdl);
		mutexRelease(mtx);

		handleClose(hdl);
		handleClose(mtx);
		return;
	}
	cout("Dummy cout: {cfb94d47-7371-4080-a0b2-c3c4c6deafd6}\n"); // shared_uuid@g
}
