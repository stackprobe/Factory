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
	cout("�����đ����Ƀ��[���̑���M���s��Ȃ��悤�ɂ�����Ƒ҂B\n");
	coSleep(3000);

	cout("mailUnlock() started.\n");
	mutexUnlock(MailMutexHandle);
	cout("mailUnlock() OK!\n");
}
