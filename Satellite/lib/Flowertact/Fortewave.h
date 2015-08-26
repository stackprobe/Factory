#pragma once

#include "C:\Factory\Common\all.h"
#include "..\Serializer.h"
#include "..\Tools.h"

typedef struct Frtwv_st
{
	char *Ident;
	uint Mutex;
	uint MessagePostEvent;
	char *MessageDir;
}
Frtwv_t;

Frtwv_t *Frtwv_Create(uchar *ident);
void Frtwv_Release(Frtwv_t *i);
void Frtwv_Clear(Frtwv_t *i);
void Frtwv_Send(Frtwv_t *i, void *data, uint depth);
void Frtwv_SendSD(Frtwv_t *i, autoBlock_t *sendData);
void *Frtwv_Recv(Frtwv_t *i, uint depth, uint millis);
autoBlock_t *Frtwv_RecvSD(Frtwv_t *i, uint millis);
