/**
 * @file SkipListDefinitions.h
 *
 * @author Pavel Chursin
 * @date Oct 2020
 */

#ifndef SKIP_LIST_DEFINITIONS_H
#define SKIP_LIST_DEFINITIONS_H

/** ������������ ������ */
#define SKIP_LIST_MAX_HEIGHT 12

/**
 * ���������� ������
 * @param[in] pFirst	������ ����
 * @param[in] pSecond	������ ����
 */
typedef int FSkipListComp(
	PVOID pFirst,
	PVOID pSecond
);

/**
 * ������� ����������� ����� ��� ����������� ���� ���� ����-��������
 * @param[in] dwHeight	������
 * @param[in] fNewLevel	��� �� �������� ������� �� ������ �������
 * @param[in] pKey		����
 * @param[in] pValue	��������
 */
typedef int FSkipListPrinter(
	DWORD dwHeight,
	BOOL  fNewLevel,
	PVOID pKey,
	PVOID pValue
);

/**
 * ������� ����������� �������� ���� ���������
 * @param[in] dwHeight	������
 */
typedef int FSkipListNodeEraser(
	PVOID	pNode
);

/**
 * ������� ����������� �������� ���� ���������
 * @param[in] dwHeight	������
 */
typedef int FSkipListNodeValueChanger(
	PVOID	*pValueDest,
	PVOID	pValueSrc
);

/**
 * ������������� ������
 * @return ������
 */
static DWORD
RandomHeight()
{
	DWORD dwRandom = LehmerRandom();
	DWORD dwBranching = 4;
	DWORD dwHeight = 1;

	while (dwHeight < SKIP_LIST_MAX_HEIGHT &&
		LehmerRandom() % dwBranching == 0)
	{
		dwHeight++;
	}

	return dwHeight;
}

#endif