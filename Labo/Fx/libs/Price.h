typedef struct Price_st
{
	double Bid;
	double Ask;
}
Price_t;

// ---- cdtor ----

Price_t *CreatePrice(double bid, double ask);
void ReleasePrice(Price_t *i);

// ---- accessor ----

double P_GetBid(Price_t *i);
double P_GetAsk(Price_t *i);
double P_GetMid(Price_t *i);

// ----
