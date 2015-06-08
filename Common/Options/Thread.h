#pragma once

#include "C:\Factory\Common\all.h"

/*
	thread_tls static uint TlsVar;
*/
#define thread_tls \
	__declspec(thread)

uint runThread(void (*userFunc)(void *), void *userInfo);
void waitThread(uint hdl);
int waitThreadEx(uint hdl, uint millis);
void collectDeadThreads(autoList_t *hdls);

typedef struct critical_st
{
	CRITICAL_SECTION Csec;
}
critical_t;

void initCritical(critical_t *i);
void fnlzCritical(critical_t *i);
void enterCritical(critical_t *i);
void leaveCritical(critical_t *i);

void critical(void);
void uncritical(void);
void inner_uncritical(void);
void inner_critical(void);
