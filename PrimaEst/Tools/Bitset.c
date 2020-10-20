/**
 * @file Bitset.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"


 /**
 * Создать байтовое множество
 * @param[in] dwCount		Количество битов в множестве
 * @return					Экземпляр.
 */
PSBitset
CreateBitset(
	DWORD	dwCount
)
{
	PSBitset psBitset = malloc(sizeof(SBitset));
	if (!psBitset)
	{
		return NULL;
	}

	DWORD dwBlocks = dwCount / (sizeof(BS_TYPE) * 0x8);
	DWORD dwAlign = dwCount % sizeof(BS_TYPE) == 0 ? 0 : 1;
	DWORD dwSize = (dwBlocks + dwAlign) * sizeof(BS_TYPE);
	PBS_TYPE pBuffer = malloc(dwSize);
	if (!pBuffer)
	{
		free(psBitset);
		return NULL;
	}

	memset(pBuffer, 0, dwSize);

	psBitset->dwBlocksCount = dwBlocks + dwAlign;
	psBitset->pllBegin = pBuffer + (psBitset->dwBlocksCount - 1);
	psBitset->pllSet = pBuffer;
	psBitset->dwCount = dwCount;
	psBitset->dwSize = dwSize;

	return psBitset;
}

/**
* Удалить байтовое множество
* @param[in] psBitset		Экземпляр
*/
VOID
DeleteBitset(
	PSBitset	psBitset
)
{
	if (psBitset != NULL)
	{
		free(psBitset->pllSet);
		free(psBitset);

		psBitset = NULL;
	}
}

/**
* Установить бит без проверки на попадание в множество
* @param[in] psBitset		Экземпляр
* @param[in] dwCount		Номер бита
* @return					Экземпляр.
*/
__inline
static
VOID 
InternalBitsetSetBit(
	PSBitset	psBitset,
	DWORD		dwNumber
)
{
	DWORD dwOffset = dwNumber / BS_TYPE_BITS;
	DWORD dwByteNum = BS_TYPE_OFFSET - dwNumber % BS_TYPE_BITS;

	PBS_TYPE pdwOffset = psBitset->pllBegin - dwOffset;
	BS_TYPE	 dwMask = (BS_TYPE)1 << (BS_TYPE_OFFSET - dwByteNum);

	*pdwOffset |= dwMask;
}


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
)
{
	if (dwNumber > psBitset->dwCount)
	{
		return FALSE;
	}

	InternalBitsetSetBit(psBitset, dwNumber);
	return TRUE;
}


