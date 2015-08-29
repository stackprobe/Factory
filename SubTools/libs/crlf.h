#pragma once

#include "C:\Factory\Common\all.h"

void CRLF_CheckFile(char *file, uint *p_num_cr, uint *p_num_crlf, uint *p_num_lf);
void CRLF_ConvFile(char *rFile, char *wFile, char *newLine);