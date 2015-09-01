/*
	zip.exe /P ZIP-FILE SRC-DIR [BASE-NAME]

		SRC-DIR -> ZIP-FILE

	zip.exe /R ZIP-FILE [BASE-NAME]

		ZIP-FILE -> ZIP-FILE

	zip.exe /RB ZIP-FILE

		ZIP-FILE -> ZIP-FILE

	zip.exe /RBD ROOT-DIR

		{ ZIP-FILE -> ZIP-FILE } @ under ROOT-DIR

	zip.exe /O OUT-DIR PROJ-NAME

		OUT-DIR -> OUT-DIR \\ { PROJ-NAME } .zip

	zip.exe /G OUT-DIR PROJ-NAME

		OUT-DIR -> OUT-DIR \\ { PROJ-NAME } _v123.zip
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\CRandom.h"

/*
	7-Zip Command line version 9.20
	空白を含まないパスであること。
*/
#define ZIP7_LOCAL_FILE "7za.exe"
#define ZIP7_FILE "C:\\app\\7za920\\" ZIP7_LOCAL_FILE

#define VER_BETA 1100000000

static char *GetZip7File(void)
{
	static char *file;

	if(!file)
	{
		file = ZIP7_LOCAL_FILE;

		if(!existFile(file))
		{
			file = ZIP7_FILE;
			errorCase_m(!existFile(file), "[" ZIP7_FILE "]が見つかりません。");
		}
	}
	return file;
}
static char *IntoIfOneDir(char *dir)
{
	cout("dir1: %s\n", dir);

	for(; ; )
	{
		autoList_t *paths = ls(dir);

		if(getCount(paths) != 1 || !existDir(getLine(paths, 0)))
		{
			releaseDim(paths, 1);
			break;
		}
		dir = strz(dir, getLine(paths, 0));
		releaseDim(paths, 1);
	}
	cout("dir2: %s\n", dir);
	return dir;
}
static void ExtractZipFile(char *zipFile, char *destDir)
{
	coExecute_x(xcout("%s x \"%s\" -o\"%s\"", GetZip7File(), zipFile, destDir));
}
static void PackZipFile(char *zipFile, char *srcDir)
{
	removeFileIfExist(zipFile);

	addCwd(srcDir);
	coExecute_x(xcout("%s a \"%s\" *", GetZip7File(), zipFile));
	unaddCwd();
}
static uint InputVersion(void) // ret: 0 == canel, 1 ～ 999 == "0.01" ～ "9.99", VER_BETA == BETA
{
	char *sVersion;
	uint version;

	cout("######################################################################\n");
	cout("## バージョン番号を入力して下さい。[1-999] as 0.01-9.99, [] as BETA ##\n");
	cout("######################################################################\n");

	sVersion = coInputLine();

	if(*sVersion)
	{
		version = toValue(sVersion);
		m_range(version, 0, 999);
	}
	else
		version = coil_esc ? 0 : VER_BETA;

	memFree(sVersion);
	return version;
}
static uint FindStringInExe(autoBlock_t *text, char *ptnString) // ret: UINTMAX == not found
{
	uint index;
	uint rPos = 0;

	for(index = 0; index < getSize(text); index++)
	{
	recheck:
		if(getByte(text, index) == ptnString[rPos]) // 最後の '\0' まで一致するか判定する。
		{
			if(ptnString[rPos] == '\0') // ? found
				return index - rPos;

			rPos++;
		}
		else if(rPos)
		{
			rPos = 0;
			goto recheck;
		}
	}
	return UINTMAX; // not found
}
static void DestroyFindVersionPtn(autoBlock_t *text, uint startPos, uint count)
{
	uint index;

	for(index = 0; index < count; index++)
	{
		int chr;

#if 1
		do
		{
			chr = '\x21' + getCryptoByte() % ('\x7e' + 1 - '\x21');
		}
		while(chr == ':');
#else
		chr = ';'; // ':' でなければ何でも良い。
#endif

		setByte(text, startPos + index, chr);
	}
}
static void ReplaceVersion(char *dir, uint version) // version: 1 ～ 999, VER_BETA
{
	autoList_t *files = lssFiles(dir);
	char *file;
	uint index;
	char *sVersion;

	if(version == VER_BETA)
		sVersion = strx("BETA");
	else
		sVersion = xcout("%u.%02u", version / 100, version % 100);

	foreach(files, file, index)
	{
		if(!_stricmp("txt", getExt(file)))
		{
			char *text = readText(file);
			char *newText;

			newText = strx(text);
			newText = replaceLine(newText, "$version$", sVersion, 1);

			if(strcmp(text, newText)) // ? text != newText
			{
				LOGPOS();
				writeOneLineNoRet(file, newText);
			}
			memFree(text);
			memFree(newText);
		}
		else if(!_stricmp("exe", getExt(file)))
		{
			static char *CONCERT_PTN = "{a9a54906-791d-4e1a-8a71-a4c69359cf68}:0.00"; // shared_uuid
			autoBlock_t *text = readBinary(file);
			uint conPos;

			conPos = FindStringInExe(text, CONCERT_PTN);

			if(conPos != UINTMAX)
			{
				LOGPOS();
				errorCase(strlen(sVersion) != 4); // 2bs
				strcpy((char *)directGetBuffer(text) + conPos + strlen(CONCERT_PTN) - 4, sVersion);
				errorCase(FindStringInExe(text, CONCERT_PTN) != UINTMAX); // ? 2箇所以上ある
				DestroyFindVersionPtn(text, conPos, strlen(CONCERT_PTN) - 5);
				writeBinary(file, text);
			}
			releaseAutoBlock(text);
		}
	}
	releaseDim(files, 1);
	memFree(sVersion);
}
static char *GetPathTailVer(uint version) // ret: bind
{
	static char *pathTail;

	memFree(pathTail);

	if(version == VER_BETA)
		pathTail = xcout("_BETA_%s", c_makeCompactStamp(NULL));
	else
		pathTail = xcout("_v%03u", version);

	return pathTail;
}
static void PackZipFileEx(char *zipFile, char *srcDir, int srcDirRmFlag, char *baseName, uint version)
{
	char *workDir = makeTempDir(NULL);
	char *destDir;

	srcDir = strx(srcDir);
	srcDir = IntoIfOneDir(srcDir);

	destDir = strx(workDir);

	if(baseName)
	{
		destDir = combine_xc(destDir, baseName);

		if(version)
			destDir = addLine(destDir, GetPathTailVer(version));

		createDir(destDir);
	}
	if(srcDirRmFlag)
	{
		moveDir(srcDir, destDir);
		removeDir(srcDir);
	}
	else
		copyDir(srcDir, destDir);

	if(version)
		ReplaceVersion(destDir, version);

	PackZipFile(zipFile, workDir);

	forceRemoveDir(workDir);

	memFree(srcDir);
	memFree(workDir);
	memFree(destDir);
}
static void RepackZipFile(char *zipFile, char *baseName)
{
	char *workDir = makeTempDir(NULL);

	ExtractZipFile(zipFile, workDir);
	PackZipFileEx(zipFile, workDir, 1, baseName, 0);

	memFree(workDir);
}
static void RepackAllZipFile(char *rootDir)
{
	autoList_t *files = lssFiles(rootDir);
	char *file;
	uint index;

	foreach(files, file, index)
	{
		if(!_stricmp("ZIP", getExt(file)))
		{
			char *baseName = getLocal(file);

			baseName = changeExt(baseName, "");
			baseName = lineToFairLocalPath_x(baseName, 100);

			cout("file: %s\n", file);
			cout("baseName: %s\n", baseName);

			RepackZipFile(file, baseName);

			memFree(baseName);
		}
	}
	releaseDim(files, 1);
}
int main(int argc, char **argv)
{
	errorCase(!existFile(ZIP7_LOCAL_FILE) && !existFile(ZIP7_FILE));

	/*
		★★★ パスはこの関数内でフルパスにすること。★★★
	*/

	if(argIs("/P"))
	{
		char *zipFile;
		char *srcDir;
		char *baseName;

		zipFile = makeFullPath(nextArg());
		srcDir  = makeFullPath(nextArg());
		baseName = hasArgs(1) ? lineToFairLocalPath(nextArg(), 100) : NULL;

		cout("[Pack]\n");
		cout("zipFile: %s\n", zipFile);
		cout("srcDir: %s\n", srcDir);
		cout("baseName: %s\n", baseName ? baseName : "<none>");

		PackZipFileEx(zipFile, srcDir, 0, baseName, 0);

		memFree(zipFile);
		memFree(srcDir);
		memFree(baseName);
		return;
	}
	if(argIs("/R"))
	{
		char *zipFile;
		char *baseName;

		zipFile = makeFullPath(nextArg());
		baseName = hasArgs(1) ? lineToFairLocalPath(nextArg(), 100) : NULL;

		cout("[Repack]\n");
		cout("zipFile: %s\n", zipFile);
		cout("baseName: %s\n", baseName ? baseName : "<none>");

		RepackZipFile(zipFile, baseName);

		memFree(zipFile);
		memFree(baseName);
		return;
	}
	if(argIs("/RB"))
	{
		char *zipFile;
		char *baseName;

		zipFile = makeFullPath(nextArg());
		baseName = getLocal(zipFile);
		baseName = changeExt(baseName, "");
		baseName = lineToFairLocalPath_x(baseName, 100);

		cout("[Repack(Base)]\n");
		cout("zipFile: %s\n", zipFile);
		cout("baseName: %s\n", baseName);

		RepackZipFile(zipFile, baseName);

		memFree(zipFile);
		memFree(baseName);
		return;
	}
	if(argIs("/RBD"))
	{
		char *rootDir = makeFullPath(nextArg());

		cout("[Repack(Base+RootDir)]\n");
		cout("rootDir: %s\n", rootDir);

		RepackAllZipFile(rootDir);

		memFree(rootDir);
		return;
	}
	if(argIs("/O")) // C:\\Dev のリリース向け
	{
		char *outDir;
		char *projName;
		char *destZipFile;
		char *midZipFile;

		outDir = makeFullPath(nextArg());
		projName = lineToFairLocalPath(nextArg(), 100);
		destZipFile = combine_cx(outDir, addExt(strx(projName), "zip"));
		midZipFile = makeTempPath("zip");

		cout("[Pack]\n");
		cout("outDir: %s\n", outDir);
		cout("projName: %s\n", projName);
		cout("destZipFile: %s\n", destZipFile);
		cout("midZipFile: %s\n", midZipFile);

		PackZipFileEx(midZipFile, outDir, 1, projName, 0);
		createDir(outDir);
		coExecute_x(xcout("Compact.exe /C \"%s\"", outDir)); // 圧縮
		copyFile(midZipFile, destZipFile);
		removeFile(midZipFile);

		memFree(outDir);
		memFree(projName);
		memFree(destZipFile);
		memFree(midZipFile);
		return;
	}
	if(argIs("/G")) // C:\\Dev のゲーム（バージョン番号付きアプリ）のリリース向け
	{
		char *outDir;
		char *projName;
		char *destZipFile;
		char *midZipFile;
		uint version = InputVersion();

		outDir = makeFullPath(nextArg());
		projName = lineToFairLocalPath(nextArg(), 100);
		destZipFile = combine_cx(outDir, addExt(xcout("%s%s", projName, GetPathTailVer(version)), "zip"));
		midZipFile = makeTempPath("zip");

		if(version)
		{
			cout("[Pack(Game)]\n");
			cout("outDir: %s\n", outDir);
			cout("projName: %s\n", projName);
			cout("destZipFile: %s\n", destZipFile);
			cout("midZipFile: %s\n", midZipFile);
			cout("version: %03u\n", version);

			PackZipFileEx(midZipFile, outDir, 1, projName, version);
			createDir(outDir);
			coExecute_x(xcout("Compact.exe /C \"%s\"", outDir)); // 圧縮
			copyFile(midZipFile, destZipFile);
			removeFile(midZipFile);
		}
		else
			cout("キャンセルされました。\n");

		memFree(outDir);
		memFree(projName);
		memFree(destZipFile);
		memFree(midZipFile);
		return;
	}
}
