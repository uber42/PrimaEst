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
	1000
};

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
}

static
void
TestIntChanger(void** pValueDest, void* pValueSrc)
{
	*pValueDest = pValueSrc;
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
		PSSkipList psSkipList = CreateSkipList(
			TestIntComparator,
			TestIntEraser,
			TestIntChanger);

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
		PSSkipList psSkipList = CreateSkipList(
			TestIntComparator,
			TestIntEraser,
			TestIntChanger);

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

	PSSkipList psSkipList = CreateSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

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
SkipListSelection()
{
	HANDLE hOutFile = CreateFile(
		".\\Benchmarks\\SkipList\\SkipListSelection.bin",
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

	CloseHandle(hOutFile);

	free(plFindSelection);
	free(plSetSelection);
}


LONG*
InternalLockFreeSetSelection();

LONG*
InternalLockFreeFindSelection();


LONG*
InternalLockFreeSetSelection()
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
		PSLockFreeSkipList psSkipList = CreateLockFreeSkipList(
			TestIntComparator,
			TestIntEraser,
			TestIntChanger);

		LONG lTime = 0;
		LONG lTotal = 0;

		for (size_t j = 1; j <= nSelectionCount[i]; j++)
		{
			lTime = clock();

			LockFreeSkipListSet(psSkipList, (void*)j, (void*)j);

			lTotal += (clock() - lTime);
		}

		LockFreeSkipListClose(psSkipList);

		pnTime[i] = lTotal;
	}

	return pnTime;
}

LONG*
InternalLockFreeFindSelection()
{
	DWORD dwSize = ARRAYSIZE(nSelectionCount) * sizeof(LONG);
	LONG* pnTime = (LONG*)malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	memset(pnTime, 0, sizeof(dwSize));

	PSLockFreeSkipList psSkipList = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	for (size_t j = 1; j <= nSelectionCount[ARRAYSIZE(nSelectionCount) - 1]; j++)
	{
		LockFreeSkipListSet(psSkipList, (void*)j, (void*)j);
	}

	for (int i = 0; i < ARRAYSIZE(nSelectionCount); i++)
	{
		LONG lTime = 0;
		LONG lTotal = 0;

		for (size_t j = 1; j <= nSelectionCount[i]; j++)
		{
			int rnd = rand() % nSelectionCount[i];

			lTime = clock();

			LockFreeSkipListFind(psSkipList, (void*)rnd);

			lTotal += (clock() - lTime);
		}

		pnTime[i] = lTotal;
	}

	LockFreeSkipListClose(psSkipList);

	return pnTime;
}



/**
 * Выборка основных операций LockFreeSkipList
 * Insert, Search
 */
VOID
LockFreeSkipListSelection()
{
	HANDLE hOutFile = CreateFile(
		".\\Benchmarks\\SkipList\\LockFreeSkipListSelection.bin",
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		0, NULL
	);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	LONG* plSetSelection = InternalLockFreeSetSelection();
	LONG* plFindSelection = InternalLockFreeFindSelection();

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

	CloseHandle(hOutFile);

	free(plFindSelection);
	free(plSetSelection);
}


#define SKIP_LIST_BENCHMARK_THREAD_COUNT	63
#define SKIP_LIST_BENCHMARK_BOUND			10e3

typedef struct _SBenchmarkSkipListArg
{
	/** Блокировка для записи */
	PCRITICAL_SECTION	crWriteLock;

	/** Время выполнения */
	LONG				nTime;

	/** Экземпляр скип листа */
	PVOID				psSkipList;

	/** Количество потоков */
	DWORD				dwThreadsCount;

	/** Начало диапозона */
	DWORD				dwFrom;

	/** Конец диапозона */
	DWORD				dwTo;
} SBenchmarkSkipListArg, *PSBenchmarkSkipListArg;


LONG*
InternalMultiThreadLockFreeSetSelection();

LONG*
InternalMultiThreadLockFreeFindSelection();

LONG*
InternalMultiThreadSetSelection();

LONG*
InternalMultiThreadFindSelection();

