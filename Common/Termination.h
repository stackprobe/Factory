void addFinalizer(void (*finalizer)(void));
void unaddFinalizer(void (*finalizer)(void));
void runFinalizers(void);
void termination(uint errorlevel);

extern int errorOccurred;
extern char *errorPosSource;
extern uint errorPosLineno;
extern char *errorPosFunction;
extern char *errorPosMessage;

void error2(char *module, uint lineno, char *section, char *message);

#define error() \
	error2(__FILE__, __LINE__, __FUNCTION__, NULL)

#define error_m(message) \
	error2(__FILE__, __LINE__, __FUNCTION__, (message))

#define errorCase(cond) \
	do { \
	if((cond)) error(); \
	} while(0)

#define errorCase_m(cond, message) \
	do { \
	if((cond)) error_m((message)); \
	} while(0)
