#include "all.h"

// ---- flags ----

int sockServerMode;

// ----

int lastSystemRet; // ? ! コマンドが正常に実行出来て 0 を返した。

void execute(char *commandLine)
{
	lastSystemRet = system(commandLine);
}
void execute_x(char *commandLine)
{
	execute(commandLine);
	memFree(commandLine);
}
void coExecute(char *commandLine)
{
	cout("cmdln: %s\n", commandLine);
	execute(commandLine);
}
void coExecute_x(char *commandLine)
{
	coExecute(commandLine);
	memFree(commandLine);
}
void sleep(uint millis) // ts_
{
	/*
		他スレッドに制御を渡すために、inner_uncritical -> sleep(0) -> inner_critical しているところがあるが、
		sleep(0) だと制御が渡らない。sleep(1) なら渡る。-> 強制的に sleep(1) にする。
	*/
	if(!millis)
		millis = 1;

	Sleep(millis);
}
void coSleep(uint millis)
{
	uint elapse = 0;

	cout("%uミリ秒待ちます...\n", millis);

	while(elapse < millis)
	{
		uint m = m_min(millis - elapse, 300);

		sleep(m);
		elapse += m;
		cout("\r%uミリ秒経ちました。", elapse);
	}
	cout("\n");
}
void noop(void)
{
	// noop
}
void noop_u(uint dummy)
{
	// noop
}
void noop_uu(uint dummy1, uint dummy2)
{
	// noop
}
uint getZero(void)
{
	return 0;
}
char *getEnvLine(char *name) // ret: c_
{
	char *line;

	errorCase(!name);
	line = getenv(name);

	if(!line)
		line = "";

	return line;
}
#if 0
static DWORD GetTickCount_TEST(void)
{
	static int initOnce;
	static uint baseTick;

	if(!initOnce)
	{
		initOnce = 1;
		baseTick = UINT_MAX - 10000 - GetTickCount();
	}
	return baseTick + GetTickCount();
}
#endif
uint64 nowTick(void)
{
//	uint currTick = GetTickCount_TEST();
	uint currTick = GetTickCount();
	static uint lastTick;
	static uint64 baseTick;
	uint64 retTick;
	static uint64 lastRetTick;

	if(currTick < lastTick) // ? カウンタが戻った -> オーバーフローした？
	{
		uint diffTick = lastTick - currTick;

		if(UINTMAX / 2 < diffTick) // オーバーフローだろう。
		{
			LOGPOS();
			baseTick += (uint64)UINT_MAX + 1;
		}
		else // オーバーフローか？
		{
			LOGPOS();
			baseTick += diffTick; // 前回と同じ戻り値になるように調整する。
		}
	}
	lastTick = currTick;
	retTick = baseTick + currTick;
	errorCase(retTick < lastRetTick); // 2bs
	lastRetTick = retTick;
	return retTick;
}
uint now(void)
{
	return (uint)(nowTick() / 1000);
}
int pulseSec(uint span, uint *p_nextSec)
{
	static uint defNextSec;
	uint sec = now();

	if(!p_nextSec)
		p_nextSec = &defNextSec;

	if(sec < *p_nextSec)
		return 0;

	*p_nextSec = sec + span;
	return 1;
}
int eqIntPulseSec(uint span, uint *p_nextSec)
{
	static uint callPerCheck = 1;
	static uint count;

	count++;

	if(count % callPerCheck == 0 && pulseSec(span, p_nextSec))
	{
		count = m_max(callPerCheck / 3, count / 10);
		callPerCheck = m_max(1, count);
		count = 0;
		return 1;
	}
	return 0;
}
uint getTick(void)
{
	return GetTickCount();
}
uint getUDiff(uint tick1, uint tick2) // ret: tick2 - tick1
{
	if(tick2 < tick1)
	{
		return 0xffffffffu - ((tick1 - tick2) - 1u);
	}
	return tick2 - tick1;
}
sint getIDiff(uint tick1, uint tick2) // ret: tick2 - tick1
{
	uint diff = getUDiff(tick1, tick2);

	if(diff < 0x80000000u)
	{
		return (sint)diff;
	}
	return (-0x7fffffff - 1) + (sint)(diff - 0x80000000u);
}
int isLittleEndian(void)
{
	uint dword = 1;
	return *(uchar *)&dword;
}

