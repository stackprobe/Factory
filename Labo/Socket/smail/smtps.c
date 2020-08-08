#include "smtps.h"

void SendMail(char *smtpServer, uint portno, char *user, char *pass, char *fromMailAddress, char *toMailAddress, autoBlock_t *mail)
{
	char *upFile = makeTempPath(NULL);

	LOGPOS();

	CheckMailCommonParams(smtpServer, portno, user, pass);
	errorCase(m_isEmpty(fromMailAddress));
	errorCase(m_isEmpty(toMailAddress));
	errorCase(!mail);

	LOGPOS();

	writeBinary(upFile, mail);

	if(!IsFairMailAddress(fromMailAddress))
	{
		coutJLine_x(xcout("���M�����[���A�h���X�ɖ�肪���邽�߃��[�����M���s���܂���B[%s]\n", fromMailAddress));
		goto endFunc;
	}
	if(!IsFairMailAddress(toMailAddress))
	{
		coutJLine_x(xcout("���M�惁�[���A�h���X�ɖ�肪���邽�߃��[�����M���s���܂���B[%s]\n", toMailAddress));
		goto endFunc;
	}

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

endFunc:
	removeFile(upFile);
	memFree(upFile);

	LOGPOS();
}
