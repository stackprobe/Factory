#include "FilingCase.h"

static int IsId(char *nameOrId)
{
	return lineExp("<32,09AFaf>", nameOrId);
}
static char *NameOrIdToId(char *nameOrId)
{
	char *id;

	if(IsId(nameOrId))
		id = strx(nameOrId);
	else
		id = sha512_128Line(nameOrId);

	return id;
}
char *FC_GetNewId(void)
{
	return MakeRandHexID();
}

// ---- lock ----

#define COMMON_MTX "{fb4fc70a-88ba-4240-a674-3a86052a750c}"

static uint Mtx;

static void DoLock(void)
{
	errorCase(!Mtx);
	Mtx = mutexLock(COMMON_MTX);
}
static void DoUnlock(void)
{
	errorCase(Mtx);
	mutexUnlock(Mtx);
	Mtx = 0;
}

// ---- table ----

#define ROOT_DIR "C:\\appdata\\FilingCase"

static char *GetTableDir(char *tableId)
{
	return combine(ROOT_DIR, tableId);
}
autoList_t *FC_GetAllTableId(void)
{
	autoList_t *ret;

	DoLock();
	{
		if(existDir(ROOT_DIR))
			ret = lsDirs(ROOT_DIR);
		else
			ret = newList();
	}
	DoUnlock();

	return ret;
}
void FC_SwapTable(char *tableNameOrId1, char *tableNameOrId2)
{
	char *tableId1;
	char *tableId2;
	char *tmpId;
	char *dir1;
	char *dir2;
	char *dir3;

	errorCase(!tableNameOrId1);
	errorCase(!tableNameOrId2);

	tableId1 = NameOrIdToId(tableNameOrId1);
	tableId2 = NameOrIdToId(tableNameOrId2);
	tmpId = FC_GetNewId();

	dir1 = GetTableDir(tableId1);
	dir2 = GetTableDir(tableId2);
	dir3 = GetTableDir(tmpId);

	DoLock();
	{
		createPath(dir1, 'd');
		createPath(dir2, 'd');

		moveFile(dir1, dir3);
		moveFile(dir2, dir1);
		moveFile(dir3, dir2);

		removeDirIfEmpty(dir1);
		removeDirIfEmpty(dir2);
	}
	DoUnlock();

	memFree(tableId1);
	memFree(tableId2);
	memFree(tmpId);
	memFree(dir1);
	memFree(dir2);
	memFree(dir3);
}
void FC_DeleteTable(char *tableNameOrId)
{
	char *tableId;
	char *dir;

	errorCase(!tableNameOrId);

	tableId = NameOrIdToId(tableNameOrId);

	dir = GetTableDir(tableId);

	DoLock();
	{
		forceRemoveDir(dir);
	}
	DoUnlock();

	memFree(tableId);
	memFree(dir);
}

// ---- column ----

autoList_t *FC_GetAllColumnId(char *tableNameOrId)
{
	error(); // TODO
	return NULL;
}
void FC_SwapColumn(char *tableNameOrId, char *columnNameOrId1, char *columnNameOrId2)
{
	error(); // TODO
}
void FC_DeleteColumn(char *tableNameOrId, char *columnNameOrId)
{
	error(); // TODO
}

// ---- row ----

autoList_t *FC_GetAllRowId(char *tableNameOrId, char *columnNameOrId)
{
	error(); // TODO
	return NULL;
}
uint FC_GetRowCount(char *tableNameOrId, char *columnNameOrId)
{
	error(); // TODO
	return 0;
}

// ---- value ----

autoBlock_t *FC_GetValue(char *tableNameOrId, char *rowNameOrId, char *columnNameOrId)
{
	error(); // TODO
	return NULL;
}
void FC_SetValue(char *tableNameOrId, char *rowNameOrId, char *columnNameOrId, autoBlock_t *value)
{
	error(); // TODO
}
char *FC_GetRowId(char *tableNameOrId, char *columnNameOrId, autoBlock_t *value)
{
	error(); // TODO
	return NULL;
}

// ---- str_value ----

char *FC_GetStrValue(char *tableNameOrId, char *rowNameOrId, char *columnNameOrId)
{
	error(); // TODO
	return NULL;
}
void FC_SetStrValue(char *tableNameOrId, char *rowNameOrId, char *columnNameOrId, char *value)
{
	error(); // TODO
}
char *FC_GetStrRowId(char *tableNameOrId, char *columnNameOrId, char *value)
{
	error(); // TODO
	return NULL;
}

// ----

void FC_SwapRow(char *tableNameOrId, char *rowNameOrId1, char *rowNameOrId2)
{
	error(); // TODO
}
void FC_DeleteRow(char *tableNameOrId, char *rowNameOrId)
{
	error(); // TODO
}