#define FILE_SHARE_COUNTER "C:\\Factory\\tmp\\Counter.txt"

uint64 nextCommonCount(void)
{
	char *line;
	uint64 counter;

	mutex();

	if(existFile(FILE_SHARE_COUNTER))
	{
		line = readFirstLine(FILE_SHARE_COUNTER);
		counter = toValue64(line);
		memFree(line);
	}
	else // カウンタ初期化
	{
		counter = toValue64Digits_xc(makeCompactStamp(NULL), hexadecimal) << 8;
		// ex. 1980/01/02 02:03:55 -> 0x1980010202035500
	}
	errorCase(counter == UINT64MAX); // カンスト..有り得ないだろうけど..

	if(UINT64MAX / 2 < counter)
		LOGPOS(); // カンスト注意..有り得ないだろうけど..

	line = xcout("%I64u", counter + 1);
	writeOneLine(FILE_SHARE_COUNTER, line);

	unmutex();

	memFree(line);
	return counter;
}
static char *c_GetTempSuffix(void)
{
	static char *ret;
	static int useFactoryTmpDir;

	if(ret)
		memFree(ret);
	else
		useFactoryTmpDir = isFactoryDirEnabled() && existDir("C:\\Factory\\tmp");

	if(!useFactoryTmpDir)
	{
		static uint pid;
		static uint64 pFATime;
		static uint64 counter;

		errorCase(counter == UINT64MAX); // カンスト..有り得ないだろうけど..

		if(UINT64MAX / 2 < counter)
			LOGPOS(); // カンスト注意..有り得ないだろうけど..

		if(!pid)
		{
			pid = (uint)GetCurrentProcessId();
			errorCase(!pid); // 0 == System Idle Process
			pFATime = (uint64)time(NULL);
		}
		ret = xcout("%x_%I64x_%I64x", pid, pFATime, counter);
		counter++;
	}
	else
		ret = xcout("%I64x", nextCommonCount());

	return ret;
}
char *makeTempPath(char *ext) // ext: NULL ok
{
	static char *pbase;
	char *path;

	if(!pbase)
	{
		int useFactoryTmpDir = isFactoryDirEnabled() && existDir("C:\\Factory\\tmp");

		if(!useFactoryTmpDir)
		{
			char *tmpDir = combine(getSelfDir(), "tmp");

			if(existDir(tmpDir))
				pbase = combine(getSelfDir(), "tmp\\");
			else
				pbase = combine(getSelfDir(), "$tmp$");

			memFree(tmpDir);
		}
		else
			pbase = "C:\\Factory\\tmp\\";
	}
	for(; ; )
	{
		path = xcout("%s%s", pbase, c_GetTempSuffix());

		if(ext)
			path = addExt(path, ext);

		if(!accessible(path))
			break;

		memFree(path);
	}
	return path;
}
char *makeTempFile(char *ext) // ext: NULL ok
{
	char *file = makeTempPath(ext);
	createFile(file);
	return file;
}
char *makeTempDir(char *ext) // ext: NULL ok
{
	char *dir = makeTempPath(ext);
	createDir(dir);
	return dir;
}
char *makeFreeDir(void)
{
	char *path;

	mutex();
	path = toCreatablePath(strx("C:\\1"), 999 - 1);
	createDir(path);
	unmutex();

	return path;
}

#define SELFBUFFSIZE 1024
#define SELFBUFFMARGIN 16

