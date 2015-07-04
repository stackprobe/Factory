// ---- flags ----

extern int sockServerMode;

// ----

extern int lastSystemRet;

void execute(char *commandLine);
void execute_x(char *commandLine);
void coExecute(char *commandLine);
void coExecute_x(char *commandLine);
void sleep(uint millis);
void coSleep(uint millis);
void noop(void);
void noop_u(uint dummy);
void noop_uu(uint dummy1, uint dummy2);
uint getZero(void);
char *getEnvLine(char *name);
uint64 nowTick(void);
uint now(void);
int pulseSec(uint span, uint *p_nextSec);
int eqIntPulseSec(uint span, uint *p_nextSec);
uint getTick(void);
uint getUDiff(uint tick1, uint tick2);
sint getIDiff(uint tick1, uint tick2);
int isLittleEndian(void);

uint64 nextCommonCount(void);
char *makeTempPath(char *ext);
char *makeTempFile(char *ext);
char *makeTempDir(char *ext);
char *makeFreeDir(void);

char *getSelfFile(void);
char *getSelfDir(void);

char *getOutFile(char *localFile);
char *c_getOutFile(char *localFile);
void openOutDir(void);

autoList_t *tokenizeArgs(char *str);

int hasArgs(uint count);
int argIs(char *spell);
char *getArg(uint index);
char *nextArg(void);
void skipArg(uint count);
autoList_t *getFollowArgs(uint index);
autoList_t *allArgs(void);
uint getFollowArgCount(uint index);
uint getArgIndex(void);
void setArgIndex(uint index);

char *innerResPathFltr(char *path);

char *LOGPOS_Time(void);
