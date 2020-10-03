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
);

#endif // FNV1A_H
