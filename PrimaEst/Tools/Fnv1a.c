/**
 * @file Fnv1a.c
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#include "../global.h"

 /**
 * Посчитать один байт
 * @param[in] ucByte			Байт данных
 * @param[in] dwHashSeed		Начальный хэш
 * @return					Результат.
 */
DWORD
Fnv1aCodeOneByte(
	BYTE	ucByte,
	DWORD	dwHashSeed
);

/**
* Посчитать FNV1A
* @param[in] pData			Данные
* @param[in] dwDataLength	Длина данных
* @param[in] dwDataSize		Размер данных
* @param[in] dwHashSeed		Начальный хэш
* @return					Результат
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
* Посчитать один байт
* @param[in] ucByte			Байт данных
* @param[in] dwHashSeed		Начальный хэш
* @return					Результат.
*/
DWORD
Fnv1aCodeOneByte(
	BYTE	ucByte,
	DWORD	dwHashSeed
)
{
	return (ucByte ^ dwHashSeed)* FNV_1A_PRIME;
}