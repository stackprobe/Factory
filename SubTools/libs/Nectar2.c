#include "Nectar2.h"

enum
{
	E_SEND,
	E_RECV,
	E_BIT_0,
	E_BIT_1,
	E_BIT_2,
	E_BIT_3,
	E_BIT_4,
	E_BIT_5,
	E_BIT_6,
	E_BIT_7,

	E_MAX, // == NECTAR2_E_MAX
};

// cdtor >

Nectar2_t *CreateNectar2(char *ident)
{
	Nectar2_t *i = (Nectar2_t *)memAlloc(sizeof(Nectar2_t));
	uint index;

	for(index = 0; index < E_MAX; index++)
	{
		i->Handles[index] = eventOpen_x(xcout("Nectar2_%s_%u", ident, index));
	}
	return i;
}
void ReleaseNectar2(Nectar2_t *i)
{
	uint index;

	for(index = 0; index < E_MAX; index++)
	{
		handleClose(i->Handles[index]);
	}
	memFree(i);
}

// < cdtor
