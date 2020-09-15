#include "libs\Server.h"

typedef struct Info_st
{
	int Status; // "CUPL" == connected, input user, input password, logged in
	char *User;
}
Info_t;

void *CreateTelnetServerPerformInfo(void)
{
	Info_t *i = (Info_t *)memAlloc(sizeof(Info_t));

	i->Status = 'C';
	i->User = NULL;

	return i;
}
void ReleaseTelnetServerPerformInfo(void *vi)
{
	Info_t *i = (Info_t *)vi;

	memFree(i->User);
	memFree(i);
}

#define DUMMY_PROMPT_FORMAT "C:/Users/%s.ADMINISTRATOR>"

char *TelnetServerPerform(char *inputLine, void *vi)
{
	Info_t *i = (Info_t *)vi;

	switch(i->Status)
	{
	case 'C':
		i->Status = 'U';

		return strx(
			"Welcome to Masshirosoft Helmet Service\r\n"
			"\r\n"
			"login: "
			);

	case 'U':
		if(inputLine && *inputLine)
		{
			i->Status = 'P';
			i->User = strx(inputLine);
			i->User = setStrLenRng(i->User, 4, 12, '_');
			line2csym(i->User);

			return strx("password: ");
		}
		break;

	case 'P':
		if(inputLine && *inputLine)
		{
			i->Status = 'L';

			return xcout(
				"\r\n"
				"*==============================================================================\r\n"
				"Masshirosoft Helmet Server.\r\n"
				"*==============================================================================\r\n"
				"\r\n"
				DUMMY_PROMPT_FORMAT
				,i->User
				);
		}
		break;

	case 'L':
		if(inputLine)
		{
			if(*inputLine)
			{
				cout("[%s] Somebody typed: %s\n", c_makeJStamp(NULL, 1), inputLine);

				if(
					!_stricmp(inputLine, "EXIT") ||
					!_stricmp(inputLine, "QUIT") ||
					!_stricmp(inputLine, "BYE")
					)
					return NULL;

				return xcout(
					"The filename, directory name, or volume label syntax is incorrect.\r\n"
					"\r\n"
					DUMMY_PROMPT_FORMAT
					,i->User
					);
			}
			return xcout(DUMMY_PROMPT_FORMAT, i->User);
		}
		break;

	default:
		error();
	}
	return strx("");
}
