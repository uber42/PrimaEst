#include "../global.h"



typedef struct _SBloomFilterKeysTest
{
	DWORD	dwValue[4];
} SBloomFilterKeysTest, * PSBloomFilterKeysTest;

typedef struct _SBloomFilterMultiThreadTest
{
	/** Экземпляр фильтра блума */
	PSBloomFilter psBloomFilter;

	/** Начало диапозона */
	DWORD	dwFrom;

	/** Конец диапозона */
	DWORD	dwTo;

	/** Ключ для вставки */
	PSBloomFilterKeysTest psKeys;
} SBloomFilterMultiThreadTest, *PSBloomFilterMultiThreadTest;

static
DWORD
WINAPI
BloomFilterTestRoutine(
	LPVOID	lpThreadParameter
)
{
	SBloomFilterMultiThreadTest sTest = *(SBloomFilterMultiThreadTest*)lpThreadParameter;

	for (DWORD dwIdx = sTest.dwFrom; dwIdx < sTest.dwTo; dwIdx++)
	{
		BloomFilterAdd(
			sTest.psBloomFilter,
			sTest.psKeys[dwIdx].dwValue,
			sizeof(SBloomFilterKeysTest)
		);
	}

	return 0;
}


VOID
BloomFilterTest()
{
	DWORD dwCount = 10000;

	SBloomFilter sBloomFilter;
	InitializeBloomFilter(&sBloomFilter, dwCount, 10);

	PSBloomFilterKeysTest psKeys = malloc(dwCount * sizeof(SBloomFilterKeysTest));
	if (!psKeys)
	{
		return;
	}

	for (DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++)
	{
		psKeys[dwIdx].dwValue[0] = LehmerRandom();
		psKeys[dwIdx].dwValue[1] = LehmerRandom();
		psKeys[dwIdx].dwValue[2] = LehmerRandom();
		psKeys[dwIdx].dwValue[3] = LehmerRandom();

		BloomFilterAdd(
			&sBloomFilter,
			psKeys[dwIdx].dwValue,
			sizeof(psKeys[dwIdx].dwValue)
		);
	}

	for (DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++)
	{
		assert(BloomFilterTryMatch(
			&sBloomFilter,
			psKeys[dwIdx].dwValue,
			sizeof(psKeys[dwIdx].dwValue))
		);
	}

	DeleteBloomFilter(&sBloomFilter);
}

#define BLOOM_FILTER_TEST_THREAD_COUNT 20

VOID
BloomFilterMultiThreadTest()
{
	DWORD dwCount = 100000;
	DWORD dwThreadCount = BLOOM_FILTER_TEST_THREAD_COUNT;

	SBloomFilter sBloomFilter;
	InitializeBloomFilter(&sBloomFilter, dwCount, 10);

	PSBloomFilterKeysTest psKeys = malloc(dwCount * sizeof(SBloomFilterKeysTest));
	if (!psKeys)
	{
		return;
	}

	for (DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++)
	{
		psKeys[dwIdx].dwValue[0] = LehmerRandom();
		psKeys[dwIdx].dwValue[1] = LehmerRandom();
		psKeys[dwIdx].dwValue[2] = LehmerRandom();
		psKeys[dwIdx].dwValue[3] = LehmerRandom();

		BloomFilterAdd(
			&sBloomFilter,
			psKeys[dwIdx].dwValue,
			sizeof(psKeys[dwIdx].dwValue)
		);
	}

	HANDLE hThread[BLOOM_FILTER_TEST_THREAD_COUNT] = { 0 };
	SBloomFilterMultiThreadTest hArgs[BLOOM_FILTER_TEST_THREAD_COUNT] = { 0 };

	SBloomFilter sBloomFilterMT;
	InitializeBloomFilter(&sBloomFilterMT, dwCount, 10);

	for (DWORD dwIdx = 0; dwIdx < dwThreadCount; dwIdx++)
	{
		hArgs[dwIdx].dwFrom = (dwCount / BLOOM_FILTER_TEST_THREAD_COUNT) * dwIdx;
		hArgs[dwIdx].dwTo = (dwCount / BLOOM_FILTER_TEST_THREAD_COUNT) * (dwIdx + 1);
		hArgs[dwIdx].psBloomFilter = &sBloomFilterMT;
		hArgs[dwIdx].psKeys = psKeys;

		hThread[dwIdx] = CreateThread(
			NULL, 0,
			BloomFilterTestRoutine,
			&hArgs[dwIdx], 0, NULL
		);
	}

	WaitForMultipleObjects(dwThreadCount, hThread, TRUE, INFINITE);

	for (DWORD dwIdx = 0; dwIdx < dwThreadCount; dwIdx++)
	{
#pragma warning(suppress : 6387)
#pragma warning(suppress : 6001)
		CloseHandle(hThread[dwIdx]);
	}

	for (DWORD dwIdx = 0; dwIdx < dwCount; dwIdx++)
	{
		assert(BloomFilterTryMatch(
			&sBloomFilterMT,
			psKeys[dwIdx].dwValue,
			sizeof(psKeys[dwIdx].dwValue))
		);
	}

	assert(sBloomFilter.dwCount == sBloomFilterMT.dwCount);
	assert(sBloomFilter.sBitset.dwBlocksCount == sBloomFilterMT.sBitset.dwBlocksCount);
	assert(sBloomFilter.sBitset.dwCount == sBloomFilterMT.sBitset.dwCount);
	assert(sBloomFilter.sBitset.dwSize == sBloomFilterMT.sBitset.dwSize);

	for (DWORD dwIdx = 0; dwIdx < sBloomFilter.sBitset.dwBlocksCount; dwIdx++)
	{
		BS_TYPE f = sBloomFilter.sBitset.pllSet[dwIdx];
		BS_TYPE s = sBloomFilterMT.sBitset.pllSet[dwIdx];

		assert(sBloomFilter.sBitset.pllSet[dwIdx] == sBloomFilterMT.sBitset.pllSet[dwIdx]);
	}

	DeleteBloomFilter(&sBloomFilterMT);
	DeleteBloomFilter(&sBloomFilter);
}