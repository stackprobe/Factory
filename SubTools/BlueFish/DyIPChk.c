#include "C:\Factory\Common\all.h"

#define SAVE_DATA_FILE "C:\\appdata\\DyIPChk_SaveData.txt"

#define DY_IP_CHK_COMMAND "C:\\Factory\\Labo\\Socket\\hget.exe /L http://ieserver.net/ipcheck.shtml IP.tmp"
#define IP_OUT_FILE "IP.tmp"

static char *LastDyIP;
static time_t LastUpdatedTime;

static void LoadData(void)
{
	if(existFile(SAVE_DATA_FILE))
	{
		FILE *fp = fileOpen(SAVE_DATA_FILE, "rt");

		LastDyIP = neReadLine(fp);
		LastUpdatedTime = toValue64(neReadLine(fp));

		fileClose(fp);
	}
	else
	{
		LastDyIP = strx("no-data");
		LastUpdatedTime = 0;
	}
}
static void SaveData(void)
{
	{
		FILE *fp = fileOpen(SAVE_DATA_FILE, "wt");

		writeLine(fp, LastDyIP);
		writeLine_x(fp, xcout("%I64d", LastUpdatedTime));

		fileClose(fp);
	}
}
static char *GetIP(char *command)
{
	char *dir = makeTempDir("DyIPChk.tmp");
	char *ip = NULL;

	addCwd(dir);
	{
		coExecute(DY_IP_CHK_COMMAND);

		if(existFile(IP_OUT_FILE))
		{
			ip = readText_b(IP_OUT_FILE);
			ucTrim(ip);

			if(!lineExp("<1,3,09>.<1,3,09>.<1,3,09>.<1,3,09>", ip)) // ? ! IP address
			{
				memFree(ip);
				ip = NULL;
			}
			removeFile(IP_OUT_FILE);
		}
	}
	unaddCwd();

	removeDir_x(dir);
	return ip;
}
static char *GetDyIP(void)
{
	return GetIP(DY_IP_CHK_COMMAND);
}
static char *GetDomainIP(char *domain)
{
	return GetIP(xcout("C:\\Factory\\Labo\\Socket\\Lookup.exe %s > %s", domain, IP_OUT_FILE)); // gomi
}
static int CheckDomainIP(void)
{
	return 1; // TODO
}
int main(int argc, char **argv)
{
	uint retCode = 0;
	char *dyIP;
	time_t currTime = time(NULL);

	LoadData();

	cout("L.LastDyIP: %s\n", LastDyIP);
	cout("L.LastUpdatedTime: %I64d\n", LastUpdatedTime);

	dyIP = GetDyIP();

	if(dyIP)
	{
		cout("dyIP: %s\n", dyIP);

		if(strcmp(LastDyIP, dyIP))
		{
			memFree(LastDyIP);
			LastDyIP = dyIP;

			retCode = 1;
		}
		else
			memFree(dyIP);
	}
	if(CheckDomainIP())
		retCode = 1;

	if(LastUpdatedTime + 35 * 86400 < currTime)
		retCode = 1;

	if(retCode == 1)
		LastUpdatedTime = currTime;

	cout("S.LastDyIP: %s\n", LastDyIP);
	cout("S.LastUpdatedTime: %I64d\n", LastUpdatedTime);

	SaveData();

	cout("retCode: %u\n", retCode);
	return retCode;
}
