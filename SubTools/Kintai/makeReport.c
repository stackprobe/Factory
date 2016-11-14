/*
	[フォーマット]

	Projects.csv

		行_くりかえし
		{
			プロジェクトコード , プロジェクト名 , 担当者
		}

	TimeCard.csv

		行_くりかえし
		{
			以下いずれか
			{
				開始日時 , "開始" , プロジェクトコード , 備考
				終了日時 , "終了"
			}
		}

	開始日時, 終了日時 ... YYYYMMDDhhmm 形式, 15分単位, ex. 201611091145

	プロジェクトコード     ... line2JToken(, 1, 0) && not ""
	プロジェクト名, 担当者 ... line2JToken(, 1, 1) && not ""
	備考                   ... line2JToken(, 1, 1)
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\TimeData.h"
#include "C:\Factory\Common\Options\csv.h"
#include "C:\Factory\Common\Options\Calc2.h"

#define FILE_PROJECTS "S:\\Kintai\\Projects.csv"
#define FILE_TIMECARD "S:\\Kintai\\TimeCard.csv"

static autoList_t *ProjectsTable;
static autoList_t *TimeCardTable;
static autoList_t *ActualTimes;

typedef struct ActualTime_st
{
	uint64 StartStamp;
	uint64 EndStamp;
	char *Code;
	char *Comment;
	char *ProjName;
	char *ProjTantou;
}
ActualTime_t;

static ActualTime_t *Curr;

static void CheckStamp(uint64 stamp)
{
	uint64 retStamp;

	errorCase(!m_isRange(stamp, 100001010000, 999912312345));

	switch((uint)(stamp % 100))
	{
	case 0:
	case 15:
	case 30:
	case 45:
		break;

	default:
		error();
	}

	stamp *= 100;
	retStamp = TimeData2Stamp(TSec2TimeData(TimeData2TSec(Stamp2TimeData(stamp))));

	errorCase(stamp != retStamp);
}
static void EndActualTime(uint64 stamp)
{
	if(!Curr)
	{
		LOGPOS();
		return;
	}
	errorCase(stamp <= Curr->StartStamp);

	Curr->EndStamp = stamp;

	addElement(ActualTimes, (uint)Curr);
	Curr = NULL;
}
static void AddProjInfoToCurr(void)
{
	autoList_t *row;
	uint rowidx;

	foreach(ProjectsTable, row, rowidx)
	{
		char *code;
		char *projName;
		char *projTantou;
		uint c = 0;

		code       = getLine(row, c++);
		projName   = getLine(row, c++);
		projTantou = getLine(row, c++);

		if(!strcmp(code, Curr->Code))
		{
			Curr->ProjName   = strx(projName);
			Curr->ProjTantou = strx(projTantou);

			return;
		}
	}
	error();
}
static char *GetDate(uint64 stamp)
{
	TimeData_t td = Stamp2TimeData(stamp * 100);

	return xcout("%04u/%02u/%02u", td.Y, td.M, td.D);
}

#define REPORT_COLIDX_HOUR 1

static int IsSameActual(autoList_t *row1, autoList_t *row2)
{
	char *cell1;
	uint colidx;

	foreach(row1, cell1, colidx)
	{
		if(colidx != REPORT_COLIDX_HOUR)
		{
			char *cell2 = getLine(row2, colidx);

			if(strcmp(cell1, cell2))
			{
				return 0;
			}
		}
	}
	return 1;
}

static uint64 ReportStartStamp;
static uint64 ReportEndStamp;

static void Go2(void)
{
	autoList_t *report = newList();
	uint index;

	{
		autoList_t *row = newList();

		addElement(row, (uint)strx("日付"));
		addElement(row, (uint)strx("作業時間(h)"));
		addElement(row, (uint)strx("プロジェクトコード"));
		addElement(row, (uint)strx("プロジェクト名"));
		addElement(row, (uint)strx("担当者"));
//		addElement(row, (uint)strx("備考"));

		addElement(report, (uint)row);
	}

	foreach(ActualTimes, Curr, index)
	{
		if(m_isRange(Curr->EndStamp, ReportStartStamp, ReportEndStamp))
		{
			uint64 startTSec = TimeData2TSec(Stamp2TimeData(Curr->StartStamp * 100));
			uint64 endTSec   = TimeData2TSec(Stamp2TimeData(Curr->EndStamp   * 100));
			uint64 tSec;
			char *th;
			autoList_t *row = newList();

			tSec = endTSec - startTSec;
			th = calc_xc(xcout("%I64u", tSec), '/', "3600");

			addElement(row, (uint)GetDate(Curr->StartStamp));
			addElement(row, (uint)th);
			addElement(row, (uint)strx(Curr->Code));
			addElement(row, (uint)strx(Curr->ProjName));
			addElement(row, (uint)strx(Curr->ProjTantou));
//			addElement(row, (uint)strx(Curr->Comment));

			addElement(report, (uint)row);
		}
	}

	LOGPOS();

	// 統合
	{
		uint rowidx;

		for(rowidx = 1; rowidx < getCount(report); rowidx++)
		{
			autoList_t *row1 = getList(report, rowidx - 1);
			autoList_t *row2 = getList(report, rowidx);

			if(IsSameActual(row1, row2))
			{
				char *th1 = getLine(row1, REPORT_COLIDX_HOUR);
				char *th2 = getLine(row2, REPORT_COLIDX_HOUR);
				char *th;

				th = calc_xc(th1, '+', th2); // th1 はここで解放する。

				setElement(row1, REPORT_COLIDX_HOUR, (uint)th);
				desertElement(report, rowidx);
				releaseDim(row2, 1); // th2 はここで解放されるよ。
				rowidx--;
			}
		}
	}

	LOGPOS();

	writeCSVFile(c_getOutFile_x(xcout("作業時間_%I64u-%I64u.csv", ReportStartStamp, ReportEndStamp)), report);

	openOutDir();

	releaseDim(report, 2);
}
static void Go(void)
{
	uint rowidx;

	LOGPOS();

	ProjectsTable = readCSVFileTR(FILE_PROJECTS);
	TimeCardTable = readCSVFileTR(FILE_TIMECARD);

	errorCase(getCount(getList(ProjectsTable, 0)) != 3);
	errorCase(getCount(getList(TimeCardTable, 0)) != 4);

	LOGPOS();

	for(rowidx = 0; rowidx < getCount(ProjectsTable); rowidx++) // チェックのみ
	{
		autoList_t *row;
		char *code;
		char *projName;
		char *projTantou;
		uint c = 0;

		cout("プロジェクト・リスト %u 行目を処理します。\n", rowidx + 1);

		row = getList(ProjectsTable, rowidx);

		code       = getLine(row, c++);
		projName   = getLine(row, c++);
		projTantou = getLine(row, c++);

		errorCase(!isJToken(code, 1, 0));
		errorCase(!*code);
		errorCase(!isJToken(projName, 1, 1));
		errorCase(!*projName);
		errorCase(!isJToken(projTantou, 1, 1));
		errorCase(!*projTantou);
	}

	ActualTimes = newList();

	for(rowidx = 0; rowidx < getCount(TimeCardTable); rowidx++)
	{
		autoList_t *row;
		uint64 stamp;
		char *state;
		char *code;
		char *comment;
		uint c = 0;

		cout("タイムカード %u 行目を処理します。\n", rowidx + 1);

		row = getList(TimeCardTable, rowidx);

		stamp = toValue64(getLine(row, c++));
		state   = getLine(row, c++);
		code    = getLine(row, c++);
		comment = getLine(row, c++);

		CheckStamp(stamp);

		if(!strcmp(state, "開始"))
		{
			EndActualTime(stamp);

			errorCase(!isJToken(code, 1, 0));
			errorCase(!*code);
			errorCase(!isJToken(comment, 1, 0));

			Curr = nb(ActualTime_t);
			Curr->StartStamp = stamp;
//			Curr->EndStamp
			Curr->Code = strx(code);
			Curr->Comment = strx(comment);

			AddProjInfoToCurr();
		}
		else if(!strcmp(state, "終了"))
		{
			EndActualTime(stamp);
		}
		else
		{
			error();
		}
	}
	errorCase(Curr);

	LOGPOS();

	Go2();
}
static uint64 GetNearSundayTSec(void)
{
	TimeData_t td = GetNowTimeData();
	uint64 tSec;

	td.H = 0;
	td.I = 0;
	td.S = 0;

	tSec = TimeData2TSec(td);

	if(td.W < 3)
		tSec -= (td.W + 1) * 86400;
	else
		tSec += (6 - td.W) * 86400;

	cout("付近の日曜日 = %I64u\n", TimeData2Stamp(TSec2TimeData(tSec)));

	return tSec;
}
int main(int argc, char **argv)
{
	if(hasArgs(2))
	{
		ReportStartStamp = toValue64(nextArg());
		ReportEndStamp   = toValue64(nextArg());
	}
	else
	{
		uint64 startTSec;
		uint64 endTSec = GetNearSundayTSec();

		startTSec = endTSec - 7 * 86400;
		endTSec -= 15 * 60;

		ReportStartStamp = TimeData2Stamp(TSec2TimeData(startTSec)) / 100;
		ReportEndStamp   = TimeData2Stamp(TSec2TimeData(endTSec))   / 100;
	}

	CheckStamp(ReportStartStamp);
	CheckStamp(ReportEndStamp);

	Go();
}
