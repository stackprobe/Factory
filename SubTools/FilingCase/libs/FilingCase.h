#pragma once

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h" // <- sha512

uint FC_GetCount(char *table);
autoList_t *FC_GetAllId(char *table);
char *FC_GetId(char *table, char *column, char *value);
char *FC_GetValue(char *table, char *id, char *column);
void FC_SetValue(char *table, char *id, char *column, char *value);
void FC_Delete(char *table, char *id);
void FC_Truncate(char *table);
