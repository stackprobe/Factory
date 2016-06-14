/*
	Members File
		グループを構成するメンバーのメールアドレスのリスト
		シンプルなメールアドレスであること。< > で括るとか認めない。ex. aaa@bbb.com
		グループ内でメールアドレスの重複は認めない。

	Group Names File
		グループ名のリスト
		並びと個数は Members File の指定順に一致する。
		<1,,09AZaz> を想定する。長さを特に制限しないが、件名に挿入するので数文字程度にしてね。
		グループ名の重複は認めない。

	Unreturn Members File
		自分のメールを自分自身に配信しないようにするメンバーのリスト
		グループを問わず Members File のメールアドレスとの完全一致

	Send Only Members File
		送信しかしないメンバーのリスト
		グループを問わず Members File のメールアドレスとの完全一致

	----

	メンバーからのメールをグループ全員に送信
	複数のグループに属するメンバーは件名の "[グループ名]" から判断 ex. "[ML]"
	判断できなかった場合は "[]" を件名にして送信元のメンバーだけに返信
*/

#include "pop.h"
#include "smtp.h"
#include "tools.h"

#define STOP_FLAG_FILE "C:\\Factory\\tmp\\mailForward_Stop.flg"

static char *PopServer;
static uint PopPortno = 110;
static char *SmtpServer;
static uint SmtpPortno = 25;
static char *PopUserName;
static char *PopPassphrase;
static char *SelfMailAddress;
static autoList_t *GroupList;
static autoList_t *GroupNameList;
static autoList_t *UnreturnMemberList;
static autoList_t *SendOnlyMemberList;
static char *CounterFileBase;

static char *GetCounterFile(char *groupName)
{
	static char *file;
	memFree(file);
	return file = xcout("%s[%s].txt", CounterFileBase, groupName);
}
static uint GetCounter(char *groupName)
{
	uint counter = 1; // 初期値

	if(existFile(GetCounterFile(groupName)))
	{
		char *line = readFirstLine(GetCounterFile(groupName));

		if(lineExp("<1,9,09>", line)) // 10億-1でカンスト
			counter = toValue(line);

		memFree(line);
	}
	cout("Counter: %u\n", counter);
	return counter;
}
static uint NextCounter(char *groupName)
{
	uint counter = GetCounter(groupName);

	writeOneLine_cx(GetCounterFile(groupName), xcout("%u", counter + 1));
	return counter;
}
static char *MakeSubjectFrom(char *groupName, char *mailFrom, uint counter)
{
	char *name = strx(mailFrom);
	char *subject;

	strchrEnd(name, '@')[0] = '\0';
	subject = xcout("[%s]%s(%u)", groupName, name, counter);
	memFree(name);
	return subject;
}
static char *MakeDateField(void)
{
	updateStampDataTime(0L);

	return xcout(
		"%s, %u %s %04u %02u:%02u:%02u +0900"
		,getEWeekDay(lastStampData.weekday)
		,lastStampData.day
		,getEMonth(lastStampData.month)
		,lastStampData.year
		,lastStampData.hour
		,lastStampData.minute
		,lastStampData.second
		);
}

