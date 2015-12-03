#include "Json.h"

/*
	ret:
		K, V ... •\Ž¦‰Â”\

		release:
			releaseDim(ret.K, 1);
			releaseDim(ret.V, 1);
*/
map_t JsonDecode(char *src)
{
	map_t dest;

	dest.K = newList();
	dest.V = newList();

	// TODO

	return dest;
}
