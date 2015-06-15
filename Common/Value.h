#define m_min(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define m_max(v1, v2) ((v1) < (v2) ? (v2) : (v1))

#define m_minim(var, value) ((var) = m_min((var), (value)))
#define m_maxim(var, value) ((var) = m_max((var), (value)))
#define m_range(var, minval, maxval) \
	do { \
	m_maxim((var), (minval)); \
	m_minim((var), (maxval)); \
	} while(0)

extern char *decimal;
extern char *binadecimal;
extern char *octodecimal;
extern char *hexadecimal;

uint64 toValue64Digits(char *line, char *digits);
uint64 toValue64(char *line);
uint toValueDigits(char *line, char *digits);
uint toValue(char *line);

sint64 toInt64Digits(char *line, char *digits);
sint64 toInt64(char *line);
sint toIntDigits(char *line, char *digits);
sint toInt(char *line);

uint iSqrt64(uint64 value);
uint iSqrt(uint value);
uint iRoot64(uint64 value, uint extend);
uint iRoot(uint value, uint extend);

double dPow(double value, uint extend);

void valueToBlock(void *block, uint value);
uint blockToValue(void *block);
void value64ToBlock(void *block, uint64 value);
uint64 blockToValue64(void *block);

sint64 d2i64(double value);
sint d2i(double value);

// _x
uint64 toValue64Digits_xc(char *line, char *digits);
uint toValueDigits_xc(char *line, char *digits);
uint64 toValue64_x(char *line);
uint toValue_x(char *line);
