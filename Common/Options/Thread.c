#include "Thread.h"

typedef struct Info_st
{
	uint Busy;
	void (*UserFunc)(void *);
	void *UserInfo;
}
Info_t;

#define INFO_MAX 3000

static Info_t *Infos;
static uint InfoIndex;

static uint __stdcall Perform(void *prm)
{
	Info_t info = *(Info_t *)prm;

	((Info_t *)prm)->Busy = 0;

	info.UserFunc(info.UserInfo);
	return 0;
}

/*
	スレッドの終了を待つには waitThread() を使用すること。
	waitThread(runThread(FuncTh));
	_beginthreadex() で生成したスレッドは明示的に CloseHandle() する必要があるらしいので、waitThread() は必須

	どの関数がスレッドセーフかとかぜんぜん把握していないので、原則として全関数「非スレッドセーフ」とする。この関数もスレッドセーフではない。
	スレッドセーフな関数はコメントに記載する。-> // ts_

	非スレッドセーフな関数だらけなので userFunc() は、最初から最後まで cirtical(), uncritical() で囲んだ方が安全だろう。
	要所要所で一時的に inner_uncritical(), inner_critical() する。

	CritCommonLockCount は thread_tls であることに注意せよ。
	runThread() を呼び出した側で critical() されていると、runThread() 側で uncritical() されるまで userFunc() 側の critical() は待たされる。
	userFunc() 側で uncritical() しても runThread() 側の critical() はアンロックされない！(スレッドが違うから！)
*/
uint runThread(void (*userFunc)(void *), void *userInfo)
{
	Info_t *info;
	uint hdl;

	errorCase(!userFunc);

	if(!Infos)
		Infos = na(Info_t, INFO_MAX);

	info = Infos + InfoIndex;

	errorCase(info->Busy); // ? INFO_MAX 回前の Perform() の info がまだ参照されていない。

	info->Busy = 1;
	info->UserFunc = userFunc;
	info->UserInfo = userInfo;

	InfoIndex = (InfoIndex + 1) % INFO_MAX;

	hdl = (uint)_beginthreadex(0, 0, Perform, info, 0, 0);

	if(hdl == 0) // ? 失敗
	{
		error();
	}
	return hdl;
}
void waitThread(uint hdl) // ts_
{
	WaitForSingleObject((HANDLE)hdl, INFINITE);

	critical();
	{
		CloseHandle((HANDLE)hdl);
	}
	uncritical();
}
int waitThreadEx(uint hdl, uint millis) // ts_ ret: ? 終了した。
{
	if(WaitForSingleObject((HANDLE)hdl, millis) != WAIT_TIMEOUT)
	{
		critical();
		{
			CloseHandle((HANDLE)hdl);
		}
		uncritical();
		return 1;
	}
	return 0;
}
void collectDeadThreads(autoList_t *hdls)
{
	uint hdl;
	uint n;

	foreach(hdls, hdl, n)
	{
		if(waitThreadEx(hdl, 0))
		{
			setElement(hdls, n, 0);
		}
	}
	removeZero(hdls);
}

void initCritical(critical_t *i)
{
	InitializeCriticalSection(&i->Csec);
}
void fnlzCritical(critical_t *i)
{
	DeleteCriticalSection(&i->Csec);
}
void enterCritical(critical_t *i)
{
	EnterCriticalSection(&i->Csec);
}
void leaveCritical(critical_t *i)
{
	LeaveCriticalSection(&i->Csec);
}

#define INNER_UNLOCK_MAX 3

static int CritCommonInited;
static critical_t CritCommon;
thread_tls static uint CritCommonLockCount;
thread_tls static uint InnerUnlockCount;
thread_tls static uint InnerLockCountList[INNER_UNLOCK_MAX];

#define CritErrorCase(cond) \
	do { \
	if((cond)) { \
		critical(); \
		error(); \
	} \
	} while(0)

void critical(void)
{
	if(!CritCommonLockCount)
	{
		if(!CritCommonInited) // 遅くとも最初の runThread() 呼び出し前(＝最初のスレッド生成前)に呼ばれる。-> 初期化は atomic に行われる。
		{
			CritCommonInited = 1;
			initCritical(&CritCommon);
		}
		enterCritical(&CritCommon);
	}
	CritCommonLockCount++;
}
void uncritical(void)
{
	CritErrorCase(!CritCommonLockCount);

	CritCommonLockCount--;

	if(!CritCommonLockCount)
	{
		leaveCritical(&CritCommon);
	}
}
void inner_uncritical(void)
{
	CritErrorCase(INNER_UNLOCK_MAX <= InnerUnlockCount);

	if(CritCommonLockCount)
	{
		leaveCritical(&CritCommon);
	}
	InnerLockCountList[InnerUnlockCount] = CritCommonLockCount;
	InnerUnlockCount++;
	CritCommonLockCount = 0;
}
void inner_critical(void)
{
	CritErrorCase(!InnerUnlockCount);
	CritErrorCase(CritCommonLockCount);

	InnerUnlockCount--;
	CritCommonLockCount = InnerLockCountList[InnerUnlockCount];

	if(CritCommonLockCount)
	{
		enterCritical(&CritCommon);
	}
}
