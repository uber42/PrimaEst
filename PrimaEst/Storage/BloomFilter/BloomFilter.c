/**
 * @file BloomFilter.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../../global.h"

 /**
 * Инициализировать фильтр Блума
 * @param[inout] psBloomFilter		Экзмепляр
 * @param[in]	 dwCapacity			Вместимость фильтра
 * @param[in]	 dwErrorProbably	Вероятность ложноположительного срабатывания [1; 100]
 * @return							Результат работы.
 */
BOOL
InitializeBloomFilter(
	PSBloomFilter	psBloomFilter,
	DWORD			dwCapacity,
	DWORD			dwErrorProbably
)
{
	if (dwErrorProbably < 1 || dwErrorProbably > 100)
	{
		LogError("[Bloom Filter] Вероятность ложноположительного срабатывания не принадлежит диапозону [1; 100]");
		return FALSE;
	}

	/** 0.4761 ~ ln2 * ln2 */
	DWORD dwBitsetSize = (DWORD)(-(dwCapacity * log((DOUBLE)dwErrorProbably / 100)) / 0.4761);
	/** 0.1732 ~ ln2 / 4 */
	DWORD dwHashCount = (DWORD)((dwBitsetSize / dwCapacity) * 0.1732);

	if (dwHashCount < 1)
	{
		dwHashCount = 1;
	}
	else if (dwHashCount > 8)
	{
		dwHashCount = 8;
	}

	BOOL bResult = InitializeBitset(
		&psBloomFilter->sBitset, dwBitsetSize);
	if (!bResult)
	{
		return FALSE;
	}

	psBloomFilter->dwCount = 0;
	psBloomFilter->dwCapacity = dwCapacity;
	psBloomFilter->dwErrorProbably = dwErrorProbably;
	psBloomFilter->dwHashCount = dwHashCount;

	return TRUE;
}

/**
* Добавить значение в фильтр Блума
* @param[inout] psBloomFilter	Экзмепляр
* @param[in]	pValue			Значение
* @param[in]	dwSize			Размер значения
*/
VOID
BloomFilterAdd(
	PSBloomFilter	psBloomFilter,
	PVOID			pValue,
	DWORD			dwSize
)
{
	InterlockedIncrement(&psBloomFilter->dwCount);

	DWORD	dwHashes[4];
	DWORD	dwDeltas[4];

#if defined(_M_X64) || defined(_M_AMD64)
	LONG64 pllHash[2];

	MurMurHash128(
		pValue, dwSize,
		BLOOM_FILTER_HASH_SEED,
		pllHash);

	dwHashes[0] = (DWORD)(pllHash[0] & 0xFFFFFFFF);
	dwHashes[1] = (DWORD)(pllHash[0] >> 32);
	dwHashes[2] = (DWORD)(pllHash[1] & 0xFFFFFFFF);
	dwHashes[3] = (DWORD)(pllHash[1] >> 32);

#elif defined(_M_IX86)
	MurMurHash128(
		pValue, dwSize,
		BLOOM_FILTER_HASH_SEED,
		dwHashes);
#endif

	for (DWORD dwHashIdx = 0; dwHashIdx < ARRAYSIZE(dwHashes); dwHashIdx++)
	{
		dwDeltas[dwHashIdx] = (dwHashes[dwHashIdx] >> 17) | (dwHashes[dwHashIdx] << 15);
	}

	PSBitset psBitset = &psBloomFilter->sBitset;
	for (DWORD dwidx = 0; dwidx < psBloomFilter->dwHashCount; dwidx++)
	{
		for (DWORD dwHashIdx = 0; dwHashIdx < ARRAYSIZE(dwHashes); dwHashIdx++)
		{
			BitsetSetBitProxy(psBitset, dwHashes[dwHashIdx] % psBitset->dwSize);
			dwHashes[dwHashIdx] += dwDeltas[dwHashIdx];
		}
	}
}

/**
* Проверить МОЖЕТ ли фильтр содержать значение
* @param[inout] psBloomFilter	Экзмепляр
* @param[in]	pValue			Значение
* @param[in]	dwSize			Размер значения
* @return						Результат работы.
*/
BOOL
BloomFilterTryMatch(
	PSBloomFilter	psBloomFilter,
	PVOID			pValue,
	DWORD			dwSize
)
{
	DWORD	dwHashes[4];
	DWORD	dwDeltas[4];

#if defined(_M_X64) || defined(_M_AMD64)
	LONG64 pllHash[2];

	MurMurHash128(
		pValue, dwSize,
		BLOOM_FILTER_HASH_SEED,
		pllHash);

	dwHashes[0] = (DWORD)(pllHash[0] & 0xFFFFFFFF);
	dwHashes[1] = (DWORD)(pllHash[0] >> 32);
	dwHashes[2] = (DWORD)(pllHash[1] & 0xFFFFFFFF);
	dwHashes[3] = (DWORD)(pllHash[1] >> 32);

#elif defined(_M_IX86)
	MurMurHash128(
		pValue, dwSize,
		BLOOM_FILTER_HASH_SEED,
		dwHashes);
#endif

	for (DWORD dwHashIdx = 0; dwHashIdx < ARRAYSIZE(dwHashes); dwHashIdx++)
	{
		dwDeltas[dwHashIdx] = (dwHashes[dwHashIdx] >> 17) | (dwHashes[dwHashIdx] << 15);
	}

	PSBitset psBitset = &psBloomFilter->sBitset;
	for (DWORD dwidx = 0; dwidx < psBloomFilter->dwHashCount; dwidx++)
	{
		for (DWORD dwHashIdx = 0; dwHashIdx < ARRAYSIZE(dwHashes); dwHashIdx++)
		{
			if (!BitsetCheckBitProxy(psBitset, dwHashes[dwHashIdx] % psBitset->dwSize))
			{
				return FALSE;
			}
			dwHashes[dwHashIdx] += dwDeltas[dwHashIdx];
		}
	}

	return TRUE;
}

/**
* Очистить
* @param[inout] psBloomFilter	Экзмепляр
*/
VOID
ClearBloomFilter(
	PSBloomFilter	psBloomFilter
)
{
	ClearBitset(&psBloomFilter->sBitset);
	psBloomFilter->dwCount = 0;
}

/**
* Удалить фильтр Блума
* @param[inout] psBloomFilter	Экзмепляр
*/
VOID
DeleteBloomFilter(
	PSBloomFilter	psBloomFilter
)
{
	DeleteBitset(&psBloomFilter->sBitset);
	psBloomFilter->dwCount = 0;
}