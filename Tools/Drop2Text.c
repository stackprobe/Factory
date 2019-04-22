#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	autoList_t *paths = newList();

	for(; ; )
	{
		char *path = dropPath();

		if(!path)
			break;

		addElement(paths, (uint)path);
	}
	writeLines_cx("C:\\Factory\\tmp\\Drop2Text_Last.txt", editLines_x(paths));
}
