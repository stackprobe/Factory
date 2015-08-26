#include "Fortewave.h"

#define IDENT_PREFIX "Fortewave_{d8600f7d-1ff4-47f3-b1c9-4b5aa15b6461}_" // shared_uuid:3

Frtwv_t *Frtwv_Create(uchar *ident)
{
	Frtwv_t *i = nb(Frtwv_t);
	char *identHash;
	char *mIdent;
	char *eIdent;

	errorCase(!ident);

	identHash = GetSHA512_128String(ident);
	ident = xcout(IDENT_PREFIX "%s", identHash);
	mIdent = xcout("%s_m", ident);
	eIdent = xcout("%s_e", ident);

	i->Ident = ident;
	i->Mutex = mutexOpen(mIdent);
	i->MessagePostEvent = eventOpen(eIdent);
	i->MessageDir = combine(GetTmp(), ident);

	memFree(identHash);
//	memFree(ident);
	memFree(mIdent);
	memFree(eIdent);

	return i;
}
void Frtwv_Release(Frtwv_t *i)
{
	errorCase(!i);

	memFree(i->Ident);
	handleClose(i->Mutex);
	handleClose(i->MessagePostEvent);
	memFree(i->MessageDir);
	memFree(i);
}

void Frtwv_Clear(Frtwv_t *i)
{
	errorCase(!i);

	handleWaitForever(i->Mutex);
	{
		forceRemoveDirIfExist(i->MessageDir);
	}
	mutexRelease(i->Mutex);
}

static uint GMR_FirstNo;
static uint GMR_NextNo;

static void GetMessageRange(Frtwv_t *i)
{
	if(existDir(i->MessageDir))
	{
		autoList_t *files = lsFiles(i->MessageDir);
		char *file;
		uint index;
		uint firstNo = UINTMAX;
		uint lastNo = 0;

		foreach(files, file, index)
		{
			uint no = toValue(getLocal(file));

			m_minim(firstNo, no);
			m_maxim(lastNo, no);
		}
		releaseDim(files, 1);

		GMR_FirstNo = firstNo;
		GMR_NextNo = lastNo + 1;
	}
	else
	{
		GMR_FirstNo = 0;
		GMR_NextNo = 0;
	}
}
static void Renumber(Frtwv_t *i)
{
	uint no;

	for(no = GMR_FirstNo; no < GMR_NextNo; no++)
	{
		char *rFile = combine_cx(i->MessageDir, xcout("%04u", no));
		char *wFile = combine_cx(i->MessageDir, xcout("%04u", no - GMR_FirstNo));

		moveFile(rFile, wFile);

		memFree(rFile);
		memFree(wFile);
	}

	GMR_NextNo -= GMR_FirstNo;
	GMR_FirstNo = 0;
}
static void TryRenumber(Frtwv_t *i)
{
	if(1000 < GMR_FirstNo) // ? ディレクトリが有る。&& ファイルが有る。&& 欠番が十分にある。
	{
		Renumber(i);
	}
}

void Frtwv_Send(Frtwv_t *i, void *data, uint depth)
{
	autoBlock_t *sendData;

	errorCase(!i);
	errorCase(!data);
	// depth

	sendData = Stllt_Serializer(data, depth);
	Frtwv_SendSD(i, sendData);
	releaseAutoBlock(sendData);
}
void Frtwv_SendSD(Frtwv_t *i, autoBlock_t *sendData)
{
	errorCase(!i);
	errorCase(!sendData);

	handleWaitForever(i->Mutex);
	{
		char *file;

		GetMessageRange(i);
		TryRenumber(i);

		if(!GMR_NextNo)
			createDir(i->MessageDir);

		file = combine_cx(i->MessageDir, xcout("%04u", GMR_NextNo));
		writeBinary(file, sendData);
		memFree(file);
	}
	mutexRelease(i->Mutex);

	eventSet(i->MessagePostEvent);
}

void *Frtwv_Recv(Frtwv_t *i, uint depth, uint millis)
{
	autoBlock_t *recvData;
	void *ret;

	errorCase(!i);
	// depth

	recvData = Frtwv_RecvSD(i, millis);
	ret = Stllt_Deserializer(recvData, depth);
	releaseAutoBlock(recvData);
	return ret;
}
static autoBlock_t *TryRecv(Frtwv_t *i)
{
	autoBlock_t *recvData = NULL;

	handleWaitForever(i->Mutex);
	{
		GetMessageRange(i);

		if(GMR_NextNo) // ? ! no data
		{
			char *file = combine_cx(i->MessageDir, xcout("%04u", GMR_FirstNo));

			recvData = readBinary(file);
			removeFile(file);
			memFree(file);

			if(GMR_FirstNo + 1 == GMR_NextNo)
				removeDir(i->MessageDir);
		}
	}
	mutexRelease(i->Mutex);

	return recvData;
}
autoBlock_t *Frtwv_RecvSD(Frtwv_t *i, uint millis)
{
	autoBlock_t *recvData;

	errorCase(!i);
	// millis

	recvData = TryRecv(i);

	if(!recvData)
	{
		handleWaitForMillis(i->MessagePostEvent, millis);
		recvData = TryRecv(i);
	}
	return recvData;
}
