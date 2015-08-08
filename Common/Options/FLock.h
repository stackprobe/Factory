#pragma once

#include "C:\Factory\Common\all.h"

FILE *FLockLoop(char *file);
FILE *FLock(char *file);
void FUnlock(FILE *fp);
