#include "C:\Factory\Common\all.h"
#include <tlhelp32.h>

// ---- ParentProcId ----

static int ParentProcId = -1;

static int IsParentAlive(void)
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

				if(procId == ParentProcId)
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
		ParentProcId = toValue(nextArg());

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
		ParentProcId = toValue(nextArg());

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
		ParentProcId = toValue(nextArg());

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
		ParentProcId = toValue(nextArg());

		beganEvent = eventOpen(beganName);
		deadEvent = eventOpen(deadName);
		mtx = mutexOpen(mtxName);

		eventSet(beganEvent);
		HdlWaitForMillisEx(deadEvent, INFINITE);

		handleWaitForever(mtx);
		forceRemovePathIfExist(targetPath);
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
		char *trueFile;

		ParentProcId = toValue(nextArg());
		trueFile = nextArg();

		if(IsParentAlive())
			createFile(trueFile);

		return;
	}
}
