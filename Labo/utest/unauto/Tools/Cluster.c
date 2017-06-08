#include "C:\Factory\Common\all.h"

#define CLUSTER_EXE "C:\\Factory\\Tools\\Cluster.exe"

static void MakeReportFile(char *dir, char *repFile)
{
	error(); // TODO
}
static void MakeClusterI(char *rDir, char *wFile)
{
	coExecute_x(xcout("%s /I /M \"%s\" \"%s\"", CLUSTER_EXE, wFile, rDir));
}
static void UnmakeClusterI(char *rFile, char *wDir)
{
	coExecute_x(xcout("%s /R \"%s\" \"%s\"", CLUSTER_EXE, rFile, wDir));
}
static void ClusterWithInfoTest(char *dir)
{
	char *repFile1 = makeTempPath("ClusterWithInfo-Test-Report-1.txt");
	char *repFile2 = makeTempPath("ClusterWithInfo-Test-Report-2.txt");
	char *mFile = makeTempPath(NULL);
	char *wDir = makeTempPath(NULL);

	MakeReportFile(dir, repFile1);

	MakeClusterI(dir, mFile);
	UnmakeClusterI(mFile, wDir);

	MakeReportFile(wDir, repFile2);

	errorCase(!isSameFile(repFile1, repFile2));

	// ëSïîçÌèúÇµÇ»Ç¢ÅB

	memFree(repFile1);
	memFree(repFile2);
	memFree(mFile);
	memFree(wDir);
}
int main(int argc, char **argv)
{
	ClusterWithInfoTest(c_dropDir());
}
