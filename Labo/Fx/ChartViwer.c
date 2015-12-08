#include "libs\all.h"

#define MSG_MUTEX "C$$appdata$$Chart$$Command.msg mutex object"
#define MSG_FILE "C:\\appdata\\Chart\\Command.msg"

#define GRAPH_SIZE 80

static double HiPrice;
static double LowPrice;
static double Graph[GRAPH_SIZE];
static double GraphMin[GRAPH_SIZE];
static double GraphMax[GRAPH_SIZE];

static void Main2(void)
{
	for(; ; )
	{
		int key = getKey();

		if(key == 0x1b)
			break;

		if(key == 0x1f48) // è„
		{
		}
		else if(key == 0x1f50) // â∫
		{
		}
	}
}
int main(int argc, char **argv)
{
	Main2();
}
