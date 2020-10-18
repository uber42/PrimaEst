/**
 * @file Bitset.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef BITSET_H
#define BITSET_H

/** Тип отвечающий за один блок */
typedef unsigned __int64 BS_TYPE, * PBS_TYPE;



#define BS_TYPE_BITS	(sizeof(BS_TYPE) * 0x8)
#define BS_TYPE_OFFSET	(BS_TYPE_BITS - 0x1)

/**
 * Структура для хранения битов
 */
typedef struct _SBitset
{
	/** Количество битов */
	DWORD		dwCount;

	/** Размер множества */
	DWORD		dwSize;

	/** Количество блоков выделенной памяти */
	DWORD		dwBlocksCount;

	/** Биты */
	PBS_TYPE	pllSet;

	/** Указатель на начало */
	PBS_TYPE	pllBegin;
} SBitset, *PSBitset;


/**
* Создать байтовое множество
* @param[in] dwCount		Количество битов в множестве
* @return					Экземпляр.
*/
PSBitset
CreateBitset(
	DWORD	dwCount
);

/**
* Удалить байтовое множество
* @param[in] psBitset		Экземпляр
*/
VOID
DeleteBitset(
	PSBitset	psBitset
);


/**
* Установить бит
* @param[in] psBitset		Экземпляр
* @param[in] dwCount		Номер бита
* @return					Экземпляр.
*/
BOOL
BitsetSetBit
(
	PSBitset	psBitset,
	DWORD		dwNumber
);


/**
* Установить маску 8 бит
* @param[in] psBitset		Экземпляр
* @param[in] bMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet8Mask
(
	PSBitset	psBitset,
	BYTE		bMask,
	DWORD		dwOffset
);

/**
* Установить маску 16 бит
* @param[in] psBitset		Экземпляр
* @param[in] wMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet16Mask
(
	PSBitset	psBitset,
	WORD		wMask,
	DWORD		dwOffset
);

/**
* Установить маску 32 бит
* @param[in] psBitset		Экземпляр
* @param[in] dwMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet32Mask
(
	PSBitset	psBitset,
	DWORD		dwMask,
	DWORD		dwOffset
);

/**
* Установить маску 64 бит для x86
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet64Mask_x86
(
	PSBitset	psBitset,
	DWORD		pdwMask[2],
	DWORD		dwOffset
);

/**
* Установить маску 128 бит для x86
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Номер бита
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet128Mask_x86
(
	PSBitset	psBitset,
	DWORD		pdwMask[4],
	DWORD		dwOffset
);

/**
* Установить маску 64 бит для x64
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet64Mask_x64
(
	PSBitset	psBitset,
	ULONG64		llMask,
	DWORD		dwOffset
);

/**
* Установить маску 128 бит для x64
* @param[in] psBitset		Экземпляр
* @param[in] pllMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetSet128Mask_x64
(
	PSBitset	psBitset,
	ULONG64		pllMask[2],
	DWORD		dwOffset
);


/**
* Проверить бит
* @param[in] psBitset		Экземпляр
* @param[in] dwCount		Номер бита
* @return					Экземпляр.
*/
BOOL
BitsetCheckBit
(
	PSBitset	psBitset,
	DWORD		dwNumber
);


/**
* Проверить маску 8 бит
* @param[in] psBitset		Экземпляр
* @param[in] bMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck8Mask
(
	PSBitset	psBitset,
	BYTE		bMask,
	DWORD		dwOffset
);

/**
* Проверить маску 16 бит
* @param[in] psBitset		Экземпляр
* @param[in] wMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck16Mask
(
	PSBitset	psBitset,
	WORD		dwMask,
	DWORD		dwOffset
);

/**
* Проверить маску 32 бит
* @param[in] psBitset		Экземпляр
* @param[in] dwMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck32Mask
(
	PSBitset	psBitset,
	DWORD		dwMask,
	DWORD		dwOffset
);

/**
* Проверить маску 64 бит для x86
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck64Mask_x86
(
	PSBitset	psBitset,
	DWORD		pdwMask[2],
	DWORD		dwOffset
);

/**
* Проверить маску 128 бит для x86
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Номер бита
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck128Mask_x86
(
	PSBitset	psBitset,
	DWORD		pdwMask[4],
	DWORD		dwOffset
);

/**
* Проверить маску 64 бит для x64
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck64Mask_x64
(
	PSBitset	psBitset,
	ULONG64		llMask,
	DWORD		dwOffset
);

/**
* Проверить маску 128 бит для x64
* @param[in] psBitset		Экземпляр
* @param[in] pllMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
BOOL
BitsetCheck128Mask_x64
(
	PSBitset	psBitset,
	ULONG64		pllMask[2],
	DWORD		dwOffset
);


#endif //!BITSET_H