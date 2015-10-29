#include "GmailSend.h"

static char *GetRSAExeFile(void)
{
	static char *file;

	if(!file)
		file = GetCollaboFile("C:\\app\\Kit\\GmailSend\\GmailSend.exe");

	return file;
}

static autoList_t *ToList;
static autoList_t *CCList;
static autoList_t *BCCList;
static autoList_t *Attachments;
static char *From;
static char *Subject;
static char *Body;
static char *User;
static char *Password;
static char *Host;
static uint Port;
static int SSLDisabled;

static void INIT(void)
{
	{
		static int passed;

		if(passed)
			return;

		passed = 1;
	}

	ToList = newList();
	CCList = newList();
	BCCList = newList();
	Attachments = newList();
}
void GS_SetFrom(char *from)
{
	INIT();

	if(from)
	{
		from = strx(from);
		toAsciiLine(from, 0, 0, 0);
		strzp_x(&From, from);
	}
}
void GS_SetSubject(char *subject)
{
	errorCase(m_isEmpty(subject));

	INIT();

	line2JToken(subject, 1, 1);
	strzp_x(&Subject, subject);
}

// TODO
