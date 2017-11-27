#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	for(; ; )
	{
		char *path = dropDirFile();

		if(existDir(path))
		{
			coExecute_x(xcout("Compact.exe /C /S:\"%s\"", path));
		}
		else // file
		{
			coExecute_x(xcout("Compact.exe /C \"%s\"", path));
		}
	}
}
