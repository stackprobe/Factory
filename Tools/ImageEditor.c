/*
	ImageEditor.exe [‰æ‘œƒtƒ@ƒCƒ‹]
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\SubTools\libs\Image.h"

static char *ImgFile;

static void ImageEditor(void)
{
	errorCase(!ImgFile); // 2bs

	SetImageId(1);
	SetImageSize(30, 30);
	SetImageId(0);
	LoadImageFile(ImgFile);

	for(; ; )
	{
		char *command;

		cout("command ?\n");
		command = coInputLine();

		if(!_stricmp(command, "q"))
		{
			memFree(command);
			break;
		}
		if(!_stricmp(command, "s!"))
		{
			cout("Save: %s\n", ImgFile);
			SaveImageFile(ImgFile);
		}
		else if(!_stricmp(command, "s"))
		{
			char *file = toCreatableTildaPath(strx(ImgFile), IMAX);

			cout("Save: %s\n", file);
			SaveImageFile(file);
			memFree(file);
		}

		// TODO

		else
		{
			cout("unknown command !\n");
		}
		memFree(command);
	}
}
int main(int argc, char **argv)
{
	if(hasArgs(1))
		ImgFile = nextArg();
	else
		ImgFile = c_dropFile();

	ImageEditor();
}
