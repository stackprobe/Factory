void DC_ToFairCatalog(autoList_t *catalog);
autoList_t *DC_GetCatalog(char *dir);
int DC_CreateDir(char *dir);
int DC_RemoveDir(char *dir);
int DC_RemoveFile(char *file);
int DC_AddFilePart(char *wFile, uint64 startPos, autoBlock_t *rData);
autoBlock_t *DC_GetFilePart(char *rFile, uint64 startPos, uint readSize);
int DC_SetFileTime(char *wFile, uint64 stamp);