static
DWORD
WINAPI
SetLockFreeSkipListRoutine(
	LPVOID lpThreadParameter
)
{
	PSBenchmarkSkipListArg arg = (SBenchmarkSkipListArg*)lpThreadParameter;
	PSLockFreeSkipList psSkipList = (PSLockFreeSkipList)arg->psSkipList;

	LONG lTime = 0;
	LONG lTotal = 0;

	for (DWORD i = arg->dwFrom; i < arg->dwTo; i++)
	{
		lTime = clock();

		LockFreeSkipListSet(psSkipList, (void*)i, (void*)i);

		lTotal += (clock() - lTime);
	}

	arg->nTime = lTotal;

	return 0;
}

static
DWORD
WINAPI
FindLockFreeSkipListRoutine(
	LPVOID lpThreadParameter
)
{
	PSBenchmarkSkipListArg arg = (SBenchmarkSkipListArg*)lpThreadParameter;
	PSLockFreeSkipList psSkipList = (PSLockFreeSkipList)arg->psSkipList;

	LONG lTime = 0;
	LONG lTotal = 0;

	for (DWORD i = arg->dwFrom; i < arg->dwTo; i++)
	{
		lTime = clock();

		LockFreeSkipListFind(psSkipList, (void*)i);

		lTotal += (clock() - lTime);
	}

	arg->nTime = lTotal;

	return 0;
}

static
DWORD
WINAPI
FindSkipListRoutine(
	LPVOID lpThreadParameter
)
{
	PSBenchmarkSkipListArg arg = (SBenchmarkSkipListArg*)lpThreadParameter;
	PSSkipList psSkipList = (PSSkipList)arg->psSkipList;

	LONG lTime = 0;
	LONG lTotal = 0;

	for (DWORD i = arg->dwFrom; i < arg->dwTo; i++)
	{
		lTime = clock();

		EnterCriticalSection(arg->crWriteLock);
		SkipListFind(psSkipList, (void*)i);
		LeaveCriticalSection(arg->crWriteLock);

		lTotal += (clock() - lTime);
	}

	arg->nTime = lTotal;

	return 0;
}

static
DWORD
WINAPI
SetSkipListRoutine(
	LPVOID lpThreadParameter
)
{
	PSBenchmarkSkipListArg arg = (SBenchmarkSkipListArg*)lpThreadParameter;
	PSSkipList psSkipList = (PSSkipList)arg->psSkipList;

	LONG lTime = 0;
	LONG lTotal = 0;

	for (DWORD i = arg->dwFrom; i < arg->dwTo; i++)
	{
		lTime = clock();

		EnterCriticalSection(arg->crWriteLock);
		SkipListSet(psSkipList, (void*)i, (void*)i);
		LeaveCriticalSection(arg->crWriteLock);

		lTotal += (clock() - lTime);
	}

	arg->nTime = lTotal;

	return 0;
}

LONG*
InternalMultiThreadSetSelection()
{
	DWORD dwSize = SKIP_LIST_BENCHMARK_THREAD_COUNT * sizeof(LONG);
	LONG* pnTime = (LONG*)malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	for (int i = 0; i < SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		pnTime[i] = 0;
	}

	HANDLE hThreads[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	SBenchmarkSkipListArg sArgs[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	CRITICAL_SECTION crWriteLock;

	InitializeCriticalSection(&crWriteLock);

	for (DWORD i = 1; i <= SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		PSSkipList psSkipList = CreateSkipList(
			TestIntComparator,
			TestIntEraser,
			TestIntChanger);

		for (DWORD j = 0; j < i; j++)
		{
			sArgs[j].crWriteLock = &crWriteLock;
			sArgs[j].dwThreadsCount = i;
			sArgs[j].nTime = 0;
			sArgs[j].psSkipList = psSkipList;
			sArgs[j].dwFrom = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * j;
			sArgs[j].dwTo = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * (j + 1);

			hThreads[j] = CreateThread(
				NULL, 0, SetSkipListRoutine,
				&sArgs[j], CREATE_SUSPENDED, NULL);
		}

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6387)
			ResumeThread(hThreads[j]);
		}

		WaitForMultipleObjects(i, hThreads, TRUE, INFINITE);

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6385)
			pnTime[i - 1] += sArgs[j].nTime;
#pragma warning(suppress: 6387)
			CloseHandle(hThreads[j]);
		}

		SkipListClose(psSkipList);
	}

	DeleteCriticalSection(&crWriteLock);

	return pnTime;
}