char *getSelfFile(void)
{
	static char *fileBuff;

	if(!fileBuff)
	{
		fileBuff = memAlloc(SELFBUFFSIZE + SELFBUFFMARGIN);

		if(!GetModuleFileName(NULL, fileBuff, SELFBUFFSIZE))
			error();

		/*
			? フルパスの実行可能ファイルではない。
		*/
		errorCase(strlen(fileBuff) < 8); // 最短でも "C:\\a.exe"
		errorCase(!m_isalpha(fileBuff[0]));
		errorCase(memcmp(fileBuff + 1, ":\\", 2));
		errorCase(_stricmp(strchr(fileBuff, '\0') - 4, ".exe"));

		fileBuff = strr(fileBuff);
	}
	return fileBuff;
}
char *getSelfDir(void)
{
	static char *dirBuff;

	if(!dirBuff)
		dirBuff = getParent(getSelfFile());

	return dirBuff;
}

static char *GetOutDir(void)
{
	static char *dir;

	if(!dir)
		dir = makeTempDir("out");

	return dir;
}
char *getOutFile(char *localFile)
{
	return combine(GetOutDir(), localFile);
}
char *c_getOutFile(char *localFile)
{
	static char *outFile;
	memFree(outFile);
	return outFile = getOutFile(localFile);
}
void openOutDir(void)
{
	execute_x(xcout("START %s", GetOutDir()));
}

// ---- args ----

autoList_t *tokenizeArgs(char *str)
{
	autoList_t *args = newList();
	autoBlock_t *buff = newBlock();
	char *p;
	int literalMode = 0;

	for(p = str; *p; p++)
	{
		if(literalMode)
		{
			if(*p == '"' && (p[1] == ' ' || !p[1]))
			{
				literalMode = 0;
				goto addEnd;
			}
		}
		else
		{
			if(*p == ' ')
			{
				addElement(args, (uint)unbindBlock2Line(buff));
				buff = newBlock();
				goto addEnd;
			}
			if(*p == '"' && !getSize(buff))
			{
				literalMode = 1;
				goto addEnd;
			}
		}

		if(isMbc(p))
		{
			addByte(buff, *p);
			p++;
		}
		else if(*p == '\\' && (p[1] == '\\' || p[1] == '"'))
		{
			p++;
		}
		addByte(buff, *p);
	addEnd:;
	}
	addElement(args, (uint)unbindBlock2Line(buff));
	return args;
}

static autoList_t *Args;

static void ReadSysArgs(void)
{
	uint argi;

	for(argi = 0; argi < getCount(Args); )
	{
		char *arg = getLine(Args, argi);

		if(!_stricmp(arg, "//$")) // 読み込み中止
		{
			desertElement(Args, argi);
			break;
		}
		else if(!_stricmp(arg, "//F")) // パラメータをファイルから読み込んで置き換える。
		{
			char *text;
			autoList_t *subArgs;

			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			text = readResourceText(innerResPathFltr(arg));
			subArgs = tokenizeArgs(text);
			memFree(text);

			while(getCount(subArgs))
				insertElement(Args, argi, (uint)unaddElement(subArgs));

			releaseAutoList(subArgs);
		}
		else if(!_stricmp(arg, "//R")) // パラメータをファイルから読み込んで置き換える。レスポンスファイル (改行を引数の区切りと見なす)
		{
			autoList_t *subArgs;

			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			subArgs = readLines(arg);

			while(getCount(subArgs))
				insertElement(Args, argi, (uint)unaddElement(subArgs));

			releaseAutoList(subArgs);
		}
		else if(!_stricmp(arg, "//O")) // 標準出力(coutの出力)をファイルに書き出す。★注意：termination();しないとストリーム開きっぱ！
		{
			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			setCoutWrFile(arg, 0);
		}
		else if(!_stricmp(arg, "//A")) // 標準出力(coutの出力)をファイルに追記する。★注意：termination();しないとストリーム開きっぱ！
		{
			desertElement(Args, argi);
			arg = (char *)desertElement(Args, argi);

			setCoutWrFile(arg, 1);
		}
		else
			argi++;
	}
}
static autoList_t *GetArgs(void)
{
	if(!Args)
	{
		uint argi;

		Args = newList();

		for(argi = 1; argi < __argc; argi++)
		{
			addElement(Args, (uint)__argv[argi]);
		}
		ReadSysArgs();
	}
	return Args;
}

