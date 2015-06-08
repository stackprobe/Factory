#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\csv.h"
#include "libs\BcpReader.h"

int main(int argc, char **argv)
{
	char *bcpFile;
	char *csvFile;
	autoList_t *table;

	bcpFile = nextArg();
	csvFile = nextArg();

	cout("< %s\n", bcpFile);
	cout("> %s\n", csvFile);

	table = SqlBcpReader(bcpFile);
	writeCSVFile_cx(csvFile, table);
}
