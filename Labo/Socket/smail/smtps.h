#pragma once

#include "C:\Factory\Common\all.h"
#include "Common.h"

void SendMail(char *smtpServer, uint portno, char *user, char *pass, char *fromMailAddress, char *toMailAddress, autoBlock_t *mail);
