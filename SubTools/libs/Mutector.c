#include "Mutector.h"

#define COMMON_ID "{fab3c841-8891-4273-8bd1-50525845fea7}"

enum
{
	M_Sender,
	M_Recver,
	M_Sync_0,
	M_Sync_1,
	M_Sync_2,
	M_Bit_0_0,
	M_Bit_0_1,
	M_Bit_0_2,
	M_Bit_1_0,
	M_Bit_1_1,
	M_Bit_1_2,

	M_MAX, // == MUTECTOR_MTX_MAX
};

Mutector_t *CreateMutector(char *name)
{
	Mutector_t *i = (Mutector_t *)memAlloc(sizeof(Mutector_t));
	char *ident;
	uint index;

	sha512_evacuate();
	{
		sha512_makeHashLine(name);
		sha512_128_makeHexHash();

		ident = strx(sha512_hexHash);
	}
	sha512_unevacuate();

	for(index = 0; index < MUTECTOR_MTX_MAX; index++)
	{
		i->Mtxs[index] = mutexOpen_x(xcout("%s_%u", ident, index++));
		i->Statuses[index] = 0;
	}
	memFree(ident);

	return i;
}
void ReleaseMutector(Mutector_t *i)
{
	uint index;

	for(index = 0; index < MUTECTOR_MTX_MAX; index++)
	{
		if(i->Statuses[index])
			mutexRelease(i->Mtxs[index]);

		handleClose(i->Mtxs[index]);
	}
	memFree(i);
}

// <-- cdtor

static void Set(Mutector_t *i, uint index, int status)
{
	if(i->Statuses[index] ? !status : status)
	{
		if(status)
			handleWaitForever(i->Mtxs[index]);
		else
			mutexRelease(i->Mtxs[index]);

		i->Statuses[index] = status;
	}
}
static void Clear(Mutector_t *i, int status)
{
	uint index;

	for(index = 0; index < MUTECTOR_MTX_MAX; index++)
	{
		Set(i, index, status);
	}
}
static int TrySet(Mutector_t *i, uint index)
{
	errorCase(i->Statuses[index]); // この系は無いはず！

	if(handleWaitForMillis(i->Mtxs[index], 0))
	{
		i->Statuses[index] = 1;
		return 1;
	}
	return 0;
}
static int Get(Mutector_t *i, uint index)
{
	errorCase(i->Statuses[index]); // この系は無いはず！

	if(handleWaitForMillis(i->Mtxs[index], 0))
	{
		mutexRelease(i->Mtxs[index]);
		return 0;
	}
	return 1;
}

// ---- send ----

static uint M0 = 0;
static uint M1 = 1;

static void SendBit(Mutector_t *i, int b0, int b1)
{
	M0++;
	M1++;
	M0 %= 3;
	M1 %= 3;

	Set(i, M_Sync_0 + M1, 1);
	Set(i, M_Sync_0 + M0, 0);
	Set(i, M_Bit_0_0 + M1, b0);
	Set(i, M_Bit_1_0 + M1, b1);
}
void MutectorSend(Mutector_t *i, autoBlock_t *message)
{
	uint index;
	uint bit;

	Set(i, M_Sender, 1);

	// Recver.Perform() 実行中かどうか検査
	{
		if(
			TrySet(i, M_Sync_0) &&
			TrySet(i, M_Sync_1) &&
			TrySet(i, M_Sync_2)
			)
			goto endFunc; // recver is not running

		Set(i, M_Sync_0, 0);
		Set(i, M_Sync_1, 0);
		Set(i, M_Sync_2, 0);
	}

	SendBit(i, 1, 1);

	for(index = 0; index < getSize(message); index++)
	{
		int chr = getByte(message, index);

		for(bit = 1 << 7; bit; bit >>= 1)
		{
			if(chr & bit)
				SendBit(i, 0, 1);
			else
				SendBit(i, 1, 0);
		}
	}
	SendBit(i, 0, 0);
	SendBit(i, 0, 0);
	SendBit(i, 0, 0);

endFunc:
	Clear(i, 0);
}
void MutectorSendLine(Mutector_t *i, char *line)
{
	autoBlock_t gab;
	MutectorSend(i, gndBlockLineVar(line, gab));
}

// ---- recv_サーバー ----

void MutectorRecv(Mutector_t *i, int (*interlude)(void), void (*recved)(autoBlock_t *))
{
	error(); // TODO
}

// ----
