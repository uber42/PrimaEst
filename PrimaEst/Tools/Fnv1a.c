/**
 * @file Fnv1a.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

 /**
 * ��������� ���� ����
 * @param[in] ucByte			���� ������
 * @param[in] dwHashSeed		��������� ���
 * @return					���������.
 */
DWORD
Fnv1aCodeOneByte(
	BYTE	ucByte,
	DWORD	dwHashSeed
);

/**
* ��������� FNV1A
* @param[in] pData			������
* @param[in] dwDataLength	����� ������
* @param[in] dwDataSize		������ ������
* @param[in] dwHashSeed		��������� ���
* @return					���������
*/
DWORD
Fnv1aCompute(
	PVOID		pData,
	DWORD		dwDataLength,
	DWORD		dwDataSize,
	DWORD		dwHashSeed
)
{
	DWORD dwBytesCount = dwDataLength * dwDataSize;

	if (pData == NULL)
	{
		return 0;
	}

	unsigned char* pszCastedData = (unsigned char*)pData;
	DWORD dwHash = dwHashSeed;

	while (dwBytesCount--)
	{
		dwHash = Fnv1aCodeOneByte(*pszCastedData++, dwHash);
	}

	return dwHash;
}

/**
* ��������� ���� ����
* @param[in] ucByte			���� ������
* @param[in] dwHashSeed		��������� ���
* @return					���������.
*/
DWORD
Fnv1aCodeOneByte(
	BYTE	ucByte,
	DWORD	dwHashSeed
)
{
	return (ucByte ^ dwHashSeed)* FNV_1A_PRIME;
}