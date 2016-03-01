/*
	                       order                                   用途
	-------------------------------------------------------------------
	rapidSortLines                        strcmp (-> simpleComp)
	rapidSortJLinesICase   mbs_stricmp -> strcmp (-> simpleComp)
	sortJLinesICase        mbs_stricmp                             ls系の戻り値
*/

#include "all.h"

void gnomeSort(autoList_t *list, sint (*funcComp)(uint, uint)) // 安定ソート
{
	uint nearidx;
	uint faridx;

	for(faridx = 1; faridx < getCount(list); faridx++)
	{
		for(nearidx = faridx; nearidx; nearidx--)
		{
			if(funcComp(getElement(list, nearidx - 1), getElement(list, nearidx)) <= 0)
				break;

			swapElement(list, nearidx - 1, nearidx);
		}
	}
}
void combSort(autoList_t *list, sint (*funcComp)(uint, uint))
{
	uint span = getCount(list);
	uint nearidx;
	uint faridx;

	for(; ; )
	{
		span *= 10;
		span /= 13;

		if(span < 2)
			break;

		if(span == 9 || span == 10)
			span = 11;

		for(nearidx = 0, faridx = span; faridx < getCount(list); nearidx++, faridx++)
		{
			if(0 < funcComp(getElement(list, nearidx), getElement(list, faridx)))
			{
				swapElement(list, nearidx, faridx);
			}
		}
	}
	gnomeSort(list, funcComp);
}
void insertSort(autoList_t *list, sint (*funcComp)(uint, uint)) // 安定ソート
{
	uint nearidx;
	uint faridx;
	uint minidx;

	for(nearidx = 0; nearidx + 1 < getCount(list); nearidx++)
	{
		minidx = nearidx;

		for(faridx = nearidx + 1; faridx < getCount(list); faridx++)
		{
			if(funcComp(getElement(list, faridx), getElement(list, minidx)) < 0)
			{
				minidx = faridx;
			}
		}
		if(minidx != nearidx)
		{
			swapElement(list, nearidx, minidx);
		}
	}
}

#define ABANDON 1022 // 2 * n
#define CUTOVER 16

/*
	クイックソートもどき
	同値が多いと遅くなることに注意 <- ならないっぽい。@ 2016.3.1
*/
void rapidSort(autoList_t *list, sint (*funcComp)(uint, uint))
{
	autoList_t *rangeStack = createAutoList(ABANDON + 2);
	uint startidx;
	uint endnextidx;
	uint nearidx;
	uint pivotidx;
	uint pivot;
	uint faridx;

	/*
	if(getCount(list) < 2) // ? ソート不要
	{
		goto endsort;
	}
	*/

	addElement(rangeStack, 0);
	addElement(rangeStack, getCount(list));

	while(getCount(rangeStack))
	{
		endnextidx = unaddElement(rangeStack);
		startidx = unaddElement(rangeStack);

		/*
		if(endnextidx < startidx + 2) // ? ソート不要
		{
			continue;
		}
		*/
		if(endnextidx < startidx + CUTOVER)
		{
			autoList_t sublist = gndSubElements(list, startidx, endnextidx - startidx);

			insertSort(&sublist, funcComp);
			continue;
		}

		nearidx = startidx;
		pivotidx = (startidx + endnextidx) / 2;
		faridx = endnextidx - 1;

		for(; ; )
		{
			pivot = getElement(list, pivotidx);

#if 0 // 同値が多いと遅くなる。@ 2016.3.1
			while(nearidx < pivotidx && funcComp(getElement(list, nearidx), pivot) <= 0)
			{
				nearidx++;
			}
			while(pivotidx < faridx && funcComp(pivot, getElement(list, faridx)) <= 0)
			{
				faridx--;
			}
#else
			while(funcComp(getElement(list, nearidx), pivot) < 0)
			{
				nearidx++;
			}
			while(funcComp(pivot, getElement(list, faridx)) < 0)
			{
				faridx--;
			}
#endif

			if(nearidx == faridx)
			{
				break;
			}

			swapElement(list, nearidx, faridx);

			if(nearidx == pivotidx)
			{
				pivotidx = faridx;
				nearidx++;
			}
			else if(faridx == pivotidx)
			{
				pivotidx = nearidx;
				faridx--;
			}
			else
			{
				nearidx++;
				faridx--;
			}
		}

		addElement(rangeStack, startidx);
		addElement(rangeStack, pivotidx);

		addElement(rangeStack, pivotidx + 1);
		addElement(rangeStack, endnextidx);

		if(ABANDON < getCount(rangeStack))
		{
			combSort(list, funcComp);
			goto endsort;
		}
	}

endsort:
	releaseAutoList(rangeStack);
}
void rapidSortLines(autoList_t *lines)
{
	rapidSort(lines, (sint (*)(uint, uint))strcmp2);
}
void rapidSortJLinesICase(autoList_t *lines)
{
	rapidSort(lines, (sint (*)(uint, uint))strcmp3);
}

