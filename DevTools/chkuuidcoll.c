/*
	chkuuidcoll.exe ディレクトリ ...

	機能：UUIDの重複をチェックする。

		大文字小文字だけ違うUUID -> 違うと見なす。
*/

#include "C:\Factory\Common\all.h"

#define UUID_FMT "{99999999-9999-9999-9999-999999999999}"
#define SHARED_PTN "shared_uuid"
#define NUM_JOINT_CHR ':'

typedef struct UUID_st
{
	char *File;
	uint LineNo;
	char *UUID;
	int SharedFlag;
	uint SharedNum; // 0 == 個数指定無し
}
UUID_t;

static autoList_t *UUIDList;

static void LineFltr(char *line)
{
	char *p;

	for(p = line; *p; p++)
	{
		if(isMbc(p))
		{
			*p++ = 'K';
			*p = 'K';
		}
		else if(m_ishexadecimal(*p))
		{
			*p = '9';
		}
	}
}
static void CollectUUIDByFile(char *file)
{
	autoList_t *lines = readLines(file);
	char *line;
	uint index;

	foreach(lines, line, index)
	{
		char *fmt = strx(line);
		char *p;

		LineFltr(fmt);

		for(p = fmt; p = strstr(p, UUID_FMT); p += strlen(UUID_FMT))
		{
			char *uuidP = line + ((uint)p - (uint)fmt);

			if(!startsWith(uuidP, UUID_FMT)) // ? not UUID_FMT
			{
				UUID_t *i = nb(UUID_t);
				char *p;

				i->File = strx(file);
				i->LineNo = index + 1;
				i->UUID = strxl(uuidP, strlen(UUID_FMT));
				i->SharedFlag = (int)(p = mbs_strstr(uuidP + strlen(UUID_FMT), SHARED_PTN));
//				i->SharedNum = 0;

				if(p)
				{
					p += strlen(SHARED_PTN);

					if(*p == NUM_JOINT_CHR) // 個数指定
					{
						char *q;

						p++;

						for(q = p; *q; q++)
							if(!m_isdecimal(*q))
								break;

						errorCase(q == p); // 個数が記述されていない。

						i->SharedNum = toValue_x(strxl(p, (uint)q - (uint)p));

						errorCase(i->SharedNum < 2); // 意味のない個数
					}
				}
				addElement(UUIDList, (uint)i);
			}
		}
		memFree(fmt);
	}
	releaseDim(lines, 1);
}
static int IsTargetFile(char *file)
{
	char *ext = getExt(file);

	return
		!_stricmp(ext, "c") ||
		!_stricmp(ext, "h") ||
		!_stricmp(ext, "cs") ||
		!_stricmp(ext, "cpp") ||
		!_stricmp(ext, "java");
}
static void CollectUUIDByPath(char *path)
{
	if(existDir(path))
	{
		autoList_t *files = lssFiles(path);
		char *file;
		uint index;

		foreach(files, file, index)
			if(IsTargetFile(file))
				CollectUUIDByFile(file);

		releaseDim(files, 1);
	}
	else
		CollectUUIDByFile(path);
}
static void CollectUUID(void)
{
	UUIDList = newList();

	if(hasArgs(1))
	{
		while(hasArgs(1))
		{
			char *path = makeFullPath(nextArg());

			CollectUUIDByPath(path);
			memFree(path);
		}
	}
	else
		CollectUUIDByPath(c_dropDirFile());
}

// ---- DispAllUUID ----

static sint CompUUIDByFile(uint i, uint j)
{
	return mbs_stricmp(((UUID_t *)i)->File, ((UUID_t *)j)->File);
}
static sint CompUUIDByUUID(uint i, uint j)
{
	sint ret = _stricmp(((UUID_t *)i)->UUID, ((UUID_t *)j)->UUID);

	if(ret)
		return ret;

	return mbs_stricmp(((UUID_t *)i)->File, ((UUID_t *)j)->File);
}
static void DispAllUUID_2(char *title)
{
	UUID_t *i;
	uint index;

	cout("---- %s\n", title);

	foreach(UUIDList, i, index)
	{
		char *sflg;

		if(!i->SharedFlag)
		{
			sflg = strx("-");
		}
		else if(!i->SharedNum)
		{
			sflg = strx("S");
		}
		else
		{
			sflg = xcout("%u", i->SharedNum);
		}

		cout("%s [%s] %s (%u)\n", i->UUID, sflg, i->File, i->LineNo);

		memFree(sflg);
	}
}
static void DispAllUUID()
{
	rapidSort(UUIDList, CompUUIDByFile);
	DispAllUUID_2("ASC FILE");
	rapidSort(UUIDList, CompUUIDByUUID);
	DispAllUUID_2("ASC UUID");
}

// ----

static uint GetUUIDCount(char *target)
{
	UUID_t *i;
	uint index;
	uint count = 0;

	foreach(UUIDList, i, index)
		if(!strcmp(i->UUID, target)) // 大文字小文字だけ違うUUID -> 違うと見なす。
			count++;

	return count;
}
static void DispAllWarning(void)
{
	UUID_t *i;
	uint index;

	foreach(UUIDList, i, index)
	{
		if(i->SharedFlag && GetUUIDCount(i->UUID) < 2)
		{
			cout("----\n");
			cout("共有指定されているけど共有されていない。\n");
			cout("%s (%u)\n", i->File, i->LineNo);
		}
	}
}
static void DispAllSharedNumError(void)
{
	UUID_t *i;
	uint index;

	foreach(UUIDList, i, index)
	{
		if(i->SharedFlag && i->SharedNum && i->SharedNum != GetUUIDCount(i->UUID))
		{
			cout("----\n");
			cout("【警告】個数が合わない。\n");
			cout("指定の個数＝%u\n", i->SharedNum);
			cout("実際の個数＝%u\n", GetUUIDCount(i->UUID));
			cout("%s (%u)\n", i->File, i->LineNo);
		}
	}
}
static void DispAllCollision(void)
{
	UUID_t *i;
	uint index;

	foreach(UUIDList, i, index)
	{
		if(!i->SharedFlag && 1 < GetUUIDCount(i->UUID))
		{
			cout("----\n");
			cout("【警告】重複している。\n");
			cout("%s (%u)\n", i->File, i->LineNo);
		}
	}
}

int main(int argc, char **argv)
{
	antiSubversion = 1;

	CollectUUID();
	DispAllUUID();
	DispAllWarning();
	DispAllSharedNumError();
	DispAllCollision();
}
