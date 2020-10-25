/**
 * @file BloomFilter.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

/**
 * Установка платфомозависимых функций 
 */

#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#if defined(_M_X64) || defined(_M_AMD64)

#define MurMurHash128 MurmurHash3_x64_128
#define BitsetSet128Mask BitsetSet128MaskAligned_x64
#define BitsetCheck128Mask BitsetCheck128MaskAligned_x64

#elif defined(_M_IX86)

#define MurMurHash128 MurmurHash3_x86_128
#define BitsetSet128Mask BitsetSet128MaskAligned_x86
#define BitsetCheck128Mask BitsetCheck128MaskAligned_x86

#endif

#define BLOOM_FILTER_HASH_SIZE			0x80
#define BLOOM_FILTER_HASH_SEED			0xBADC0DED
#define BLLOM_FILTER_HASH_SEED_OFFSET	0x000041A7

/**
 * Структура фильтр Блума
 */
typedef struct _SBloomFilter
{
	/** Битовое множество */
	SBitset			sBitset;

	/** Количество хранимых элементов */
	volatile DWORD	dwCount;

	/** Вместимость */
	DWORD			dwCapacity;

	/** Количество хэш-функций */
	DWORD			dwHashCount;

	/** Вероятность ложноположительного срабатывания */
	DWORD			dwErrorProbably;

} SBloomFilter, *PSBloomFilter;


/**
* Инициализировать фильтр Блума
* @param[inout] psBloomFilter	Экзмепляр
* @param[in]	dwCapacity		Вместимость фильтра
* @param[in]	dwErrorProbably	Вероятность ложноположительного срабатывания  [1; 1000000]
* @return						Результат работы.
*/
BOOL
InitializeBloomFilter(
	PSBloomFilter	psBloomFilter,
	DWORD			dwCapacity,
	DWORD			dwErrorProbably
);

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
);

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
);


/**
* Очистить фильтр Блума
* @param[inout] psBloomFilter	Экзмепляр
*/
VOID
ClearBloomFilter(
	PSBloomFilter	psBloomFilter
);


/**
* Удалить фильтр Блума
* @param[inout] psBloomFilter	Экзмепляр
*/
VOID
DeleteBloomFilter(
	PSBloomFilter	psBloomFilter
);


#endif