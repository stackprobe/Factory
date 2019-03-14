/*
	DyIPChk.exe チェックするドメイン名
*/

#include "C:\Factory\Common\all.h"

#define SAVE_DATA_FILE "C:\\appdata\\DyIPChk_SaveData.txt"

#define DY_IP_CHK_COMMAND "C:\\Factory\\Labo\\Socket\\hget.exe /L http://ieserver.net/ipcheck.shtml IP.tmp"
#define IP_OUT_FILE "IP.tmp"

static uint NoIPCount;
static time_t LastUpdatedTime;

static void LoadData(void)
{
	if(existFile(SAVE_DATA_FILE))
	{
		FILE *fp = fileOpen(SAVE_DATA_FILE, "rt");

		NoIPCount = toValue(neReadLine(fp));
		LastUpdatedTime = toValue64(neReadLine(fp));

		fileClose(fp);
	}
	else
	{
		NoIPCount = 0;
		LastUpdatedTime = 0;
	}
}
static void SaveData(void)
{
	{
		FILE *fp = fileOpen(SAVE_DATA_FILE, "wt");

		writeLine_x(fp, xcout("%u", NoIPCount));
		writeLine_x(fp, xcout("%I64d", LastUpdatedTime));

		fileClose(fp);
	}
}
static char *GetIP_x(char *command)
{
	char *dir = makeTempDir("DyIPChk.tmp");
	char *ip = NULL;

	addCwd(dir);
	{
		coExecute(command);

		if(existFile(IP_OUT_FILE))
		{
			ip = readFirstLine(IP_OUT_FILE);

			coutJLine_x(xcout("Recved_IP.1=[%s]", ip)); // test

			if(!lineExp("<1,3,09>.<1,3,09>.<1,3,09>.<1,3,09>", ip)) // ? ! IP address
			{
				memFree(ip);
				ip = NULL;
			}
			removeFile(IP_OUT_FILE);
		}
	}
	unaddCwd();

	removeDir(dir);

	memFree(dir);
	memFree(command);

	coutJLine_x(xcout("Recved_IP.2=[%s]", ip ? ip : "<NULL>")); // test

	return ip;
}
static char *GetDyIP(void)
{
	return GetIP_x(strx(DY_IP_CHK_COMMAND));
}
static char *GetDomainIP(char *domain)
{
	return GetIP_x(xcout("C:\\Factory\\Tools\\Lookup.exe %s > %s", domain, IP_OUT_FILE));
}
static void CheckDyIP(char *domain)
{
	uint retCode = 0;
	char *dyIP;
	char *domainIP;
	time_t currTime = time(NULL);

	cout("domain: %s\n", domain);

	LoadData();

	cout("L.NoIPCount: %u\n", NoIPCount);
	cout("L.LastUpdatedTime: %I64d\n", LastUpdatedTime);

	dyIP = GetDyIP();
	domainIP = GetDomainIP(domain);

	if(dyIP && domainIP)
	{
		if(strcmp(dyIP, domainIP))
		{
			cout("IP不一致\n");
			retCode = 1;
		}
		NoIPCount = 0;
	}
	else
		NoIPCount++;

	if(10 < NoIPCount)
		retCode = 1;

	cout("currTime - LastUpdatedTime == %.3f\n", (currTime - LastUpdatedTime) / 86400.0);

	if(LastUpdatedTime + 35 * 86400 < currTime)
		retCode = 1;

	if(retCode == 1)
	{
		NoIPCount = 0;
		LastUpdatedTime = currTime;
	}
	cout("S.NoIPCount: %u\n", NoIPCount);
	cout("S.LastUpdatedTime: %I64d\n", LastUpdatedTime);

	SaveData();

	memFree(dyIP);
	memFree(domainIP);
	cout("retCode: %u\n", retCode);
	termination(retCode);
}
int main(int argc, char **argv)
{
	char *domain = nextArg();

	errorCase_m(!lineExp("<1,300,-.09AZaz>", domain), "不正なドメイン名");

	CheckDyIP(domain);
}
