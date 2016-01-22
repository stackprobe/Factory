/*
	"" 渡したとき

	_fullpath()   カレントディレクトリ <- 注意
	_access()     アクセス不可
	_mkdir()      失敗
	_chdir()      失敗
*/

#include "all.h"

void trimPathWithEdge(char *path)
{
	escapeYen(path);
	trim(path, '/');
	restoreYen(path);
}
void trimPath(char *path)
{
	escapeYen(path);
	trimSequ(path, '/');
	restoreYen(path);
}
char *combine(char *dir, char *file)
{
	char *dirFile;

	if(dir[0] && dir[1] == ':' && dir[2] == '\0') // ? "X:" -> "X:."
	{
		static char buff[] = "?:.";
		buff[0] = dir[0];
		dir = buff;
	}
	dirFile = xcout("%s\\%s", dir, file);
	trimPath(dirFile);
	return dirFile;
}

static char *GetLocalEx(char *path, uint p_index)
{
	char *p = mbs_strrchr(path, '\\');

	if(!p)
		return path;

	return p + p_index;
}
static char *GetExtEx(char *path, uint p_index)
{
	char *local = getLocal(path);
	char *p;

	p = strrchr(local, '.');

	/*
		.gitignore など、最初の '.' は拡張子のデリミタとは見なさない。".aaabbbccc" は拡張子無し。"z.aaabbbccc" の拡張子は "aaabbbccc"
	*/
	if(!p || p == local)
		return strchr(local, '\0');

	return p + p_index;
}
char *getLocalDelim(char *path)
{
	return GetLocalEx(path, 0);
}
char *getLocal(char *path)
{
	return GetLocalEx(path, 1);
}
char *getExtDelim(char *path)
{
	return GetExtEx(path, 0);
}
char *getExt(char *path)
{
	return GetExtEx(path, 1);
}
/*
	newLocal == "" のとき、"C:\\abc" -> "C:" になることに注意！
*/
char *changeLocal(char *path, char *newLocal)
{
	path = strxm(path, strlen(newLocal));

	if(*newLocal)
	{
		strcpy(getLocal(path), newLocal);
	}
	else
	{
		*getLocalDelim(path) = '\0';
	}
	return path;
}
char *getParent(char *path) // ret: path が存在するディレクトリを path を元に生成する。
{
	path = changeLocal(path, "");

	if(!*path)
	{
		path = addChar(path, '.');
	}
	else if(path[1] == ':' && !path[2])
	{
		path = addChar(path, '\\');
	}
	return path;
}
char *changeExt(char *path, char *newExt)
{
	path = strxm(path, 1 + strlen(newExt));

	if(*newExt)
	{
		char *ext = getExtDelim(path);

		*ext++ = '.';
		strcpy(ext, newExt);
	}
	else
	{
		*getExtDelim(path) = '\0';
	}
	return path;
}
char *addLocal(char *path, char *newLocal) // strr() 注意
{
	if(*newLocal)
	{
		path = addChar(path, '\\');
		path = addLine(path, newLocal);
	}
	return path;
}
char *addExt(char *path, char *newExt) // strr() 注意
{
	if(*newExt)
	{
		path = addChar(path, '.');
		path = addLine(path, newExt);
	}
	return path;
}

int isAbsPath(char *path)
{
	errorCase(!path);
	return lineExp("<1,AZaz>:\\<>", path);
}
int isRootDirAbs(char *path)
{
	errorCase(!path);
	return lineExp("<1,AZaz>:\\", path);
}

