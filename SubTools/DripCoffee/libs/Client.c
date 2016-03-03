#include "Client.h"

// ---- Tools ----

static char *StampToLine(uint64 stamp)
{
	uint y;
	uint m;
	uint d;
	uint h;
	uint i;
	uint s;

	s = (uint)(stamp % 100);
	stamp /= 100;
	i = (uint)(stamp % 100);
	stamp /= 100;
	h = (uint)(stamp % 100);
	stamp /= 100;
	d = (uint)(stamp % 100);
	stamp /= 100;
	m = (uint)(stamp % 100);
	stamp /= 100;
	y = (uint)stamp;

	return xcout("%u 年 %02u 月 %02u 日 %02u 時 %02u 分 %02u 秒", y, m, d, h, i, s);
}

// ---- Perform ----

static FILE *RFp;
static FILE *WFp;

static void Prm_Start(void)
{
	error(); // TODO
}
static void Prm_AddLine(char *line)
{
	error(); // TODO
}
static void Prm_AddBlock(autoBlock_t *block)
{
	error(); // TODO
}
static int Idle(void)
{
	return 1;
}
static void Perform(void)
{
	error(); // TODO
}
static char *Ans_GetLine(void)
{
	error(); // TODO
	return NULL;
}
static autoBlock_t *Ans_GetBlock(void)
{
	error(); // TODO
	return NULL;
}
static void Ans_End(void)
{
	error(); // TODO
}

// ----

int DCC_Echo(void)
{
	char *line;
	int ret;

	Prm_Start();
	Prm_AddLine("ECHO");

	Perform();

	line = Ans_GetLine();

	Ans_End();

	ret = !strcmp(line, "DRIP-COFFEE_OK");
	memFree(line);
	return ret;
}
char *DCC_Lock(char *lockerInfo) // ret: エラー情報, NULL == 成功
{
	char *result;
	char *retLockerInfo;
	uint64 stamp;
	char *lStamp;
	char *ret;

	errorCase(!lockerInfo);

	Prm_Start();
	Prm_AddLine("LOCK");
	Prm_AddLine(lockerInfo);

	Perfrom();

	result = Ans_GetLine();
	retLockerInfo = Ans_GetLine();
	stamp = toValue64_x(Ans_GetLine());

	Ans_End();

	lStamp = StampToLine(stamp);

	if(!strcmp(result, "SUCCESSFUL"))
	{
		ret = NULL;
	}
	else if(!strcmp(result, "FAILED"))
	{
		ret = xcout(
			"指定されたリポジトリ・パスはロックされています。\n"
			"ロックしているユーザー：\n"
			"%s\n"
			"ロックした日時：\n"
			"%s"
			,retLockerInfo
			,lStamp
			);
	}
	else
	{
		ret = strx("ロックに失敗しました。");
	}
	memFree(result);
	memFree(retLockerInfo);
	memFree(lStamp);
	return ret;
}

// TODO
