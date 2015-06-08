#include "BitList.h"

bitList_t *newBitList(void)
{
	bitList_t *i = memAlloc(sizeof(bitList_t));

	i->Buffer = newList();

	return i;
}
void releaseBitList(bitList_t *i)
{
	releaseAutoList(i->Buffer);
	memFree(i);
}

// <-- cdtor

uint refBit(bitList_t *i, uint index)
{
	return refElement(i->Buffer, index / 32) >> index % 32 & 1;
}
void putBit(bitList_t *i, uint index, uint value)
{
	uint c = refElement(i->Buffer, index / 32);

	if(value)
		c |= 1 << (index % 32);
	else
		c &= ~(1 << (index % 32));

	putElement(i->Buffer, index / 32, c);
}
uint getBitListSize(bitList_t *i) // ret: ÅŒã‚Ì‚P‚ðŠÜ‚ÞÅ¬‚ÌƒTƒCƒY
{
	uint c;

	for(c = getCount(i->Buffer) * 32; c; c--)
		if(refBit(i, c - 1))
			break;

	return c;
}

// <-- accessor
