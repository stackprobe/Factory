#include "GitResourceMask.h"

#define FLAG_FILE "_gitresmsk"

// ---- Image ----

static void MaskResImage(char *file)
{
	char *midFile = makeTempPath("png");

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" /MASK-RESOURCE-IMAGE \"%s\" \"%s\"", FILE_TOOLKIT_EXE, file, midFile));

	if(!_stricmp("png", getExt(file)))
	{
		removeFile(file);
		moveFile(midFile, file);
		createFile(midFile);
	}
	else
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" /IMG-TO-IMG \"%s\" \"%s\" 0", FILE_TOOLKIT_EXE, midFile, file));
//		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" /IMG-TO-IMG \"%s\" \"%s\"", FILE_TOOLKIT_EXE, midFile, file));
//		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" \"%s\" \"%s\"", FILE_BMPTOCSV_EXE, midFile, file));
	}
	removeFile(midFile);
	memFree(midFile);
}

// ---- Sound ----

#define WAV_HZ 8000
//#define WAV_HZ 44100

static uint MRS_GetSoundLength(char *file)
{
	char *repFile = makeTempPath(NULL);
	autoList_t *lines;
	char *line;
	uint index;
	uint ret;

	coExecute_x(xcout("START \"\" /B /WAIT \"%s\" \"%s\" 2> \"%s\"", FILE_FFPROBE_EXE, file, repFile));

	lines = readLines(repFile);

	foreach(lines, line, index)
	{
		char *p = strstr(line, "Duration:");
		char *q;

		// sample:
		// ... Duration: 00:01:12.96, start: 0.000000, ...

		if(p)
		{
			p = strchr(p, ' ');
			errorCase(!p);
			p++;

			// 行の最後かも？？？

			if(q = strchr(p, ','))
				*q = '\0';

			if(q = strchr(p, ' ')) // ブランクで終わるかも？？？
				*q = '\0';

			errorCase(!lineExp("<2,09>:<2,09>:<2,09>.<2,09>", p));

			p[2] = '\0';
			p[5] = '\0';
			p[8] = '\0';

			ret =
				(uint)d2i(
				(
					toValue(p) * 360000 +
					toValue(p + 3) * 6000 +
					toValue(p + 6) * 100 +
					toValue(p + 9)
				) / 100.0 * WAV_HZ);

			goto endLoop;
		}
	}
	error(); // ? not found "Duration:"
endLoop:

	releaseDim(lines, 1);
	removeFile(repFile);
	memFree(repFile);
	return ret;
}
static void MRS_MakeWavFile(char *file, uint length)
{
	char *csvFile = makeTempPath(NULL);
	FILE *fp;
	uint count;

	LOGPOS();
	fp = fileOpen(csvFile, "wt");

	for(count = 0; count < length; count++)
	{
		writeLine(fp, "32768,32768"); // 無音！
	}
	fileClose(fp);
	LOGPOS();

	writeWAVFileFromCSVFile(csvFile, file, WAV_HZ);
	LOGPOS();

	removeFile(csvFile);
	memFree(csvFile);
}
static void MRS_MakeSoundFile(char *rFile, char *wFile)
{
	if(!_stricmp("wav", getExt(wFile)))
	{
		moveFile(rFile, wFile);
		createFile(rFile);
	}
	else
	{
		coExecute_x(xcout("START \"\" /B /WAIT \"%s\" -i \"%s\" \"%s\"", FILE_FFMPEG_EXE, rFile, wFile));
	}
}
static void MaskResSound(char *file)
{
	char *tmpFile = makeTempPath(getExt(file));
	char *wavFile = makeTempPath("wav");
	char *outFile = makeTempPath(getExt(file));

	moveFile(file, tmpFile);

	MRS_MakeWavFile(wavFile, MRS_GetSoundLength(tmpFile));
	MRS_MakeSoundFile(wavFile, outFile);

	moveFile(outFile, file);

	removeFile(tmpFile);
	removeFile(wavFile);
	memFree(tmpFile);
	memFree(wavFile);
	memFree(outFile);
}

// ---- Other ----

static void MaskResMovie(char *file)
{
	writeOneLine(file, "//// dummy data ////"); // TODO
}

// ---- Other ----

static void MaskResOther(char *file)
{
	writeOneLine(file, "//// dummy data ////");
}

// ----

static void MaskResourceFile(char *file)
{
	char *ext = getExt(file);

	cout("* %s\n", file);

	     if(!_stricmp(ext, "bmp"  )) MaskResImage(file);
	else if(!_stricmp(ext, "jpeg" )) MaskResImage(file);
	else if(!_stricmp(ext, "jpg"  )) MaskResImage(file);
	else if(!_stricmp(ext, "png"  )) MaskResImage(file);
	else if(!_stricmp(ext, "gif"  )) MaskResImage(file);
	else if(!_stricmp(ext, "mp3"  )) MaskResSound(file);
	else if(!_stricmp(ext, "wav"  )) MaskResSound(file);
	else if(!_stricmp(ext, "mpeg" )) MaskResMovie(file);
	else if(!_stricmp(ext, "mpg"  )) MaskResMovie(file);
	else if(!_stricmp(ext, "csv"  )) MaskResOther(file);
	else if(!_stricmp(ext, "otf"  )) MaskResOther(file);
}
void GitResourceMask(char *rootDir)
{
	autoList_t *files = lssFiles(rootDir);
	char *file;
	uint index;
	autoList_t *targets = newList();

	LOGPOS();

	// 外部コマンド存在確認
	{
		errorCase(!existFile(FILE_FFMPEG_EXE));
		errorCase(!existFile(FILE_FFPROBE_EXE));
//		errorCase(!existFile(FILE_BMPTOCSV_EXE));
		errorCase(!existFile(FILE_TOOLKIT_EXE));
	}

	RemoveGitPaths(files);

	foreach(files, file, index)
	{
		if(!_stricmp(FLAG_FILE, getLocal(file)))
		{
			char *prefix = addChar(changeLocal(file, ""), '\\');
			char *subfile;
			uint subfile_index;

			foreach(files, subfile, subfile_index)
				if(startsWithICase(subfile, prefix))
					addElement(targets, (uint)subfile);

			memFree(prefix);

			distinct2(targets, simpleComp, noop_u);
		}
	}
	foreach(targets, file, index)
	{
		MaskResourceFile(file);
	}
	releaseDim(files, 1);
	releaseAutoList(targets);

	LOGPOS();
}
