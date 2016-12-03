#pragma once

#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\sha512.h"

typedef struct Nector_st
{
	uint EvData;
	uint EvCtrl;
	uint EvSend;
	uint EvRecv;
}
Nector_t;

Nector_t *CreateNector(char *name);
void ReleaseNector(Nector_t *i);

void NectorSend(Nector_t *i, autoBlock_t *buff);
void NectorSendLine(Nector_t *i, char *line);

autoBlock_t *NectorRecv(Nector_t *i);
autoBlock_t *NectorReceipt(Nector_t *i);
