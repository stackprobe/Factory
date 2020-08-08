#include "tools.h"

static autoList_t *MP_HeaderKeys;
static autoList_t *MP_HeaderValues;
static autoBlock_t *MP_Body;

void MailParser(autoBlock_t *mail)
{
	uint rPos;

	errorCase(!mail);

	if(MP_HeaderKeys) // 前回の内容をクリア
	{
		releaseDim(MP_HeaderKeys, 1);
		releaseDim(MP_HeaderValues, 1);
		releaseAutoBlock(MP_Body);
	}
	MP_HeaderKeys = newList();
	MP_HeaderValues = newList();

	for(rPos = 0; ; )
	{
		uint i = rPos;
		char *headerLine;
		int folding;

		for(; i < getSize(mail); i++)
			if(b_(mail)[i] == '\n')
				break;

		if(i == getSize(mail))
		{
			cout("ヘッダの終端が見つかりません。\n");
			break;
		}
		b_(mail)[i] = '\0';
		headerLine = (char *)b_(mail) + rPos;
		rPos = i + 1;
		folding = m_isspace(headerLine[0]) && getCount(MP_HeaderKeys);
		ucTrimEdge(headerLine);

		if(!*headerLine)
			break;

		if(folding)
		{
			char *lastValue = (char *)unaddElement(MP_HeaderValues);

			lastValue = addChar(lastValue, ' ');
			lastValue = addLine(lastValue, headerLine);

			addElement(MP_HeaderValues, (uint)lastValue);
		}
		else
		{
			char *p = strchr(headerLine, ':');

			if(p)
			{
				char *key = headerLine;
				char *value;

				*p = '\0';
				value = p + 1;

				ucTrimEdge(key);
				ucTrimEdge(value);

				addElement(MP_HeaderKeys,   (uint)strx(key));
				addElement(MP_HeaderValues, (uint)strx(value));
			}
			else
			{
				line2JLine(headerLine, 1, 0, 0, 1); // 表示のため
				cout("不明なヘッダ行 = [%s]\n", headerLine);
			}
		}
	}
	MP_Body = getFollowBytes(mail, rPos);
}
char *MP_GetHeaderValue(char *targKey) // ret: strx(), NULL == 見つからない。
{
	char *key;
	uint index;

	errorCase(m_isEmpty(targKey));

	errorCase(!MP_HeaderKeys);
//	errorCase(!MP_HeaderValues);

	foreach(MP_HeaderKeys, key, index)
		if(!_stricmp(key, targKey))
			return strx(getLine(MP_HeaderValues, index));

	return NULL;
}
autoBlock_t *c_MP_GetBody(void)
{
	errorCase(!MP_Body);

	return MP_Body;
}
