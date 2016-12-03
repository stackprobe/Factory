#include "C:\Factory\Common\all.h"
#include "C:\Factory\SubTools\libs\Nector.h"

static void Recved(autoBlock_t *message)
{
	char *line = ab_toLine(message);

	line2JLine(line, 1, 1, 1, 1);
	cout("[%s]\n", line);
	memFree(line);
}
int main(int argc, char **argv)
{
	Nector_t *i = CreateNector("Nector_Test");

	LOGPOS();

	while(waitKey(0) != 0x1b)
	{
		autoBlock_t *message = NectorReceipt(i);

		if(message)
		{
			Recved(message);
			releaseAutoBlock(message);
		}
	}
	LOGPOS();
	ReleaseNector(i);
}
