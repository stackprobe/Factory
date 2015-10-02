typedef struct stampData_st
{
	uint year;  // 1970 Å` 3001
	uint month; // 1 Å` 12
	uint day;   // 1 Å` 31
	uint weekday; // 0 Å` 6 == ì˙ Å` ìy
	uint hour;   // 0 Å` 23
	uint minute; // 0 Å` 59
	uint second; // 0 Å` 59
}
stampData_t;

char *getEMonth(uint month);
char *getEWeekDay(uint weekday);
char *getJWeekDay(uint weekday);

char *makeStamp(time_t t);

extern stampData_t lastStampData;

void updateStampData(char *stamp);
void updateStampDataTime(time_t t);
stampData_t *getStampData(char *stamp);
stampData_t *getStampDataTime(time_t t);

int isAllowStampData(stampData_t *i);

char *makeJStamp(stampData_t *i, int nonWeekDay);
char *makeCompactStamp(stampData_t *i);
time_t stampDataToTime(stampData_t *i);
time_t compactStampToTime(char *stamp);

char *c_makeStamp(time_t t);
char *c_makeJStamp(stampData_t *i, int nonWeekDay);
char *c_makeCompactStamp(stampData_t *i);
