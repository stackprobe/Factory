#include "DirToStream.h"

#define WRITER_BUFFSIZE (1024 * 1024 * 17)
#define READER_BUFFSIZE (1024 * 1024 * 16)

#define SIGN_DIR 'D'
#define SIGN_FILE 'F'
#define SIGN_ENDDIR 0x00

void VTreeToStream(VTree_t *vt, void (*streamWriter)(uchar *, uint))
{
	uint count = vt->GetLocalCount();
	uint index;
	uchar buffer[9];

	for(index = 0; index < count; index++)
	{
		char *file = vt->GetLocal(index);

		streamWriter(file, strlen(file) + 1);

		if(vt->IsDir(index))
		{
			buffer[0] = SIGN_DIR;
			streamWriter(buffer, 1);

			vt->IntoDir(index);
			VTreeToStream(vt, streamWriter);
			vt->ExitDir();
		}
		else
		{
			uint64 size = vt->GetSize(index);
			uint64 readPos;
			uint readSize;
			void *block = memAlloc(WRITER_BUFFSIZE);

			buffer[0] = SIGN_FILE;
			value64ToBlock(buffer + 1, size);
			streamWriter(buffer, 9);

			for(readPos = 0; readPos < size; readPos += readSize)
			{
				readSize = m_min(WRITER_BUFFSIZE, (uint)(size - readPos));
				vt->GetEntity(index, readPos, readSize, block);
				streamWriter(block, readSize);
			}
			memFree(block);
		}
		memFree(file);
	}
	buffer[0] = SIGN_ENDDIR;
	streamWriter(buffer, 1);
}
void DirToStream(char *dir, void (*streamWriter)(uchar *, uint))
{
	autoList_t *pathsStack = newList();
	autoList_t *paths;

enterDir:
	addCwd(dir);
	paths = ls(".");
	eraseParents(paths);
	rapidSortLines(paths);

	for(; ; )
	{
		uchar buffer[9];

		while(getCount(paths))
		{
			char *path = (char *)unaddElement(paths);
			FILE *fp;
			autoBlock_t *block;

			streamWriter(path, strlen(path) + 1);

			if(existDir(path))
			{
				buffer[0] = SIGN_DIR;
				streamWriter(buffer, 1);

				dir = path;
				addElement(pathsStack, (uint)paths);
				goto enterDir;
			}
			buffer[0] = SIGN_FILE;
			value64ToBlock(buffer + 1, getFileSize(path));
			streamWriter(buffer, 9);

			fp = fileOpen(path, "rb");

			while(block = readBinaryStream(fp, WRITER_BUFFSIZE))
			{
				streamWriter(directGetBuffer(block), getSize(block));
				releaseAutoBlock(block);
			}
			fileClose(fp);

			memFree(path);
		}
		releaseAutoList(paths);

		buffer[0] = SIGN_ENDDIR;
		streamWriter(buffer, 1);

		if(!getCount(pathsStack))
		{
			break;
		}
		paths = (autoList_t *)unaddElement(pathsStack);
		unaddCwd();
	}
	releaseAutoList(pathsStack);
	unaddCwd();
}

int STD_TrustMode;
int STD_ReadStop;

static void (*STD_StreamReader)(uchar *, uint);

static void STD_ReadStream(uchar *block, uint size)
{
	if(!STD_ReadStop)
	{
		STD_StreamReader(block, size);

		if(!STD_ReadStop)
		{
			return;
		}
	}
	memset(block, 0x00, size);
}

/*
	dir - 出力先、存在する空のディレクトリであること。

	streamReader(uchar *block, uint size)
		次の size バイトを block に書き込む。
		要求されたサイズのデータは必ず返さなければならない。
		ユーザーから中断の要求があったり、これ以上データを用意出来なくなるなど、中止したい場合は STD_ReadStop に !0 をセットする。
*/
void StreamToDir(char *dir, void (*streamReader)(uchar *, uint))
{
	uint dirDepth = 0;

	STD_ReadStop = 0;
	STD_StreamReader = streamReader;

	addCwd(dir);

	for(; ; )
	{
		char *path = strx("");
		uchar buffer[8];

		for(; ; )
		{
			STD_ReadStream(buffer, 1);

			if(buffer[0] == '\0')
				break;

			path = addChar(path, buffer[0]);
		}

		if(path[0] == '\0') // ? == SIGN_ENDDIR
		{
			unaddCwd();

			if(!dirDepth)
			{
				memFree(path);
				break;
			}
			dirDepth--;
		}
		else
		{
			if(STD_TrustMode)
			{
				/*
					STD_TrustMode != 0 であればどんな名前もウェルカムだが、
					フルパスや相対パスも書け、システムフォルダとか壊すような記述も出来るわけで、
					流石にそれは怖いので、せめてそれらは弾く。
				*/
				errorCase(strchr(path, ':'));
				errorCase(mbs_strchr(path, '\\'));
			}
			else
			{
#if 1
				char *newPath = lineToFairLocalPath(path, strlen_x(getCwd()));

				if(strcmp(newPath, path))
				{
					line2JLine(path, 1, 0, 0, 1);

					cout("################################\n");
					cout("## ローカル名は矯正されました ##\n");
					cout("################################\n");
					cout("< %s\n", path);
					cout("> %s\n", newPath);
				}
				memFree(path);
				path = newPath;
#else // OLD CODE
				path = lineToFairLocalPath_x(path, strlen_x(getCwd()));
#endif
			}
			STD_ReadStream(buffer, 1);

			if(buffer[0] == SIGN_DIR)
			{
				createDir(path);
				addCwd(path);
				dirDepth++;
			}
			else if(buffer[0] == SIGN_FILE)
			{
				uint64 count;
				FILE *fp;

				STD_ReadStream(buffer, 8);
				count = blockToValue64(buffer);

				fp = fileOpen(path, "wb");

				while(0i64 < count && !STD_ReadStop)
				{
					autoBlock_t *block = nobCreateBlock(count < (uint64)READER_BUFFSIZE ? (uint)count : READER_BUFFSIZE);

					STD_ReadStream(directGetBuffer(block), getSize(block));
					writeBinaryBlock(fp, block);
					count -= getSize(block);

					releaseAutoBlock(block);
				}
				fileClose(fp);
			}
			else
				error();
		}
		memFree(path);
	}
}
