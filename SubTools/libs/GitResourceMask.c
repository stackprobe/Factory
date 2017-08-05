#include "GitResourceMask.h"

#define FLAG_FILE "_gitresmsk"

static void MaskResImage(char *file, char *fType)
{
	createFile(file); // TODO
}
static void MaskResSound(char *file, char *fType)
{
	createFile(file); // TODO
}
static void MaskResourceFile(char *file)
{
	char *ext = getExt(file);

	     if(!_stricmp(ext, "bmp")) MaskResImage(file, "BMP");
	else if(!_stricmp(ext, "png")) MaskResImage(file, "PNG");
	else if(!_stricmp(ext, "mp3")) MaskResSound(file, "MP3");
	else if(!_stricmp(ext, "wav")) MaskResSound(file, "WAV");
}
void GitResourceMask(char *rootDir)
{
	autoList_t *files = lssFiles(rootDir);
	char *file;
	uint index;
	autoList_t *targets = newList();

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
}
