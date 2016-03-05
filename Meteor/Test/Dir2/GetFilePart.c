#include "..\..\Dir2.h"

int main(int argc, char **argv)
{
	autoBlock_t *ret = DC_GetFilePart(getArg(0), toValue64(getArg(1)), toValue(getArg(2)));
	char *tmp;

	errorCase_m(!ret, "失敗しました。");

	tmp = toPrintLine(ret, 1);

	cout("取得したサイズ = %u\n", getSize(ret));
	cout("取得したデータ = [%s]\n", tmp);

	releaseAutoBlock(ret);
	memFree(tmp);
}
