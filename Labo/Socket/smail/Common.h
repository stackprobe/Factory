#pragma once

#include "C:\Factory\Common\all.h"
#include "..\mail\mutex.h"
#include "pop3s.h"
#include "smtps.h"
#include "tools.h"

void CheckMailServer(char *server);
void CheckMailPortno(uint portno);
void CheckMailUser(char *user);
void CheckMailPass(char *pass);
void CheckMailCommonParams(char *server, uint portno, char *user, char *pass);
void CheckMailAddress(char *mailAddress);

#define NETWORK_TIMEOUT_SEC 300
//#define NETWORK_TIMEOUT_SEC 120
//#define NETWORK_TIMEOUT_SEC 60
//#define NETWORK_TIMEOUT_SEC 30
