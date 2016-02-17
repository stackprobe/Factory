#include "..\..\all.h"

int main(int argc, char **argv)
{
	errorCase_m(!DC_SetFileTime(getArg(0), toValue64(getArg(1))), "é∏îsÇµÇ‹ÇµÇΩÅB"); // g
}
