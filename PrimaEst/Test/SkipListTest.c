/**
 * @file SkipListTest.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

static
int
TestIntComparator(void* a, void* b)
{
	size_t _F = (size_t)a;
	size_t _S = (size_t)b;
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
	return printf("%d=%d ", (int)pKey, (int)pValue);
}

VOID
SkipListAddTest()
{
	const size_t nCount = 100;

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);

	for (size_t i = 1; i <= nCount; i++)
	{
		SkipListSet(psSkipList, (void*)i, (void*)i);
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

		assert((size_t)psSkipListNode->pKey == dwIdx++);
	}

	SkipListClose(psSkipList);
}

VOID
SkipListSetTest()
{
	const size_t nCount = 20;

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);

	for (size_t i = 1; i <= nCount; i++)
	{
		SkipListSet(psSkipList, (void*)i, (void*)i);
		SkipListSet(psSkipList, (void*)i, (void*)(0x400 + i));
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
			pLink[0]
		);

		assert((size_t)psSkipListNode->pKey == dwIdx &&
			(size_t)psSkipListNode->pValue == (0x400 + dwIdx));
		dwIdx++;
	}

	SkipListClose(psSkipList);
}

VOID
SkipListFindTest()
{
	const size_t nCount = 20;

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);

	for (size_t i = 1; i <= nCount; i++)
	{
		SkipListSet(psSkipList, (void*)i, (void*)i);
		SkipListSet(psSkipList, (void*)i, (void*)(0x400 + i));
	}

	for (size_t i = 1; i <= nCount; i++)
	{
		PSSkipListNode psSkipListNode = SkipListFind(psSkipList, (void*)i);
		assert(psSkipListNode->pKey == (void*)i);
	}

	PSSkipListNode psSkipListNode = SkipListFind(psSkipList, (void*)9999);
	assert(psSkipListNode == NULL);

	SkipListClose(psSkipList);
}

VOID
SkipListRemoveTest()
{
	const size_t nCount = 20;

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);

	for (size_t i = 1; i <= nCount; i++)
	{
		SkipListSet(psSkipList, (void*)i, (void*)i);
	}

	SkipListPrint(psSkipList, TestSkipListPrinter);

	for (size_t i = 2; i <= nCount; i+= 5)
	{
		SkipListRemove(psSkipList, (void*)i);

		PSSkipListNode psSkipListNode = SkipListFind(psSkipList, (void*)i);
		assert(psSkipListNode == NULL);
	}

	SkipListClear(psSkipList);
}

VOID
SkipListPrintTest()
{
	const size_t nCount = 20;

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);

	for (size_t i = 1; i <= nCount; i++)
	{
		SkipListSet(psSkipList, (void*)i, (void*)i);
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

		assert((size_t)psSkipListNode->pKey == dwIdx++);
	}

	SkipListPrint(psSkipList, TestSkipListPrinter);

	SkipListClose(psSkipList);
}