/**
* Установить маску
* @param[in] psBitset		Экземпляр
* @param[in] bMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
__inline
static
VOID
InternalBitsetSetMask
(
	PSBitset	psBitset,
	BS_TYPE		bMask,
	DWORD		dwOffset
)
{
	DWORD dwBsOffset = dwOffset / BS_TYPE_BITS;
	DWORD dwByteNum = dwOffset % BS_TYPE_BITS;

	PBS_TYPE pbCurrentBlock = psBitset->pllBegin - dwBsOffset;
	PBS_TYPE pbNextBlock = pbCurrentBlock--;

	BS_TYPE  bLowPart = (BS_TYPE)bMask << dwByteNum;
	BS_TYPE  bHighPart = (BS_TYPE)bMask >> (BS_TYPE_BITS - dwByteNum);

	*pbCurrentBlock |= bHighPart;
	*pbNextBlock |= bLowPart;
}

/**
* Установить маску в выровненной памяти
* @param[in] psBitset		Экземпляр
* @param[in] bMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
__inline
static
VOID
InternalBitsetSetMaskAligned
(
	PSBitset	psBitset,
	BS_TYPE		bMask,
	DWORD		dwOffset
)
{
	DWORD dwBsOffset = dwOffset / BS_TYPE_BITS;

	PBS_TYPE pbCurrentBlock = psBitset->pllBegin - dwBsOffset;

	*pbCurrentBlock |= bMask;
}

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
)
{
	if (dwOffset + sizeof(bMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	InternalBitsetSetMask(psBitset, (BS_TYPE)bMask, dwOffset);

	return TRUE;
}

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
)
{
	if (dwOffset + sizeof(wMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	InternalBitsetSetMask(psBitset, (BS_TYPE)wMask, dwOffset);

	return TRUE;
}

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
)
{
	if (dwOffset + sizeof(dwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	InternalBitsetSetMask(psBitset, (BS_TYPE)dwMask, dwOffset);

	return TRUE;
}

/**
* Установить маску 64 бит
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
)
{
	if (dwOffset + sizeof(pdwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	BS_TYPE bsMask = ((BS_TYPE)pdwMask[0]) | ((BS_TYPE)pdwMask[1] << 32);

	InternalBitsetSetMask(psBitset, bsMask, dwOffset);

	return TRUE;
}

/**
* Установить маску 128 бит
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
)
{
	if (dwOffset + sizeof(pdwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	BS_TYPE bsMaskLow = ((BS_TYPE)pdwMask[0]) | ((BS_TYPE)pdwMask[1] << 32);
	BS_TYPE bsMaskHigh = ((BS_TYPE)pdwMask[2]) | ((BS_TYPE)pdwMask[3] << 32);

	InternalBitsetSetMask(psBitset, bsMaskLow, dwOffset);
	InternalBitsetSetMask(psBitset, bsMaskHigh, dwOffset + BS_TYPE_BITS);

	return TRUE;
}

/**
* Проверить маску 64 бит для x64
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
)
{
	if (dwOffset + sizeof(llMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	InternalBitsetSetMask(psBitset, (BS_TYPE)llMask, dwOffset);

	return TRUE;
}

/**
* Проверить маску 128 бит для x64
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
)
{
	if (dwOffset + sizeof(pllMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	InternalBitsetSetMask(psBitset, (BS_TYPE)pllMask[0], dwOffset);
	InternalBitsetSetMask(psBitset, (BS_TYPE)pllMask[1], dwOffset + BS_TYPE_BITS);

	return TRUE;
}

/**
* Проверить бит без проверки на попадание в границы
* @param[in] psBitset		Экземпляр
* @param[in] dwCount		Номер бита
* @return					Экземпляр.
*/
__inline
static
BOOL
InternalBitsetCheckBit(
	PSBitset	psBitset,
	DWORD		dwNumber
)
{
	DWORD dwOffset = dwNumber / BS_TYPE_BITS;
	DWORD dwByteNum = BS_TYPE_OFFSET - dwNumber % BS_TYPE_BITS;

	PBS_TYPE pdwOffset = psBitset->pllBegin - dwOffset;
	BS_TYPE	 dwMask = (BS_TYPE)1 << (BS_TYPE_OFFSET - dwByteNum);

	return (*pdwOffset & dwMask) == dwMask;
}


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
)
{
	if (dwNumber > psBitset->dwCount)
	{
		return FALSE;
	}

	return InternalBitsetCheckBit(psBitset, dwNumber);
}

