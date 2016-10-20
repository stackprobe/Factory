/*
	12 * 34 + 56 / 78
		Keisan.exe 12 * 34 = L ; 56 / 79 = R ; @L + @R
		Keisan.exe 56 / 79 = R ; 12 * 34 + @R

	12 - 34 / (56 + 78)
		Keisan.exe 56 + 78 !/ 34 !- 12

	ÇQÇÃïΩï˚ç™Çè¨êîëÊ100à Ç‹Ç≈ãÅÇﬂÇÈÅB
		Keisan.exe /b 100 2 r 2

	èàóù       ãLçÜ   ç∂çÄ     âEçÄ       ï]âøíl     ê∏ìx
	---------------------------------------------------------
	â¡éZ       +      ëΩî{í∑   ëΩî{í∑     ëΩî{í∑     ä€Çﬂñ≥Çµ
	å∏éZ       -      ëΩî{í∑   ëΩî{í∑     ëΩî{í∑     ä€Çﬂñ≥Çµ
	èÊéZ       *      ëΩî{í∑   ëΩî{í∑     ëΩî{í∑     ä€Çﬂñ≥Çµ
	èúéZ       /      ëΩî{í∑   ëΩî{í∑     ëΩî{í∑     è≠êîëÊ basement à Ç‹Ç≈, è≠êîëÊ basement + 1 à à»â∫êÿÇËéÃÇƒ
	èËó]       M      ëΩî{í∑   ëΩî{í∑     ëΩî{í∑     ä€Çﬂñ≥Çµ
	Ç◊Ç´èÊ     P      ëΩî{í∑   10êiuint   ëΩî{í∑     ä€Çﬂñ≥Çµ
	Ç◊Ç´ç™     R      ëΩî{í∑   10êiuint   ëΩî{í∑     è≠êîëÊ basement à Ç‹Ç≈, è≠êîëÊ basement + 1 à à»â∫êÿÇËéÃÇƒ
	ëŒêî       L      ëΩî{í∑   ëΩî{í∑     10êiuint   êÆêî,                   è≠êîëÊ 1 à à»â∫êÿÇËéÃÇƒ
	êiêîïœä∑   X      ëΩî{í∑   10êiuint   ëΩî{í∑     è≠êîëÊ basement à Ç‹Ç≈, è≠êîëÊ basement + 1 à à»â∫êÿÇËéÃÇƒ
	êÿÇËéÃÇƒ   I      ëΩî{í∑   10êiuint   ëΩî{í∑     è¨êîëÊ [âEçÄ] à Ç‹Ç≈, è≠êîëÊ [âEçÄ] + 1 à à»â∫êÿÇËéÃÇƒ

	'10êiuint' ÇÕ radix, basement Ç…ä÷ÇÌÇÁÇ∏ 10 êiêî 0 Å` 4294967295 ÇÃêÆêî

	ì¡éÍÇ»â
		x / 0   -> 0
		Å`1 L x -> 0
		x L Å`1 -> 0
		ëŒêîÇÃâÇ™ 2^32 à»è„ÇÃèÍçá -> ïsíË
*/

#include "C:\Factory\Common\all.h"
#include "C:\Factory\Common\Options\BigInt.h"
#include "C:\Factory\Common\Options\Calc.h"

static char *LeftPad(char *line)
{
	if(strlen(line) & 1)
	{
		static char *padLine;

		memFree(padLine);
		padLine = xcout("0%s", line);
		line = padLine;
	}
	return line;
}
static autoList_t *MakeHexOp(char *line)
{
	autoBlock_t *bop = makeBlockHexLine(LeftPad(line));
	autoList_t *op = newList();
	uint index;
	uint value;

	reverseBytes(bop);

	for(index = 0; index < (getSize(bop) + 3) / 4; index++)
	{
		value = refByte(bop, index * 4) +
			refByte(bop, index * 4 + 1) * 0x100 +
			refByte(bop, index * 4 + 2) * 0x10000 +
			refByte(bop, index * 4 + 3) * 0x1000000;

		addElement(op, value);
	}
	releaseAutoBlock(bop);
	return op;
}
static void DispHexOp(autoList_t *op, uint bound, char *title)
{
	uint index;
	uint value;

	op = copyAutoList(op);

	if(getCount(op) < bound)
	{
		setCount(op, bound);
	}
	reverseElements(op);
	cout("%s: ", title);

	foreach(op, value, index)
	{
		if(index)
		{
			cout(":");
		}
		cout("%08x", value);
	}
	cout("\n");
	releaseAutoList(op);
}
static void HexKeisan(void)
{
	uint bound = 8;
	autoList_t *op1;
	autoList_t *op2;
	autoList_t *op3;
	autoList_t *ans;
	int op;

// readArgs:
	if(argIs("/B")) // Bound
	{
		bound = toValue(nextArg());
//		goto readArgs;
	}

	op1 = MakeHexOp(nextArg());

	while(hasArgs(2))
	{
		op = nextArg()[0];
		op2 = MakeHexOp(nextArg());

		cout("OPERATOR: %c\n", op);
		DispHexOp(op1, bound, "OP-001");
		DispHexOp(op2, bound, "OP-002");

		switch(m_tolower(op))
		{
		case '+': ans = addBigInt(op1, op2); break;
		case '-':
			ans = subBigInt(op1, op2);
			errorCase(!ans);
			break;

		case '*': ans = mulBigInt(op1, op2); break;
		case '/': ans = divBigInt(op1, op2, NULL); break;
		case 'm': ans = modBigInt(op1, op2); break;
		case 'p': ans = powerBigInt(op1, op2); break;
		case 'o':
			op3 = MakeHexOp(nextArg());
			DispHexOp(op3, bound, "OP-003");
			ans = modPowerBigInt(op1, op2, op3);
			releaseAutoList(op3);
			break;

		default:
			error();
		}
		DispHexOp(ans, bound, "ANSWER");

		releaseAutoList(op1);
		releaseAutoList(op2);

		op1 = ans;
	}
	releaseAutoList(op1);
}