sint strcmp3(char *line1, char *line2) // mbs_
{
	sint retval = mbs_stricmp(line1, line2);

	if(retval)
		return retval;

	return strcmp2(line1, line2);
}
sint strcmp2(char *line1, char *line2)
{
#if 1 // @ 2016.3.1
	return strcmp(line1, line2);
#else
	sint retval = strcmp(line1, line2);

	if(retval)
		return retval;

	return m_simpleComp(line1, line2);
#endif
}
sint simpleComp(uint v1, uint v2)
{
	return m_simpleComp(v1, v2);
/*
	if(v1 < v2)
		return -1;

	if(v2 < v1)
		return 1;

	return 0;
*/
}
sint pSimpleComp(uint v1, uint v2)
{
	return m_simpleComp(*(uint *)v1, *(uint *)v2);
}
sint pSimpleComp2(uint v1, uint v2)
{
	sint retval = pSimpleComp(v1, v2);

	if(retval)
		return retval;

	return m_simpleComp(v1, v2);
}

// list はソート済みであること。
uint binSearch(autoList_t *list, uint target, sint (*funcComp)(uint, uint))
{
	uint p = 0;
	uint q = getCount(list);

	while(p < q)
	{
		uint mid = (p + q) / 2;
		sint comp;

		comp = funcComp(target, getElement(list, mid));

		if(0 < comp)
		{
			p = mid + 1;
		}
		else if(comp < 0)
		{
			q = mid;
		}
		else
		{
			return mid;
		}
	}
	return getCount(list); // not found
}
uint binSearchLines(autoList_t *lines, char *lineFind)
{
	return binSearch(lines, (uint)lineFind, (sint (*)(uint, uint))strcmp);
}

static autoList_t *GB_List;
static uint GBL_Start;
static uint GBL_Count;
static sint GBL_Step;

static uint GBL_ToIndex(uint index)
{
	return GBL_Start + (sint)(index) * GBL_Step;
}
static uint GBL_GetFarthest(uint target, sint (*funcComp)(uint, uint))
{
	uint p = 0;
	uint q = GBL_Count;

	while(p + 1 < q)
	{
		uint mid = (p + q) / 2;

		if(funcComp(target, getElement(GB_List, GBL_ToIndex(mid)))) // ? not same
		{
			q = mid;
		}
		else
		{
			p = mid;
		}
	}
	return GBL_ToIndex(p);
}

// list はソート済みであること。
int getBound(autoList_t *list, uint target, sint (*funcComp)(uint, uint), uint bound[2]) // ret: ? found, 見つからないとき bound を変更しない。
{
	uint mid = binSearch(list, target, funcComp);

	if(mid == getCount(list))
		return 0;

	GB_List = list;
	GBL_Start = mid;
	GBL_Count = mid + 1;
	GBL_Step = -1;

	bound[0] = GBL_GetFarthest(target, funcComp);

//	GB_List = list;
//	GBL_Start = mid;
	GBL_Count = getCount(list) - mid;
	GBL_Step = 1;

	bound[1] = GBL_GetFarthest(target, funcComp);

	return 1;
}
int getBoundLines(autoList_t *lines, char *lineFind, uint bound[2])
{
	return getBound(lines, (uint)lineFind, (sint (*)(uint, uint))strcmp, bound);
}