/*
	_fullpath("") -> カレントディレクトリを返す。
	_fullpath("NUL") -> "\\\\.\\NUL" を返す。
*/
char *makeFullPath(char *path)
{
	char *retPath;

	errorCase(m_isEmpty(path));
	path = _fullpath(NULL, path, 0);
	errorCase(m_isEmpty(path));
	retPath = strx(path);
	free(path);

	/*
		ローカルディスク上のフルパスになってない？
		"NUL", "COM1" etc. -> "\\\\.\\NUL", "\\\\.\\COM1" etc.
	*/
	errorCase(!m_isalpha(retPath[0]));
	errorCase(retPath[1] != ':');
	errorCase(retPath[2] != '\\');

	/*
		_fullpath() は "C:\\ABC\\DEF\\" の最後の '\\' を取ってくれない。
	*/
	if(!isAbsRootDir(retPath))
	{
		escapeYen(retPath);

		if(strchr(retPath, '\0')[-1] == '/')
		{
			*(strchr(retPath, '\0') - 1) = '\0';
		}
		restoreYen(retPath);
	}
	return retPath;
}
int isRootDir(char *path)
{
	int retval;

	path = makeFullPath(path);
	retval = isAbsRootDir(path);
	memFree(path);
	return retval;
}
char *getFullPath(char *path, char *baseDir)
{
	char *retPath;

	if(!baseDir)
		baseDir = ".";

	errorCase(m_isEmpty(path));
	errorCase(m_isEmpty(baseDir));

	addCwd(baseDir);
	path = _fullpath(NULL, path, 0);
	unaddCwd();

	retPath = strx(path);
	free(path);
	return retPath;
}

int accessible(char *path)
{
	return !_access(path, 0); // ? アクセス可
}
/*
	ローカルディスク上の存在するルートディレクトリ、ディレクトリ、ファイルについて真を返す。
	ワイルドカードは使えない -> 偽を返す。
	"" -> 偽を返す。
*/
int existPath(char *path) // path: NULL -> 不可(指定してはならない)
{
	static char *lastPath;

	/*
		'\\' -> '/'
	*/
	path = strx(path);
	escapeYen(path);
	memFree(lastPath);
	lastPath = path;

	/*
		_access()は...
		パスの直後に1つ以上の '.' があると、それを取り除いた場合と同じ結果を返す。_access("abc.txt") == _access("abc.txt...")
		本関数はこれを認めない。但し、カレントDIRや親DIRを指す ".", ".." は認める。
	*/
	if(*path && strchr(path, '\0')[-1] == '.' && !lineExp("<>//<1,2,..>", path) && !lineExp("<1,2,..>", path))
	{
		return 0;
	}

	/*
		_access()は...
		ルートディレクトリ以外でもパスの最後に '\\' があっても良い。_access("C:\\abc\\def") == _access("C:\\abc\\def\\")
		本関数はファイルとサブディレクトリの最後に '\\' が付くことを認めない。
	*/
	if(*path && strchr(path, '\0')[-1] == '/' && !lineExp("<1,AZaz>://", path) && strcmp(path, "/"))
	{
		return 0;
	}

	/*
		_access()は...
		'\\' を2つ以上続けて書いても良いようだ。_access("C:\\Dir\\Sub") == _access("C:\\\\Dir\\\\\\Sub")
		本関数は余計な '\\' を認めない。ついでにネットワークパスもここで阻止する。
	*/
	if(mbs_strstr(path, "//"))
	{
		return 0;
	}

	/*
		_access("") -> !0 (アクセス不可) を返す。
		_access(存在しないドライブ || 準備のできていないドライブ) -> !0 (アクセス不可) になる。
	*/
	return !_access(path, 0); // ? アクセス可
}
/*
	Xドライブが存在する && 準備出来ている -> existDir("X:\\") は真を返す。
	Xドライブが存在しない || 準備出来ていない -> existDir("X:\\") は偽を返す。
*/
int existDir(char *dir)
{
	return existPath(dir) && (updateFindData(dir), lastFindData.attrib & _A_SUBDIR);
}
int existFile(char *file)
{
	return existPath(file) && (updateFindData(file), !(lastFindData.attrib & _A_SUBDIR));
}

