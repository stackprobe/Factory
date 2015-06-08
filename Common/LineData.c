#include "all.h"

tagRng_t lastTagRng;

int updateTagRng(char *str, char *bgnPtn, char *endPtn, int ignoreCase)
{
	tagRng_t *i = &lastTagRng;
	char *p = mbs_strstrCase(str, bgnPtn, ignoreCase);

	if(!p)
		return 0;

	i->bgn = p;
	i->innerBgn = p + strlen(bgnPtn);

	p = mbs_strstrCase(i->innerBgn, endPtn, ignoreCase);

#if 1
	if(!p)
		return 0;

	i->innerEnd = p;
	i->end = p + strlen(endPtn);

#else // –v
	if(p)
	{
		i->innerEnd = p;
		i->end = p + strlen(endPtn);
	}
	else
	{
		p = strchr(i->innerBgn, 0x00);

		i->innerEnd = p;
		i->end = p;
	}
#endif
	return 1;
}
