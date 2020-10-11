/**
 * @file Selection.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../../global.h"
#include <time.h>


/**
 * Размеры выборок
 */
static int nSelectionCount[] =
{
	100,
	1000,
	10000,
	100000,
	200000,
	300000,
	400000,
	500000,
	600000,
	700000,
	800000,
	900000,
	1000000,
	//10000000,
};

static
int
TestIntComparator(void* a, void* b)
{
	size_t _F = (size_t)a;
	size_t _S = (size_t)b;
	return a < b ? -1 : a > b ? 1 : 0;
}

LONG*
InternalSetSelection();

LONG*
InternalRemoveSelection();

LONG*
InternalFindSelection();

LONG*
InternalSetSelection()
{
	DWORD dwSize = ARRAYSIZE(nSelectionCount) * sizeof(LONG);
	LONG* pnTime = (LONG * )malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	memset(pnTime, 0, sizeof(dwSize));

	for (int i = 0; i < ARRAYSIZE(nSelectionCount); i++)
	{
		PSSkipList psSkipList = CreateSkipList(TestIntComparator);

		LONG lTime = 0;
		LONG lTotal = 0;
		
		for (size_t j = 1; j <= nSelectionCount[i]; j++)
		{
			lTime = clock();

			SkipListSet(psSkipList, (void*)j, (void*)j);

			lTotal += (clock() - lTime);
		}

		SkipListClose(psSkipList);

		pnTime[i] = lTotal;
	}

	return pnTime;
}

LONG*
InternalRemoveSelection()
{
	DWORD dwSize = ARRAYSIZE(nSelectionCount) * sizeof(LONG);
	LONG* pnTime = (LONG*)malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	memset(pnTime, 0, sizeof(dwSize));

	for (int i = 0; i < ARRAYSIZE(nSelectionCount); i++)
	{
		PSSkipList psSkipList = CreateSkipList(TestIntComparator);

		for (size_t j = 1; j <= nSelectionCount[ARRAYSIZE(nSelectionCount) - 1]; j++)
		{
			SkipListSet(psSkipList, (void*)j, (void*)j);
		}

		LONG lTime = 0;
		LONG lTotal = 0;

		for (size_t j = 1; j <= nSelectionCount[i]; j++)
		{
			int rnd = rand() % nSelectionCount[i];

			lTime = clock();

			SkipListRemove(psSkipList, (void*)rnd);

			lTotal += (clock() - lTime);
		}

		SkipListClose(psSkipList);

		pnTime[i] = lTotal;
	}

	return pnTime;
}

LONG*
InternalFindSelection()
{
	DWORD dwSize = ARRAYSIZE(nSelectionCount) * sizeof(LONG);
	LONG* pnTime = (LONG * )malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	memset(pnTime, 0, sizeof(dwSize));

	PSSkipList psSkipList = CreateSkipList(TestIntComparator);

	for (size_t j = 1; j <= nSelectionCount[ARRAYSIZE(nSelectionCount) - 1]; j++)
	{
		SkipListSet(psSkipList, (void*)j, (void*)j);
	}

	for (int i = 0; i < ARRAYSIZE(nSelectionCount); i++)
	{
		LONG lTime = 0;
		LONG lTotal = 0;
		
		for (size_t j = 1; j <= nSelectionCount[i]; j++)
		{
			int rnd = rand() % nSelectionCount[i];

			lTime = clock();

			SkipListFind(psSkipList, (void*)rnd);

			lTotal += (clock() - lTime);
		}
		
		pnTime[i] = lTotal;
	}

	SkipListClose(psSkipList);

	return pnTime;
}

 /**
  * Выборка основных операций
  * Insert, Search, Remove
  */
VOID
SelectionGeneralOperation()
{
	HANDLE hOutFile = CreateFile(
		".\\Benchmarks\\SkipList\\selection.bin",
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		0, NULL
	);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	LONG* plSetSelection = InternalSetSelection();
	LONG* plFindSelection = InternalFindSelection();
	LONG* plRemoveSelection = InternalRemoveSelection();
	
	DWORD dwBytesWrite = 0;
	WriteFile(
		hOutFile,
		plSetSelection,
		sizeof(LONG) * ARRAYSIZE(nSelectionCount),
		&dwBytesWrite, NULL);
	WriteFile(
		hOutFile,
		plFindSelection,
		sizeof(LONG) * ARRAYSIZE(nSelectionCount),
		&dwBytesWrite, NULL);
	WriteFile(
		hOutFile,
		plRemoveSelection,
		sizeof(LONG) * ARRAYSIZE(nSelectionCount),
		&dwBytesWrite, NULL);

	CloseHandle(hOutFile);

	free(plRemoveSelection);
	free(plFindSelection);
	free(plSetSelection);
}