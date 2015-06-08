#include "C:\Factory\Common\Options\SockServerTh.h"
#include "C:\Factory\OpenSource\md5.h"
#include "..\MultiPart.h"

#define BORDERLINE "========================================"
#define HTTP_NEWLINE "\r\n"

static char *GetBody(char *file)
{
	autoBlock_t *body;
	char *retLine;

	if(70 < getFileSize(file))
	{
		FILE *fp = fileOpen(file, "rb");

		body = nobCreateBlock(30 + 5 + 30);

		{
			autoBlock_t start = gndSubBytes(body,      0, 30);
			autoBlock_t end   = gndSubBytes(body, 30 + 5, 30);

			fileRead(fp, &start);
			fileSeek(fp, SEEK_END, -30);
			fileRead(fp, &end);
		}
		memcpy((uchar *)directGetBuffer(body) + 30, " ... ", 5);

		fileClose(fp);
	}
	else
		body = readBinary(file);

	retLine = toPrintLine(body, 1);
	releaseAutoBlock(body);
	return retLine;
}
static void PerformTh(int sock, char *strip)
{
	SockStream_t *i = CreateSockStream(sock, 3600);
	char *header;
	autoList_t *parts;
	char *file;
	FILE *fp;
	httpPart_t *part;
	uint index;
	char *bodyHash;
	char *body;
	char *strw;

	parts = httpRecvRequestMultiPart(i, &header);

	file = makeTempPath("txt");
	fp = fileOpen(file, "wt");

	line2JLine(header, 1, 0, 0, 1);
	writeLine(fp, strw = xcout("Header: %s", header)); memFree(strw);
	writeLine(fp, BORDERLINE);

	foreach(parts, part, index)
	{
		bodyHash = md5_makeHexHashFile(part->BodyFile);
		body = GetBody(part->BodyFile);

		writeLine(fp, strw = xcout("Part: %u", index)); memFree(strw);
		writeLine(fp, strw = xcout("Name: %s", part->Name)); memFree(strw);
		writeLine(fp, strw = xcout("Local File: %s", part->LocalFile)); memFree(strw);
		writeLine(fp, strw = xcout("Body Size: %I64u", getFileSize(part->BodyFile))); memFree(strw);
		writeLine(fp, strw = xcout("Body Hash: %s", bodyHash)); memFree(strw);
		writeLine(fp, strw = xcout("Body: %s", body)); memFree(strw);
		writeLine(fp, BORDERLINE);

		memFree(bodyHash);
		memFree(body);
	}
	fileClose(fp);

	// http*()‚ÌŠJ•ú
	memFree(header);
	httpReleaseParts(parts);

	httpSendResponseFile(i, file);
	ReleaseSockStream(i);

	removeFile(file);
	memFree(file);
}
static int IdleTh(void)
{
	while(hasKey())
	{
		if(getKey() == 0x1b)
		{
			return 0;
		}
		cout("ESC to EXIT.\n");
	}
	return 1;
}
int main(int argc, char **argv)
{
	sockServerTh(PerformTh, hasArgs(1) ? toValue(nextArg()) : 80, 10, IdleTh);
}
