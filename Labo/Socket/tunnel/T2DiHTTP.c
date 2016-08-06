#include "C:\Factory\Common\all.h"

static uint64 *MkUI64(uint64 value)
{
	uint64 *p = (uint64 *)memAlloc(sizeof(uint64));

	*p = value;
	return p;
}
static char *MkTmpFile(autoBlock_t *fileData)
{
	char *file = makeTempPath(NULL);

	writeBinary(file, fileData);
	return file;
}

// ---- ReadLine ----

static uint64 Stamp;
static uint Id;
static int Kind;
static autoBlock_t *RawData;

static int ReadLine(FILE *rfp)
{
	char *line = readLine(rfp);
	char *p;

	if(!line)
		return 0;

	errorCase(!lineExp("<17,09>,<4,10,09>,<1,CDRS>,\"<>\"", line));

	p = line + 17;
	p[0] = '\0';
	Stamp = toValue64(line);
	p[0] = ',';

	p = strchr(line + 22, ',');
	p[0] = '\0';
	Id = toValue(line + 18);
	p[0] = ',';

	Kind = p[1];

	if(RawData)
		releaseAutoBlock(RawData);

	RawData = newBlock();

	for(p += 4; *p != '"'; p++)
	{
		int chr;

		errorCase(!*p);

		if(*p == '\\' && p[1] && p[2])
		{
			int v1 = m_c2i(p[1]);
			int v2 = m_c2i(p[2]);

			chr = (v1 << 4) | v2;

			p += 2;
		}
		else
			chr = *p;

		addByte(RawData, chr);
	}
	return 1;
}

// ---- LoadFile ----

static autoList_t *SendStamps; // { uint64 *stamp, ... }
static autoList_t *SendFiles;
static autoList_t *RecvStamps; // { uint64 *stamp, ... }
static autoList_t *RecvFiles;
static uint ConId;

static void UnloadFile(void)
{
	char *file;
	uint index;

	if(!SendStamps) // ? ! loaded
		return;

	foreach(SendFiles, file, index)
		removeFile(file);

	foreach(RecvFiles, file, index)
		removeFile(file);

	releaseDim(SendStamps, 1);
	releaseDim(SendFiles, 1);
	releaseDim(RecvStamps, 1);
	releaseDim(RecvFiles, 1);
}
static void LoadFile(char *rFile)
{
	FILE *rfp = fileOpen(rFile, "rt");

	UnloadFile();

	SendStamps = newList();
	SendFiles = newList();
	RecvStamps = newList();
	RecvFiles = newList();
	ConId = 0;

	while(ReadLine(rfp))
	{
		errorCase(Id < 1000);

		if(ConId)
			errorCase(ConId != Id);
		else
			ConId = Id;

		switch(Kind)
		{
		case 'S':
			addElement(SendStamps, (uint)MkUI64(Stamp));
			addElement(SendFiles, (uint)MkTmpFile(RawData));
			break;

		case 'R':
			addElement(RecvStamps, (uint)MkUI64(Stamp));
			addElement(RecvFiles, (uint)MkTmpFile(RawData));
			break;

		case 'C':
		case 'D':
			break;

		default:
			error();
		}
	}
	fileClose(rfp);
}

// ---- PraseHTTPReqRes ----

static void ParseHTTPReqRes(void)
{
	// TODO
}

// ----

static void ProcFile(char *rFile)
{
	LoadFile(rFile);
	ParseHTTPReqRes();
}
static void T2DiHTTP(char *dir)
{
	autoList_t *files = lsFiles(dir);
	char *file;
	uint index;

	foreach(files, file, index)
	{
		ProcFile(file);
	}
	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
	T2DiHTTP(hasArgs(1) ? nextArg() : c_dropDir());

	UnloadFile();
}
