#include "MergeSort.h"

#define READER_MAX 16

typedef struct Reader_st
{
	FILE *FP;
	uint Element;
}
Reader_t;

sint (*CR_Comp)(uint, uint);

static sint CompReader(uint v1, uint v2)
{
	Reader_t *r1 = (Reader_t *)v1;
	Reader_t *r2 = (Reader_t *)v2;

	if(!r1->Element)
		return r2->Element ? 1 : 0;

	if(!r2->Element)
		return -1;

	return CR_Comp(r1->Element, r2->Element);
}
static char *GetPartFile(char *partsDir, uint partIndex)
{
	static char *file;

	memFree(file);
	file = combine_cx(partsDir, xcout("%010u.part", partIndex));
	return file;
}
static void CommitPart(
	char *partsDir,
	uint partIndex,
	char *wMode,
	void (*writeElement_x)(FILE *fp, uint element),
	sint (*compElement)(uint element1, uint element2),
	autoList_t *elements
	)
{
LOGPOS();
	rapidSort(elements, compElement);
LOGPOS();

	{
		FILE *fp = fileOpen(GetPartFile(partsDir, partIndex), wMode);
		uint element;
		uint index;

		foreach(elements, element, index)
			writeElement_x(fp, element);

		fileClose(fp);
		releaseAutoList(elements);
	}
}
static void MergePart(
	char *partsDir,
	uint partCount,
	char *destFile,
	char *rMode,
	char *wMode,
	uint (*readElement)(FILE *fp),
	void (*writeElement_x)(FILE *fp, uint element),
	sint (*compElement)(uint element1, uint element2)
	)
{
	uint rIndex;
	uint wIndex;
	uint readerCount;
	autoList_t *readers;
	Reader_t readerList[READER_MAX];
	uint index;
	FILE *wfp;

	errorCase(READER_MAX < 2); // 2bs
	errorCase(partCount < 2); // 2bs

	rIndex = 0;
	wIndex = partCount;

	CR_Comp = compElement;

	for(; ; )
	{
		readerCount = m_min(READER_MAX, wIndex - rIndex);
		readers = newList();

		for(index = 0; index < readerCount; index++)
		{
			FILE *fp = fileOpen(GetPartFile(partsDir, rIndex + index), rMode);

			readerList[index].FP = fp;
			readerList[index].Element = readElement(fp);

			addElement(readers, (uint)(readerList + index));
		}
		insertSort(readers, CompReader);

		rIndex += readerCount;
		wfp = fileOpen(rIndex == wIndex ? destFile : GetPartFile(partsDir, wIndex), wMode);

		for(; ; )
		{
			Reader_t *r = (Reader_t *)getElement(readers, 0);

			if(!r->Element)
				break;

			writeElement_x(wfp, r->Element);
			r->Element = readElement(r->FP);

			for(index = 1; index < readerCount; index++)
			{
				if(CompReader(getElement(readers, index - 1), getElement(readers, index)) <= 0)
					break;

				swapElement(readers, index - 1, index);
			}
		}
		fileClose(wfp);

		for(index = 0; index < readerCount; index++)
		{
			fileClose(readerList[index].FP);
			removeFile(GetPartFile(partsDir, rIndex - readerCount + index));
		}
		releaseAutoList(readers);

		if(rIndex == wIndex)
			break;

		wIndex++;
	}
}

/*
	srcFile
		読み込み元ファイル

	destFile
		出力先ファイル
		srcFile と同じファイルであっても良い。

	textMode
		真のとき -> テキストモード
		偽のとき -> バイナリーモード

	uint readElement(FILE *fp)
		１レコード読み込む。
		これ以上レコードが無いときは 0 を返すこと。

	void writeElement_x(FILE *fp, uint element)
		１レコード書き込む。
		必要であれば element を開放すること。

	sint compElement(uint element1, uint element2)
		element1 と element2 を比較した結果を strcmp() 的に返す。

	partSize
		メモリに一度期に読み込める「レコードの合計バイト数」の最大値の目安
		srcFile のシーク位置の変化をバイトに換算しているだけ。
		１レコード毎に recordConstWeightSize を加える。
		0 のときは常に１レコードずつになる。

	recordConstWeightSize
		100 くらいを指定してね。

*/
void MergeSort(
	char *srcFile,
	char *destFile,
	int textMode,
	uint (*readElement)(FILE *fp),
	void (*writeElement_x)(FILE *fp, uint element),
	sint (*compElement)(uint element1, uint element2),
	uint partSize,
	uint recordConstWeightSize
	)
{
	char *rMode;
	char *wMode;
	FILE *fp;
	char *partsDir = makeTempPath("parts");
	uint partCount = 0;
	autoList_t *elements = NULL;
	uint64 startPos = 0;

	if(textMode)
	{
		rMode = "rt";
		wMode = "wt";
	}
	else
	{
		rMode = "rb";
		wMode = "wb";
	}
	fp = fileOpen(srcFile, rMode);
	createDir(partsDir);

	for(; ; )
	{
		uint element = readElement(fp);
		uint64 currPos;
		uint64 readSize;

		if(!element)
			break;

		if(!elements)
			elements = createAutoList(partSize / 1024); // XXX

		addElement(elements, element);

		currPos = _ftelli64(fp);
		errorCase(currPos < 0);
		readSize = currPos - startPos;
		readSize += (uint64)getCount(elements) * recordConstWeightSize;

		if(partSize < readSize)
		{
			CommitPart(partsDir, partCount, wMode, writeElement_x, compElement, elements);
			partCount++;
			elements = NULL;
			startPos = currPos;
		}
	}
	if(elements)
	{
		CommitPart(partsDir, partCount, wMode, writeElement_x, compElement, elements);
		partCount++;
	}
	fileClose(fp);

	switch(partCount)
	{
	default: // 2-
		MergePart(partsDir, partCount, destFile, rMode, wMode, readElement, writeElement_x, compElement);
		break;

	case 1:
		removeFileIfExist(destFile);
		moveFile(GetPartFile(partsDir, 0), destFile);
		break;

	case 0:
		createFile(destFile);
		break;
	}
	removeDir(partsDir);
	memFree(partsDir);
}

void MergeSortTextComp(char *srcFile, char *destFile, sint (*funcComp)(char *, char *), uint partSize)
{
	MergeSort(srcFile, destFile, 1, (uint (*)(FILE *))readLine_strr, (void (*)(FILE *, uint))writeLine_x, (sint (*)(uint, uint))funcComp, partSize, 10);
	// 各行 0～2 バイトのとき、10 にしたら 1.8 GB くらい食った。100 で 180 MB くらい。100 が安全っぽい。@ 2016.3.18
	// <- readLine の中で createBlock(128); してるせいだろ。readLine -> readLine_strr にした。100 -> 10 にした。@ 2016.4.8
	// 数ギガの巨大ファイルについて、
	// 全行 0 バイトでピーク時 220 MB くらい。
	// 全行 0～2 バイト（平均 1 バイト）でピーク時 210 MB くらい。
	// 全行 0～1000 バイト（平均 500 バイト）でピーク時 140 MB くらい。
}
void MergeSortText(char *srcFile, char *destFile, uint partSize)
{
	MergeSortTextComp(srcFile, destFile, strcmp, partSize);
}
void MergeSortTextICase(char *srcFile, char *destFile, uint partSize)
{
//	MergeSortTextComp(srcFile, destFile, strcmp3, partSize); // strcmp3 <- rapidSort() に渡すので mbs_stricmp じゃマズい。<- マズくない。@ 2016.6.8
	MergeSortTextComp(srcFile, destFile, mbs_stricmp, partSize);
}