sint dirFileComp(char *path1, char *path2) // ソート用、ディレクトリ -> ファイルの順、あとは mbs_stricmp() 順
{
	int d1 = existDir(path1);
	int d2 = existDir(path2);

	if(d1 && !d2) return -1;
	if(d2 && !d1) return 1;

	return mbs_stricmp(path1, path2);
}
void dirFileSort(autoList_t *paths)
{
	rapidSort(paths, (sint (*)(uint, uint))dirFileComp);
}
void dirFileSortDirCount(autoList_t *paths, uint dirCount)
{
	autoList_t dirs = gndSubElements(paths, 0, dirCount);
	autoList_t files = gndFollowElements(paths, dirCount);

	sortJLinesICase(&dirs);
	sortJLinesICase(&files);
}

/*
	_mkdir("") -> !0 (失敗) を返す。
*/
int creatable(char *path)
{
	if(_mkdir(path)) // ? 失敗
		return 0;

	removeDir(path);
	return 1;
}
int overwritable(char *path)
{
	return existFile(path) || creatable(path); // リードオンリーとかだったらダメだけど、まあいいや。
}
void createDir(char *dir)
{
	if(_mkdir(dir)) // ? 失敗
	{
		error();
	}
}
void createFile(char *file)
{
	fileClose(fileOpen(file, "wb"));
}
void createDirIfNotExist(char *dir)
{
	if(!existDir(dir))
	{
		createDir(dir);
	}
}
void createFileIfNotExist(char *file)
{
	if(!existFile(file))
	{
		createFile(file);
	}
}
void removeDir(char *dir)
{
	uint c;

	for(c = 1; ; c++)
	{
		if(!_rmdir(dir)) // ? 成功
		{
			break;
		}
		cout("[%u] fault remove dir: %s\n", c, dir);
		errorCase(c == 10);
		sleep(c * 100);
	}
	if(2 <= c)
	{
		cout("remove dir ok @ %u\n", c);
	}
}
void removeFile(char *file)
{
	uint c;

	for(c = 1; ; c++)
	{
		if(!remove(file)) // ? 成功
		{
			break;
		}
		cout("[%u] fault remove file: %s\n", c, file);
		errorCase(c == 10);
		sleep(c * 100);
	}
	if(2 <= c)
	{
		cout("remove file ok @ %u\n", c);
	}
}
void removeDirIfExist(char *dir)
{
	if(existDir(dir))
	{
		removeDir(dir);
	}
}
void removeDirIfEmpty(char *dir)
{
	if(!lsCount(dir))
	{
		removeDir(dir);
	}
}
void removeFileIfExist(char *file)
{
	if(existFile(file))
	{
		removeFile(file);
	}
}
void removePath(char *path)
{
	(existFile(path) ? removeFile : removeDir)(path);
}
void removePathIfExist(char *path)
{
	removeDirIfExist(path);
	removeFileIfExist(path);
}
void forceClearDir(char *dir)
{
	autoList_t *paths = lss(dir);
	char *path;
	uint index;

	reverseElements(paths);

	foreach(paths, path, index)
	{
		removePath(path);
	}
	releaseDim(paths, 1);
}
void forceRemoveDir(char *dir)
{
	forceClearDir(dir);
	removeDir(dir);
}
void forceRemoveDirIfExist(char *dir)
{
	if(existDir(dir))
	{
		forceRemoveDir(dir);
	}
}
void forceRemovePath(char *path)
{
	(existFile(path) ? removeFile : forceRemoveDir)(path);
}
void forceRemovePathIfExist(char *path)
{
	forceRemoveDirIfExist(path);
	removeFileIfExist(path);
}

