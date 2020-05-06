#include "all.h"

uint lastMemoryLoad;
uint64 lastMemoryFree;
uint64 lastMemorySize;
uint64 lastPageFileFree;
uint64 lastPageFileSize;
uint64 lastVirtualFree;
uint64 lastVirtualSize;
uint64 lastExVirtualFree;

void updateMemory(void)
{
	MEMORYSTATUSEX ms;

	memset(&ms, 0x00, sizeof(ms));
	ms.dwLength = sizeof(MEMORYSTATUSEX);
//	ms = { sizeof(MEMORYSTATUSEX) };

	if(!(int)GlobalMemoryStatusEx(&ms)) // ? ���s
	{
		error();
	}
	lastMemoryLoad = (uint)ms.dwMemoryLoad;
	lastMemoryFree = (uint64)ms.ullAvailPhys;
	lastMemorySize = (uint64)ms.ullTotalPhys;
	lastPageFileFree = (uint64)ms.ullAvailPageFile;
	lastPageFileSize = (uint64)ms.ullTotalPageFile;
	lastVirtualFree = (uint64)ms.ullAvailVirtual;
	lastVirtualSize = (uint64)ms.ullTotalVirtual;
	lastExVirtualFree = (uint64)ms.ullAvailExtendedVirtual;
}

uint64 lastDiskFree_User; // ���̃v���Z�X���g�p�o����󂫗̈�T�C�Y
uint64 lastDiskFree;
uint64 lastDiskSize;

void updateDiskSpace(int drive)
{
	char dir[4];

	dir[0] = drive;
	dir[1] = ':';
	dir[2] = '\\';
	dir[3] = '\0';

	/*
		�h���C�u�����݂��Ȃ� || �����o���Ă��Ȃ� -> error();
	*/
	updateDiskSpace_Dir(dir);
}
void updateDiskSpace_Dir(char *dir)
{
	ULARGE_INTEGER a;
	ULARGE_INTEGER f;
	ULARGE_INTEGER t;

	errorCase(m_isEmpty(dir));

	/*
		�h���C�u�����݂��Ȃ� || �����o���Ă��Ȃ� || �f�B���N�g�������݂��Ȃ� -> ���s����B
	*/
	if(!(int)GetDiskFreeSpaceEx((LPCTSTR)dir, &a, &t, &f)) // ? ���s
	{
		error();
	}
	lastDiskFree_User = (uint64)a.LowPart | (uint64)a.HighPart << 32;
	lastDiskFree = (uint64)f.LowPart | (uint64)f.HighPart << 32;
	lastDiskSize = (uint64)t.LowPart | (uint64)t.HighPart << 32;
}
