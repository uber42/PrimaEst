/**
 * @file Bitset.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef BITSET_H
#define BITSET_H

/** ��� ���������� �� ���� ���� */
typedef unsigned __int64 BS_TYPE, * PBS_TYPE;



#define BS_TYPE_BITS	(sizeof(BS_TYPE) * 0x8)
#define BS_TYPE_OFFSET	(BS_TYPE_BITS - 0x1)

/**
 * ��������� ��� �������� �����
 */
typedef struct _SBitset
{
	/** ���������� ����� */
	DWORD		dwCount;

	/** ������ ��������� */
	DWORD		dwSize;

	/** ���������� ������ ���������� ������ */
	DWORD		dwBlocksCount;

	/** ���� */
	PBS_TYPE	pllSet;

	/** ��������� �� ������ */
	PBS_TYPE	pllBegin;
} SBitset, *PSBitset;


/**
* ������� �������� ���������
* @param[in] dwCount		���������� ����� � ���������
* @return					���������.
*/
PSBitset
CreateBitset(
	DWORD	dwCount
);

/**
* ������� �������� ���������
* @param[in] psBitset		���������
*/
VOID
DeleteBitset(
	PSBitset	psBitset
);


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
);


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
);

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
);

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
);

/**
* ���������� ����� 64 ��� ��� x86
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
);

/**
* ���������� ����� 128 ��� ��� x86
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
);

/**
* ���������� ����� 64 ��� ��� x64
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
);

/**
* ���������� ����� 128 ��� ��� x64
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
);


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
);


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
);

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
);

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
);

/**
* ��������� ����� 64 ��� ��� x86
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
);

/**
* ��������� ����� 128 ��� ��� x86
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
);

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
);

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
);


#endif //!BITSET_H