/*
	path
		相対可
		ルートディレクトリの場合何もしない。

	mode
		"DFXdf" == dir, file, no-create, dir-if-not-exist, file-if-not-exist
*/
void createPath(char *path, int mode)
{
	autoList_t *ptkns;
	uint index;

	path = makeFullPath(path);
	cout("CREATE_PATH_%c: [%s]\n", mode, path);
	escapeYen(path);
	ptkns = tokenize(path, '/');
	memFree(path);
	path = strx(getLine(ptkns, 0));

	for(index = 1; ; index++)
	{
		path = addChar(path, '\\');
		path = addLine(path, getLine(ptkns, index));

		if(index == getCount(ptkns) - 1)
			break;

		createDirIfNotExist(path);
	}
	switch(mode)
	{
	case 'D':
		createDir(path);
		break;

	case 'F':
		createFile(path);
		break;

	case 'X':
		break;

	case 'd':
		createDirIfNotExist(path);
		break;

	case 'f':
		createFileIfNotExist(path);
		break;

	default:
		error();
	}
	memFree(path);
	releaseDim(ptkns, 1);
}

char *getCwd(void)
{
	char *dirBuff = _getcwd(NULL, 0);
	char *dir;

	if(!dirBuff)
	{
		error();
	}
	dir = strx(dirBuff);
	free(dirBuff);
	return dir;
}
void changeCwd(char *dir)
{
	/*
		_chdir("") -> !0 (失敗) を返す。
	*/
	if(_chdir(dir)) // ? 失敗
	{
		error();
	}
}

static autoList_t *CwdStack;

void addCwd(char *dir)
{
	if(!CwdStack)
		CwdStack = createAutoList(1);

	addElement(CwdStack, (uint)getCwd());
	changeCwd(dir);
}
void unaddCwd(void)
{
	char *dir = (char *)unaddElement(CwdStack);

	changeCwd(dir);
	memFree(dir);
}
void mkAddCwd(char *dir)
{
	createDirIfNotExist(dir);
	addCwd(dir);
}
void unaddAllCwd(void)
{
	while(CwdStack && getCount(CwdStack))
		unaddCwd();
}

char *eraseRoot(char *path, char *root)
{
	uint rootlen;

	// "x:\\abc" -> "x:\\abc\\", "x:\\" -> "x:\\"
	root = strx(root);
	trimPathWithEdge(root);
	root = addChar(root, '\\');

	rootlen = strlen(root);

	errorCase(strlen(path) <= rootlen); // ? ルートより短い || ルートそのもの
	errorCase(mbs_strnicmp(path, root, rootlen) != 0); // ? ルート不一致

	memFree(root);

	return path + rootlen;
}
void eraseRoots(autoList_t *pathList, char *root)
{
	char *path;
	uint index;

	foreach(pathList, path, index)
	{
		setElement(pathList, index, (uint)eraseRoot(path, root));
	}
}
char *changeRoot(char *path, char *root, char *newRoot) // ret: newRoot == NULL ? path : strr(path)
{
	// ルートの削除
	if(root)
	{
		copyLine(path, eraseRoot(path, root));
	}

	// ルートの追加
	if(newRoot)
	{
		path = combine_cx(newRoot, path);
	}
	return path;
}
void changeRoots(autoList_t *pathList, char *root, char *newRoot)
{
	char *path;
	uint index;

	foreach(pathList, path, index)
	{
		setElement(pathList, index, (uint)changeRoot(path, root, newRoot));
	}
}
void changeRootsAbs(autoList_t *pathList, char *root, char *newRoot)
{
	if(root)
		root = makeFullPath(root);

	if(newRoot)
		newRoot = makeFullPath(newRoot);

	changeRoots(pathList, root, newRoot);

	memFree(root);
	memFree(newRoot);
}
void eraseParent(char *path)
{
	copyLine(path, getLocal(path));
}
void eraseParents(autoList_t *pathList)
{
	char *path;
	uint index;

	foreach(pathList, path, index)
	{
		eraseParent(path);
	}
}

