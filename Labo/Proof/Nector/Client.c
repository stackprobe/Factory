/*
	START Server.exe

	で、起動しておいて、

	Client.exe 朝比奈みらいちゃんのなんたらかんたら...

	ってやる。
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\SubTools\libs\Nector.h"

int main(int argc, char **argv)
{
	Nector_t *i = CreateNector("Nector_Test");

	NectorSendLine(i, nextArg());
	ReleaseNector(i);
}
