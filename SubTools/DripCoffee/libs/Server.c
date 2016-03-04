/*
	ECHO
		prm
			なし
		ans
			"DRIP-COFFEE_OK"

	GET-CATALOG
		prm
			TARGET-REL-DIR           isFairRelPath / ""
		ans
			CATALOG                  DC_ToFairCatalog

	CREATE-DIR
		prm
			PARENT-REL-DIR           isFairRelPath / ""
			TARGET-LCL-DIR           isFairLocalPath
		ans
			"SUCCESSFUL" / 切断

	DELETE-DIR
		prm
			PARENT-REL-DIR           isFairRelPath / ""
			TARGET-LCL-DIR           isFairLocalPath
		ans
			"SUCCESSFUL" / 切断

	DELETE-FILE
		prm
			PARENT-REL-DIR           isFairRelPath / ""
			TARGET-LCL-FILE          isFairLocalPath
		ans
			"SUCCESSFUL" / 切断

	UPLOAD
		prm
			PARENT-REL-DIR           isFairRelPath / ""
			TARGET-LCL-FILE          isFairLocalPath
			START-POS                0 ～ IMAX_64         # ファイル終端より先 -> エラー @ dir2.exe
			STAMP                    0 ～ IMAX_64         # YYYYMMDDhhmmssLLL
			DATA                     bin
		ans
			"SUCCESSFUL" / 切断

	DOWNLOAD
		prm
			PARENT-REL-DIR           isFairRelPath / ""
			TARGET-LCL-FILE          isFairLocalPath
			START-POS                0 ～ IMAX_64         # ファイル終端より先 -> エラー @ dir2.exe
			READ-SIZE                0 ～ IMAX            # ファイル終端より先 -> エラー @ dir2.exe
		ans
			DATA                     bin
*/

#include "Server.h"

static int Perform(char *prmFile, char *ansFile)
{
	FILE *rfp = fileOpen(prmFile, "rb");
	FILE *wfp = fileOpen(ansFile, "wb");
	int ret = 0;
	char *command;

	command = readLine(rfp);

	line2JToken(command, 0, 0);

	if(!strcmp(command, "ECHO"))
	{
		writeLine(wfp, "DRIP-COFFEE_OK");
		ret = 1;
	}
	else if(!strcmp(command, "GET-CATALOG"))
	{
		error(); // TODO
	}
	else if(!strcmp(command, "CREATE-DIR"))
	{
		error(); // TODO
	}
	else if(!strcmp(command, "DELETE-DIR"))
	{
		error(); // TODO
	}
	else if(!strcmp(command, "DELETE-FILE"))
	{
		error(); // TODO
	}
	else if(!strcmp(command, "UPLOAD"))
	{
		error(); // TODO
	}
	else if(!strcmp(command, "DOWNLOAD"))
	{
		error(); // TODO
	}
	else
	{
		cout("不明なコマンド: %s\n", command);
	}

	fileClose(rfp);
	fileClose(wfp);

	return ret;
}

#define EV_STOP "{c8724bde-ee89-4610-b125-cf948b423b1f}"

static uint EvStop;

static int Idle(void)
{
	int keep = 1;

	errorCase(!EvStop); // 2bs

	while(hasKey())
		if(getKey() == 27) // ? escape
			keep = 0;

	if(handleWaitForMillis(EvStop, 0))
		keep = 0;

	return keep;
}
void DCS_Start(uint portno)
{
	errorCase(EvStop); // ? already opened
	EvStop = eventOpen(EV_STOP);

	sockServer(Perform, portno, 20, 2100000, Idle);

	handleClose(EvStop);
	EvStop = 0;
}
void DCS_Stop(void)
{
	eventWakeup(EV_STOP);
}
