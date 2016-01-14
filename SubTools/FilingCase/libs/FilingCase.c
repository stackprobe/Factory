/*
	TABLE_DIR

		C:\\appdata\\FilingCase\\<TABLE_HASH>

	RECORD_FILE

		<TABLE_DIR>\\Record\\<RECORD_ID> = text lines {{ key, value } ... }

	LINK_FILE

		<TABLE_DIR>\\Link\\<COLUMN_HASH>\\<VALUE_HASH> = <RECORD_ID>

	TABLE_HASH
	RECORD_ID
	COLUMN_HASH
	VALUE_HASH

		SHA512_128
*/

#include "FilingCase.h"

// ---- lock ----

#define COMMON_MTX "{2c68fc51-0890-47ea-898e-97c00d485dcb}"

static uint Mtx;

static void DoLock(void)
{
	errorCase(Mtx);
	Mtx = mutexLock(COMMON_MTX);
}
static void DoUnlock(void)
{
	errorCase(!Mtx);
	mutexUnlock(Mtx);
	Mtx = 0;
}

// ---- hash ----

static char *ToHash(char *text)
{
	return strx(sha512_128Line(text));
}
static int IsFairHash(char *hash)
{
	return lineExp("<32,09AFaf>", hash);
}

// ---- dir, file ----

#define ROOT_DIR "C:\\appdata\\FilingCase"
#define LDIR_RECORD "Record"
#define LDIR_LINK "Link"

static char *GetTableDir(char *table)
{
	return combine_cx(ROOT_DIR, ToHash(table));
}
static char *GetRecordDir(char *table)
{
	return combine_xc(GetTableDir(table), LDIR_RECORD);
}
static char *NameOrIdToId(char *nameOrId)
{
	char *id;

	if(IsFairHash(nameOrId))
		id = strx(nameOrId);
	else
		id = ToHash(nameOrId);

	return id;
}
static char *GetRecordFile(char *table, char *nameOrId)
{
	return combine_xx(GetRecordDir(table), NameOrIdToId(nameOrId));
}
static char *GetLinkDir(char *table)
{
	return combine_xc(GetTableDir(table), LDIR_LINK);
}
static char *GetLinkColumnDir(char *table, char *column)
{
	return combine_xx(GetLinkDir(table), ToHash(column));
}
static char *GetLinkFile(char *table, char *column, char *value)
{
	return combine_xx(GetLinkColumnDir(table, column), ToHash(value));
}

// ---- record ----

static autoList_t *Columns; // NULL 検査用
static autoList_t *Values;

static void LoadRecord(char *table, char *nameOrId)
{
	char *file = GetRecordFile(table, nameOrId);

	if(Columns)
	{
		releaseDim(Columns, 1);
		releaseDim(Values,  1);
	}
	Columns = newList();
	Values  = newList();

	if(existFile(file))
	{
		autoList_t *lines = readLines(file);
		uint index;

		if(getCount(lines) == 1 && getLine(lines, 0)[0] == '\0')
		{
			releaseDim(lines, 1);
			lines = newList();
		}
		errorCase(getCount(lines) % 2 != 0);

		for(index = 0; index < getCount(lines); index += 2)
		{
			char *column = getLine(lines, index + 0);
			char *value  = getLine(lines, index + 1);

			line2JToken(column, 1, 1);
			line2JToken(value,  1, 1);

			addElement(Columns, (uint)column);
			addElement(Values,  (uint)value);
		}
		releaseAutoList(lines);
	}
	memFree(file);
}
static void SaveRecord(char *table, char *nameOrId)
{
	char *file = GetRecordFile(table, nameOrId);

	errorCase(!Columns); // ? not loaded

	createPath(file, 'X');

	{
		FILE *fp = fileOpen(file, "wb");
		uint index;

		for(index = 0; index < getCount(Columns); index++)
		{
			char *column = getLine(Columns, index);
			char *value  = getLine(Values,  index);

			line2JToken(column, 1, 1); // 2bs
			line2JToken(value,  1, 1); // 2bs

			writeLine(fp, column);
			writeLine(fp, value);
		}
		fileClose(fp);
	}

	memFree(file);
}
static uint GetColumnIndex(char *column)
{
	errorCase(!Columns); // ? not loaded

	return findLineCase(Columns, column, 1);
}
static char *GetValue(char *column)
{
	uint index = GetColumnIndex(column);
	char *ret;

	if(index < getCount(Columns))
	{
		ret = getLine(Values, index);
	}
	else
	{
		ret = "";
	}
	ret = strx(ret);
	return ret;
}
static void SetValue(char *column, char *value)
{
	uint index;

	column = strx(column);
	value  = strx(value);

	line2JToken(column, 1, 1);
	line2JToken(value,  1, 1);

	index = GetColumnIndex(column);

	if(index < getCount(Columns))
	{
		if(*value)
		{
			strzp((char **)directGetPoint(Values, index), value);
		}
		else
		{
			memFree((char *)desertElement(Columns, index));
			memFree((char *)desertElement(Values,  index));
		}
		memFree(column);
		memFree(value);
	}
	else
	{
		if(*value)
		{
			addElement(Columns, (uint)column);
			addElement(Values,  (uint)value);
		}
		else
		{
			memFree(column);
			memFree(value);
		}
	}
}