/**
* Проверить маску
* @param[in] psBitset		Экземпляр
* @param[in] bMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
__inline
static
BOOL
InternalBitsetCheckMask
(
	PSBitset	psBitset,
	BS_TYPE		bMask,
	DWORD		dwOffset
)
{
	DWORD dwBsOffset = dwOffset / BS_TYPE_BITS;
	DWORD dwByteNum = dwOffset % BS_TYPE_BITS;

	PBS_TYPE pbCurrentBlock = psBitset->pllBegin - dwBsOffset;
	PBS_TYPE pbNextBlock = pbCurrentBlock--;

	BS_TYPE  bLowPart = (BS_TYPE)bMask << dwByteNum;
	BS_TYPE  bHighPart = (BS_TYPE)bMask >> (BS_TYPE_BITS - dwByteNum);


	return	(*pbCurrentBlock & bHighPart) == bHighPart &&
			(*pbNextBlock & bLowPart) == bLowPart;
}

/** 
* Проверить маску в выровненной памяти
* @param[in] psBitset		Экземпляр
* @param[in] bMask			Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
*/
__inline
static
BOOL
InternalBitsetCheckMaskAligned
(
	PSBitset	psBitset,
	BS_TYPE		bMask,
	DWORD		dwOffset
)
{
	DWORD dwBsOffset = dwOffset / BS_TYPE_BITS;
	PBS_TYPE pbCurrentBlock = psBitset->pllBegin - dwBsOffset;

	return	(*pbCurrentBlock & bMask) == bMask;
}

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
)
{
	if (dwOffset + sizeof(bMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	return InternalBitsetCheckMask(psBitset, (BS_TYPE)bMask, dwOffset);
}

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
)
{
	if (dwOffset + sizeof(dwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	return InternalBitsetCheckMask(psBitset, (BS_TYPE)dwMask, dwOffset);
}

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
)
{
	if (dwOffset + sizeof(dwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	return InternalBitsetCheckMask(psBitset, (BS_TYPE)dwMask, dwOffset);
}

/**
* Проверить маску 64 бит
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
)
{
	if (dwOffset + sizeof(pdwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	BS_TYPE bsMask = ((BS_TYPE)pdwMask[0]) | ((BS_TYPE)pdwMask[1] << 32);

	return InternalBitsetCheckMask(psBitset, (BS_TYPE)bsMask, dwOffset);
}

/**
* Проверить маску 128 бит
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
)
{
	if (dwOffset + sizeof(pdwMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	BS_TYPE bsMaskLow = ((BS_TYPE)pdwMask[0]) | ((BS_TYPE)pdwMask[1] << 32);
	BS_TYPE bsMaskHigh = ((BS_TYPE)pdwMask[2]) | ((BS_TYPE)pdwMask[3] << 32);

	return	InternalBitsetCheckMask(psBitset, bsMaskLow, dwOffset) &&
			InternalBitsetCheckMask(psBitset, bsMaskHigh, dwOffset + BS_TYPE_BITS);
}

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
)
{
	if (dwOffset + sizeof(llMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	return InternalBitsetCheckMask(psBitset, (BS_TYPE)llMask, dwOffset);
}

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
)
{
	if (dwOffset + sizeof(pllMask) > psBitset->dwCount)
	{
		return FALSE;
	}

	return	InternalBitsetCheckMask(psBitset, (BS_TYPE)pllMask[0], dwOffset) &&
			InternalBitsetCheckMask(psBitset, (BS_TYPE)pllMask[1], dwOffset + BS_TYPE_BITS);
}

/**
* Установить маску 128 бит для x86
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Номер бита
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
* Использовать только если уверены что обращаетесь к участкам выровненным по 64 бита
* и не выходите за границы созданного байтового множества
*/
VOID
BitsetSet128MaskAligned_x86
(
	PSBitset	psBitset,
	DWORD		pdwMask[4],
	DWORD		dwOffset
)
{
	BS_TYPE bsMaskLow = ((BS_TYPE)pdwMask[0]) | ((BS_TYPE)pdwMask[1] << 32);
	BS_TYPE bsMaskHigh = ((BS_TYPE)pdwMask[2]) | ((BS_TYPE)pdwMask[3] << 32);

	InternalBitsetSetMaskAligned(psBitset, bsMaskLow, dwOffset);
	InternalBitsetSetMaskAligned(psBitset, bsMaskHigh, dwOffset + BS_TYPE_BITS);
}

/**
* Установить маску 128 бит для x64 в выровненной памяти
* @param[in] psBitset		Экземпляр
* @param[in] pllMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
* Использовать только если уверены что обращаетесь к участкам выровненным по 64 бита
* и не выходите за границы созданного байтового множества
*/
VOID
BitsetSet128MaskAligned_x64
(
	PSBitset	psBitset,
	ULONG64		pllMask[2],
	DWORD		dwOffset
)
{
	InternalBitsetSetMaskAligned(psBitset, pllMask[0], dwOffset);
	InternalBitsetSetMaskAligned(psBitset, pllMask[1], dwOffset + BS_TYPE_BITS);
}

/**
* Проверить маску 128 бит для x86  в выровненной памяти
* @param[in] psBitset		Экземпляр
* @param[in] pdwMask		Номер бита
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
* Использовать только если уверены что обращаетесь к участкам выровненным по 64 бита
* и не выходите за границы созданного байтового множества
*/
BOOL
BitsetCheck128MaskAligned_x86
(
	PSBitset	psBitset,
	DWORD		pdwMask[4],
	DWORD		dwOffset
)
{
	BS_TYPE bsMaskLow = ((BS_TYPE)pdwMask[0]) | ((BS_TYPE)pdwMask[1] << 32);
	BS_TYPE bsMaskHigh = ((BS_TYPE)pdwMask[2]) | ((BS_TYPE)pdwMask[3] << 32);

	return	InternalBitsetCheckMaskAligned(psBitset, (BS_TYPE)bsMaskLow, dwOffset) &&
			InternalBitsetCheckMaskAligned(psBitset, (BS_TYPE)bsMaskHigh, dwOffset + BS_TYPE_BITS);
}

/**
* Проверить маску 128 бит для x64 в выровненной памяти
* @param[in] psBitset		Экземпляр
* @param[in] pllMask		Маска
* @param[in] dwOffset		Сдвиг от начала множества
* @return					Экземпляр.
* Использовать только если уверены что обращаетесь к участкам выровненным по 64 бита
* и не выходите за границы созданного байтового множества
*/
BOOL
BitsetCheck128MaskAligned_x64
(
	PSBitset	psBitset,
	ULONG64		pllMask[2],
	DWORD		dwOffset
)
{
	return	InternalBitsetCheckMaskAligned(psBitset, (BS_TYPE)pllMask[0], dwOffset) &&
			InternalBitsetCheckMaskAligned(psBitset, (BS_TYPE)pllMask[1], dwOffset + BS_TYPE_BITS);
}