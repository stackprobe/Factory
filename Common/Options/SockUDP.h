#pragma once

#include "SockClient.h"

int sockUDPOpenSend(uchar ip[4], char *domain, uint portno);
int sockUDPOpenRecv(uint portno);
void sockUDPSend(int sock, autoBlock_t *block);
autoBlock_t *sockUDPRecv(int sock, uint szMax);
void sockUDPClose(int sock);
