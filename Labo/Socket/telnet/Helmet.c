#include "libs\Server.h"

typedef struct Info_st
{
	int Inited;
}
Info_t;

void *CreateTelnetServerPerformInfo(void)
{
	Info_t *i = (Info_t *)memAlloc(sizeof(Info_t));

	i->Inited = 0;

	return i;
}
void ReleaseTelnetServerPerformInfo(void *vi)
{
	Info_t *i = (Info_t *)vi;

	// none

	memFree(i);
}

#define DUMMY_PROMPT "C:/Users/Administrator>"

char *TelnetServerPerform(char *inputLine, void *vi)
{
	Info_t *i = (Info_t *)vi;

	if(!i->Inited)
	{
		i->Inited = 1;

		return strx(
			"*==============================================================================\r\n"
			"Masshirosoft Helmet Server.\r\n"
			"*==============================================================================\r\n"
			"\r\n"
			DUMMY_PROMPT
			);
	}
	if(inputLine)
	{
		if(
			!_stricmp(inputLine, "EXIT") ||
			!_stricmp(inputLine, "QUIT") ||
			!_stricmp(inputLine, "BYE")
			)
			return NULL;

		if(*inputLine)
			return strx(
				"The filename, directory name, or volume label syntax is incorrect.\r\n"
				"\r\n"
				DUMMY_PROMPT
				);

		return strx(DUMMY_PROMPT);
	}
	return strx("");
}