LONG*
InternalMultiThreadFindSelection()
{
	DWORD dwSize = SKIP_LIST_BENCHMARK_THREAD_COUNT * sizeof(LONG);
	LONG* pnTime = (LONG*)malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	for (int i = 0; i < SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		pnTime[i] = 0;
	}

	PSSkipList psSkipList = CreateSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	for (size_t j = 1; j <= nSelectionCount[ARRAYSIZE(nSelectionCount) - 1]; j++)
	{
		SkipListSet(psSkipList, (void*)j, (void*)j);
	}

	HANDLE hThreads[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	SBenchmarkSkipListArg sArgs[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	CRITICAL_SECTION crWriteLock;

	InitializeCriticalSection(&crWriteLock);

	for (DWORD i = 1; i <= SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		for (DWORD j = 0; j < i; j++)
		{
			sArgs[j].crWriteLock = &crWriteLock;
			sArgs[j].dwThreadsCount = i;
			sArgs[j].nTime = 0;
			sArgs[j].psSkipList = psSkipList;
			sArgs[j].dwFrom = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * j;
			sArgs[j].dwTo = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * (j + 1);

			hThreads[j] = CreateThread(
				NULL, 0, FindSkipListRoutine,
				&sArgs[j], CREATE_SUSPENDED, NULL);
		}

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6387)
			ResumeThread(hThreads[j]);
		}
		
		DWORD dwWaitRes = WaitForMultipleObjects(i, hThreads, TRUE, INFINITE);

		if (dwWaitRes != WAIT_OBJECT_0)
		{
			printf("BAD");
		}

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6385)
			pnTime[i - 1] += sArgs[j].nTime;
#pragma warning(suppress: 6387)
			CloseHandle(hThreads[j]);
		}
	}

	DeleteCriticalSection(&crWriteLock);

	SkipListClose(psSkipList);

	return pnTime;
}

LONG*
InternalMultiThreadLockFreeSetSelection()
{
	DWORD dwSize = SKIP_LIST_BENCHMARK_THREAD_COUNT * sizeof(LONG);
	LONG* pnTime = (LONG*)malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	for (int i = 0; i < SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		pnTime[i] = 0;
	}

	HANDLE hThreads[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	SBenchmarkSkipListArg sArgs[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	CRITICAL_SECTION crWriteLock;

	InitializeCriticalSection(&crWriteLock);

	for (DWORD i = 1; i <= SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		PSLockFreeSkipList psSkipList = CreateLockFreeSkipList(
			TestIntComparator,
			TestIntEraser,
			TestIntChanger);

		for (DWORD j = 0; j < i; j++)
		{
			sArgs[j].crWriteLock = &crWriteLock;
			sArgs[j].dwThreadsCount = i;
			sArgs[j].nTime = 0;
			sArgs[j].psSkipList = psSkipList;
			sArgs[j].dwFrom = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * j;
			sArgs[j].dwTo = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * (j + 1);

			hThreads[j] = CreateThread(
				NULL, 0, SetLockFreeSkipListRoutine,
				&sArgs[j], CREATE_SUSPENDED, NULL);
		}

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6387)
			ResumeThread(hThreads[j]);
		}

		WaitForMultipleObjects(i, hThreads, TRUE, INFINITE);

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6385)
			pnTime[i - 1] += sArgs[j].nTime;
#pragma warning(suppress: 6387)
			CloseHandle(hThreads[j]);
		}

		LockFreeSkipListClose(psSkipList);
	}

	DeleteCriticalSection(&crWriteLock);

	return pnTime;
}

