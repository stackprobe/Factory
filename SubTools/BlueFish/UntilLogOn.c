/*
	�I�͂P���
	���O�I����ʂ͉摜�A�f�X�N�g�b�v�͒P�F�Ȃ̂ŁA
	�X�N���[���L���v�`�������摜�� (0, 0) �` (99, 0) ���S�ē����F�ł���΃f�X�N�g�b�v�ƌ��Ȃ��B
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Meteor\Toolkit.h"
#include "C:\Factory\SubTools\libs\bmp.h"

static int IsLoggedOn(void)
{
	char *dir = makeTempDir(NULL);
	int ret = 0;

	addCwd(dir);
	{
		coExecute(FILE_TOOLKIT_EXE " /PRINT-SCREEN ss_");

		if(existFile("ss_01.bmp"))
		{
			autoList_t *bmp = readBMPFile("ss_01.bmp");
			autoList_t *bmpRow;
			uint x;

			bmpRow = getList(bmp, 0);

			for(x = 1; x < 100; x++)
				if(getElement(bmpRow, 0) != getElement(bmpRow, x))
					goto endBmp;

			ret = 1;

		endBmp:
			releaseDim_BR(bmp, 2, NULL);
		}
	}
	unaddCwd();

	recurRemoveDir_x(dir);
	return ret;
}
int main(int argc, char **argv)
{
	LOGPOS();
	errorCase(!existFile(FILE_TOOLKIT_EXE)); // �O���R�}���h���݊m�F

	while(!IsLoggedOn())
		if(coWaitKey(5000) == 0x1b)
			break;

	LOGPOS();
}