static void CopyFile_DM(char *srcFile, char *destFile, char *destMode)
{
	uint64 buffSize = getFileSize(srcFile);
	FILE *rfp;
	FILE *wfp;

	buffSize = m_min(buffSize, 128 * 1024 * 1024);

	rfp = fileOpen(srcFile, "rb");
	wfp = fileOpen(destFile, destMode);

	if(buffSize)
	{
		for(; ; )
		{
			autoBlock_t *block = readBinaryStream(rfp, buffSize);

			if(!block)
				break;

			writeBinaryBlock(wfp, block);
			releaseAutoBlock(block);
		}
	}
	fileClose(rfp);
	fileClose(wfp);
}
void joinFile(char *bodyFile, char *tailFile)
{
	CopyFile_DM(tailFile, bodyFile, "ab");
}
void copyFile(char *srcFile, char *destFile)
{
	CopyFile_DM(srcFile, destFile, "wb");
}
static void CopyDir_Abs(char *srcDir, char *destDir) // destDir は作成されていること。
{
	autoList_t *srcPaths = lss(srcDir);
	autoList_t *destPaths;
	uint index;

	destPaths = copyLines(srcPaths);
	changeRoots(destPaths, srcDir, destDir); // <- srcDir, destDir 共に絶対パスであること。

	for(index = 0; index < lastDirCount; index++)
	{
		char *dir = getLine(destPaths, index);

		if(!existDir(dir)) // 上書きに対応してみる。
			createDir(dir);
	}
	for(; index < getCount(srcPaths); index++)
	{
		copyFile(getLine(srcPaths, index), getLine(destPaths, index));
	}
	releaseDim(srcPaths, 1);
	releaseDim(destPaths, 1);
}
void copyDir(char *srcDir, char *destDir) // destDir は作成されていること。
{
	srcDir = makeFullPath(srcDir);
	destDir = makeFullPath(destDir);

	CopyDir_Abs(srcDir, destDir);

	memFree(srcDir);
	memFree(destDir);
}

/*
	moveFile()    ファイル - 何処でも
	          ディレクトリ - 同じディレクトリ内
	moveDir()     ファイル - 不可
	          ディレクトリ - 何処でも
*/

/*
	destFile が既に存在する場合 error() になる。
*/
void moveFile(char *srcFile, char *destFile)
{
	if(rename(srcFile, destFile))
	{
		error();
	}
}
static void MoveDir_Abs(char *srcDir, char *destDir) // destDir は作成されていること。srcDir は空のディレクトリとして残る。
{
	autoList_t *srcPaths = lss(srcDir);
	autoList_t *destPaths;
	uint index;

	destPaths = copyLines(srcPaths);
	changeRoots(destPaths, srcDir, destDir); // <- srcDir, destDir 共に絶対パスであること。

	for(index = 0; index < lastDirCount; index++)
	{
		char *dir = getLine(destPaths, index);

		if(!existDir(dir)) // 上書きに対応してみる。
			createDir(dir);
	}
	for(; index < getCount(srcPaths); index++)
	{
		char *srcFile = getLine(srcPaths, index);
		char *destFile = getLine(destPaths, index);

		if(existFile(destFile)) // 上書きに対応してみる。
			semiRemovePath(destFile);

		moveFile(srcFile, destFile);
	}
	for(index = lastDirCount; index; )
	{
		index--;
		removeDir(getLine(srcPaths, index));
	}
	releaseDim(srcPaths, 1);
	releaseDim(destPaths, 1);
}
void moveDir(char *srcDir, char *destDir) // destDir は作成されていること。srcDir は空のディレクトリとして残る。
{
	srcDir = makeFullPath(srcDir);
	destDir = makeFullPath(destDir);

	MoveDir_Abs(srcDir, destDir);

	memFree(srcDir);
	memFree(destDir);
}

void copyPath(char *rPath, char *wPath)
{
	errorCase(m_isEmpty(rPath));
	errorCase(m_isEmpty(wPath));

	if(existDir(rPath))
	{
		createDirIfNotExist(wPath);
		forceClearDir(wPath);
		copyDir(rPath, wPath);
	}
	else
		copyFile(rPath, wPath);
}
void movePath(char *rPath, char *wPath)
{
	errorCase(m_isEmpty(rPath));
	errorCase(m_isEmpty(wPath));

	if(existDir(rPath))
	{
		createDirIfNotExist(wPath);
		moveDir(rPath, wPath);
		removeDir(rPath);
	}
	else
		moveFile(rPath, wPath);
}

