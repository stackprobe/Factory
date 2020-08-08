#pragma once

#include "C:\Factory\Common\all.h"
#include "Common.h"

void MailParser(autoBlock_t *mail);
char *MP_GetHeaderValue(char *targKey);
autoBlock_t *c_MP_GetBody(void);
