#include "mutex.h"

#define MAILMUTEXNAME "cerulean.charlotte Factory mail mutex object"

static uint MailMutexHandle;

void mailLock(void)
{
	cout("mailLock() started.\n");
	MailMutexHandle = mutexLock(MAILMUTEXNAME);
	cout("mailLock() OK!\n");
}
void mailUnlock(void)
{
	cout("mailUnlock() started.\n");
	mutexUnlock(MailMutexHandle);
	cout("mailUnlock() OK!\n");
}
