#include "Common.h"

void CheckMailCommonParams(char *server, uint portno, char *user, char *pass)
{
	errorCase(!server);
	errorCase(!lineExp("<1,300,-.09AZaz>", server));

	errorCase(!m_isRange(portno, 1, 65535));

	errorCase(!user);
	errorCase(!lineExp("<1,100,@@..09AZaz>", user)); // �󔒕s��, todo: �����镶����K�v�ɉ����Ēǉ�

	errorCase(!pass);
	errorCase(!lineExp("<1,100,09AZaz>", pass)); // �󔒕s��, todo: �����镶����K�v�ɉ����Ēǉ�
}
int IsFairMailAddress(char *mailAddress)
{
	return mailAddress && lineExp("<1,300,@@__-.09AZaz>", mailAddress);
}
