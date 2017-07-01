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
	FILE *fp;

	cout("SEND-FILE\n");

	getFileStamp(file, &createStamp, NULL, &writeStamp);

	errorCase(!m_isRange(createStamp, STAMP_MIN, STAMP_MAX));
	errorCase(!m_isRange(writeStamp,  STAMP_MIN, STAMP_MAX));

	SockSendValue64(ss, createStamp);
	SockSendValue64(ss, writeStamp);
	SockSendValue64(ss, getFileSize(file));

	SockSendChar(ss, 'A'); // Alive

	fp = fileOpen(file, "rb");

	for(; ; )
	{
		autoBlock_t *buff = readBinaryStream(fp, 20000000); // 20 MB

		if(!buff)
			break;

		SockSendBlock(ss, directGetBuffer(buff), getSize(buff));
		releaseAutoBlock(buff);
	}
	fileClose(fp);

	SockFlush(ss);

	cout("SEND-FILE-OK\n");
}
void NS_RecvFile(SockStream_t *ss, char *file)
{
	char *midFile = makeTempPath(NULL);
	uint64 createStamp;
	uint64 writeStamp;
	uint64 fileSize;
	uint64 count;
	FILE *fp;

	cout("RECV-FILE\n");

	createStamp = SockRecvValue64(ss);
	writeStamp  = SockRecvValue64(ss);
	fileSize    = SockRecvValue64(ss);

	if(SockRecvChar(ss) != 'A') // Alive
		goto endFunc;

	errorCase(!m_isRange(createStamp, STAMP_MIN, STAMP_MAX));
	errorCase(!m_isRange(writeStamp,  STAMP_MIN, STAMP_MAX));
	errorCase(FILE_SIZE_MAX < fileSize);

	fp = fileOpen(midFile, "wb");

	for(count = 0; count < fileSize; count++)
		writeChar(fp, SockRecvChar(ss));

	fileClose(fp);

	NS_CreateParent(file);
	moveFile(midFile, file);
	setFileStamp(file, createStamp, 0, writeStamp);

endFunc:
	memFree(midFile);

	cout("RECV-FILE-OK\n");
}