// ---- link ----

static void GenerateLinkColumn(char *table, char *column)
{
	char *rDir = GetRecordDir(table);
	char *wDir = GetLinkColumnDir(table, column);
	autoList_t *rFiles;
	char *rFile;
	uint index;

	LOGPOS();

	forceRemoveDirIfExist(wDir);
	createPath(wDir, 'D');

	rFiles = lsFiles(rDir);

	foreach(rFiles, rFile, index)
	{
		char *id = getLocal(rFile);
		char *value;
		char *wFile;

		errorCase(!IsFairHash(id));

		LoadRecord(table, id);

		value = GetValue(column);
		wFile = GetLinkFile(table, column, value);

		writeOneLineNoRet_b(wFile, id);

		memFree(value);
		memFree(wFile);
	}
	memFree(rDir);
	memFree(wDir);
	releaseDim(rFiles, 1);

	LOGPOS();
}
static void AddLinkColumnValue(char *table, char *column, char *value, char *nameOrId)
{
	char *file = GetLinkFile(table, column, value);
	char *id = NameOrIdToId(nameOrId);

	createPath(file, 'X');
	writeOneLineNoret_b(file, id);

	memFree(file);
	memFreE(id);
}
static void RemoveLinkColumnValue(char *table, char *column, char *value)
{
	char *file = GetLinkFile(table, column, value);

	removeFileIfExist(file);

	memFree(file);
}

// ----

uint FC_GetCount(char *table)
{
	uint ret;

	errorCase(!table);

	DoLock();
	{
		char *dir = GetRecordDir(table);

		if(existDir(dir))
			ret = lsCount(dir);
		else
			ret = 0;

		memFree(dir);
	}
	DoUnlock();

	return ret;
}
autoList_t *FC_GetAllId(char *table)
{
	autoList_t *ret;

	errorCase(!table);

	DoLock();
	{
		char *dir = GetRecordDir(table);

		if(existDir(dir))
		{
			ret = lsFiles(dir);
			eraseParents(ret);
		}
		else
			ret = newList();

		memFree(dir);
	}
	DoUnlock();

	return ret;
}
char *FC_GetId(char *table, char *column, char *value) // ret: NULL == not found
{
	char *ret;

	errorCase(!table);
	errorCase(!column);
	errorCase(!value);

	DoLock();
	{
		char *dir  = GetLinkColumnDir(table, column);
		char *file = GetLinkFile(table, column, value);

		if(!existDir(dir))
			GenerateLinkColumn(table, column);

		if(existFile(file))
		{
			ret = readText_b(file);
			errorCase(!IsFairHash(ret));
		}
		else
			ret = NULL;

		memFree(dir);
		memFree(file);
	}
	DoUnlock();

	return ret;
}
char *FC_GetNewId(void)
{
	return MakeRandHexID();
}
char *FC_GetValue(char *table, char *nameOrId, char *column) // ret: NULL 無し, "" == レコード・カラムが存在しない。
{
	char *ret;

	errorCase(!table);
	errorCase(!nameOrId);
	errorCase(!column);

	DoLock();
	{
		LoadRecord(table, nameOrId);
		ret = GetValue(column);
	}
	DoUnlock();

	return ret;
}
void FC_SetValue(char *table, char *nameOrId, char *column, char *value)
{
	errorCase(!table);
	errorCase(!nameOrId);
	errorCase(!column);
	errorCase(!value);

	DoLock();
	{
		LoadRecord(table, nameOrId);

		// カラム・リンクがあれば更新する。
		{
			char *dir = GetLinkColumnDir(table, column);

			if(existDir(dir))
			{
				char *oldValue = GetValue(column);

				RemoveLinkColumnValue(table, column, oldValue);
				AddLinkColumnValue(table, column, value, nameOrId);

				memFree(oldValue);
			}
			memFree(dir);
		}

		SetValue(column, value);
		SaveRecord(table, nameOrId);
	}
	DoUnlock();
}
void FC_Delete(char *table, char *nameOrId)
{
	errorCase(!table);
	errorCase(!nameOrId);

	DoLock();
	{
		LoadRecord(table, nameOrId);

		// 全ての値のリンクを削除する。
		{
			char *column;
			uint index;

			foreach(Columns, column, index)
			{
				RemoveLinkColumnValue(table, column, getLine(Values, index));
			}
		}

		{
			char *file = GetRecordFile(table, nameOrId);
			removeFileIfExist(file);
			memFree(file);
		}
	}
	DoUnlock();
}
void FC_Truncate(char *table)
{
	errorCase(!table);

	DoLock();
	{
		char *dir = GetTableDir(table);
		forceRemoveDirIfExist(dir);
		memFree(dir);
	}
	DoUnlock();
}
