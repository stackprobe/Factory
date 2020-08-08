#include "smtps.h"

void SendMail(char *smtpServer, uint portno, char *user, char *pass, char *fromMailAddress, char *toMailAddress, autoBlock_t *mail)
{
	char *upFile = makeTempPath(NULL);

	LOGPOS();

	CheckMailCommonParams(smtpServer, portno, user, pass);
	CheckMailAddress(fromMailAddress);
	CheckMailAddress(toMailAddress);
	errorCase(!mail);

	mailLock();
	LOGPOS_T();

	coExecute(xcout(
		"curl smtps://%s:%u -u %s:%s --mail-from %s --mail-rcpt %s -T \"%s\""
		,smtpServer
		,portno
		,user
		,pass
		,fromMailAddress
		,toMailAddress
		,upFile
		));

	LOGPOS_T();
	mailUnlock();

	removeFile(upFile);
	memFree(upFile);

	LOGPOS();
}
