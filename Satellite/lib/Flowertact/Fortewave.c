#include "Fortewave.h"

#define IDENT_PREFIX "Fortewave_{d8600f7d-1ff4-47f3-b1c9-4b5aa15b6461}_" // shared_uuid:3

Frtwv_t *Frtwv_Create(uchar *ident)
{
	Frtwv_t *i = nb(Frtwv_t);
	char *identHash;
	char *mIdent;
	char *eIdent;

	errorCase(!ident);

	identHash = getSHA512_128String(ident);
	ident = xcout(IDENT_PREFIX "%s", identHash);
	mIdent = xcout("%s_m", ident);
	eIdent = xcout("%s_e", ident);

	i->Ident = ident;
	i->Mutex = mutexOpen(mIdent);
	i->MessagePostEvent = eventOpen(eIdent);
	i->MessageDir;

	memFree(identHash);
//	memFree(ident);
	memFree(mIdent);
	memFree(eIdent);

	return i;
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
		// TODO
	}
	mutexRelease(i->Mutex);

	// TODO
}

// TODO
