/*
	¼—ï.(100`466).(10000`96399)

	----

	rev_a: ¼—ï
	rev_b: rev_a ‚Ì”N‚Ì 1/1 ‚©‚ç‰½“ú–Ú‚©
	rev_c: rev_a, rev_b ‚Ì“ú‚Ì 0:0:0 ‚©‚ç‰½•bŒo‰ß‚µ‚½‚©

	rev_b ‚ª 100 –¢–‚É‚È‚ç‚È‚¢‚æ‚¤‚É rev_a ‚ğ’²®‚·‚éB
	rev_c ‚ª 10000 –¢–‚É‚È‚ç‚È‚¢‚æ‚¤‚É rev_a, rev_b ‚ğ’²®‚·‚éB

	rev_b ‚Ì‹NZ“ú‚Í 1, rec_c ‚Ì‹NZ‚Í 0 ‚Å‚ ‚é‚±‚Æ‚É’ˆÓ
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Date2Day.h"

static void MakeRev(time_t revTime, int printOnly) // revTime: 0 == Œ»
{
	stampData_t *sd = getStampDataTime(revTime);
	uint y;
	uint m;
	uint d;
	uint h;
	uint n;
	uint s;
	uint rev_a;
	uint rev_b;
	uint rev_c;
	char *rev;

	y = sd->year;
	m = sd->month;
	d = sd->day;
	h = sd->hour;
	n = sd->minute;
	s = sd->second;

	rev_a = y;
	rev_b = 1 + Date2Day(y, m, d) - Date2Day(y, 1, 1);
	rev_c = h * 3600 + n * 60 + s;

	if(rev_c < 10000)
	{
		rev_b--;
		rev_c += 86400;
	}
	if(rev_b < 100)
	{
		rev_a--;
		rev_b += Date2Day(rev_a + 1, 1, 1) - Date2Day(rev_a, 1, 1);
	}
	rev = xcout("%u.%u.%u", rev_a, rev_b, rev_c);

	cout("%s\n", rev);

	if(!printOnly)
		viewLineNoRet_NB(rev, 1);

	memFree(rev);
}
static void UnmakeRev(char *rev)
{
	autoList_t *tkns = tokenize(rev, '.');
	uint64 sec;
	uint y;
	uint m;
	uint d;
	uint s;

	errorCase(getCount(tkns) != 3);

	sec = Date2Day(toValue(getLine(tkns, 0)), 1, 1) + toValue(getLine(tkns, 1)) - 1;
	sec *= 86400;
	sec += toValue(getLine(tkns, 2));
	Day2Date(sec / 86400, &y, &m, &d);
	s = sec % 86400;

	cout("%04u/%02u/%02u %02u:%02u:%02u\n"
		,y
		,m
		,d
		,s / 3600
		,(s / 60) % 60
		,s % 60
		);

	releaseDim(tkns, 1);
}
int main(int argc, char **argv)
{
	time_t revTime = 0;
	int printOnly = 0;

	if(argIs("/D"))
	{
		stampData_t sd;

		sd.year = toValue(nextArg());
		sd.month = toValue(nextArg());
		sd.day = toValue(nextArg());
		sd.weekday = 0;
		sd.hour = toValue(nextArg());
		sd.minute = toValue(nextArg());
		sd.second = toValue(nextArg());

		errorCase(!isAllowStampData(&sd));
		cout("%I64d\n", stampDataToTime(&sd));
		return;
	}
	if(argIs("/T"))
	{
		cout("%I64d\n", time(NULL));
		return;
	}
	if(argIs("/U"))
	{
		UnmakeRev(nextArg());
		return;
	}
	if(argIs("/P"))
	{
		printOnly = 1;
	}
	if(hasArgs(1))
	{
		revTime = toValue64(nextArg());
		errorCase(revTime < 0);
	}

	MakeRev(revTime, printOnly);
}
