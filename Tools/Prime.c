#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\Prime.h"

int main(int argc, char **argv)
{
	if(argIs("/F"))
	{
		uint64 value = toValue64(nextArg());
		uint64 factors[64];
		uint index;

		Factorization(value, factors);

		cout("%I64u factors are:\n", value);

		for(index = 0; factors[index] != 0; index++)
			cout("%I64u\n", factors[index]);

		return;
	}
	if(hasArgs(2))
	{
		uint64 minval;
		uint64 maxval;
		uint64 value;

		minval = toValue64(nextArg());
		maxval = toValue64(nextArg());

		if(maxval == 0)
			maxval = UINT64MAX - 1;

		errorCase(maxval < minval);
		errorCase(maxval == UINT64MAX);

		for(value = minval; value <= maxval; value++)
			cout("%I64u %s prime.\n", value, IsPrime(value) ? "is" : "is not");

		return;
	}
	if(hasArgs(1))
	{
		uint64 value = toValue64(nextArg());

		cout("%I64u %s prime.\n", value, IsPrime(value) ? "is" : "is not");
	}

	for(; ; )
	{
		char *line;
		uint64 value;

		cout("input:\n");
		line = coInputLine();

		if(!*line)
		{
			memFree(line);
			break;
		}
		value = toValue64(line);
		cout("%I64u %s prime.\n", value, IsPrime(value) ? "is" : "is not");
		memFree(line);
	}
}
