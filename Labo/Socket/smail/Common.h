#pragma once

#include "C:\Factory\Common\all.h"
#include "pop3s.h"
#include "smtps.h"
#include "tools.h"

void CheckMailCommonParams(char *server, uint portno, char *user, char *pass);
int IsFairMailAddress(char *mailAddress);
