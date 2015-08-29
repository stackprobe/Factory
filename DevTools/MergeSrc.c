/*
	MergeSrc.exe マスターDIR スレーブDIR

	----

	以下をスレーブからマスターに上書きコピーする。

		1. マスターとスレーブで内容が異なるファイル
		2. スレーブにしか無いファイル
*/

#include "C:\Factory\Common\all.h"

#define CRLF_EXE_FILE "C:\\Factory\\Tools\\crlf.exe"
#define CRLF_MAX_SIZE 20000000

static char *RDir;
static char *WDir;
static autoList_t *RWFiles;

static int DS_IsSameFile(char *file1, char *file2)
{
	int ret;

	if(
		getFileSize(file1) < CRLF_MAX_SIZE &&
		getFileSize(file2) < CRLF_MAX_SIZE
		)
	{
		char *wkFile1 = makeTempPath(NULL);
		char *wkFile2 = makeTempPath(NULL);

		copyFile(file1, wkFile1);
		copyFile(file2, wkFile2);

		coExecute_x(xcout("START /B /WAIT \"\" \"%s\" /LF \"%s\"", CRLF_EXE_FILE, wkFile1));
		coExecute_x(xcout("START /B /WAIT \"\" \"%s\" /LF \"%s\"", CRLF_EXE_FILE, wkFile2));

		ret = isSameFile(wkFile1, wkFile2);

		removeFile_x(wkFile1);
		removeFile_x(wkFile2);
	}
	else
		ret = isSameFile(file1, file2);

	return ret;
}
static void DoSearch(void)
{
	autoList_t *files = lssFiles(RDir);
	char *rFile;
	uint index;

	sortJLinesICase(files);

	foreach(files, rFile, index)
	{
		char *wFile = changeRoot(strx(rFile), RDir, WDir);

		if(!existFile(wFile) || !DS_IsSameFile(rFile, wFile)) // ? マージすべきファイル
		{
			addElement(RWFiles, (uint)strx(eraseRoot(rFile, RDir)));
		}
		memFree(wFile);
	}
	releaseDim(files, 1);
}
static void DC_OutputDiff(int keepTree)
{
	char *dir0;
	char *dir1;
	char *dir2;
	char *file;
	uint index;

	dir0 = makeFreeDir();
	dir1 = combine(dir0, "1_r_s");
	dir2 = combine(dir0, "2_w_m");

	createDir(dir1);
	createDir(dir2);

	foreach(RWFiles, file, index)
	{
		char *rFile = combine(RDir, file);
		char *wFile = combine(WDir, file);
		char *file1;
		char *file2;

		if(keepTree)
		{
			file1 = combine(dir1, file);
			file2 = combine(dir2, file);

			createPath(file1, 'X');
			createPath(file2, 'X');
		}
		else
		{
			char *file0 = strx(file);

			escapeYen(file0);
			file0 = replaceLine(file0, "/", "￥", 0);

			file1 = combine(dir1, file0);
			file2 = combine(dir2, file0);

			memFree(file0);

			/*
				重複し得るけどレアケースなので、これでいいや、、、
			*/
			file1 = toCreatablePath(file1, 1000);
			file2 = toCreatablePath(file2, 1000);
		}
		copyFile(rFile, file1);

		if(existFile(wFile))
			copyFile(wFile, file2);

		memFree(rFile);
		memFree(wFile);
		memFree(file1);
		memFree(file2);
	}
	execute_x(xcout("START \"\" \"%s\"", dir0));

	memFree(dir0);
	memFree(dir1);
	memFree(dir2);
}
static void DoConfirm(void)
{
restart:
	execute("CLS");

	cout("< %s\n", RDir);
	cout("> %s\n", WDir);

	{
		char *file;
		uint index;

		foreach(RWFiles, file, index)
		{
			{
				char *rFile = combine(RDir, file);
				char *wFile = combine(WDir, file);

				if(
					existFile(wFile) &&
					getFileSize(rFile) < CRLF_MAX_SIZE &&
					getFileSize(wFile) < CRLF_MAX_SIZE
					)
				{
					int r_nlt;
					int w_nlt;

					execute_x(xcout("START /B /WAIT \"\" \"%s\" \"%s\"", CRLF_EXE_FILE, rFile));
					r_nlt = lastSystemRet;
					execute_x(xcout("START /B /WAIT \"\" \"%s\" \"%s\"", CRLF_EXE_FILE, wFile));
					w_nlt = lastSystemRet;

					if(r_nlt != w_nlt)
						cout("### 改行コードが違います。### %u -> %u", r_nlt, w_nlt);
				}
				memFree(rFile);
				memFree(wFile);
			}

			cout("[%u] %s\n", index + 1, file);
		}
	}

	cout("------------\n");
	cout("ENTER = 続行\n");
	cout("0 = 確認用差分出力_階層ナシ\n");
	cout("1 = 確認用差分出力_階層アリ\n");
	cout("SPACE = 絞り込み\n");
	cout("OTHER = 中止\n");
	cout("------------\n");

	switch(getKey())
	{
	case 0x0d:
		break;

	case '0':
		DC_OutputDiff(0);
		goto restart;

	case '1':
		DC_OutputDiff(1);
		goto restart;

	case 0x20:
		RWFiles = selectLines_x(RWFiles);
		goto restart;

	default:
		termination(0);
	}

	{
		const char *CFM_PTN = "WRITE";
		char *line;

		cout("### 確認のため %s と入力してね。### (ignore case)\n", CFM_PTN);
		line = coInputLine();

		if(_stricmp(line, CFM_PTN)) // ? 一致しない。
		{
			memFree(line);
			goto restart;
		}
		memFree(line);
	}
}
static void DoMerge(void)
{
	char *file;
	uint index;

	foreach(RWFiles, file, index)
	{
		char *rFile = combine(RDir, file);
		char *wFile = combine(WDir, file);
		char *retCode = NULL;

		cout("< %s\n", rFile);
		cout("> %s\n", wFile);

		if(existFile(wFile))
			semiRemovePath(wFile);
		else
			createPath(wFile, 'X');

		copyFile(rFile, wFile);
	}
}

static void MergeDir(char *masterDir, char *slaveDir)
{
	errorCase(m_isEmpty(masterDir));
	errorCase(m_isEmpty(slaveDir));

	masterDir = makeFullPath(masterDir);
	slaveDir = makeFullPath(slaveDir);

	errorCase(!existDir(masterDir));
	errorCase(!existDir(slaveDir));
	errorCase(!mbs_stricmp(masterDir, slaveDir)); // マスターとスレーブが同じDIR

	cout("[マスター] > %s\n", masterDir);
	cout("[スレーブ] < %s\n", slaveDir);
	cout("続行？\n");

	if(getKey() == 0x1b)
		termination(0);

	cout("続行します。\n");

	RDir = slaveDir;
	WDir = masterDir;
	RWFiles = newList();

	DoSearch();
	DoConfirm();
	DoMerge();
}
int main(int argc, char **argv)
{
	MergeDir(getArg(0), getArg(1));
}
