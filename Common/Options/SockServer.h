#pragma once

#include "Socket.h"
#include "SockConnectMon.h"

extern uchar sockClientIp[4];
extern uint sockUserTransmitIndex;

void sockServerPerformInterrupt(void);
void sockServerEx(int (*funcPerform)(char *, char *),
	int (*funcTransmit)(int, void *),
	void *(*funcCreateUserInfo)(void),
	void (*funcReleaseUserInfo)(void *),
	uint portno, uint connectmax, uint64 uploadmax, int (*funcIdle)(void));
void sockServer(int (*funcPerform)(char *, char *), uint portno, uint connectmax, uint64 uploadmax, int (*funcIdle)(void));
void sockServerUserTransmit(
	int (*funcTransmit)(int, void *),
	void *(*funcCreateUserInfo)(void),
	void (*funcReleaseUserInfo)(void *),
	uint portno, uint connectmax, int (*funcIdle)(void));
