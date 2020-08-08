#pragma once

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CryptoRand.h"
#include "Common.h"

char *MakeMailMessageID(char *senderMailAddr);

// ---- MailParser ----

void MailParser(autoBlock_t *mail);
char *MP_GetHeaderValue(char *targKey);
autoBlock_t *c_MP_GetBody(void);

// ----
