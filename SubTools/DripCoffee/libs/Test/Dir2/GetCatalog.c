#include "..\..\all.h"

int main(int argc, char **argv)
{
	autoList_t *catalog = DC_GetCatalog(nextArg());
	char *line;
	uint index;

	errorCase_m(!catalog, "カタログの取得に失敗しました。");

	foreach(catalog, line, index)
		cout("%s\n", line);

	releaseDim(catalog, 1);
}
