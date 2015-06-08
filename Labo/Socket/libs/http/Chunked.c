#include "Chunked.h"

#define RECVSIZE_ONCE_MAX (1024 * 1024 * 16)

httpChunkedRecver_t *httpCreateChunkedRecver(SockStream_t *ss)
{
	httpChunkedRecver_t *i = (httpChunkedRecver_t *)memAlloc(sizeof(httpChunkedRecver_t));

	i->Stream = ss;
	i->RemSize = 0;

	return i;
}
void httpReleaseChunkedRecver(httpChunkedRecver_t *i)
{
	memFree(i);
}
autoBlock_t *httpRecvChunked(httpChunkedRecver_t *i) // ret == NULL: I—¹
{
	uint rSize;
	void *rBlock;

	errorCase(!i);
	errorCase(!i->Stream); // ? Šù‚ÉI—¹

	if(!i->RemSize)
	{
		char *line = SockRecvLine(i->Stream, 30); // Å‘å‚W•¶Žš‚¾‚¯‚ÇA‰üs‚Æ‚©‚ ‚é‚µƒMƒŠƒMƒŠ‚É‚·‚é•K—v‚à‚È‚¢‚©‚È‚ÆB

		strchrEnd(line, ';')[0] = '\0'; // chunk-extension ‚Ì”rœ

		i->RemSize = toValueDigits(line, hexadecimal);
		memFree(line);

		if(!i->RemSize)
		{
			i->Stream = NULL;
			return NULL;
		}
	}
	rSize = m_min(RECVSIZE_ONCE_MAX, i->RemSize);
	rBlock = memAlloc(rSize);

	SockRecvBlock(i->Stream, rBlock, rSize);
	i->RemSize -= rSize;

	if(!i->RemSize)
	{
		// CR-LF
		SockRecvChar(i->Stream);
		SockRecvChar(i->Stream);
	}
	return bindBlock(rBlock, rSize);
}
