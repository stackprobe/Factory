#pragma once

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Collabo.h"

int FileExporter(char *rDir, char *wDir);
int FileImporter(char *rDir);

// ---- FileExportTouchImport ----

void FileExportTouchImport(char *targetDir, int (*callback)(char *entityFile, char *realPath));

// ----