static void DistributeOne(autoList_t *mail, char *groupName, char *memberFrom, char *memberTo, uint counter)
{
	char *date;
	char *sendFrom;
	char *sendTo;
	char *subject;
	char *subjectFrom;
	char *messageID;
	char *mimeVersion;
	char *contentType;
	char *contentTransferEncoding;
	char *xMailer;
	autoList_t *sendData = newList();
	uint count;

	date = GetMailHeader(mail, "Date");
	sendFrom = strx(memberFrom);
	sendTo = strx(memberTo);
	subject = GetMailHeader(mail, "Subject");
	subjectFrom = MakeSubjectFrom(groupName, memberFrom, counter);
	messageID = MakeMailMessageID(SelfMailAddress);
	mimeVersion = GetMailHeader(mail, "MIME-Version");
	contentType = GetMailHeader(mail, "Content-Type");
	contentTransferEncoding = GetMailHeader(mail, "Content-Transfer-Encoding");
	xMailer = strx("mf");

	// 日付を付け直す。
	{
		LOGPOS();
		memFree(date);
		date = MakeDateField();
		LOGPOS();
	}

	if(!contentTransferEncoding)
		contentTransferEncoding = strx("7bit");

	if(
		!date ||
		!subject ||
		!mimeVersion ||
		!contentType ||
		!contentTransferEncoding
		)
	{
		cout(
			"Header Error! %u %u %u %u %u\n"
			,date
			,subject
			,mimeVersion
			,contentType
			,contentTransferEncoding
			);
		goto endfunc;
	}

	addElement(sendData, (uint)xcout("Date: %s", date));

	/*
		docomo は Reply-To を無視する。
		vodafone も送信元が携帯のものに対しては Reply-To を無視するようだ。
	*/
	if(
		// 注意: if else の処理を入れ替えた。
		/*
		strstr(memberTo, "@docomo.ne.jp") ||
		strstr(memberTo, ".vodafone.ne.jp") || // @?.vodafone.ne.jp
		strstr(memberTo, "@gmail.com")
		*/
		strstr(memberTo, "@di.pdx.ne.jp")
		)
	{
		addElement(sendData, (uint)xcout("From: %s", memberFrom));
		addElement(sendData, (uint)xcout("To: %s", memberTo));
		addElement(sendData, (uint)xcout("Reply-To: %s", SelfMailAddress));
	}
	else
	{
		addElement(sendData, (uint)xcout("From: %s", SelfMailAddress));
		addElement(sendData, (uint)xcout("To: %s", memberTo));
	}
	addElement(sendData, (uint)xcout("Subject: %s", subjectFrom));
	addElement(sendData, (uint)xcout("Message-Id: %s", messageID));
	addElement(sendData, (uint)xcout("MIME-Version: %s", mimeVersion));
	addElement(sendData, (uint)xcout("Content-Type: %s", contentType));
	addElement(sendData, (uint)xcout("Content-Transfer-Encoding: %s", contentTransferEncoding));
	addElement(sendData, (uint)xcout("X-Mailer: %s", xMailer));
	addElement(sendData, (uint)strx(""));
	addLines_x(sendData, GetMailBody(mail));

	sendMailEx(SmtpServer, SmtpPortno, memberFrom, memberTo, sendData, 4);

endfunc:
	memFree(date);
	memFree(sendFrom);
	memFree(sendTo);
	memFree(subject);
	memFree(subjectFrom);
	memFree(messageID);
	memFree(mimeVersion);
	memFree(contentType);
	memFree(contentTransferEncoding);
	memFree(xMailer);
	releaseDim(sendData, 1);
}
static void Distribute(autoList_t *mail, autoList_t *memberList, char *groupName, char *mailFrom)
{
	char *memberFrom = NULL;
	char *member;
	uint index;
	int unreturn;
	uint counter;

	foreach(memberList, member, index)
	{
		if(strstr(mailFrom, member))
		{
			memberFrom = member;
			break;
		}
	}

//	memberFrom = refElement(memberList, findLineComp(memberList, member, strstr)); // これと同じはずだけど分かりにくいなぁ...
	errorCase(!memberFrom); // ? not found
	unreturn = findLine(UnreturnMemberList, memberFrom) < getCount(UnreturnMemberList); // ? 'memberFrom' is unreturn member

	counter = NextCounter(groupName);

	foreach(memberList, member, index)
	{
		int sendonly = findLine(SendOnlyMemberList, member) < getCount(SendOnlyMemberList); // ? 'member' is sendonly member

		if(unreturn && member == memberFrom)
		{}
		else if(sendonly)
		{}
		else
		{
			DistributeOne(mail, groupName, memberFrom, member, counter);
		}
	}
}
static void RecvEvent(autoList_t *mail)
{
	char *mailFrom = GetMailHeader(mail, "From");

	if(!mailFrom)
	{
		cout("No mailFrom!\n");
		return;
	}

	{
		autoList_t *indexes = newList();
		autoList_t *memberList;
		uint index;
		uint index_index;
		char *mail_myself = NULL;

		foreach(GroupList, memberList, index)
		{
			char *member;
			uint member_index;

			foreach(memberList, member, member_index)
			{
				if(strstr(mailFrom, member))
				{
					addElement(indexes, index);
					mail_myself = member;
				}
			}
		}

		{
			char *subject = GetMailHeader(mail, "Subject");

			if(subject)
			{
				foreach(indexes, index, index_index)
				{
					char *groupPtn = xcout("[%s]", getLine(GroupNameList, index));

					if(mbs_stristr(subject, groupPtn)) // ? ターゲット発見
					{
						setCount(indexes, 1);
						setElement(indexes, 0, index);
						memFree(groupPtn);
						break;
					}
					memFree(groupPtn);
				}
				memFree(subject);
			}
		}

		if(2 <= getCount(indexes))
		{
			char *subject = GetMailHeader(mail, "Subject");

			if(!subject || !mbs_stristr(subject, "[]")) // ? 空のグループパターンが無い
			{
				errorCase(!mail_myself);
				DistributeOne(mail, "", mail_myself, mail_myself, 0);
				setCount(indexes, 0);
			}
			memFree(subject);
		}

		foreach(indexes, index, index_index)
		{
			Distribute(mail, (autoList_t *)getElement(GroupList, index), getLine(GroupNameList, index), mailFrom);
		}
		releaseAutoList(indexes);
	}
	memFree(mailFrom);
}
static void RecvLoop(void)
{
	double lastHTm = -1.0;
	double currHTm;

	for(; ; )
	{
		autoList_t *mails = mailRecv(PopServer, PopPortno, PopUserName, PopPassphrase, 3, 1024 * 1024 * 64, 1);
		autoList_t *mail;
		uint index;
		uint currTime;
		uint waitMax;

		foreach(mails, mail, index)
		{
			/*
				即返信すると破棄されることがある？
			*/
			coSleep(3000);

			RecvEvent(mail);
		}
		releaseDim(mails, 2);

		currHTm = now() / 3600.0;

		if(index) // ? 何かメールを受信した。
			lastHTm = currHTm;

		if(lastHTm + 1.0 < currHTm) // ? 最後の受信からかなり経った。
			waitMax = 15; // 45 sec
		else
			waitMax = 3; // 9 sec

		cout("lastHTm: %f\n", lastHTm);
		cout("currHTm: %f\n", currHTm);
		cout("waitMax: %u\n", waitMax);

		for(index = 0; index < waitMax; index++)
		{
			while(hasKey())
				if(getKey() == 0x1b)
					goto endLoop;

			sleep(3000);
		}
	}
endLoop:;
}
int main(int argc, char **argv)
{
	GroupList = newList();
	UnreturnMemberList = newList();
	SendOnlyMemberList = newList();

readArgs:
	if(argIs("/PD")) // Pop server Domain
	{
		PopServer = nextArg();
		goto readArgs;
	}
	if(argIs("/PP")) // Pop server Port number
	{
		PopPortno = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/SD")) // Smtp server Domain
	{
		SmtpServer = nextArg();
		goto readArgs;
	}
	if(argIs("/SP")) // Smtp server Port number
	{
		SmtpPortno = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/U")) // User name
	{
		PopUserName = nextArg();
		goto readArgs;
	}
	if(argIs("/P")) // Passphrase
	{
		PopPassphrase = nextArg();
		goto readArgs;
	}
	if(argIs("/M")) // self Mail address
	{
		SelfMailAddress = nextArg();
		goto readArgs;
	}
	if(argIs("/F")) // members File
	{
		addElement(GroupList, (uint)readResourceLines(nextArg()));
		goto readArgs;
	}
	if(argIs("/G")) // Groups file
	{
		addElements_x(GroupList, readResourceFilesLines(nextArg()));
		goto readArgs;
	}
	if(argIs("/N")) // group Names file
	{
		GroupNameList = readResourceLines(nextArg());
		goto readArgs;
	}
	if(argIs("/UR")) // Un-Return members file
	{
		addElements_x(UnreturnMemberList, readResourceLines(nextArg()));
		goto readArgs;
	}
	if(argIs("/SO")) // Send Only members file
	{
		addElements_x(SendOnlyMemberList, readResourceLines(nextArg()));
		goto readArgs;
	}
	if(argIs("/C")) // Counter file path-base
	{
		CounterFileBase = nextArg();
		goto readArgs;
	}

	errorCase(m_isEmpty(PopServer));
	errorCase(!PopPortno || 0xffff < PopPortno);
	errorCase(m_isEmpty(SmtpServer));
	errorCase(!SmtpPortno || 0xffff < SmtpPortno);
	errorCase(m_isEmpty(PopUserName));
	errorCase(m_isEmpty(PopPassphrase));
	errorCase(m_isEmpty(SelfMailAddress));
	errorCase(!getCount(GroupList));

	{
		autoList_t *memberList;
		uint index;

		foreach(GroupList, memberList, index)
		{
			errorCase(getCount(memberList) < 2);
		}
	}
	errorCase(!GroupNameList);
	errorCase(getCount(GroupList) != getCount(GroupNameList));
	errorCase(m_isEmpty(CounterFileBase));

	GetCounter("test"); // カウンタ取得テスト

	cmdTitle("mailForward");

	SockStartup();
	RecvLoop();
	SockCleanup();

	cout("\\e\n");
}
