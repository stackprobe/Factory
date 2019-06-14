#include "C:\Factory\Common\Options\SockServer.h"
#include "C:\Factory\Common\Options\SockStream.h"
#include "gDefine.h"

#define ROOTDIR "C:\\appdata\\gLanes"
#define LANE_NUM_LMT 1000
#define FILE_NUM_LMT 1000
#define TOTAL_SIZE_LMT 30000000000ui64 // 30 gb
#define BUFFSIZE 4000000

static uchar Buff[BUFFSIZE];

static uint64 GetTotalSize(void)
{
	autoList_t *files;
	char *file;
	uint index;
	uint64 totalSize = 0ui64;

	LOGPOS();
	files = lssFiles(ROOTDIR);
	LOGPOS();

	foreach(files, file, index)
		totalSize += getFileSize(file);

	LOGPOS();
	releaseDim(files, 1);
	cout("totalSize: %I64u\n", totalSize);
	return totalSize;
}
static void Upload(SockStream_t *ss, char *laneDir)
{
	uint64 totalSize = GetTotalSize();
	uint64 fileSize;
	char *name;
	char *file;
	FILE *fp;
	autoBlock_t gab;

	LOGPOS();

	if(LANE_NUM_LMT < lsCount(ROOTDIR))
	{
		cout("���������[������������I\n");
		SockSendLine(ss, "NG");
		return;
	}
	LOGPOS();

	for(; ; )
	{
		LOGPOS();
		SockSendLine(ss, "READY.1");

		if(!SockRecvValue(ss))
		{
			cout("�A�b�v���[�h�I��\n");
			SockSendLine(ss, "OK");
			break;
		}
		fileSize = SockRecvValue64(ss);
		cout("fileSize: %I64u\n", fileSize);

		if(FILE_NUM_LMT < lsCount(laneDir))
		{
			cout("�������t�@�C������������I\n");
			SockSendLine(ss, "NG");
			break;
		}
		if(TOTAL_SIZE_LMT < fileSize)
		{
			cout("�������t�@�C�����傫������I\n");
			SockSendLine(ss, "NG");
			break;
		}
		if(TOTAL_SIZE_LMT - totalSize < fileSize)
		{
			cout("���������t�@�C���T�C�Y���傫������I\n");
			SockSendLine(ss, "NG");
			break;
		}
		totalSize += fileSize;
		SockSendLine(ss, "READY.2");

		name = SockRecvLine(ss, 100);
		name = lineToFairLocalPath_x(name, strlen(laneDir));
		cout("name: %s\n", name);
		file = combine(laneDir, name);
		cout("file: %s\n", file);
		fp = fileOpen(file, "wb");
		LOGPOS();

		while(0ui64 < fileSize)
		{
			uint recvSize = (uint)m_min((uint64)BUFFSIZE, fileSize);

			if(!SockRecvBlock(ss, Buff, recvSize))
			{
				cout("�������t�@�C����M�G���[�I\n");
				break; // ���� SockRecvValue64(); �� 0 ��ǂނ͂��B
			}
			writeBinaryBlock(fp, gndBlockVar(Buff, recvSize, gab));
			fileSize -= recvSize;
		}
		LOGPOS();
		fileClose(fp);
		memFree(file);
		memFree(name);
		LOGPOS();
		SockSendLine(ss, "READY.3");
	}
	LOGPOS();
}
static void Download(SockStream_t *ss, char *laneDir)
{
	autoList_t *files;
	char *file;
	uint index;

	LOGPOS();
	files = lsFiles(laneDir);
	LOGPOS();
	sortJLinesICase(files);
	LOGPOS();

	foreach(files, file, index)
	{
		uint64 fileSize = getFileSize(file);
		FILE *fp;

		LOGPOS();
		SockSendValue64(ss, fileSize);
		LOGPOS();
		SockSendLine(ss, getLocal(file));
		LOGPOS();
		fp = fileOpen(file, "rb");

		while(0ui64 < fileSize)
		{
			uint sendSize = (uint)m_min((uint64)BUFFSIZE, fileSize);
			autoBlock_t gab;

			fileRead(fp, gndBlockVar(Buff, sendSize, gab));
			SockSendBlock(ss, Buff, sendSize);
			fileSize -= sendSize;
		}
		fileClose(fp);
		LOGPOS();

		if(SockRecvChar(ss) != 'C')
		{
			cout("�������t�@�C�����M�G���[�I\n");
			break;
		}
		LOGPOS();
		SockSendChar(ss, 'D');
		SockFlush(ss);
		LOGPOS();
		removeFile(file);
		LOGPOS();
	}
	LOGPOS();
	releaseDim(files, 1);
	LOGPOS();
}
static int Perform(int sock, void *dummyPrm)
{
	SockStream_t *ss = CreateSockStream(sock, 30);
	char *command;
	char *lane;
	char *laneDir;

	command = SockRecvLine(ss, 100);
	line2JLine(command, 1, 0, 0, 0);
	cout("command: %s\n", command);

	lane = SockRecvLine(ss, 100);
	lane = lineToFairLocalPath_x(lane, strlen(ROOTDIR));
	cout("lane: %s\n", lane);

	SetSockStreamTimeout(ss, 3600); // �t�@�C������M������̂ŁA���߂�

	laneDir = combine(ROOTDIR, lane);
	cout("laneDir: %s\n", laneDir);
	createDirIfNotExist(laneDir);
	LOGPOS();

	if(!strcmp(command, COMMAND_PREFIX "u"))
	{
		Upload(ss, laneDir);
	}
	else if(!strcmp(command, COMMAND_PREFIX "d"))
	{
		Download(ss, laneDir);
	}
	else
	{
		cout("�s���ȃR�}���h\n");
	}
	LOGPOS();
	memFree(command);
	memFree(lane);
	memFree(laneDir);
	ReleaseSockStream(ss);
	return 0;
}
static int Idle(void)
{
	while(hasKey())
		if(getKey() == 0x1b)
			return 0;

	return 1;
}
int main(int argc, char **argv)
{
	LOGPOS();
	mkAppDataDir();
	LOGPOS();
	createDirIfNotExist(ROOTDIR);

	LOGPOS();
	sockServerUserTransmit(Perform, (void *(*)(void))getZero, (void (*)(void *))noop_u, PORTNO, 1, Idle);
	LOGPOS();
}
