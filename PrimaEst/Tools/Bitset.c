/**
 * @file Bitset.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"


 /**
 * ������� �������� ���������
 * @param[in] dwCount		���������� ����� � ���������
 * @return					���������.
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
* ������� �������� ���������
* @param[in] psBitset		���������
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
* ���������� ��� ��� �������� �� ��������� � ���������
* @param[in] psBitset		���������
* @param[in] dwCount		����� ����
* @return					���������.
*/
inline
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
* ���������� ���
* @param[in] psBitset		���������
* @param[in] dwCount		����� ����
* @return					���������.
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
* ���������� �����
* @param[in] psBitset		���������
* @param[in] bMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
*/
inline
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
* ���������� ����� 8 ���
* @param[in] psBitset		���������
* @param[in] bMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ���������� ����� 16 ���
* @param[in] psBitset		���������
* @param[in] wMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ���������� ����� 32 ���
* @param[in] psBitset		���������
* @param[in] dwMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ���������� ����� 64 ���
* @param[in] psBitset		���������
* @param[in] pdwMask		�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ���������� ����� 128 ���
* @param[in] psBitset		���������
* @param[in] pdwMask		����� ����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 64 ��� ��� x64
* @param[in] psBitset		���������
* @param[in] pdwMask		�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 128 ��� ��� x64
* @param[in] psBitset		���������
* @param[in] pllMask		�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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

	BS_TYPE bsMask = ((BS_TYPE)pllMask[0]) | ((BS_TYPE)pllMask[1] << 32);

	InternalBitsetSetMask(psBitset, (BS_TYPE)bsMask, dwOffset);

	return TRUE;
}

/**
* ��������� ��� ��� �������� �� ��������� � �������
* @param[in] psBitset		���������
* @param[in] dwCount		����� ����
* @return					���������.
*/
inline
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
* ��������� ���
* @param[in] psBitset		���������
* @param[in] dwCount		����� ����
* @return					���������.
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
* ��������� �����
* @param[in] psBitset		���������
* @param[in] bMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
*/
inline
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
* ��������� ����� 8 ���
* @param[in] psBitset		���������
* @param[in] bMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 16 ���
* @param[in] psBitset		���������
* @param[in] wMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 32 ���
* @param[in] psBitset		���������
* @param[in] dwMask			�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 64 ���
* @param[in] psBitset		���������
* @param[in] pdwMask		�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 128 ���
* @param[in] psBitset		���������
* @param[in] pdwMask		����� ����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 64 ��� ��� x64
* @param[in] psBitset		���������
* @param[in] pdwMask		�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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
* ��������� ����� 128 ��� ��� x64
* @param[in] psBitset		���������
* @param[in] pllMask		�����
* @param[in] dwOffset		����� �� ������ ���������
* @return					���������.
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

	BS_TYPE bsMask = ((BS_TYPE)pllMask[0]) | ((BS_TYPE)pllMask[1] << 32);

	return InternalBitsetCheckMask(psBitset, (BS_TYPE)bsMask, dwOffset);
}