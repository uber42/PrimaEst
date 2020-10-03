/**
 * @file Fnv1a.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef FNV1A_H
#define FNV1A_H

#define FNV_1A_SEED		0x811C9DC5
#define FNV_1A_PRIME	0x01000193

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
);

#endif // FNV1A_H
