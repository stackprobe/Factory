#include "libs\all.h"

#define MSG_MUTEX "C$$appdata$$Chart$$Command.msg mutex object"
#define MSG_FILE "C:\\appdata\\Chart\\Command.msg"

static void Main2(void)
{
	for(; ; )
	{
		int key = getKey();

		if(key == 0x1b)
			break;
	}
}
int main(int argc, char **argv)
{
	Main2();
}