void setFileSize(char *file, uint64 size)
{
	FILE *fp;
	int fh;

	errorCase(SINT64MAX < size);

	fp = fileOpen(file, "ab");
	fh = _fileno(fp);

	/*
		_chsize_s
			ファイルが拡張される場合、拡張部分には 0x00 が補填される。
	*/
	if(_chsize_s(fh, (sint64)size) != 0) // ? 失敗
	{
		error();
	}
	fileClose(fp);
}
uint64 getFileSizeFP(FILE *fp)
{
	sint64 size;

	if(_fseeki64(fp, 0I64, SEEK_END) != 0) // ? 失敗
	{
		error();
	}
	size = _ftelli64(fp);

	if(size < 0I64)
	{
		error();
	}
	return size;
}
uint64 getFileSize(char *file)
{
	FILE *fp = fileOpen(file, "rb");
	uint64 size;

	size = getFileSizeFP(fp);
	fileClose(fp);
	return size;
}
uint64 getDirSize(char *dir)
{
	autoList_t *files = lss(dir);
	uint index;
	uint64 size = 0;

	for(index = lastDirCount; index < getCount(files); index++)
	{
		size += getFileSize(getLine(files, index));
	}
	releaseDim(files, 1);
	return size;
}

#define HIDEMARU_PATH "C:\\Program Files\\Hidemaru\\Hidemaru.exe"
#define HIDEMARU_X86_PATH "C:\\Program Files (x86)\\Hidemaru\\Hidemaru.exe"

void editTextFile(char *file)
{
	char *commandLine = xcout("START \"\" /WAIT \"%s\" \"%s\""
		,accessible(HIDEMARU_X86_PATH) ? HIDEMARU_X86_PATH : HIDEMARU_PATH
		,file
		);

	execute(commandLine);
	memFree(commandLine);
}
autoList_t *editTextLines(autoList_t *lines)
{
	char *file = makeTempPath("txt");
	autoList_t *retLines;

	writeLines(file, lines);

	editTextFile(file);

	if(!existFile(file)) createFile(file); // 秀丸は空にして保存で削除できる
	retLines = readLines(file);
	removeFile(file);
	memFree(file);

	return retLines;
}
void viewTextLines(autoList_t *lines)
{
	releaseDim(editTextLines(lines), 1);
}
void semiRemovePath(char *path)
{
	static char *destDir;
	static uint destCount;
	char *destPath;

	if(!destDir)
		destDir = makeFreeDir();

	destPath = combine(destDir, getLocal(path));
	destPath = toCreatablePath(destPath, destCount);
	movePath(path, destPath);
	memFree(destPath);
	destCount++;
}

static autoList_t *RFAT_Files;

void removeFileAtTermination(char *file)
{
	errorCase(m_isEmpty(file));

	if(!RFAT_Files)
		RFAT_Files = newList();

	addElement(RFAT_Files, (uint)makeFullPath(file));
}
void termination_scheduledRemoveFile(void)
{
	if(RFAT_Files)
	{
		char *file;
		uint index;

		LOGPOS();

		foreach(RFAT_Files, file, index)
		{
			cout("### %s\n", file);
			removeFile(file);
		}
		releaseDim(RFAT_Files, 1);
		RFAT_Files = NULL;
	}
}