static autoList_t *MemoryKeys;
static autoList_t *MemoryValues;

static void InitMemory(void)
{
	MemoryKeys = newList();
	MemoryValues = newList();
}
static void FnlzMemory(void)
{
	releaseDim(MemoryKeys, 1);
	releaseDim(MemoryValues, 1);
}
static void SetMemory(char *key, char *value)
{
	uint index = findLineCase(MemoryKeys, key, 1);

	if(index < getCount(MemoryKeys))
	{
		memFree(getLine(MemoryValues, index));
		setElement(MemoryValues, index, (uint)strx(value));
	}
	else
	{
		addElement(MemoryKeys, (uint)strx(key));
		addElement(MemoryValues, (uint)strx(value));
	}
}
static char *GetMemory(char *key)
{
	return getLine(MemoryValues, findLineCase(MemoryKeys, key, 1));
}

static char *InsSepOp(char *line, uint ranks)
{
	if(ranks) // x , n -> x Ç… n åÖçèÇ›Ç≈ ',' Çì¸ÇÍÇÈÅB
	{
		uint decpidx = (uint)strchrEnd(line, '.') - (uint)line;
		uint index;

		for(index = decpidx + ranks + 1; index < strlen(line); index += ranks + 1) {
			line = insertChar(line, index, ',');
		}
		for(index = decpidx; ranks < index; ) {
			index -= ranks;
			line = insertChar(line, index, ',');
		}
		if(startsWith(line, "-,")) eraseChar(line + 1);
	}
	else // x , 0 -> í∑Ç≥Çï‘Ç∑ÅB
	{
		char *swrk = xcout("%u", strlen(line));
		memFree(line);
		line = swrk;
	}
	return line;
}
static void Main2(void)
{
	uint radix = 10;
	uint basement = 10;
	char *op1;
	char *pop;
	char *op2;
	char *ans;
	int noShowMarume = 0;
	int onceNoShowMarume = 0;

	InitMemory();

	if(argIs("/H")) // Hex
	{
		HexKeisan();
		goto endFunc;
	}

readArgs:
	if(argIs("/R") || argIs("/X")) // Radix
	{
		radix = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/B")) // Basement
	{
		basement = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/D"))
	{
		calcBracketedDecimalMin = toValue(nextArg());
		goto readArgs;
	}
	if(argIs("/-M"))
	{
		noShowMarume = 1;
		goto readArgs;
	}

	op1 = strx(nextArg());

	while(hasArgs(2))
	{
		pop= nextArg();
		op2 = strx(nextArg());
		onceNoShowMarume = 0;

		if(op2[0] == '@')
		{
			ans = strx(GetMemory(op2 + 1));
			memFree(op2);
			op2 = ans;
		}
		calcLastMarume = 0;
	reopration:
		switch(m_tolower(*pop))
		{
		case '!':
			{
				char *swapop = op1;
				op1 = op2;
				op2 = swapop;
			}
			pop++;
			goto reopration;

		case '+': ans = calcLine(op1, '+', op2, radix, 0); break;
		case '-': ans = calcLine(op1, '-', op2, radix, 0); break;
		case '*': ans = calcLine(op1, '*', op2, radix, 0); break;
		case '/': ans = calcLine(op1, '/', op2, radix, basement); break;
		case 'm':
			{
				char *opd = calcLine(op1, '/', op2, radix, 0);
				char *opm = calcLine(opd, '*', op2, radix, 0);
				ans = calcLine(op1, '-', opm, radix, 0); // ïÑçÜÇÕ op1 Ç…àÍívÇ∑ÇÈÅB
				memFree(opd);
				memFree(opm);
				calcLastMarume = 0;
			}
			break;

		case 'p': ans = calcPower(op1, toValue(op2), radix); break;
		case 'r': ans = calcRootPower(op1, toValue(op2), radix, basement); break;
		case 'l': ans = xcout("%u", calcLogarithm(op1, op2, radix)); break;

		case 'b': ans = strx(op1); basement = toValue(op2); break;
		case 'x':
			{
				uint newRadix = toValue(op2);
				ans = changeRadixCalcLine(op1, radix, newRadix, basement);
				radix = newRadix;
			}
			break;

		case 'i': ans = calcLine(op1, '/', "1", radix, toValue(op2)); onceNoShowMarume = 1; break;
		case '=': ans = strx(op1); SetMemory(op2, op1); break;
		case ';': ans = strx(op2); break;
		case ',': ans = InsSepOp(strx(op1), toValue(op2)); break;

		default:
			error();
		}
		if(calcLastMarume && !noShowMarume && !onceNoShowMarume)
		{
			char *tmp;

			if(m_tolower(*pop) == 'l')
				tmp = calcLineToMarume(ans, 0);
			else
				tmp = calcLineToMarume(ans, basement);

			cout("%s\n", tmp);
			memFree(tmp);
		}
		else
			cout("%s\n", ans);

		memFree(op1);
		memFree(op2);

		op1 = ans;
	}
	memFree(op1);

endFunc:
	FnlzMemory();
}
int main(int argc, char **argv)
{
	Main2();
	termination(0);
}
