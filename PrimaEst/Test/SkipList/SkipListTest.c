#include "../../global.h"


static
int 
TestIntComparator(void* a, void* b)
{
	int _F = (int)a;
	int _S = (int)b;
	return a < b ? -1 : a > b ? 1 : 0;
}

static
int
TestSkipListPrinter(
	DWORD dwHeight,
	BOOL  fNewLevel,
	PVOID pKey,
	PVOID pValue)
{
	if (fNewLevel)
	{
		printf("\nHEIGHT : %d\n", dwHeight);
	}
	printf("%d=%d ", (int)pKey, (int)pValue);
}

VOID
AddTest()
{
	const int nCount = 100;

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);
	
	for (int i = 1; i <= nCount; i++)
	{
		SkipListAdd(psSkipList, (void*)i, (void*)i);
	}

	DWORD dwIdx = 1;

	PSList sList = SkipListGetAll(psSkipList);
	for (PSList sListCurrent = sList->pFlink;
		sListCurrent != sList;
		sListCurrent = sListCurrent->pFlink)
	{
		PSSkipListNode psSkipListNode = CONTAINING_RECORD(
			sListCurrent,
			SSkipListNode,
			pLink
		);

		assert((int)psSkipListNode->pKey == dwIdx++);
	}

	SkipListClear(psSkipList);
}

VOID
SetTest()
{

}