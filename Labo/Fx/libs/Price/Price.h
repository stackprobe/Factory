typedef struct Price_st
{
	double Bid;
	double Ask;
}
Price_t;

Price_t *CreatePrice(double bid, double ask);
void ReleasePrice(Price_t *i);

// <-- cdtor

double GetBid(Price_t *i);
double GetAsk(Price_t *i);
double GetMid(Price_t *i);

// <-- accessor