LONG*
InternalMultiThreadLockFreeFindSelection()
{
	DWORD dwSize = SKIP_LIST_BENCHMARK_THREAD_COUNT * sizeof(LONG);
	LONG* pnTime = (LONG*)malloc(dwSize);
	if (!pnTime)
	{
		return NULL;
	}

	for (int i = 0; i < SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		pnTime[i] = 0;
	}

	PSLockFreeSkipList psSkipList = CreateLockFreeSkipList(
		TestIntComparator,
		TestIntEraser,
		TestIntChanger);

	for (size_t j = 1; j <= nSelectionCount[ARRAYSIZE(nSelectionCount) - 1]; j++)
	{
		LockFreeSkipListSet(psSkipList, (void*)j, (void*)j);
	}

	HANDLE hThreads[SKIP_LIST_BENCHMARK_THREAD_COUNT];
	SBenchmarkSkipListArg sArgs[SKIP_LIST_BENCHMARK_THREAD_COUNT];

	for (DWORD i = 1; i <= SKIP_LIST_BENCHMARK_THREAD_COUNT; i++)
	{
		for (DWORD j = 0; j < i; j++)
		{
			sArgs[j].dwThreadsCount = i;
			sArgs[j].nTime = 0;
			sArgs[j].psSkipList = psSkipList;
			sArgs[j].dwFrom = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * j;
			sArgs[j].dwTo = (DWORD)(SKIP_LIST_BENCHMARK_BOUND / i) * (j + 1);

			hThreads[j] = CreateThread(
				NULL, 0, FindLockFreeSkipListRoutine,
				&sArgs[j], CREATE_SUSPENDED, NULL);
		}

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6387)
			ResumeThread(hThreads[j]);
		}

		WaitForMultipleObjects(i, hThreads, TRUE, INFINITE);

		for (DWORD j = 0; j < i; j++)
		{
#pragma warning(suppress: 6385)
			pnTime[i - 1] += sArgs[j].nTime;
#pragma warning(suppress: 6387)
			CloseHandle(hThreads[j]);
		}
	}

	LockFreeSkipListClose(psSkipList);

	return pnTime;
}


/**
 * Выборка основных операций SkipList  при конкурентном доступе
 * Insert, Search
 */
VOID
MultiThreadSetSkipListSelection()
{
	HANDLE hOutFile = CreateFile(
		".\\Benchmarks\\SkipList\\SetMultiThreadSelection.bin",
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		0, NULL
	);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	LONG* plSelection = InternalMultiThreadSetSelection();
	LONG* plLockFreeSelection = InternalMultiThreadLockFreeSetSelection();

	DWORD dwBytesWrite = 0;
	WriteFile(
		hOutFile,
		plSelection,
		sizeof(LONG) * SKIP_LIST_BENCHMARK_THREAD_COUNT,
		&dwBytesWrite, NULL);
	WriteFile(
		hOutFile,
		plLockFreeSelection,
		sizeof(LONG) * SKIP_LIST_BENCHMARK_THREAD_COUNT,
		&dwBytesWrite, NULL);

	CloseHandle(hOutFile);

	free(plSelection);
	free(plLockFreeSelection);
}


/**
 * Выборка основных операций LockFreeSkipList при конкурентном доступе
 * Insert, Search
 */
VOID
MultiThreadFindSkipListSelection()
{
	HANDLE hOutFile = CreateFile(
		".\\Benchmarks\\SkipList\\FindMultiThreadSelection.bin",
		GENERIC_WRITE,
		0, NULL,
		CREATE_ALWAYS,
		0, NULL
	);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	LONG* plFindSelection = InternalMultiThreadFindSelection();
	LONG* plFindLockFreeSelection = InternalMultiThreadLockFreeFindSelection();

	DWORD dwBytesWrite = 0;
	WriteFile(
		hOutFile,
		plFindSelection,
		sizeof(LONG) * SKIP_LIST_BENCHMARK_THREAD_COUNT,
		&dwBytesWrite, NULL);
	WriteFile(
		hOutFile,
		plFindLockFreeSelection,
		sizeof(LONG) * SKIP_LIST_BENCHMARK_THREAD_COUNT,
		&dwBytesWrite, NULL);

	CloseHandle(hOutFile);

	free(plFindSelection);
	free(plFindLockFreeSelection);
}