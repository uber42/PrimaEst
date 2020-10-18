/**
 * @file LockFreeSkipListTest.c
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
void
TestIntEraser(void* arg)
{
	UNREFERENCED_PARAMETER(arg);

	return;
}

static
void
TestIntChanger(void** pValueDest, void*	pValueSrc)
{
	*pValueDest = pValueSrc;
}

VOID
LockFreeSkipListAddTest()
{
	PSLockFreeSkipList psLockFree = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);
	
	assert(psLockFree != NULL);

	for (int i = 1; i <= 10; i++)
	{
		LockFreeSkipListSet(psLockFree, (void*)i, (void*)i);
	}
	
	DWORD dwCount = 0;
	PSLockFreeSkipListNode *psNodes = LockFreeSkipListGetAll(
		psLockFree, &dwCount);
	PSLockFreeSkipListNode psIter = psNodes[0];
	for (int i = 1; i <= 10; i++)
	{
		assert((int)psIter->pKey == i);
		psIter = psIter->pHead[0];
	}

	LockFreeSkipListClose(psLockFree);
}

VOID
LockFreeSkipListFindTest()
{
	PSLockFreeSkipList psLockFree = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	assert(psLockFree != NULL);

	for (int i = 1; i <= 10; i++)
	{
		LockFreeSkipListSet(psLockFree, (void*)i, (void*)i);
	}
	
	for (int i = 1; i <= 10; i++)
	{
		PSLockFreeSkipListNode psNode = LockFreeSkipListFind(
			psLockFree,
			(void*)i);

		assert((int)psNode->pKey == i);
	}

	LockFreeSkipListClose(psLockFree);
}


#define SKIP_LIST_TEST_THREAD_COUNT 10
#define SKIP_LIST_TEST_BOUND		10000

typedef struct _SMultiThreadTestAddArg
{
	PSLockFreeSkipList	psLockFreeSkipList;

	DWORD				dwModus;

	PCRITICAL_SECTION	crWriteLock;
} SMultiThreadTestAddArg, *PSMultiThreadTestAddArg;

static
DWORD 
WINAPI 
MultiThreadAddRoutine(
	LPVOID lpThreadParameter
)
{
	SMultiThreadTestAddArg arg = *(SMultiThreadTestAddArg*)lpThreadParameter;

	for (int i = arg.dwModus; i <= SKIP_LIST_TEST_BOUND; i += 10)
	{
		EnterCriticalSection(arg.crWriteLock);
		LockFreeSkipListSet(arg.psLockFreeSkipList, (void*)i , (void*)i);
		LeaveCriticalSection(arg.crWriteLock);
	}

	return 0;
}

static
DWORD
WINAPI
MultiThreadFindRoutine(
	LPVOID lpThreadParameter
)
{
	SMultiThreadTestAddArg arg = *(SMultiThreadTestAddArg*)lpThreadParameter;

	int nTotalMisses = 0;
	for (int i = arg.dwModus; i <= SKIP_LIST_TEST_BOUND; i += 10)
	{
		int nCurrentMisses = -1;
		PSLockFreeSkipListNode psNode;
		do
		{
			nCurrentMisses++;
			psNode = LockFreeSkipListFind(arg.psLockFreeSkipList, (void*)i);
		}
		while (psNode != NULL && (int)psNode->pKey != i);

		if (nCurrentMisses > 0)
		{
			nTotalMisses += nCurrentMisses;
		}
	}

	printf("%d\n", nTotalMisses);

	return 0;
}

VOID
LockFreeSkipListMultiThreadAddTest()
{
	PSLockFreeSkipList psLockFree = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	assert(psLockFree != NULL);

	HANDLE hThreads[SKIP_LIST_TEST_THREAD_COUNT];
	SMultiThreadTestAddArg sArgs[SKIP_LIST_TEST_THREAD_COUNT];
	CRITICAL_SECTION crWriteLock;

	InitializeCriticalSection(&crWriteLock);

	for (int i = 0; i < ARRAYSIZE(hThreads); i++)
	{
		sArgs[i].psLockFreeSkipList = psLockFree;
		sArgs[i].dwModus = i + 1;
		sArgs[i].crWriteLock = &crWriteLock;

		hThreads[i] = CreateThread(NULL, 0, MultiThreadAddRoutine, &sArgs[i], 0, NULL);
	}

	WaitForMultipleObjects(
		SKIP_LIST_TEST_THREAD_COUNT,
		hThreads,
		TRUE, INFINITE);

	DWORD dwCount = 0;
	PSLockFreeSkipListNode* psNodes = LockFreeSkipListGetAll(
		psLockFree, &dwCount);
	PSLockFreeSkipListNode psIter = psNodes[0];
	for (int i = 1; i <= SKIP_LIST_TEST_BOUND; i++)
	{
		assert((int)psIter->pKey == i);
		psIter = psIter->pHead[0];
	}

	for (int i = 0; i < SKIP_LIST_TEST_THREAD_COUNT; i++)
	{
		CloseHandle(hThreads[i]);
	}

	DeleteCriticalSection(&crWriteLock);

	LockFreeSkipListClose(psLockFree);
}

VOID
LockFreeSkipListMultiThreadFind()
{
	PSLockFreeSkipList psLockFree = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	assert(psLockFree != NULL);

	HANDLE hThreads[SKIP_LIST_TEST_THREAD_COUNT] = { 0 };
	SMultiThreadTestAddArg sArgs[SKIP_LIST_TEST_THREAD_COUNT] = { 0 };

	for (int i = 1; i <= SKIP_LIST_TEST_BOUND; i++)
	{
		LockFreeSkipListSet(psLockFree, (void*)i, (void*)i);
	}

	for (int i = 0; i < ARRAYSIZE(hThreads); i++)
	{
		sArgs[i].psLockFreeSkipList = psLockFree;
		sArgs[i].dwModus = i + 1;

		hThreads[i] = CreateThread(NULL, 0, MultiThreadFindRoutine, &sArgs[i], 0, NULL);
	}

	WaitForMultipleObjects(
		SKIP_LIST_TEST_THREAD_COUNT,
		hThreads,
		TRUE, INFINITE);


	for (int i = 0; i < SKIP_LIST_TEST_THREAD_COUNT; i++)
	{
#pragma warning(suppress: 6387)
#pragma warning(suppress: 6001)
		CloseHandle(hThreads[i]);
	}

	LockFreeSkipListClose(psLockFree);
}

VOID
LockFreeSkipListMultiThreadAddAndFind()
{
	PSLockFreeSkipList psLockFree = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	assert(psLockFree != NULL);

	HANDLE hWriteThreads[SKIP_LIST_TEST_THREAD_COUNT];
	HANDLE hReadThreads[SKIP_LIST_TEST_THREAD_COUNT];
	SMultiThreadTestAddArg sArgs[SKIP_LIST_TEST_THREAD_COUNT];
	CRITICAL_SECTION crWriteLock;


	for (int i = 1; i <= SKIP_LIST_TEST_BOUND; i += 10)
	{
		LockFreeSkipListSet(psLockFree, (void*)i, (void*)i);
	}

	InitializeCriticalSection(&crWriteLock);

	for (int i = 0; i < SKIP_LIST_TEST_THREAD_COUNT; i++)
	{
		sArgs[i].psLockFreeSkipList = psLockFree;
		sArgs[i].dwModus = i + 1;
		sArgs[i].crWriteLock = &crWriteLock;

		hWriteThreads[i] = CreateThread(NULL, 0, MultiThreadAddRoutine, &sArgs[i], 0, NULL);
		hReadThreads[i] = CreateThread(NULL, 0, MultiThreadFindRoutine, &sArgs[i], 0, NULL);
	}

	WaitForMultipleObjects(
		SKIP_LIST_TEST_THREAD_COUNT,
		hWriteThreads,
		TRUE, INFINITE);

	WaitForMultipleObjects(
		SKIP_LIST_TEST_THREAD_COUNT,
		hReadThreads,
		TRUE, INFINITE);


	for (int i = 0; i < SKIP_LIST_TEST_THREAD_COUNT; i++)
	{
#pragma warning(suppress: 6387)
#pragma warning(suppress: 6001)
		CloseHandle(hWriteThreads[i]);
#pragma warning(suppress: 6387)
#pragma warning(suppress: 6001)
		CloseHandle(hReadThreads[i]);
	}

	DeleteCriticalSection(&crWriteLock);

	LockFreeSkipListClose(psLockFree);
}

VOID
LockFreeSkipListResetTest()
{
	PSLockFreeSkipList psLockFree = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	assert(psLockFree != NULL);

	for (int i = 1; i <= SKIP_LIST_TEST_BOUND; i++)
	{
		LockFreeSkipListSet(psLockFree, (void*)i, (void*)i);
	}
	
	assert(psLockFree->dwCount == SKIP_LIST_TEST_BOUND);

	for (int i = 1; i <= 10; i++)
	{
		static DWORD dwValue = 50;
		DWORD dwKey = (SKIP_LIST_TEST_BOUND / 10) * i;

		LockFreeSkipListSet(psLockFree, (void*)dwKey, (void*)dwValue);
		
		PSLockFreeSkipListNode psNode = LockFreeSkipListFind(psLockFree, (void*)dwKey);

		assert(psNode->pValue == dwValue);
	}

	assert(psLockFree->dwCount == SKIP_LIST_TEST_BOUND);


	LockFreeSkipListClose(psLockFree);
}