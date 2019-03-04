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
	editLines_x(paths);
}
