#include "C:\Factory\Common\Options\SockServerTh.h"
#include "..\GetPost.h"

#define BORDERLINE "========================================"
#define HTTP_NEWLINE "\r\n"

static void PerformTh(int sock, char *strip)
{
	SockStream_t *i = CreateSockStream(sock, 30);
	char *header;
	uchar *content;
	uint contentSize;
	httpDecode_t dec;
	char *line;
	uint index;
	autoBlock_t *body = newBlock();

	httpRecvRequest(i, &header, &content, &contentSize);

	{
		char *pHeader  = lineToPrintLine(header, 0);
		char *pContent = lineToPrintLine(content, 0);

		ab_addLine(body, "header: ");
		ab_addLine(body, pHeader);
		ab_addLine(body, HTTP_NEWLINE);

		ab_addLine_x(body, xcout("content size: %u" HTTP_NEWLINE, contentSize));
		ab_addLine(body, "content: ");
		ab_addLine(body, pContent);
		ab_addLine(body, HTTP_NEWLINE);

		memFree(pHeader);
		memFree(pContent);
	}

	httpDecode(header, content, &dec);

	dec.Url   = lineToPrintLine_x(dec.Url, 0);
	dec.Query = lineToPrintLine_x(dec.Query, 0);
	dec.Path  = lineToPrintLine_x(dec.Path, 0);
	line2JLine(dec.DecPath, 1, 0, 0, 1);
	foreach(dec.DirList, line, index) line2JLine(line, 0, 0, 0, 1);
	foreach(dec.Keys,    line, index) line2JLine(line, 0, 0, 0, 1);
	foreach(dec.Values,  line, index) line2JLine(line, 1, 1, 0, 1);

	ab_addLine_x(body, xcout("url: %s"      HTTP_NEWLINE, dec.Url));
	ab_addLine_x(body, xcout("query: %s"    HTTP_NEWLINE, dec.Query));
	ab_addLine_x(body, xcout("path: %s"     HTTP_NEWLINE, dec.Path));
	ab_addLine_x(body, xcout("dec-path: %s" HTTP_NEWLINE, dec.DecPath));
	ab_addLine(body, BORDERLINE HTTP_NEWLINE);

	foreach(dec.DirList, line, index)
	{
		ab_addLine_x(body, xcout("dir: %s" HTTP_NEWLINE, line));
	}
	ab_addLine(body, BORDERLINE HTTP_NEWLINE);

	foreach(dec.Keys, line, index)
	{
		ab_addLine_x(body, xcout("key: %s"   HTTP_NEWLINE, line));
		ab_addLine_x(body, xcout("value: %s" HTTP_NEWLINE, getLine(dec.Values, index)));
		ab_addLine(body, BORDERLINE HTTP_NEWLINE);
	}

	// http*()‚ÌŠJ•ú
	memFree(header);
	memFree(content);
	httpDecodeFree(&dec);

	httpSendResponse(i, body, "text/plain");

	ReleaseSockStream(i);
	releaseAutoBlock(body);
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
