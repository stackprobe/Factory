#pragma once

#pragma comment(lib, "ADVAPI32")

#include "C:\Factory\Common\all.h"
#include "C:\Factory\OpenSource\sha512.h"
#include <wincrypt.h>

void createKeyContainer(void);
void deleteKeyContainer(void);
autoBlock_t *makeCryptoBlock_MS(uint count);

uint getCryptoByte(void);
uint getCryptoRand16(void);
uint getCryptoRand24(void);
uint getCryptoRand(void);
autoBlock_t *makeCryptoRandBlock(uint count);
