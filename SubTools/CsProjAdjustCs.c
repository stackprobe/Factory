/*
	CsProjAdjustCs.exe PROJECT-DIR CS-REL-DIR
*/

#include "C:\Factory\Common\all.h"

int main(int argc, char **argv)
{
	char *projectDir;
	char *projectFile;
	char *csRelDir;
	char *csDir;
	char *successfulFile = makeTempPath("cs-proj-adjust-cs.tmp");

	projectDir = nextArg();
	csRelDir = nextArg();

	errorCase(!existDir(projectDir));
	errorCase(!isFairRelPath(csRelDir, 100));

	projectFile = combine_cx(projectDir, xcout("%s.csproj", getLocal(projectDir)));
	csDir = combine(projectDir, csRelDir);

	errorCase(!existFile(projectFile));
	errorCase(!existDir(csDir));

	coExecute_x(xcout("C:\\app\\Kit\\Toolkit2\\Toolkit2.exe /CS-PROJ-ADJUST-CS \"%s\" \"%s\" \"%s\" \"%s\"", projectDir, getLocal(projectFile), csRelDir, successfulFile));

	errorCase(!existFile(successfulFile));

	removeFile(successfulFile);

	memFree(projectFile);
	memFree(csDir);
	memFree(successfulFile);
}
