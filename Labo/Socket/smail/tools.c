#include "tools.h"

static char *GetMailAddressByHeaderValue(char *value)
{
	char *mailAddress = NULL;
	char *p = strrchr(value, '<'); // ÅŒã‚Ì

	if(p)
	{
		char *q = strchr(p + 1, '>');

		if(q)
			mailAddress = strxl(p, (uint)q - (uint)p);
	}
	if(!mailAddress)
		mailAddress = strx(value);

	ucTrim(mailAddress);
	return mailAddress;
}

static autoList_t *MP_HeaderKeys;
static autoList_t *MP_HeaderValues;
static autoBlock_t *MP_Body;

void MailParser(autoBlock_t *mail)
{
	error(); // TODO
}
char *MP_GetHeaderValue(char *targKey)
{
	char *key;
	uint index;

	errorCase(m_isEmpty(targKey));

	errorCase(!MP_HeaderKeys);
	errorCase(!MP_HeaderValues); // 2bs

	foreach(MP_HeaderKeys, key, index)
		if(!_stricmp(key, targKey))
			return getLine(MP_HeaderValues, index);

	return NULL;
}
autoBlock_t *MP_GetBody(void)
{
	errorCase(!MP_Body);

	return MP_Body;
}
