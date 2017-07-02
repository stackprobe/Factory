#include "nSyncCommon.h"

void NS_DeletePath(char *path)
{
	cout("- %s\n", path);

	recurRemovePathIfExist(path);
}
void NS_CreateParent(char *path)
{
	cout("+ %s\n", path);

	recurRemovePathIfExist(path);
	createPath(path, 'X');
}
void NS_SendFile(SockStream_t *ss, char *file)
{
	uint64 createStamp;
	uint64 writeStamp;
	uint64 fileSize;
	uint64 count;
	FILE *fp;

	cout("SEND-FILE\n");

	getFileStamp(file, &createStamp, NULL, &writeStamp);
	fileSize = getFileSize(file);

	SockSendValue64(ss, createStamp);
	SockSendValue64(ss, writeStamp);
	SockSendValue64(ss, fileSize);

	SockSendChar(ss, 'A'); // Alive

	fp = fileOpen(file, "rb");

	for(count = 0; ; )
	{
		autoBlock_t *buff = readBinaryStream(fp, 20000000); // 20 MB

		if(!buff)
			break;

		count = getSeekPos(fp);
		cmdTitle_x(xcout("nSync:Send:%I64u/%I64u=%.3fPct", count, fileSize, count * 100.0 / fileSize));

		SockSendBlock(ss, directGetBuffer(buff), getSize(buff));
		releaseAutoBlock(buff);
	}
	fileClose(fp);

	SockFlush(ss);

	cmdTitle("nSync:Send:Ok");
	cout("SEND-FILE-OK\n");
}
void NS_RecvFile(SockStream_t *ss, char *file)
{
	char *midFile = makeTempPath(NULL);
	uint64 createStamp;
	uint64 writeStamp;
	uint64 fileSize;
	uint64 count;
	uint64 repSpan;
	FILE *fp;

	cout("RECV-FILE\n");

	createStamp = SockRecvValue64(ss);
	writeStamp  = SockRecvValue64(ss);
	fileSize    = SockRecvValue64(ss);

	if(SockRecvChar(ss) != 'A') // Alive
	{
		cout("Ç±ÇÃê⁄ë±ÇÕéÄÇÒÇ≈Ç‹Ç∑ÇÌÅB\n");
		goto endFunc;
	}

	fp = fileOpen(midFile, "wb");

	repSpan = fileSize / 10;
	m_minim(repSpan, 30000000); // 30 MB

	for(count = 0; count < fileSize; count++)
	{
		if(count % repSpan == 0)
			cmdTitle_x(xcout("nSync:Recv:%I64u/%I64u=%.3fPct", count, fileSize, count * 100.0 / fileSize));

		writeChar(fp, SockRecvChar(ss));
	}
	fileClose(fp);

	setFileStamp(midFile, createStamp, 0, writeStamp);
	NS_CreateParent(file);
	moveFile(midFile, file);

endFunc:
	memFree(midFile);

	cmdTitle("nSync:Recv:Ok");
	cout("RECV-FILE-OK\n");
}
