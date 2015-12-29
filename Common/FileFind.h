extern int antiSubversion;
extern int ignoreUtfPath;
extern uint findLimiter;
extern struct _finddata_t lastFindData;
extern uint lastDirCount;

autoList_t *ls(char *dir);
autoList_t *lss(char *dir);

void updateFindData(char *path);

time_t getFileAccessTime(char *file);
time_t getFileCreateTime(char *file);
time_t getFileModifyTime(char *file);
time_t getFileWriteTime(char *file);

autoList_t *lsFiles(char *dir);
autoList_t *lsDirs(char *dir);
autoList_t *lssFiles(char *dir);
autoList_t *lssDirs(char *dir);

void ls2File(char *dir, char *dirsFile, char *filesFile);
void lss2File(char *dir, char *dirsFile, char *filesFile);

void fileSearch(char *wCard, int (*action)(struct _finddata_t *));
uint fileSearchCount(char *wCard);
int fileSearchExist(char *wCard);

uint lsCount(char *dir);
