/*
	InstagramDL.exe アカウント名 出力先DIR
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"
#include "C:\Factory\Common\Options\TimeData.h"

// ---- known url ----

#define KNOWN_URL_FILE "C:\\appdata\\instagram-dl\\known-url.txt"

static char *GetKnownUrl(void)
{
	return existFile(KNOWN_URL_FILE) ? readText_b(KNOWN_URL_FILE) : strx("URL-DUMMY"); // dummy
}
static void SetKnownUrl(char *url)
{
	if(!existFile(KNOWN_URL_FILE))
		createPath(KNOWN_URL_FILE, 'X');

	writeOneLineNoRet_b(KNOWN_URL_FILE, url);
}

// ----

static char *GetCurrImgLocal(void)
{
	uint64 stamp = GetNowStamp();

	return xcout("IMG_%08u_%06u.jpg", (uint)(stamp / 1000000ui64), (uint)(stamp % 1000000ui64));
}

static char *Account;
static char *DestDir;

static char *HGetExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\Kit\\HGet\\HGet.exe");

	return file;
}
static autoList_t *ParseUrls(char *resBodyFile)
{
	char *resBody = readText_b(resBodyFile);
	autoList_t *urls = newList();
	char *url;
	uint index;
	char *p;

	p = resBody;

	for(; ; )
	{
		char *q = strstr(p, "thumbnail_src");
		char *r;
		char *s;

		if(!q)
			break;

		r = strstr(q + 13, "http");

		if(!r)
			break;

		s = strchr(r + 4, '"');

		if(!s)
			break;

		*s = '\0';
		addElement(urls, (uint)strx(r));
		p = s + 1;
	}
	memFree(resBody);
	return urls;
}
static void Downloaded(autoBlock_t *imageData)
{
	char *imgLocal = GetCurrImgLocal();
//	char *imgLocal = GetLastImgLocal();
	char *imgFile;

	imgFile = combine(DestDir, imgLocal);
	imgFile = toCreatablePath(imgFile, IMAX);
	memFree(imgLocal);
	imgLocal = strx(getLocal(imgFile));

	cout("< %u BYTES\n", getSize(imageData));
	cout("> %s\n", imgFile);

	writeBinary(imgFile, imageData);

//	SetLastImgLocal(imgLocal);

	memFree(imgLocal);
	memFree(imgFile);
}
static int Download(char *url) // ret: ? successful
{
	char *successfulFlag = makeTempPath(NULL);
	char *resHeaderFile = makeTempPath(NULL);
	char *resBodyFile = makeTempPath(NULL);
	char *prmFile = makeTempPath(NULL);
	int ret = 0;

	url = strx(url);
	url = replaceLine(url, "/s640x640/", "/", 0);

	writeOneLineNoRet_b_cx(prmFile, xcout(
		"/RSF\n"
		"%s\n"
		"/RHF\n"
		"%s\n"
		"/RBF\n"
		"%s\n"
		"/-\n"
		"%s"
		,successfulFlag
		,resHeaderFile
		,resBodyFile
		,url
		));

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" //R \"%s\"", HGetExeFile(), prmFile));

	if(existFile(successfulFlag))
	{
		autoBlock_t *imageData = readBinary(resBodyFile);

		Downloaded(imageData);
		releaseAutoBlock(imageData);
		ret = 1;
	}
	removeFileIfExist(successfulFlag);
	removeFileIfExist(resHeaderFile);
	removeFileIfExist(resBodyFile);
	removeFileIfExist(prmFile);

	memFree(successfulFlag);
	memFree(resHeaderFile);
	memFree(resBodyFile);
	memFree(prmFile);
	memFree(url);

	return ret;
}
int main(int argc, char **argv)
{
	char *successfulFlag = makeTempPath(NULL);
	char *resHeaderFile = makeTempPath(NULL);
	char *resBodyFile = makeTempPath(NULL);

	Account = strx(nextArg());
	line2csym(Account);
	DestDir = makeFullPath(nextArg());

	cout("アカウント: %s\n", Account);
	cout("追加出力先: %s\n", DestDir);

	errorCase(!existDir(DestDir));

	coExecute_x(xcout(
		"START \"\" /B /WAIT \"%s\" /RSF \"%s\" /RHF \"%s\" /RBF \"%s\" https://www.instagram.com/%s/"
		,HGetExeFile()
		,successfulFlag
		,resHeaderFile
		,resBodyFile
		,Account
		));

	if(existFile(successfulFlag))
	{
		autoList_t *urls = ParseUrls(resBodyFile);
		char *url;
		uint index;
		char *knownUrl = GetKnownUrl();

		LOGPOS();

		foreach(urls, url, index)
			if(!strcmp(url, knownUrl))
				break;

		memFree(knownUrl);

		if(index)
		{
			LOGPOS();

			while(index)
			{
				LOGPOS();

				index--;
				url = getLine(urls, index);

				if(!Download(url))
					break;

				SetKnownUrl(url);
			}
		}
		releaseDim(urls, 1);
	}
	removeFileIfExist(successfulFlag);
	removeFileIfExist(resHeaderFile);
	removeFileIfExist(resBodyFile);

	memFree(successfulFlag);
	memFree(resHeaderFile);
	memFree(resBodyFile);
}
