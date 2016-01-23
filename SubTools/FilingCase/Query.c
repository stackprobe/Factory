#include "C:\Factory\Common\all.h"
#include "libs\FilingCase.h"

static void ExecuteQuery(char *query)
{
	// TODO
}
int main(int argc, char **argv)
{
	{
		char *query = untokenize(getFollowArgs(0), " ");

		ExecuteQuery(query);

		memFree(query);
	}
}