static uint ArgIndex;

int hasArgs(uint count)
{
	return count <= getCount(GetArgs()) - ArgIndex;
}
int argIs(char *spell)
{
	if(ArgIndex < getCount(GetArgs()))
	{
		if(!_stricmp(getLine(GetArgs(), ArgIndex), spell))
		{
			ArgIndex++;
			return 1;
		}
	}
	return 0;
}
char *getArg(uint index)
{
	errorCase(getCount(GetArgs()) - ArgIndex <= index);
	return getLine(GetArgs(), ArgIndex + index);
}
char *nextArg(void)
{
	char *arg = getArg(0);

	ArgIndex++;
	return arg;
}
void skipArg(uint count)
{
	for(; count; count--) nextArg();
}
/*
	ret: 残りのコマンド引数の index 番目以降全てを返す。
		index が残り個数と同じ場合 { } を返す。
*/
autoList_t *getFollowArgs(uint index)
{
	errorCase(getCount(GetArgs()) - ArgIndex < index);
	return recreateAutoList((uint *)directGetList(GetArgs()) + ArgIndex + index, getCount(GetArgs()) - ArgIndex - index);
}
autoList_t *allArgs(void)
{
	autoList_t *args = getFollowArgs(0);

	ArgIndex = getCount(GetArgs());
	return args;
}
uint getFollowArgCount(uint index)
{
	errorCase(getCount(GetArgs()) - ArgIndex < index);
	return getCount(GetArgs()) - (ArgIndex + index);
}
uint getArgIndex(void)
{
	return ArgIndex;
}
void setArgIndex(uint index)
{
	errorCase(getCount(GetArgs()) < index); // ? ! 全部読み終わった状態
	ArgIndex = index;
}

// ---- innerResPathFltr ----

static char *FPP_Path;

static int FindPathParent(char *dir, char *localPath) // dir: abs_dir
{
	for(; ; )
	{
		FPP_Path = combine(dir, localPath);
//cout("FPP.1:%s\n", dir); // test
//cout("FPP.2:%s\n", FPP_Path); // test

		if(existPath(FPP_Path))
			return 1;

		memFree(FPP_Path);

		if(isAbsRootDir(dir))
			return 0;

		dir = getParent(dir);
	}
}
char *innerResPathFltr(char *path)
{
	if(isFactoryDirDisabled() && startsWithICase(path, "C:\\Factory\\")) // ? Factory 無効 && Factory 配下を参照
		goto go_search;

	if(getLocal(path) != path) // ? パスを指定している。
		if(existPath(path))
			goto foundPath;

go_search:
	if(FindPathParent(getSelfDir(), getLocal(path)))
	{
		path = FPP_Path;
		goto foundPath;
	}
	if(FindPathParent(getCwd(), getLocal(path)))
	{
		path = FPP_Path;
		goto foundPath;
	}
	cout("res_nf: %s\n", path);
writeOneLine(getOutFile("innerResPathFltr_path.txt"), path); // XXX
	error(); // not found

foundPath:
	cout("res: %s\n", path);
	return path; // path, strx() 混在しているが const char[] として扱うこと。
}

// ----

char *LOGPOS_Time(void)
{
	static char buff[23]; // UINT64MAX -> "307445734561825:51.615"
	uint64 millis = nowTick();

	sprintf(buff, "%I64u:%02u.%03u", millis / 60000, (uint)((millis / 1000) % 60), (uint)(millis % 1000));

	return buff;
}