// c_
char *c_getCwd(void)
{
	static char *stock;
	memFree(stock);
	return stock = getCwd();
}
char *c_changeLocal(char *path, char *newLocal)
{
	static char *stock;
	memFree(stock);
	return stock = changeLocal(path, newLocal);
}
char *c_getParent(char *path)
{
	static char *stock;
	memFree(stock);
	return stock = getParent(path);
}
char *c_changeExt(char *path, char *newExt)
{
	static char *stock;
	memFree(stock);
	return stock = changeExt(path, newExt);
}
char *c_combine(char *dir, char *file)
{
	static char *stock;
	memFree(stock);
	return stock = combine(dir, file);
}
char *c_makeFullPath(char *path)
{
	static char *stock;
	memFree(stock);
	return stock = makeFullPath(path);
}

// _x
char *changeLocal_xc(char *path, char *newLocal)
{
	char *out = changeLocal(path, newLocal);
	memFree(path);
	return out;
}
void changeRoots_xc(autoList_t *pathList, char *root, char *newRoot)
{
	changeRoots(pathList, root, newRoot);
	memFree(root);
}
char *changeExt_xc(char *path, char *newExt)
{
	char *out = changeExt(path, newExt);
	memFree(path);
	return out;
}
char *combine_cx(char *dir, char *file)
{
	char *out = combine(dir, file);
	memFree(file);
	return out;
}
char *combine_xc(char *dir, char *file)
{
	char *out = combine(dir, file);
	memFree(dir);
	return out;
}
char *combine_xx(char *dir, char *file)
{
	char *out = combine(dir, file);
	memFree(dir);
	memFree(file);
	return out;
}
char *makeFullPath_x(char *path)
{
	char *out = makeFullPath(path);
	memFree(path);
	return out;
}
void removeFile_x(char *file)
{
	removeFile(file);
	memFree(file);
}
void removeFileIfExist_x(char *file)
{
	removeFileIfExist(file);
	memFree(file);
}
void removePath_x(char *path)
{
	removePath(path);
	memFree(path);
}
void removePathIfExist_x(char *path)
{
	removePathIfExist(path);
	memFree(path);
}
void forceClearDir_x(char *dir)
{
	forceClearDir(dir);
	memFree(dir);
}
void forceRemoveDirIfExist_x(char *dir)
{
	forceRemoveDirIfExist(dir);
	memFree(dir);
}
void forceRemovePath_x(char *path)
{
	forceRemovePath(path);
	memFree(path);
}
void forceRemovePathIfExist_x(char *path)
{
	forceRemovePathIfExist(path);
	memFree(path);
}
void forceRemoveDir_x(char *dir)
{
	forceRemoveDir(dir);
	memFree(dir);
}
void copyFile_cx(char *srcFile, char *destFile)
{
	copyFile(srcFile, destFile);
	memFree(destFile);
}
void copyFile_xc(char *srcFile, char *destFile)
{
	copyFile(srcFile, destFile);
	memFree(srcFile);
}
void copyFile_xx(char *srcFile, char *destFile)
{
	copyFile(srcFile, destFile);
	memFree(srcFile);
	memFree(destFile);
}
void moveDir_cx(char *srcDir, char *destDir)
{
	moveDir(srcDir, destDir);
	memFree(destDir);
}
void moveDir_xc(char *srcDir, char *destDir)
{
	moveDir(srcDir, destDir);
	memFree(srcDir);
}
void moveDir_xx(char *srcDir, char *destDir)
{
	moveDir(srcDir, destDir);
	memFree(srcDir);
	memFree(destDir);
}
char *getParent_x(char *path)
{
	char *out = getParent(path);
	memFree(path);
	return out;
}
autoList_t *editTextLines_x(autoList_t *lines)
{
	autoList_t *out = editTextLines(lines);
	releaseDim(lines, 1);
	return out;
}
void addCwd_x(char *dir)
{
	addCwd(dir);
	memFree(dir);
}
void mkAddCwd_x(char *dir)
{
	mkAddCwd(dir);
	memFree(dir);
}
void createFile_x(char *file)
{
	createFile(file);
	memFree(file);
}
void createPath_x(char *path, int mode)
{
	createPath(path, mode);
	memFree(path);
}
